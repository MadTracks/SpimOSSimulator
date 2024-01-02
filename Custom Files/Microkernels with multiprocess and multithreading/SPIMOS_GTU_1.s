.data

array: .word 36, 45, 26, 32, 76, 0, 122, 123, 61, 53, 42, 77, 16 , 41 , 187   #The array
arraysize: .word 15                 #Size of array.
threadpart: .word 5  				#Size of subarray.
threadnumber: .word 0				#Number of thread
temp: 	.word 0:100 #temp array
threadname1: .asciiz "mergesort-thread1"
threadname2: .asciiz "mergesort-thread2"
threadname3: .asciiz "mergesort-thread3"
space:		.asciiz		", "
newline: .asciiz "\n"
.text
.globl main

main:

addi,$v0,$0,23
syscall

la,$a0,array
addi,$a1,$0,0	
addi,$a2,$0,14
  		
jal print

la,$a0,newline
addi,$v0,$0,4
syscall

addi,$v0,$0,18
la,$a0,merge_sort_thread
la,$a1,threadname1
syscall

move $s0,$v0

addi,$v0,$0,18
la,$a0,merge_sort_thread
la,$a1,threadname2
syscall

move $s1,$v0

addi,$v0,$0,18
la,$a0,merge_sort_thread
la,$a1,threadname3
syscall

move $s2,$v0

move $a0,$s0
addi,$v0,$0,19
syscall

move $a0,$s1
addi,$v0,$0,19
syscall

move $a0,$s2
addi,$v0,$0,19
syscall

la $a0, array
addi,$a1,$0,0
addi,$a2,$0,9
addi,$a3,$0,4

jal merge

la,$a0,array
addi,$a1,$0,0	
addi,$a2,$0,14
addi,$a3,$0,9

jal merge

la,$a0,array
addi,$a1,$0,0	
addi,$a2,$0,14
  		
jal print

la,$a0,newline
addi,$v0,$0,4
syscall

addi,$v0,$0,10
syscall

merge_sort_thread:

la,$s5,threadnumber
lw,$t0,0($s5)
addi,$t1,$t0,1
sw,$t1,0($s5)

la,$t1,threadpart
lw,$t1,0($t1)
addi,$t2,$0,0
addi,$t3,$0,0
addi,$t4,$0,0

loop:
ble,$t0,$t2,loop_exit
add,$t3,$t3,$t1
add,$t4,$t4,$t1
addi,$t2,$t2,1
bne,$t2,$t0,loop
loop_exit:

add,$t4,$t4,$t1
addi,$t4,$t4,-1

sub,$t5,$t4,$t3
addi,$t6,$0,2
div,$t5,$t6
mflo,$t6
add,$t6,$t6,$t3

move $s6,$t3
move $s7,$t4
move $t9,$t6

la,$a0,array
move $a1,$t3
move $a2,$t6
jal merge_Sort

la,$a0,array
addi,$t5,$t9,1
move $a1,$t5
move $a2,$s7
jal merge_Sort

la,$a0,array
move $a1,$s6
move $a2,$s7
move $a3,$t9

jal merge

addi,$v0,$0,20
syscall

merge_Sort: 
slt,$t0,$a1,$a2  
beq,$t0,$0,merge_sort_exit
	
addi,$sp,$sp,-16
sw,$ra,12($sp)
sw,$a1,8($sp)
sw,$a2,4($sp)

add,$s0,$a1,$a2
sra,$s0,$s0,1
sw,$s0,0($sp)
				
move $a2,$s0
jal merge_Sort
	
lw,$s0,0($sp) 
addi,$s1,$s0,1
move $a1,$s1
lw,$a2,4($sp)
jal merge_Sort
	
lw,$a1,8($sp)
lw,$a2,4($sp)
lw,$a3,0($sp)
jal merge 	
				
lw,$ra,12($sp)
addi,$sp,$sp,16
jr $ra

merge_sort_exit:
jr $ra
	
merge:
move $s0,$a1
move $s1,$a1
addi,$s2,$a3,1

merge_loop1: 
blt $a3,$s0,merge_loop2
blt $a2,$s2,merge_loop2
j condition
	
condition:
sll,$t0,$s0,2
add,$t0,$t0,$a0
lw,$t1,0($t0)
sll,$t2,$s2,2
add,$t2,$t2,$a0
lw,$t3,0($t2)	
blt,$t3,$t1,condition2
la,$t4,temp
sll,$t5,$s1,2
add,$t4,$t4,$t5
sw,$t1,0($t4)
addi,$s1,$s1,1
addi,$s0,$s0,1
j merge_loop1
	
condition2:
sll,$t2,$s2,2
add,$t2,$t2,$a0
lw,$t3,0($t2)	
la,$t4,temp
sll,$t5,$s1,2
add,$t4,$t4,$t5
sw,$t3,0($t4)
addi,$s1,$s1,1
addi,$s2,$s2,1
j merge_loop1
	
merge_loop2:
blt,  $a3,$s0,merge_loop3
sll, $t0,$s0,2
add, $t0,$a0,$t0
lw, $t1,0($t0)
la,  $t2,temp
sll, $t3,$s1,2
add, $t3,$t3,$t2
sw, $t1,0($t3)
addi, $s1,$s1,1
addi, $s0,$s0,1
j merge_loop2
	

merge_loop3:
blt,$a2,$s1,merge_loop4
sll,$t2,$s2,2
add,$t2,$t2,$a0
lw,$t3,0($t2)
	
la,$t4,temp
sll,$t5,$s1,2
add,$t4,$t4,$t5
sw,$t3,0($t4)
addi,$s1,$s1,1
addi,$s2,$s2,1
j merge_loop3

merge_loop4:
move $t0,$a1
addi,$t1,$a2,1
la,$t4, temp	
j merge_loop5
merge_loop5:
slt,$t7,$t0,$t1
beq,$t7,$0,merge_exit
sll,$t2,$t0,2
add,$t3,$t2,$a0
add,$t5,$t2,$t4
lw,$t6,0($t5)
sw,$t6,0($t3)
addi,$t0,$t0,1
j merge_loop5

merge_exit:
jr $ra

print:
move $t0,$a1
move $t1,$a2
la,$t4,array
	
print_loop:
blt,$t1,$t0,print_exit
sll,$t3,$t0,2
add,$t3,$t3,$t4
lw,$t2,0($t3)
move,$a0,$t2
addi,$v0,$0,1
syscall
	
addi,$t0,$t0,1 
la,$a0,space
addi,$v0,$0,4
syscall
j print_loop
	
print_exit:
jr $ra
