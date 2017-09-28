//
//  VM.h
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/1/17.
//  Copyright © 2017 me. All rights reserved.
//

#ifndef VM_h
#define VM_h

#include "Bytecode.h"

#include <stdio.h>

#define KB(size)            ((size) * 1024)
#define GLOBAL_SECTION_SIZE KB(512)
#define STACK_FRAME_LIMIT   (4096)
#define BBVM_STACK_SIZE     (STACK_FRAME_LIMIT * KB(32))
#define MAX_SSA_VALS        (256)
#define MAX_ARG_VALS        (32)

#define BBVM_GET_CUR_STACK_FRAME(VM) (&VM->stack_frames[VM->n_stack_frames - 1])

typedef uint8_t BBVMSSAValIdx;
typedef unsigned int BBVMFFISymEntry;
typedef unsigned int BBVMGlobalValueIdx;
struct BBVMGlobalObject;
struct BBVMBasicBlockTag;
struct BBVMInstBuilder;
struct BBVM_FFI_symbolinfo;


typedef struct BBVMStackFrame {
    uint8_t * BSP;
    BBVMInst * return_addr;
    uint64_t SSA_vals[MAX_SSA_VALS];
    const uint64_t * args_in;
    uint64_t args_out[MAX_ARG_VALS];
    size_t n_in_args, n_out_args;
    BBVMSSAValIdx ret_val_dest_SSA;
} BBVMStackFrame;

typedef struct BBVirtualMachine {
    BBVMInst * instructions, * PC;
    struct BBVMBasicBlockTag * bb_tags;
    uint8_t globaldata[GLOBAL_SECTION_SIZE];
    struct BBVMGlobalObject * global_objects;
    BBVMStackFrame stack_frames[STACK_FRAME_LIMIT], * cur_stack_frame;
    uint8_t stack[BBVM_STACK_SIZE], * SP;
    struct BBVM_FFI_symbolinfo ** ffi_sym_infos;
    size_t n_instructions, n_basic_blocks, n_stack_frames, n_global_objects, n_ffi_infos;
} BBVirtualMachine;

BBVirtualMachine * BBVM_icreate(BBVMInst * instructions, size_t n_instructions);
BBVirtualMachine * BBVM_fcreate(FILE * f);

BBVirtualMachine * BBVM_bcreate(struct BBVMInstBuilder * builder);
void BBVM_run(BBVirtualMachine * VM);

void BBVM_push_stack_frame(BBVirtualMachine * VM, BBVMInst * return_addr, BBVMSSAValIdx ret_val_dest_SSA);
void BBVM_pop_stack_frame(BBVirtualMachine * VM);
uint8_t * BBVM_stack_alloc(BBVirtualMachine * VM, size_t size);

#endif /* VM_h */
