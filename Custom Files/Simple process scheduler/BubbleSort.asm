.data
sizemessage: .asciiz "Enter the size of array:\n"
inputmessage: .asciiz "Enter the integer:\n"
newline: .asciiz "\n"
array: .word 0   #Empty array

.text
.globl main

main:

la, $s0, array
la, $t3, 0($s0)

la, $a0, sizemessage
addi, $v0, $0, 4   #prints the size text.
syscall

addi, $v0, $0, 5   #reads the integer.
syscall
move $t0, $v0
addi, $t1, $0, 0 #counter

read_loop:
bge $t1,$t0,read_exit

la, $a0, inputmessage
addi, $v0, $0, 4   #prints the input text.
syscall

addi, $v0, $0, 5   #reads the integer.
syscall
move $t2, $v0
sw $t2, 0($t3)
addi, $t3,$t3,4

addi,$t1,$t1,1
j read_loop


read_exit:
la, $a0, 0($s0)
move $a1,$t0
jal Bubble_Sort
addi, $s0, $v0, 0

addi, $t0, $0, 0 #counter
print_loop:
beq $t0, $a1, print_exit
add, $t1, $t0, $t0
add, $t1, $t1, $t1
add, $t1, $s0, $t1
lw, $a0, 0($t1)
addi, $v0, $0, 1
syscall
la $a0, newline
addi $v0, $0, 4 #print newline
syscall
addi, $t0, $t0, 1 #counter++
j print_loop

print_exit:
addi, $v0, $0, 10   #exit program
syscall

Bubble_Sort:
#a0: array
#a1: size

addi, $t0, $0, 1
sub, $t2, $a1, $t0 #n-1
addi, $t0, $0, 0 #counter_i

loop1:
beq, $t0,$t2, loop1_exit
addi, $t1, $0, 0 #counter_j
sub, $t3, $t2, $t0 #n-1-i

loop2:
beq, $t1,$t3, loop2_exit
add, $t4, $t1, $t1
add, $t4, $t4, $t4 #j*4
add, $t4, $a0, $t4
lw, $t6, 0($t4)     #arr[j]

addi, $t5, $t1, 1  #j+1
add, $t5, $t5, $t5
add, $t5, $t5, $t5 #(j+1)*4
add, $t5, $a0, $t5
lw, $t7, 0($t5)     #arr[j+1]
bgt, $t6,$t7, ifcondition
loop2_end:
addi, $t1, $t1, 1  #j++
j loop2

ifcondition:

sw, $t6, 0($t5) #swap values
sw, $t7, 0($t4)
j loop2_end

loop2_exit:
addi, $t0, $t0, 1 #i++
j loop1

loop1_exit:
addi, $v0, $a0, 0
jr $ra


