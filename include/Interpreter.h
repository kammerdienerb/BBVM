//
//  Interpreter.h
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/1/17.
//  Copyright © 2017 me. All rights reserved.
//

#ifndef Interpreter_h
#define Interpreter_h

#include "VM.h"

typedef void (*void_fn_ptr)(void);
typedef void_fn_ptr (*BBVM_inst_handler)(BBVirtualMachine *, BBVMInst);

void BBVMInterp_init(void);
void BBVMInterp_main_loop(BBVirtualMachine * VM);

#endif /* Interpreter_h */
