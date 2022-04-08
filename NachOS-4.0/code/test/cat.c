#include "syscall.h"

int main()
{
    char filename[STRING_SIZE];

    // Get the filename to be displayed
    PrintString("Enter a filename: ");
    ReadString(filename, STRING_SIZE);

    // Open the file to be displayed
    int fileId = Open(filename);
    if (fileId < 0)
        PrintString("Open failed.\n");
    else
        PrintString("Open succeeded.\n");

    // Print file content to the console
    char buffer[STRING_SIZE];
    int read = Read(buffer, STRING_SIZE, fileId);

    // Close the file
    Close(fileId);

    // Print the number of bytes read
    PrintString("Read ");
    PrintInt(read);
    PrintString(" bytes from the file.\n");

    // Print the content of the file
    PrintString(buffer);
    PrintChar('\n');

    Halt();
}