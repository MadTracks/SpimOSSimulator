/* SPIM S20 MIPS simulator.
   Execute SPIM syscalls, both in simulator and bare mode.
   Execute MIPS syscalls in bare mode, when running on MIPS systems.
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


#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef _WIN32
#include <io.h>
#endif

#include "spim.h"
#include "string-stream.h"
#include "inst.h"
#include "reg.h"
#include "mem.h"
#include "sym-tbl.h"
#include "syscall.h"

#include <iostream>
using namespace std;

#ifdef _WIN32
/* Windows has an handler that is invoked when an invalid argument is passed to a system
   call. https://msdn.microsoft.com/en-us/library/a9yf33zb(v=vs.110).aspx

   All good, except that the handler tries to invoke Watson and then kill spim with an exception.

   Override the handler to just report an error.
*/

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>

void myInvalidParameterHandler(const wchar_t* expression,
   const wchar_t* function, 
   const wchar_t* file, 
   unsigned int line, 
   uintptr_t pReserved)
{
  if (function != NULL)
    {
      run_error ("Bad parameter to system call: %s\n", function);
    }
  else
    {
      run_error ("Bad parameter to system call\n");
    }
}

static _invalid_parameter_handler oldHandler;

void windowsParameterHandlingControl(int flag )
{
  static _invalid_parameter_handler oldHandler;
  static _invalid_parameter_handler newHandler = myInvalidParameterHandler;

  if (flag == 0)
    {
      oldHandler = _set_invalid_parameter_handler(newHandler);
      _CrtSetReportMode(_CRT_ASSERT, 0); // Disable the message box for assertions.
    }
  else
    {
      newHandler = _set_invalid_parameter_handler(oldHandler);
      _CrtSetReportMode(_CRT_ASSERT, 1);  // Enable the message box for assertions.
    }
}
#endif

std::deque<Thread *> runnable;
std::deque<Thread *> blocked;
Thread * running;
int id=0;
char name[100];
int set_id=1;
std::deque<Mutex *> mutexes;

/*You implement your handler here*/
void SPIM_timerHandler()
{
  if(!runnable.empty()){
    if(id!=-1){
    save_current_thread(running);
    runnable.push_back(running);
    }
    running=runnable.front();
    runnable.pop_front();
    load_selected_thread(running);
    print_status();
  }
}
/* Decides which syscall to execute or simulate.  Returns zero upon
   exit syscall and non-zero to continue execution. */
int
do_syscall ()
{
#ifdef _WIN32
    windowsParameterHandlingControl(0);
#endif

  /* Syscalls for the source-language version of SPIM.  These are easier to
     use than the real syscall and are portable to non-MIPS operating
     systems. */

  switch (R[REG_V0])
    {
    case PRINT_INT_SYSCALL:
      write_output (console_out, "%d", R[REG_A0]);
      break;

    case PRINT_FLOAT_SYSCALL:
      {
	float val = FPR_S (REG_FA0);

	write_output (console_out, "%.8f", val);
	break;
      }

    case PRINT_DOUBLE_SYSCALL:
      write_output (console_out, "%.18g", FPR[REG_FA0 / 2]);
      break;

    case PRINT_STRING_SYSCALL:
      write_output (console_out, "%s", mem_reference (R[REG_A0]));
      break;

    case READ_INT_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	R[REG_RES] = atol (str);
	break;
      }

    case READ_FLOAT_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	FPR_S (REG_FRES) = (float) atof (str);
	break;
      }

    case READ_DOUBLE_SYSCALL:
      {
	static char str [256];

	read_input (str, 256);
	FPR [REG_FRES] = atof (str);
	break;
      }

    case READ_STRING_SYSCALL:
      {
	read_input ( (char *) mem_reference (R[REG_A0]), R[REG_A1]);
	data_modified = true;
	break;
      }

    case SBRK_SYSCALL:
      {
	mem_addr x = data_top;
	expand_data (R[REG_A0]);
	R[REG_RES] = x;
	data_modified = true;
	break;
      }

    case PRINT_CHARACTER_SYSCALL:
      write_output (console_out, "%c", R[REG_A0]);
      break;

    case READ_CHARACTER_SYSCALL:
      {
	static char str [2];

	read_input (str, 2);
	if (*str == '\0') *str = '\n';      /* makes xspim = spim */
	R[REG_RES] = (long) str[0];
	break;
      }

    case EXIT_SYSCALL:
      spim_return_value = 0;
      return (0);

    case EXIT2_SYSCALL:
      spim_return_value = R[REG_A0];	/* value passed to spim's exit() call */
      return (0);

    case OPEN_SYSCALL:
      {
#ifdef _WIN32
        R[REG_RES] = _open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#else
	R[REG_RES] = open((char*)mem_reference (R[REG_A0]), R[REG_A1], R[REG_A2]);
#endif
	break;
      }

    case READ_SYSCALL:
      {
	/* Test if address is valid */
	(void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
	R[REG_RES] = _read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
	R[REG_RES] = read(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
	data_modified = true;
	break;
      }

    case WRITE_SYSCALL:
      {
	/* Test if address is valid */
	(void)mem_reference (R[REG_A1] + R[REG_A2] - 1);
#ifdef _WIN32
	R[REG_RES] = _write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#else
	R[REG_RES] = write(R[REG_A0], mem_reference (R[REG_A1]), R[REG_A2]);
#endif
	break;
      }

    case CLOSE_SYSCALL:
      {
#ifdef _WIN32
	R[REG_RES] = _close(R[REG_A0]);
#else
	R[REG_RES] = close(R[REG_A0]);
#endif
	break;
      }
	case THREAD_CREATE_SYSCALL:
      {
        Thread * t1=new Thread;
        save_current_thread(t1);
        t1->PC=R[REG_A0];
        char * tname = (char*) mem_reference(R[REG_A1]);
        tname[strlen(tname)]='\0';
        strcpy(t1->name,tname);
        t1->id=set_id;
        set_id++;
        runnable.push_back(t1);
        R[REG_RES]=(long)t1->id;
	break;
      }
	case THREAD_JOIN_SYSCALL:
      {
        int flag=0;
        do{
          flag=0;
          for(unsigned int i=0;i<runnable.size();i++){
            if(R[REG_A0]==runnable.at(i)->id){
              flag=1;
            }
          }
          for(unsigned int i=0;i<blocked.size();i++){
            if(R[REG_A0]==blocked.at(i)->id){
              flag=1;
            }
          }
          if(flag==1){
            PC-=4;
            SPIM_timerHandler();
          }
        }
        while(flag==1);
        
	break;
      }
  case THREAD_EXIT_SYSCALL:
      {
        delete running;
        id=-1;
        SPIM_timerHandler();
	break;
      }
  case THREAD_MUTEX_LOCK_SYSCALL:
      {
        //int mutex_exist=0;
        //write_output (console_out, "ID:%d Locked\n",id);
        char * mname = (char*) mem_reference(R[REG_A0]);
        mname[strlen(mname)]='\0';
        for(unsigned int i=0;i<mutexes.size();i++){
          //write_output (console_out, "mutex lock %d name:%s compname:%s\n",mutexes.at(i)->value,mutexes.at(i)->name,mname);
          if(strcmp(mutexes.at(i)->name,mname) == 0 && mutexes.at(i)->value == true){
            //write_output (console_out, "ID:%d value changed\n",id);
            mutexes.at(i)->value=false;
            //mutex_exist=1;
            break;
          }
          if(strcmp(mutexes.at(i)->name,mname) == 0 && mutexes.at(i)->value == false){
            //write_output (console_out, "ID:%d blocked\n",id);
            save_current_thread(running);
            blocked.push_back(running);
            mutexes.at(i)->thread_id.push_back(id);
            id=-1;
            PC-=4;
            SPIM_timerHandler();
            //mutex_exist=1;
            break;
          }
        }              
	break;
      }
  case THREAD_MUTEX_UNLOCK_SYSCALL:
      {
        //write_output (console_out, "Unlocked\n");
        char * mname = (char*) mem_reference(R[REG_A0]);
        mname[strlen(mname)]='\0';
        for(unsigned int i=0;i<mutexes.size();i++){
          //write_output (console_out, "mutex unlock %d name:%s compname:%s\n",mutexes.at(i)->value,mutexes.at(i)->name,mname);
          if(strcmp(mutexes.at(i)->name,mname) == 0 && mutexes.at(i)->value == false){
            //write_output (console_out, "ID:%d value changed unlock\n",id);
            mutexes.at(i)->value=true;
            if(!mutexes.at(i)->thread_id.empty()){
              int tempid = mutexes.at(i)->thread_id.front();
              mutexes.at(i)->thread_id.pop_front();
              for(unsigned j=0; j<blocked.size();j++){
                if(tempid == blocked.at(j)->id){
                  Thread * temp=blocked.at(j);
                  blocked.erase(blocked.begin()+j);
                  runnable.push_back(temp);                  
                }
              }
            }
            //SPIM_timerHandler();
            break;
          }
        }
	break;
      }
  case INIT_SYSCALL:
      {
        id=0;
        strcpy(name,"init");
        running=new Thread;
        save_current_thread(running);
  break;
      }
  case THREAD_MUTEX_CREATE_SYSCALL:
      {
        char * mname = (char*) mem_reference(R[REG_A0]);
        mname[strlen(mname)]='\0';
        Mutex * temp = new Mutex;
        strcpy(temp->name,mname);
        if(R[REG_A1]==1){
          temp->value=true;
        }
        else{
          temp->value=false;
        }
        mutexes.push_back(temp); 
  break;
      }
    default:
      run_error ("Unknown system call: %d\n", R[REG_V0]);
      break;
    }
  
  
#ifdef _WIN32
    windowsParameterHandlingControl(1);
#endif
  return (1);
}


void
handle_exception ()
{
  if (!quiet && CP0_ExCode != ExcCode_Int)
    error ("Exception occurred at PC=0x%08x\n", CP0_EPC);

  exception_occurred = 0;
  PC = EXCEPTION_ADDR;

  switch (CP0_ExCode)
    {
    case ExcCode_Int:
      break;

    case ExcCode_AdEL:
      if (!quiet)
	error ("  Unaligned address in inst/data fetch: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_AdES:
      if (!quiet)
	error ("  Unaligned address in store: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_IBE:
      if (!quiet)
	error ("  Bad address in text read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_DBE:
      if (!quiet)
	error ("  Bad address in data/stack read: 0x%08x\n", CP0_BadVAddr);
      break;

    case ExcCode_Sys:
      if (!quiet)
	error ("  Error in syscall\n");
      break;

    case ExcCode_Bp:
      exception_occurred = 0;
      return;

    case ExcCode_RI:
      if (!quiet)
	error ("  Reserved instruction execution\n");
      break;

    case ExcCode_CpU:
      if (!quiet)
	error ("  Coprocessor unuable\n");
      break;

    case ExcCode_Ov:
      if (!quiet)
	error ("  Arithmetic overflow\n");
      break;

    case ExcCode_Tr:
      if (!quiet)
	error ("  Trap\n");
      break;

    case ExcCode_FPE:
      if (!quiet)
	error ("  Floating point\n");
      break;

    default:
      if (!quiet)
	error ("Unknown exception: %d\n", CP0_ExCode);
      break;
    }
}

void save_current_thread(Thread * t){
  t->id=id;
  strcpy(t->name,name);
  memcpy(t->R,R,sizeof(R));
  t->HI=HI;
  t->LO=LO;
  t->PC=PC;
  t->nPC=nPC;
  t->FPR=FPR;
  t->FGR=FGR;
  t->FWR=FWR;
  memcpy(t->CCR,CCR,sizeof(CCR));
  memcpy(t->CPR,CPR,sizeof(CPR));
  t->stack_seg=stack_seg;
  t->stack_seg_b=stack_seg_b;
  t->stack_seg_h=stack_seg_h;
  t->stack_bot=stack_bot;
}

void load_selected_thread(Thread * t){
  id=t->id;
  strcpy(name,t->name);
  memcpy(R,t->R,sizeof(t->R));
  HI=t->HI;
  LO=t->LO;
  PC=t->PC;
  nPC=t->nPC;
  FPR=t->FPR;
  FGR=t->FGR;
  FWR=t->FWR;
  memcpy(CCR,t->CCR,sizeof(t->CCR));
  memcpy(CPR,t->CPR,sizeof(t->CPR));
  stack_seg=t->stack_seg;
  stack_seg_b=t->stack_seg_b;
  stack_seg_h=t->stack_seg_h;
  stack_bot=t->stack_bot;
}

void print_status(){
  write_output (console_out, "Thread Table Status:\n");
  for(unsigned int i=0;i<runnable.size(); i++){
    write_output (console_out, "Thread id:%d name:%s status:runnable PC:%d SP:%d\n",runnable.at(i)->id,runnable.at(i)->name,runnable.at(i)->PC,runnable.at(i)->R[29]);
  }
  for(unsigned int i=0;i<blocked.size(); i++){
    write_output (console_out, "Thread id:%d name:%s status:blocked PC:%d SP:%d\n",blocked.at(i)->id,blocked.at(i)->name,blocked.at(i)->PC,blocked.at(i)->R[29]);
  }
  write_output (console_out, "Thread id:%d name:%s status:running PC:%d SP:%d\n",running->id,running->name,running->PC,running->R[29]);
}
