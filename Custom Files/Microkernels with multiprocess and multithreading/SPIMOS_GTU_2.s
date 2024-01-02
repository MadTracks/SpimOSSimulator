.data
shared_int: .word 0 #shared memory between thread
pname: .asciiz "producer"
cname: .asciiz "consumer"
pmessage: .asciiz "producer changes the value in shared memory, the value is:"
cmessage: .asciiz "consumer reads the value in shared memory, the value is:"
mut1: .asciiz "p_mutex"
mut2: .asciiz "c_mutex"
newline: .asciiz "\n"

.text

.globl main

main:

addi,$v0,$0,23
syscall

addi,$v0,$0,24
la,$a0,mut1
addi,$a1,$0,1
syscall

addi,$v0,$0,24
la,$a0,mut2
addi,$a1,$0,0
syscall

addi,$v0,$0,18
la,$a0,producer
la,$a1,pname
syscall

move $s0,$v0

addi,$v0,$0,18
la,$a0,consumer
la,$a1,cname
syscall
move $s1,$v0

move $a0,$s0
addi,$v0,$0,19
syscall

move $a0,$s1
addi,$v0,$0,19
syscall

addi,$v0,$0,10
syscall

producer:
addi,$t0,$0,0
addi,$t1,$0,15
prod_loop:
la,$a0,mut1
addi,$v0,$0,21
syscall

la $s5,shared_int
lw $t6,0($s5)

la,$a0,pmessage
addi,$v0,$0,4
syscall

move $a0,$t6
addi,$v0,$0,1
syscall

addi, $t6,$t6,1
sw $t6,0($s5)

la,$a0,newline
addi,$v0,$0,4
syscall

la,$a0,mut2
addi,$v0,$0,22
syscall

addi,$t0,$t0,1
bne $t0,$t1,prod_loop

addi,$v0,$0,20
syscall

consumer:

addi,$t0,$0,0
addi,$t1,$0,15
cons_loop:
la,$a0,mut2
addi,$v0,$0,21
syscall

la $s5,shared_int
lw $t6,0($s5)
la,$a0,cmessage
addi,$v0,$0,4
syscall

move $a0,$t6
addi,$v0,$0,1
syscall

addi, $t6,$t6,-1
sw $t6,0($s5)

la,$a0,newline
addi,$v0,$0,4
syscall

la,$a0,mut1
addi,$v0,$0,22
syscall

addi,$t0,$t0,1
bne $t0,$t1,cons_loop

addi,$v0,$0,20
syscall

