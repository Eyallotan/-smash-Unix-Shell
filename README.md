# -smash-Unix-Shell
“smash” (small shell) will behave like a real Linux shell but it will support only a limited subset of Linux shell commands.

The program will work as follows:
- The program waits for commands that will be typed by the user and executes them (and back forth).
- The program can execute a small number of built-in commands, the commands will be listed below.
- When the program receives a command that is not one of the built-in commands (we call it an external command), it tries to run it like a normal shell. The way to run external commands will be described later.
- If one of the built-in commands was typed but with invalid parameters, the following error message should be printed: 
`smash error: > “command-line”`
where command-line is the command line as typed by the user (note that the error message should be wrapped with double quotes)
you’ll see examples later on.
- Whenever an error occurs a proper error message should be printed and the program should get back to parse and execute next typed commands.
- While the program is waiting for the next command to be entered, it should print the following text:
`smash>`


## [Full instructions and examples](Instructions.pdf) (English)

## installation
* As CMake project - target `CmakeLists.txt` or use Makefile.
