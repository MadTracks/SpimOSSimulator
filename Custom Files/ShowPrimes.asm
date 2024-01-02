.data

newline: .asciiz "\n"
primemessage: .asciiz " prime\n"
.text
.globl main

main:

addi, $s0, $0, 0
addi, $s1, $0, 1000
move $a0,$s0
move $a1,$s1

jal ShowPrimes

addi, $v0, $0 ,10	#exit program
syscall 

ShowPrimes:
#a0 integer value of "0"
#a1 integer value of "1000"

add, $t0,$a0,$0 #counter-i
add, $t1,$a1,$0

loop1:

addi,$a0,$t0,0
addi,$t8,$0,1
addi,$v0,$0,1  #print a0 value
syscall

beq $t0,$t1,loop1_exit
addi,$t5,$0,0
addi $t2,$0,2 #counter-j
loop2:
beq $t0,$0,not_prime
beq $t0,$t8,not_prime
bge $t2,$t0,loop2_exit
div $t0,$t2
mfhi $t3
beq $t3,$0,not_prime
addi $t2,$t2,1 #j++
j loop2

not_prime:
addi,$t5,$0,1
addi,$t0,$t0,1
la $a0, newline
addi $v0, $0, 4 #print newline
syscall
j loop1

loop2_exit:

la, $a0,primemessage
addi, $v0, $0, 4  #prints the "prime".
syscall
addi,$t0,$t0,1
j loop1

loop1_exit:
la $a0, newline
addi $v0, $0, 4 #print newline
syscall
move $v0, $t0
jr $ra

