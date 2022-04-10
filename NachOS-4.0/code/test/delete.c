#include "syscall.h"

#define STRING_SIZE 256

int main()
{
    char filename[STRING_SIZE];
    int status;

    // Get the filename to be removed
    PrintString("Enter a filename: ");
    ReadString(filename, STRING_SIZE);

    // Remove the file
    status = Remove(filename);
    if (status < 0)
        PrintString("Remove failed.\n");
    else
        PrintString("Remove succeeded.\n");

    Halt();
}