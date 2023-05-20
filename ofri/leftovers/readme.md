Readme
Introduction
This project includes three tools - cmp, copy, and stshell, and two libraries - codecA and codecB. These tools were developed and tested on Ubuntu 22.04.

Tool 1: cmp
The cmp tool compares two files and returns 0 if they are equal and 1 if they are different. The tool supports the following flags:

-v: verbose output, prints "equal" or "distinct" in addition to the returned int value.
-i: ignore case, treats "AAA" and "aaa" as equal.
Usage
cmp <file1> <file2> -v

Output
The tool returns an int value (0 or 1) and, if the -v flag is set, prints "equal" or "distinct".

Tool 2: copy
The copy tool copies a file to another place and/or name. The tool returns 0 on success and 1 on failure. The tool creates a new file if it does not exist, but it will not overwrite a file if it already exists. The tool supports the following flags:

-v: verbose output, prints "success" if the file is copied, or "target file exists" if the file already exists, or "general failure" on some other problem (in addition to the returned int value).
-f: force, allows overwriting the target file.
Usage
copy <file1> <file2> -v

Output
The tool returns an int value (0 or 1) and, if the -v flag is set, prints "success", "target file exists", or "general failure".

Tool 3: stshell
The stshell is a shell program that supports running CMD tools, stopping running tools by pressing Ctrl+c, redirecting output with ">" and ">>", and piping with "|". The shell can be stopped by using the "exit" command.

Usage
Run ./stshell to start the shell. Use standard shell syntax to execute commands, including piping and redirection.

Output
The shell prints the output of the executed command to the console.

Libraries: codecA and codecB
The libraries include two methods for encoding and decoding strings.

codecA
The codecA library converts all lowercase characters to uppercase and all uppercase characters to lowercase. All other characters remain unchanged.

Usage
Load the library with dlopen and call the encode or decode function with the string to encode/decode.

Output
The encode and decode functions return a string with the encoded/decoded message.

codecB
The codecB library converts all characters to the 3rd next character (adding a number of 3 to the ASCII value).

Usage
Load the library with dlopen and call the encode or decode function with the string to encode/decode.

Output
The encode and decode functions return a string with the encoded/decoded message.

stshell

The stshell program is a shell program that allows running CMD tools on the system, stopping running tools with Ctrl+c (without killing the shell itself), redirecting output with ">" and ">>", piping with "|", and stopping itself with the "exit" command.

Usage
Compile the program with the provided Makefile to generate the executable file "stshell".
Run the program by executing the "stshell" file.
Enter CMD tool commands with optional arguments and use ">" or ">>" to redirect output to a file or "|" to pipe output to another command.
Use Ctrl+c to stop running commands and "exit" to exit the shell program.

System Environment
The programs have been tested and developed on Ubuntu 22.04.