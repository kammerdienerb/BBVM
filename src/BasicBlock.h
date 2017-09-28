//
//  BasicBlock.h
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/4/17.
//  Copyright © 2017 me. All rights reserved.
//

#ifndef BasicBlock_h
#define BasicBlock_h

#include "Bytecode.h"

#include <stddef.h>

typedef uint8_t BBVMBasicBlockIdx;

typedef struct BBVMBasicBlock {
    BBVMBasicBlockIdx id;
    BBVMInst * instructions;
    size_t n_instructions, _inst_capacity;
    char * name;
} BBVMBasicBlock;

BBVMBasicBlock * BBVMBasicBlock_create(const char * name);
void BBVMBasicBlock_destroy(BBVMBasicBlock * bb);
void BBVMBasicBlock_push_instruction(BBVMBasicBlock * bb, BBVMInst inst);

#endif /* BasicBlock_h */
