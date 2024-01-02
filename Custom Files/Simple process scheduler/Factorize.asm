.data

inputmessage: .asciiz "Enter the integer:\n"
newline: .asciiz "\n"
resultmessage: .asciiz "The factors of integer are: "
errormessage: .asciiz "Integer must be greater than 0.\n"
.text
.globl main

main:

la, $a0, inputmessage
addi, $v0, $0, 4   #prints the input text.
syscall

addi, $v0, $0, 5   #reads the integer.
syscall
move $t0, $v0
ble $t0,$0,lessthanzero

la, $a0, resultmessage
addi, $v0, $0, 4   #prints the result text.
syscall

move $a0, $t0

jal Factorize
program_exit:
addi, $v0, $0 ,10	#exit program
syscall 

Factorize:
#a0 integer

addi $t1,$a0,0 #integer
addi, $t0,$0,1
addi $t2,$t1,0 #i

loop:
beq $t2,$0,loop_exit
div $t1,$t2
mfhi $t3
beq $t3,$0,print
loop_end:
sub $t2,$t2,$t0
j loop

print:
addi,$a0,$t2,0
addi,$v0,$0,1
syscall
la $a0, newline
addi $v0, $0, 4 #print newline
syscall
j loop_end

loop_exit:
move $v0, $t0
jr $ra

lessthanzero:
la, $a0, errormessage
addi, $v0, $0, 4   #prints the input text.
syscall
j program_exit

