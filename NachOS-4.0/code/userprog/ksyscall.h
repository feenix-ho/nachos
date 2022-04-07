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

void IncreasePC()
{
  /* set previous programm counter (debugging only)*/
  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

  /* set next programm counter for brach execution */
  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

// Input: - User space address (int)
//       - Limit of buffer (int)
//       - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User  memory space
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

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
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
  if (openFileTable != NULL)
  {
    for (int fd = 0; fd < FILE_NUM; ++fd)
      if (openFileTable[fd] != NULL)
        delete openFileTable[fd];

    delete[] openFileTable;
  }

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

int SysOpen(char *filename)
{
  OpenFile *openFile = kernel->fileSystem->Open(filename);
  
  if (openFile == NULL)
    return -1;
  
  if (openFileTable == NULL)
  {
    openFileTable = new OpenFile*[FILE_NUM];

    for (int i = 0; i < FILE_NUM; ++i)
      openFileTable[i] = NULL;
  }

  for (int fd = 0; fd < FILE_NUM; ++fd)
    if (openFileTable[fd] == NULL)
    {
      openFileTable[fd] = openFile;
      return fd;
    }
  
  return -1;
}

int SysClose(int fd)
{
  if (fd < 0 || fd >= FILE_NUM)
    return -1;

  if (openFileTable[fd] == NULL)
    return -1;

  delete openFileTable[fd];
  openFileTable[fd] = NULL;
  return 0;
}

int SysSeek(int position, int fd)
{
  if (fd < 0 || fd >= FILE_NUM)
    return -1;

  if (openFileTable[fd] == NULL)
    return -1;

  int length = openFileTable[fd]->Length();
  
  if (position < 0)
  {
    openFileTable[fd]->Seek(length);
    return length;
  }
  else if (position > length)
  {
    return -1;
  }
  else
  {
    openFileTable[fd]->Seek(position);
    return position;
  }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
