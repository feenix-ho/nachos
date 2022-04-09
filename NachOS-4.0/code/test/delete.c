#include "syscall.h"

int main()
{
    char filename[STRING_SIZE];

    // Get the filename to be removed
    PrintString("Enter a filename: ");
    ReadString(filename, STRING_SIZE);

    // Remove the file
    int status = Remove(filename);
    if (status < 0)
        PrintString("Remove failed.\n");
    else
        PrintString("Remove succeeded.\n");

    Halt();
}