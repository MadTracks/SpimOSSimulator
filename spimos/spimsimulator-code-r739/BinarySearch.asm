.data

array: .word 2, 4, 7, 9, 10, 16, 21, 35    #The array must be sorted.
arraysize: .word 8                  #Size of array.
element: .word 7                    #The element search for

.text

.globl main

main:

la, $s0, array
la, $s1, arraysize
la, $s2, element

la, $a0, 0($s0)
li, $a1, 0
lw, $a2, 0($s1)
lw, $a3, 0($s2)
jal binary_Search

move $a0,$v0
addi, $v0, $0, 1   #prints the index.
syscall

addi, $v0, $0, 10   #exit program
syscall


binary_Search:
#a0: array
#a1: left
#a2: right
#a3: element
bge $a2,$a1,condition1
add $v0, $0, -1
jr $ra      #exit with error

condition1:
sub $t0, $a2, $a1
addi $t1, $0, 2
div $t0, $t1
mflo $t0
add $t0, $t0, $a1  #calculate the median element
la, $t2, 0($a0)
addi $t1, $0, 4
mul $t1, $t0, $t1
add, $t2, $t2, $t1
lw, $t3, 0($t2)
beq $t3, $a3, exit1
bgt $t3, $a3, recall1
addi $a1, $t0, 1
j binary_Search     #exit with right side of array

exit1:
addi $v0, $t0, 0    #return median
jr $ra      #exit with median

recall1:
addi $t1,$0,1
sub $a2, $t0, $t1
j binary_Search #exit with left side of array
