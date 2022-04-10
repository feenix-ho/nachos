#include "syscall.h"

#define STRING_SIZE 256

int main()
{
    char filename[STRING_SIZE];
    int status;

    // Get the filename to be created
    PrintString("Enter a filename: ");
    ReadString(filename, STRING_SIZE);

    // Create the new file with specified name
    status = Create(filename);
    if (status < 0)
        PrintString("Create failed.\n");
    else
        PrintString("Create succeeded.\n");

    Halt();
}