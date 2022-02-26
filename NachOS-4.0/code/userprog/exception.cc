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

void IncreasePC()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

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
			int result = 0;
			char sign = kernel->synchConsoleIn->GetChar();
			if ('0' <= sign && sign <= '9')
				result = sign - '0';
			else if (sign == ' ' || sign == '\n')
			{
				kernel->machine->WriteRegister(2, (int)result);
				IncreasePC();
				break;
			}

			int cnt = 0;

			while (cnt <= 10)
			{
				char digit = kernel->synchConsoleIn->GetChar();
				if ('0' <= digit && digit <= '9')
					result = result * 10 + digit - '0';
				else if (digit == ' ' || digit == '\n')
					break;
				++cnt;
			}

			if (sign == '-')
				result = -result;
			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC();

			DEBUG(dbgSys, "Reading number returning with result " << result << "\n");

			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadChar:
		{
			char result = kernel->synchConsoleIn->GetChar();
			kernel->machine->WriteRegister(2, (int)result);
			IncreasePC();

			DEBUG(dbgSys, "Reading character returning with result " << (char)result << "\n");
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

			DEBUG(dbgSys, "Random number returning with result " << result << "\n");
			return;

			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintString:
		{
			int addr = kernel->machine->ReadRegister(4);
			int len = kernel->machine->ReadRegister(5);

			bool ok = true;
			for (int i = 0; i < len; ++i)
			{
				int *holder;
				ok = kernel->machine->ReadMem(addr + i, 1, holder);
				if (!ok)
					break;
				kernel->synchConsoleOut->PutChar((char)*holder);
			}

			if (!ok)
			{
				char *message = "ERROR: Cannot print the given string.\0";
				for (int i = 0; message[i] != '\0'; ++i)
					kernel->synchConsoleOut->PutChar(message[i]);
			}

			kernel->synchConsoleOut->PutChar('\n');

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
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
