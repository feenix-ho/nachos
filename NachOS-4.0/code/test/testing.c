#include "syscall.h"

int main()
{
  // int num = ReadNum();
  // char chr = ReadChar();
  // int num = RandomNum();
  char *str = "hihihihi\0";
  PrintString(str);
  Halt();
}
