#include "syscall.h"

int main()
{
    char filename[STRING_SIZE];

    // Get the filename to be created
    PrintString("Enter a filename: ");
    ReadString(filename, STRING_SIZE);

    // Create the new file with specified name
    int status = Create(filename);
    if (status < 0)
        PrintString("Create failed.\n");
    else
        PrintString("Create succeeded.\n");

    Halt();
}