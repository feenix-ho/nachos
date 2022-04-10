/*
concatenate.c
    Concatenate 2 files together.
*/
#include "syscall.h"

#define STRING_SIZE 256

int main()
{
    char filename1[STRING_SIZE], filename2[STRING_SIZE], buffer1[STRING_SIZE], buffer2[STRING_SIZE];
    char filename[STRING_SIZE], result[STRING_SIZE * 2];
    int fileId, status, read1, read2, len, write;

    // Get the filename of 1st file
    PrintString("Enter the filename of 1st file: ");
    ReadString(filename1, STRING_SIZE);

    // Open the 1st file
    fileId = Open(filename1);
    if (fileId < 0)
        PrintString("Open failed.\n");
    else
        PrintString("Open succeeded.\n");

    // Read the content of 1st file
    read1 = Read(buffer1, STRING_SIZE, fileId);
    if (read1 < 0)
        PrintString("Read failed.\n");
    else
        PrintString("Read succeeded.\n");

    // PrintString("Read ");
    // PrintNum(read1);
    // PrintString(" bytes from the file.\n");

    // Close the 1st file
    Close(fileId);

    // Get the filename of 2nd file
    PrintString("Enter the filename of 2nd file: ");
    ReadString(filename2, STRING_SIZE);

    // Open the 2nd file
    fileId = Open(filename2);
    if (fileId < 0)
        PrintString("Open failed.\n");
    else
        PrintString("Open succeeded.\n");

    // Read the content of 2nd file
    read2 = Read(buffer2, STRING_SIZE, fileId);
    if (read2 < 0)
        PrintString("Read failed.\n");
    else
        PrintString("Read succeeded.\n");

    // Close the 2nd file
    Close(fileId);

    // Concatenate the 2 strings
    // PrintString("Concatenating 2 strings...\n");

    len = Concat(buffer1, buffer2, result);

    // PrintString("Concatenation succeeded.\n");
    // PrintNum(len);
    // PrintString(" bytes are concatenated.\n");

    // Get the filename of the result file
    PrintString("Enter the filename of the result file: ");
    ReadString(filename, STRING_SIZE);

    // Create the result file
    status = Create(filename);
    if (status < 0)
        PrintString("Create failed.\n");
    else
        PrintString("Create succeeded.\n");

    // Open the result file
    fileId = Open(filename);
    if (fileId < 0)
        PrintString("Open failed.\n");
    else
        PrintString("Open succeeded.\n");

    // Write the result to the file
    write = Write(result, len - 1, fileId);
    if (write < 0)
        PrintString("Write failed.\n");
    else
        PrintString("Write succeeded.\n");

    // PrintString("Write ");
    // PrintNum(write);
    // PrintString(" bytes to the file.\n");

    // Close the result file
    Close(fileId);

    Halt();
}