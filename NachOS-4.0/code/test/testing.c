#include "syscall.h"

int main()
{
  char *filename = "123_hello.txt";
  int status = Create(filename);
  int fileId, closed, written, read, removed;
  
  // fileId  = Open(filename);

  // char *letter = "Hello there!\nNice to meet you.\n";
  // written = Write(letter, 100, fileId);
  // read = Read(letter, 100, fileId);
  // PrintString(letter);

  closed = Close(fileId);
  removed = Remove(filename);
  Halt();
}
