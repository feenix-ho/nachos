/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "synchconsole.h"
#include "kernel.h"
#include "filesys.h"

void IncreasePC()
{
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

int System2User(int virtAddr, int len, char *buffer)
{
  if (len < 0)
    return -1;
  if (len == 0)
    return len;
  int i = 0;
  int oneChar = 0;
  do
  {
    oneChar = (int)buffer[i];
    kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
    i++;
  } while (i < len && oneChar != 0);
  return i;
}

char *User2System(int virtAddr, int limit)
{
  int i; // index
  int oneChar;
  char *kernelBuf = NULL;
  kernelBuf = new char[limit + 1]; // need for terminal string
  if (kernelBuf == NULL)
    return kernelBuf;
  memset(kernelBuf, 0, limit + 1);
  // printf("\n Filename u2s:");
  for (i = 0; i < limit; i++)
  {
    kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
    kernelBuf[i] = (char)oneChar;
    // printf("%c",kernelBuf[i]);
    if (oneChar == 0)
      break;
  }
  return kernelBuf;
}

void SysHalt()
{
  // if (openFileTable != NULL)
  // {
  //   for (int fd = 0; fd < FILE_NUM; ++fd)
  //     if (openFileTable[fd] != NULL)
  //       delete openFileTable[fd];

  //   delete[] openFileTable;
  // }

  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

char SysReadChar()
{
  char c;
  c = kernel->synchConsoleIn->GetChar();

  return c;
}

void SysPrintChar(char c)
{
  kernel->synchConsoleOut->PutChar(c);
}

int SysReadString(char *buffer, int length)
{
  int cnt = 0;

  char c;
  do
  {
    c = SysReadChar();
  } while (c < 32 || c > 126);

  while (c != '\n' && c != '\0' && cnt < length)
  {
    buffer[cnt] = c;
    c = SysReadChar();
    ++cnt;
  }

  buffer[cnt] = '\0';
  return cnt;
}

int SysPrintString(char *buffer)
{
  char *message = "ERROR: Cannot print the given string.\0";
  int result;

  if (buffer == NULL)
  {
    buffer = message;
    result = -1;
  }
  else
    result = 0;

  for (int i = 0; buffer[i] != '\0'; ++i)
    SysPrintChar(buffer[i]);
  return result;
}

int SysReadNum()
{
  int result = 0;
  char sign;
  while (true)
  {
    sign = kernel->synchConsoleIn->GetChar();
    if ('0' <= sign && sign <= '9')
    {
      result = sign - '0';
      break;
    }
    if (sign == '-')
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
    return -result;
  return result;
}

void SysPrintNum(int num)
{
  // Print sign of number
  if (num < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
    num = -num;
  }

  // Convert number to string, stored in buffer
  char *buffer = new char[STRING_SIZE];
  sprintf(buffer, "%d", num);

  // Print number
  SysPrintString(buffer);
  delete[] buffer;
}

bool SysCreate(char *filename)
{
  int length = strlen(filename);
  bool success;

  if (length == 0)
  {
    DEBUG(dbgSys, "\nERROR: Empty file name.");
    return false;
  }

  if (filename == NULL)
  {
    DEBUG(dbgSys, "\nERROR: Not enough memory.")
    return false;
  }

  success = kernel->fileSystem->Create(filename);
  if (!success)
  {
    DEBUG(dbgSys, "\nERROR: Failed to create file.");
  }
  else
  {
    DEBUG(dbgSys, "\nCreated file with name " << filename);
  }
  return success;
}

int SysOpen(char *filename)
{
  if (filename == NULL || strlen(filename) == 0)
  {
    DEBUG(dbgSys, "\nERROR: Invalid file name.");
    return -1;
  }
  int fileId = kernel->fileSystem->Open(filename, 0);

  if (fileId == -1)
  {
    DEBUG(dbgSys, "\nERROR: Failed to open file.");
  }
  else
  {
    DEBUG(dbgSys, "\nOpened file with name " << filename << " at ID " << fileId << "\n");
  }
  return fileId;
}

int SysClose(int fileId)
{
  bool isOpening = kernel->fileSystem->IsOpeningID(fileId);
  if (!isOpening)
  {
    DEBUG(dbgSys, "\nERROR: File is not opening.");
    return -1;
  }

  int status = kernel->fileSystem->Close(fileId);
  if (status < 0)
  {
    DEBUG(dbgSys, "\nERROR: Failed to close file.");
  }
  else
  {
    DEBUG(dbgSys, "\nClosed file at ID " << fileId << "\n");
  }
  return status;
}

int SysRead(char *buffer, int size, int fileId)
{
  if (fileId == _ConsoleInput)
  {
    return kernel->synchConsoleIn->GetString(buffer, size);
  }
  if (fileId == _ConsoleOutput)
  {
    return -1;
  }
  return kernel->fileSystem->Read(buffer, size, fileId);
}

int SysWrite(char *buffer, int size, int fileId)
{
  if (fileId == _ConsoleInput)
  {
    return -1;
  }
  if (fileId == _ConsoleOutput)
  {
    return kernel->synchConsoleOut->PutString(buffer, size);
  }
  return kernel->fileSystem->Write(buffer, size, fileId);
}

int SysSeek(int position, int fileId)
{
  if (fileId == 0 || fileId == 1)
  {
    DEBUG(dbgSys, "\nERROR: Seek in console.");
    return -1;
  }
  return kernel->fileSystem->Seek(position, fileId);
}

int SysRemove(char *filename)
{
  if (filename == NULL || strlen(filename) == 0)
  {
    DEBUG(dbgSys, "\nERROR: Invalid file name.");
    return -1;
  }
  bool isOpening = kernel->fileSystem->IsOpeningFilename(filename);
  if (isOpening)
  {
    DEBUG(dbgSys, "\nERROR: File is currently opening.");
    return -1;
  }
  return kernel->fileSystem->Remove(filename);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
