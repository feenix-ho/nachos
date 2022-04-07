// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		}

		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
											/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			IncreasePC();

			return;

			ASSERTNOTREACHED();

			break;
		}

		case SC_ReadNum:
		{
			int result = SysReadNum();

			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC();

			DEBUG(dbgSys, "Reading number returning with result " << (int)result << "\n");

			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintNum:
		{
			int num = kernel->machine->ReadRegister(4);

			SysPrintNum(num);
			DEBUG(dbgSys, "Printing number " << (int)num << "\n");

			// Successfully printed the number
			kernel->machine->WriteRegister(2, (int)0);

			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadChar:
		{
			char result;
			do
			{
				result = SysReadChar();
			} while (result < 32 || result > 126);

			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC();

			DEBUG(dbgSys, "Reading character returning with result " << (char)result << "\n");
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintChar:
		{
			char result = kernel->machine->ReadRegister(4);
			SysPrintChar(result);
			kernel->machine->WriteRegister(2, (int)0);
			IncreasePC();

			DEBUG(dbgSys, "Printing character returning with result " << (char)result << "\n");
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_RandomNum:
		{
			RandomInit(time(NULL));
			int result = RandomNumber();
			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC();

			DEBUG(dbgSys, "Random number returning with result " << (int)result << "\n");
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadString:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			int length = kernel->machine->ReadRegister(5);

			char *buffer = new char[length + 1];
			int cnt = SysReadString(buffer, length);

			int transfered = System2User(virtAddr, cnt + 1, buffer);
			delete[] buffer;

			kernel->machine->WriteRegister(2, (int)transfered);
			IncreasePC();

			DEBUG(dbgSys, "Reading string returning with len " << (int)transfered << "\n");
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintString:
		{
			int virtAddr = kernel->machine->ReadRegister(4);

			char *buffer = User2System(virtAddr, STRING_SIZE);
			int status = SysPrintString(buffer);

			if (status == 0)
			{
				DEBUG(dbgSys, "Printing string OK\n");
			}
			else
			{
				DEBUG(dbgSys, "Printing string failed\n");
			}

			kernel->machine->WriteRegister(2, (int)status);
			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Create:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			char *buffer = User2System(virtAddr, STRING_SIZE);

			bool status = SysCreate(buffer);
			kernel->machine->WriteRegister(2, status ? 1 : -1);
			delete[] buffer;
			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Open:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			char *buffer = User2System(virtAddr, STRING_SIZE);

			int status = SysOpen(buffer);
			delete[] buffer;
			kernel->machine->WriteRegister(2, (int)status);
			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Close:
		{
			int fileId = kernel->machine->ReadRegister(4);

			int status = SysClose(fileId);
			kernel->machine->WriteRegister(2, (int)status);
			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Write:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			char *buffer = User2System(virtAddr, STRING_SIZE);
			int size = kernel->machine->ReadRegister(5);
			int fileId = kernel->machine->ReadRegister(6);

			int status = SysWrite(buffer, size, fileId);
			System2User(virtAddr, size, buffer);
			delete[] buffer;

			DEBUG(dbgSys, "\nWriten to file with status " << status);

			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Read:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int fileId = kernel->machine->ReadRegister(6);
			char *buffer = User2System(virtAddr, size);
			int status = SysRead(buffer, size, fileId);
			System2User(virtAddr, size, buffer);
			delete[] buffer;

			DEBUG(dbgSys, "\nRead from file with status " << status);

			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Seek:
		{
			int position = kernel->machine->ReadRegister(4);
			int fileId = kernel->machine->ReadRegister(5);

			int status = SysSeek(position, fileId);

			kernel->machine->WriteRegister(2, (int)status);
			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_Remove:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			char *buffer = User2System(virtAddr, STRING_SIZE);

			int status = SysRemove(buffer);
			DEBUG(dbgSys, "\nRemove file with file name" << buffer << " with status " << status);
			kernel->machine->WriteRegister(2, (int)status);

			IncreasePC();
			return;

			ASSERTNOTREACHED();
			break;
		}

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
		// CODE GOES HERE
	case NoException:
		cerr << "No exception detected.\n";
		DEBUG(dbgSys, "No exception detected.\n");

		IncreasePC();
		return;

		ASSERTNOTREACHED();
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
