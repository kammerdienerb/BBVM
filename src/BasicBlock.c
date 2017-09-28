//
//  BasicBlock.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/4/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "BasicBlock.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#define BBVM_BB_INIT_INST_CAPACITY 8

BBVMBasicBlock * BBVMBasicBlock_create(const char * name) {
    BBVMBasicBlock * bb = malloc(sizeof(BBVMBasicBlock));
    
    bb->_inst_capacity = BBVM_BB_INIT_INST_CAPACITY;
    bb->instructions = malloc(BBVM_BB_INIT_INST_CAPACITY * sizeof(BBVMInst));
    bb->n_instructions = 0;
    
    bb->name = malloc(strlen(name));
    strcpy(bb->name, name);
    
    return bb;
}

void BBVMBasicBlock_destroy(BBVMBasicBlock * bb) {
    free(bb->instructions);
    free(bb->name);
    free(bb);
}

static void BBVMBasicBlock_grow_instructions(BBVMBasicBlock * bb) {
    bb->_inst_capacity *= 2;
    bb->instructions = realloc(bb->instructions, bb->_inst_capacity * sizeof(BBVMInst));
}

void BBVMBasicBlock_push_instruction(BBVMBasicBlock * bb, BBVMInst inst) {
    if (bb->n_instructions == bb->_inst_capacity)
        BBVMBasicBlock_grow_instructions(bb);
    bb->instructions[bb->n_instructions++] = inst;
}
