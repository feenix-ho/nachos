#include "syscall.h"

int main()
{
  char *filename = "123_hello.txt";
  int status = Create(filename);
  int fileId = Open(filename);
  int closed, written, read, removed;
  
  char *letter = "Hello there!\nNice to meet you.\n";
  written = Write(letter, 100, fileId);
  read = Read(letter, 100, fileId);
  PrintString(letter);

  closed = Close(fileId);
  removed = Remove(filename);
  Halt();
}
