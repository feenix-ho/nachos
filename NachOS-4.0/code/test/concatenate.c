#include "syscall.h"

int main()
{
    char origin_filename[STRING_SIZE], new_filename[STRING_SIZE];

    // Get the origin filename
    PrintString("Enter the original filename: ");
    ReadString(origin_filename, STRING_SIZE);

    // Open the file to be displayed
    int fileId = Open(origin_filename);
    if (fileId < 0)
        PrintString("Open original file failed.\n");
    else
        PrintString("Open original file succeeded.\n");

    // Read content of original file
    char buffer[STRING_SIZE];
    int read = Read(buffer, STRING_SIZE, fileId);

    // Close the original file
    Close(fileId);

    // Get the new filename
    PrintString("Enter the new filename: ");
    ReadString(new_filename, STRING_SIZE);

    // Create the new file
    int status = Create(new_filename);
    if (status < 0)
        PrintString("Create new file failed.\n");
    else
        PrintString("Create new file succeeded.\n");

    // Open the new file
    fileId = Open(new_filename);
    if (fileId < 0)
        PrintString("Open new file failed.\n");
    else
        PrintString("Open new file succeeded.\n");

    // Write content of original file to the new file
    int write = Write(buffer, STRING_SIZE, fileId);
    if (write < 0)
        PrintString("Write to new file failed.\n");
    else
        PrintString("Write to new file succeeded.\n");

    // Close the new file
    Close(fileId);

    Halt();
}