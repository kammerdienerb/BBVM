//
//  VM.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/1/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "VM.h"
#include "Interpreter.h"
#include "BytecodeBuilder.h"
#include "FFI.h"

#include <stdlib.h>
#include <string.h>

BBVirtualMachine * BBVM_icreate(BBVMInst * instructions, size_t n_instructions) {
    BBVirtualMachine * VM = malloc(sizeof(BBVirtualMachine));
    VM->instructions = instructions;
    VM->n_instructions = n_instructions;
    return VM;
}

BBVirtualMachine * BBVM_fcreate(FILE * f) {
    if (!f)
        return NULL;
    
    fseek(f, 0, SEEK_END);
    long n_bytes = ftell(f);
    
    BBVMInst * ibuff = malloc(n_bytes);
    
    fread(ibuff, 1, n_bytes, f);
    
    return BBVM_icreate(ibuff, n_bytes / sizeof(BBVMInst));
}

BBVirtualMachine * BBVM_bcreate(BBVMInstBuilder * builder) {
    BBVirtualMachine * VM = BBVM_icreate(builder->instructions, builder->n_instructions);
    VM->bb_tags = builder->bb_tags;
    VM->n_basic_blocks = builder->n_basic_blocks;
    memcpy(VM->globaldata, builder->globaldata, GLOBAL_SECTION_SIZE);
    VM->global_objects = builder->global_objects;
    VM->n_global_objects = builder->n_global_objects;
    VM->n_ffi_infos = builder->n_ffi_infos;
    VM->ffi_sym_infos = builder->ffi_sym_infos;
    return VM;
}

static void BBVM_mark_basic_blocks(BBVirtualMachine * VM) {
    VM->n_basic_blocks = 0;
}

static void BBVM_init(BBVirtualMachine * VM) {
    VM->PC = VM->instructions;
    BBVM_mark_basic_blocks(VM);
    VM->SP = &VM->stack[0];
    
    VM->n_stack_frames = 1;
    
    BBVMStackFrame * init_stack_frame = VM->cur_stack_frame = VM->stack_frames;
    init_stack_frame->BSP = VM->SP;
    init_stack_frame->return_addr = NULL;
    init_stack_frame->n_in_args = init_stack_frame->n_out_args = 0;
}

void BBVM_run(BBVirtualMachine * VM) {
    BBVM_init(VM);
    BBVMInterp_init();
    BBVMInterp_main_loop(VM);
}

void BBVM_push_stack_frame(BBVirtualMachine * VM, BBVMInst * return_addr, BBVMSSAValIdx ret_val_dest_SSA) {
    register BBVMStackFrame * cur_frame = VM->cur_stack_frame;
    register BBVMStackFrame * next_frame = cur_frame + 1;
    VM->n_stack_frames++;
    next_frame->BSP = VM->SP;
    next_frame->return_addr = return_addr;
    next_frame->args_in = cur_frame->args_out;
    next_frame->n_in_args = cur_frame->n_out_args;
    next_frame->n_out_args = 0;
    next_frame->ret_val_dest_SSA = ret_val_dest_SSA;
    VM->cur_stack_frame = next_frame;
}

void BBVM_pop_stack_frame(BBVirtualMachine * VM) {
    register BBVMStackFrame * cur_frame = VM->cur_stack_frame;
    VM->n_stack_frames--;
    VM->SP = cur_frame->BSP;
    VM->PC = cur_frame->return_addr;
    cur_frame--;
    cur_frame->n_out_args = 0;
    VM->cur_stack_frame = cur_frame;
}

uint8_t * BBVM_stack_alloc(BBVirtualMachine * VM, size_t size) {
    uint8_t * _SP = VM->SP;
    VM->SP += size;
    return _SP;
}


