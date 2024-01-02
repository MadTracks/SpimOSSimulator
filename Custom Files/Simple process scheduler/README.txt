Assembly Files:

Factorize.asm:
It takes a positive integer input and prints the all the factors of the integer.
If the integer is negative or zero, program prints error message.
ShowPrimes.asm:
Prints all of numbers between 0 and 1000.If the numbers are prime, it prints the "prime" text with the number. 
BubbleSort.asm:
It will sort the given array.
Prints the sorted array.
The array is in the asm file.

Other Files:

Shell.asm:
It is a terminal which is used to run programs.
The user need to enter filename in order to run it.

Example:
SPIMShell > BubbleSort.asm

The working algorithm:
SPIM runs the shell,
shell calls syscall
syscall copies the current program variables(registers etc.)
syscall clears the current variables
syscall reinitializes spim
syscall loads the next program
syscall runs the next program
syscall loads the previous program
shell works in a loop again

shell can exit the loop with "exit" keyword.
Note:SPIM must be executed in the location on .asm files.

Shell.c:
This program works as same as shell.asm.
It calls the spim in order to run assembly files.

syscall.h and syscall.cpp:
There is a class named process_class.
This class keeps the information of spim process.(Program counter,registers etc.)
run_process can run the new process with filename.
save_current_process can save the current process information into a class.
load_previous_process can load the saved process information from a class.
clear_process can clear current process.
