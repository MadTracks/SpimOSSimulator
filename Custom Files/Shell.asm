.data

newline: .asciiz "\n"
shellinfo: .asciiz "SPIMShell >"
buffer: .space 1024
exit: .asciiz "exit\n"

.text

.globl main

main:

la, $s3, exit
shell_loop:

la, $a0, shellinfo
addi, $v0, $0, 4   #prints the shell text.
syscall

la, $a0, buffer
addi, $a1, $0, 1024     #reads the given input.
addi, $v0, $0, 8
syscall
move $s0,$a0

addi, $t4,$s0,0         #first byte of entered string
addi, $t5,$s3,0         #first byte of entered string
string_check:
lb $t2,0($t4)
lb $t3,0($t5)
bne $t2,$t3,continue
beq $t2,$zero,exit_program
addi $t4,$t4,1          #next char of entered string
addi $t5,$t5,1          #next char of exit string
j string_check

continue:
move $a0,$s0
addi, $v0, $0, 146  #create new process
syscall
j shell_loop

exit_program:
addi, $v0, $0, 10   #exit program
syscall

