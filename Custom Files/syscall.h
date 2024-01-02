/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.

   Copyright (c) 1990-2010, James R. Larus.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

   Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

   Neither the name of the James R. Larus nor the names of its contributors may be
   used to endorse or promote products derived from this software without specific
   prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

class process_class{

public:
reg_word R[R_LENGTH];    /*Registers*/
reg_word HI, LO;
int HI_present, LO_present;
mem_addr PC, nPC;
double *FPR;
float *FGR;
int *FWR;
reg_word CCR[4][32], CPR[4][32];
instruction **text_seg;		/*Instructions*/
bool text_modified;
mem_addr text_top;
mem_word *data_seg;
bool data_modified;
short *data_seg_h;
BYTE_TYPE *data_seg_b;	
mem_addr data_top;
mem_addr gp_midpoint;	
mem_word *stack_seg;
short *stack_seg_h;	
BYTE_TYPE *stack_seg_b;	
mem_addr stack_bot;
instruction **k_text_seg;
mem_addr k_text_top;
mem_word *k_data_seg;
short *k_data_seg_h;
BYTE_TYPE *k_data_seg_b;
mem_addr k_data_top;
};


/* Exported functions. */

void run_process(reg_word register_a0);
void save_current_process(process_class * t);
void load_previous_process(process_class t);
void clear_process();
int do_syscall ();
void handle_exception ();


#define PRINT_INT_SYSCALL	1
#define PRINT_FLOAT_SYSCALL	2
#define PRINT_DOUBLE_SYSCALL	3
#define PRINT_STRING_SYSCALL	4

#define READ_INT_SYSCALL	5
#define READ_FLOAT_SYSCALL	6
#define READ_DOUBLE_SYSCALL	7
#define READ_STRING_SYSCALL	8

#define SBRK_SYSCALL		9

#define EXIT_SYSCALL		10

#define PRINT_CHARACTER_SYSCALL	11
#define READ_CHARACTER_SYSCALL	12

#define OPEN_SYSCALL		13
#define READ_SYSCALL		14
#define WRITE_SYSCALL		15
#define CLOSE_SYSCALL		16

#define EXIT2_SYSCALL		17
#define CREATE_PROCESS_SYSCALL     146
