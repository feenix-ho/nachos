#include "syscall.h"

int main()
{
  char *filename = "123_hello.txt";
  char *letter = "Hello there!\nNice to meet you.\n";
  int status = Create(filename);
  int fileId, closed, written, read, removed;
  
  fileId  = Open(filename);

  written = Write(letter, 100, fileId);
  read = Read(letter, 100, fileId);
  PrintString(letter);

  closed = Close(fileId);
  removed = Remove(filename);
  Halt();
}
