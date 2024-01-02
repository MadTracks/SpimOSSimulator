.data

array: .word 36, 45, 26, 32, 76, 0, 122, 123, 61, 53, 42, 77, 16    #The array
arraysize: .word 13                 #Size of array.
newline: .asciiz "\n"
.text
.globl main

main:

la, $s0, array
la, $s1, arraysize

la, $a0, 0($s0)
lw, $a1, 0($s1)
addi, $a1, $a1, 1
jal Selection_Sort
addi, $s0, $v0, 0

lw, $a1, 0($s1)
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

Selection_Sort:
#a0: array
#a1: size
addi, $t1, $0, 1
sub, $t1, $a1, $t1
addi, $t0, $0, 0 #counter-i
loop1:
beq $t0,$t1, loop1_exit
addi, $t2, $t0, 0 #minimum
addi, $t3, $t0, 1 #counter-j

loop2:
beq, $t3, $a1, loop2_exit
add, $t4, $t3, $t3
add, $t4, $t4, $t4 #j*4
add, $t4, $a0, $t4
lw, $t6, 0($t4)     #arr[j]
add, $t5, $t2, $t2
add, $t5, $t5, $t5 #min*4
add, $t5, $a0, $t5
lw $t7, 0($t5)  #arr[min]
blt $t6,$t7, ifcondition
addi, $t3, $t3, 1 #j++
j loop2

loop2_exit:
add, $t8, $t0, $t0
add, $t8, $t8, $t8 
add, $t8, $a0, $t8
lw $t9, 0($t8)  #arr[i]
sw, $t9, 0($t5) #swap values
sw, $t7, 0($t8)
addi, $t0, $t0, 1 #i++
j loop1

ifcondition:
add, $t2, $t3, $0
addi, $t3, $t3, 1
j loop2

loop1_exit:
addi $v0, $a0, 0
jr $ra
