# ShellLikeProgram
  Shell-like program displays the command line prompt “myshell>” and waits for the user's
command. It reads the user’s next command, parses it into separate tokens that are used to fill the
argument vector for the command to be executed, and executes it. The program supports following commands: cd, dir, history, findloc ,bye.
For any other commands,program should consider them as system commands. For system commands, your program creates a child process using 
fork system call, and the child process executes the command by using execvp() function.
  Shell-like program supports pipe operator between two processes.
• For a pipe in the command line, you need to connect stdout of the left command to stdin of
the command following the "|". For example, if the user types "ls -al | sort", then the “ls”
command is run with stdout directed to a Unix pipe, and that the sort command is run with
stdin coming from that same pipe.
## Installation and Setup Instructions

Clown the C source code and compile it. 

### About Project

This project is written to give an idea about unix shell and Operating System operations like forking and pipes.
