# myshs
A personalized Linux-based command shell in UNIX.


After started, it prints a prompt “#” and reads a command line terminated by newline. This line should be
parsed out into a command and all its arguments.

Built-in commands:

# history [-c]
Without the argument, it prints out the recently typed commands (with their
arguments), in reverse order, with numbers. For instance, it will print:
0: history
1: start chromium-browser
2: start /usr/bin/xterm –bg green
If the argument “-c” is passed, it clears the list of recently typed commands. 
# byebye
Terminates the mysh shell (and it should save the history file).
# replay number
Re-executes the command labeled with the given number in the history
# start program [parameters]
The argument “program” is the program to execute. If the argument starts with a “/”
(such as /usr/bin/xterm, the shell should interpret it as a full path. Otherwise, the
program will be interpreted as a relative path starting from the current directory.
The program will be executed with the optional “parameters”. 
<br/>
Display an error message if the specified program cannot be found or cannot be
executed.

# background program [parameters]
It is similar to the run command, but it immediately prints the PID of the program it
started, and returns the prompt.

# terminate PID
Immediately terminate the program with the specific PID (presumably started using
background). Use the kill() function call to send a SIGKILL signal to the program. Display
success or failure.

# repeat n command …
Interprets n as the number of times the command must be run, command as the full
path to the program to execute, and the others as parameters. The command should
start the specified number of program instances, print the PIDs of the created processes
and then return to the prompt, without waiting for the processes to terminate. 

# terminateall
Which immediately terminates all the programs previously started as background
programs by the mysh shell which had not been previously terminated by it, or by
terminate. It should output something like this:
Terminating 3 processes: 16000 31012 31013
Or
No processes to terminate.
