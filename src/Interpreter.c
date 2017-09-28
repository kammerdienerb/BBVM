//
//  Interpreter.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/1/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "Interpreter.h"
#include "VM.h"
#include "BytecodeBuilder.h"
#include "FFI.h"

#include <stdlib.h>

BBVM_inst_handler * BBVM_handler_table = NULL;
#define BBVM_GET_HANDLER(OP) (BBVM_handler_table[(OP)])

#define BBVM_INST_HANDLER(name, ...) static void_fn_ptr BBVM_inst_handler_##name(BBVirtualMachine * VM, BBVMInst inst) __VA_ARGS__
#define BBVM_GET_HANDLER_REF(name) (BBVM_inst_handler_##name)

BBVM_INST_HANDLER(escape, {
    return (void_fn_ptr)BBVM_GET_HANDLER_REF(escape);
})

BBVM_INST_HANDLER(entry, {
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(alloc, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint32_t n_bytes = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = (uint64_t)BBVM_stack_alloc(VM, n_bytes);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(load, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t * SSA_val = (uint64_t*)f->SSA_vals[INST_GET_OP1_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = *SSA_val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fload, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    double * SSA_val = (double*)f->SSA_vals[INST_GET_OP1_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    double loaded = *SSA_val;
    
    f->SSA_vals[dest_idx] = *((uint64_t*)&loaded);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(store, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t * dest = (uint64_t*)f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t val = f->SSA_vals[INST_GET_OP2_IDX(inst)];

    *dest = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(storei, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t * dest = (uint64_t*)f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint32_t val = (uint32_t)INST_GET_IMMEDIATE(inst);
    
    *dest = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fstore, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t * dest = (uint64_t*)f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t val = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    
    *dest = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fstorei, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t * dest = (uint64_t*)f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint32_t val32 = (uint32_t)INST_GET_IMMEDIATE(inst);
    float valf = *((float*)&val32);
    double vald = valf;
    uint64_t val64 = *((uint64_t*)&vald);
    
    *dest = val64;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

static void_fn_ptr BBVM_inst_handler_getglobal(BBVirtualMachine * VM, BBVMInst inst) {
    BBVMStackFrame * f = VM->cur_stack_frame;
    
    BBVMGlobalObject * gobj = &VM->global_objects[INST_GET_IMMEDIATE(inst)];
    uint64_t ptr = (uint64_t)(VM->globaldata + gobj->GSP_off);
    
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = ptr;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
}

/*
BBVM_INST_HANDLER(getglobal, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    
    BBVMGlobalObject * gobj = &VM->global_objects[INST_GET_IMMEDIATE(inst)];
    uint64_t ptr = (uint64_t)(VM->globaldata + gobj->GSP_off);
    
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = ptr;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})
 */

static void_fn_ptr BBVM_inst_handler_arg(BBVirtualMachine * VM, BBVMInst inst) {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t val = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    
    f->args_out[f->n_out_args++] = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
}

/*
BBVM_INST_HANDLER(arg, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t val = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    
    f->args_out[f->n_out_args++] = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})
 */

BBVM_INST_HANDLER(argi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t val = INST_GET_IMMEDIATE(inst);
    
    f->args_out[f->n_out_args++] = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})


BBVM_INST_HANDLER(getarg, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t val = f->args_in[INST_GET_IMMEDIATE(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = val;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})



BBVM_INST_HANDLER(br, {
    BBVMBasicBlockIdx bb_idx = INST_GET_OP1_IDX(inst);
    BBVMInst * bb = VM->bb_tags[bb_idx].start;
    
    VM->PC = bb;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
})

BBVM_INST_HANDLER(brc, {
    uint64_t cond = VM->cur_stack_frame->SSA_vals[INST_GET_OP1_IDX(inst)];
    
    if (cond) {
        BBVMBasicBlockIdx bb_idx = INST_GET_OP2_IDX(inst);
        BBVMInst * bb = VM->bb_tags[bb_idx].start;
        
        VM->PC = bb;
        
        return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
    }
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(call, {
    int bb_idx = INST_GET_IMMEDIATE(inst);
    BBVMInst * bb = VM->bb_tags[bb_idx].start;
    
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    BBVM_push_stack_frame(VM, VM->PC + 1, dest_idx);
    VM->PC = bb;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
})

BBVM_INST_HANDLER(ffi_call, {
    BBVMStackFrame * f = BBVM_GET_CUR_STACK_FRAME(VM);
    
    BBVMFFISymEntry idx = INST_GET_IMMEDIATE(inst);
    BBVM_FFI_symbolinfo * info = VM->ffi_sym_infos[idx];
    
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    uint64_t * dest_addr = &(f->SSA_vals[dest_idx]);
    
    // f->args_in = f->args_out;
    // f->n_in_args = f->n_out_args;
    
    BBVM_FFI_call(VM, info, dest_addr);
    
    f->n_out_args = 0;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})
 
BBVM_INST_HANDLER(vret, {
    BBVM_pop_stack_frame(VM);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
})

static void_fn_ptr BBVM_inst_handler_ret(BBVirtualMachine * VM, BBVMInst inst) {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t val = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    (f - 1)->SSA_vals[f->ret_val_dest_SSA] = val;
    
    BBVM_pop_stack_frame(VM);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
}


BBVM_INST_HANDLER(reti, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t val = INST_GET_IMMEDIATE(inst);
    f->SSA_vals[f->ret_val_dest_SSA] = val;
    
    BBVM_pop_stack_frame(VM);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
})

BBVM_INST_HANDLER(lss, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l < r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(lssi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l < r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(leq, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l <= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(leqi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l <= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(gtr, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l > r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(gtri, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l > r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(geq, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l >= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(geqi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l >= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(equ, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l == r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(equi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l == r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(neq, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l != r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(neqi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l != r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})


BBVM_INST_HANDLER(add, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l + r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(addi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l + r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(sub, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l - r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(subi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l - r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(mul, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l * r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(muli, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l * r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(div, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l / r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(divi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l / r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(mod, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l % r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(modi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l % r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(and, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l & r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(andi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l & r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(or, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = f->SSA_vals[INST_GET_OP2_IDX(inst)];
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l | r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(ori, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint64_t l = f->SSA_vals[INST_GET_OP1_IDX(inst)];
    uint64_t r = INST_GET_IMMEDIATE(inst);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l | r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})



BBVM_INST_HANDLER(flss, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
    f->SSA_vals[dest_idx] = l < r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(flssi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l < r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fleq, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
    f->SSA_vals[dest_idx] = l <= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fleqi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l <= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fgtr, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
    f->SSA_vals[dest_idx] = l > r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fgtri, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l > r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fgeq, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
    f->SSA_vals[dest_idx] = l >= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fgeqi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l >= r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fequ, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
    f->SSA_vals[dest_idx] = l == r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fequi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l == r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fneq, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
    f->SSA_vals[dest_idx] = l != r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fneqi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
    f->SSA_vals[dest_idx] = l != r;
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fadd, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
	double result = l + r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(faddi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
	double result = l + r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fsub, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
	double result = l - r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fsubi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
	double result = l - r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fmul, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
	double result = l * r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fmuli, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
	double result = l * r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fdiv, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
	double r = *((double*)&f->SSA_vals[INST_GET_OP2_IDX(inst)]);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
   
	double result = l / r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fdivi, {
    BBVMStackFrame * f = VM->cur_stack_frame;
	double l = *((double*)&f->SSA_vals[INST_GET_OP1_IDX(inst)]);
    uint32_t r_bits = INST_GET_IMMEDIATE(inst);
	double r = *((float*)&r_bits);
    BBVMSSAValIdx dest_idx = INST_GET_DEST_IDX(inst);
    
	double result = l / r;
    f->SSA_vals[dest_idx] = *((uint64_t*)&result);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})



BBVM_INST_HANDLER(print, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint32_t idx = INST_GET_OP1_IDX(inst);
    uint64_t val = f->SSA_vals[idx];
    
    printf("%lld\n", val);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})

BBVM_INST_HANDLER(fprint, {
    BBVMStackFrame * f = VM->cur_stack_frame;
    uint32_t idx = INST_GET_OP1_IDX(inst);
    uint64_t val64 = f->SSA_vals[idx];
    double vald = *((double*)&val64);
    
    printf("%f\n", vald);
    
    return (void_fn_ptr)BBVM_GET_HANDLER(INST_GET_OPCODE(*(++VM->PC)));
})


void BBVMInterp_init() {
    BBVM_handler_table = malloc(UINT8_MAX * sizeof(BBVM_inst_handler));
    
    int i;
    for (i = 0; i < UINT8_MAX; i += 1)
        BBVM_handler_table[i] = BBVM_GET_HANDLER_REF(escape);
    
    BBVM_handler_table[OP_ENTRY] = BBVM_GET_HANDLER_REF(entry);
    /* BBVM_handler_table[OP_ESCAPE] = escape_handler; */
    
    BBVM_handler_table[OP_ALLOC] = BBVM_GET_HANDLER_REF(alloc);
    BBVM_handler_table[OP_LOAD] = BBVM_GET_HANDLER_REF(load);
    BBVM_handler_table[OP_FLOAD] = BBVM_GET_HANDLER_REF(fload);
    BBVM_handler_table[OP_STORE] = BBVM_GET_HANDLER_REF(store);
    BBVM_handler_table[OP_STOREI] = BBVM_GET_HANDLER_REF(storei);
    BBVM_handler_table[OP_FSTORE] = BBVM_GET_HANDLER_REF(fstore);
    BBVM_handler_table[OP_FSTOREI] = BBVM_GET_HANDLER_REF(fstorei);
    BBVM_handler_table[OP_GETGLOBAL] = BBVM_GET_HANDLER_REF(getglobal);
    BBVM_handler_table[OP_ARG] = BBVM_GET_HANDLER_REF(arg);
    BBVM_handler_table[OP_ARGI] = BBVM_GET_HANDLER_REF(argi);
    BBVM_handler_table[OP_GETARG] = BBVM_GET_HANDLER_REF(getarg);
    
    
    BBVM_handler_table[OP_BR] = BBVM_GET_HANDLER_REF(br);
    BBVM_handler_table[OP_BRC] = BBVM_GET_HANDLER_REF(brc);
    BBVM_handler_table[OP_CALL] = BBVM_GET_HANDLER_REF(call);
    /*BBVM_handler_table[OP_CALLI] = entry_handler;*/
    BBVM_handler_table[OP_FFI_CALL] = BBVM_GET_HANDLER_REF(ffi_call);
    BBVM_handler_table[OP_VRET] = BBVM_GET_HANDLER_REF(vret);
    BBVM_handler_table[OP_RET] = BBVM_GET_HANDLER_REF(ret);
    BBVM_handler_table[OP_RETI] = BBVM_GET_HANDLER_REF(reti);
    
    
    BBVM_handler_table[OP_LSS] = BBVM_GET_HANDLER_REF(lss);
    BBVM_handler_table[OP_LSSI] = BBVM_GET_HANDLER_REF(lssi);
    BBVM_handler_table[OP_LEQ] = BBVM_GET_HANDLER_REF(leq);
    BBVM_handler_table[OP_LEQI] = BBVM_GET_HANDLER_REF(leqi);
    BBVM_handler_table[OP_GTR] = BBVM_GET_HANDLER_REF(gtr);
    BBVM_handler_table[OP_GTRI] = BBVM_GET_HANDLER_REF(gtri);
    BBVM_handler_table[OP_GEQ] = BBVM_GET_HANDLER_REF(geq);
    BBVM_handler_table[OP_GEQI] = BBVM_GET_HANDLER_REF(geqi);
    BBVM_handler_table[OP_EQU] = BBVM_GET_HANDLER_REF(equ);
    BBVM_handler_table[OP_EQUI] = BBVM_GET_HANDLER_REF(equi);
    BBVM_handler_table[OP_NEQ] = BBVM_GET_HANDLER_REF(neq);
    BBVM_handler_table[OP_NEQI] = BBVM_GET_HANDLER_REF(neqi);
    
    BBVM_handler_table[OP_ADD] = BBVM_GET_HANDLER_REF(add);
    BBVM_handler_table[OP_ADDI] = BBVM_GET_HANDLER_REF(addi);
    BBVM_handler_table[OP_SUB] = BBVM_GET_HANDLER_REF(sub);
    BBVM_handler_table[OP_SUBI] = BBVM_GET_HANDLER_REF(subi);
    BBVM_handler_table[OP_MUL] = BBVM_GET_HANDLER_REF(mul);
    BBVM_handler_table[OP_MULI] = BBVM_GET_HANDLER_REF(muli);
    BBVM_handler_table[OP_DIV] = BBVM_GET_HANDLER_REF(div);
    BBVM_handler_table[OP_DIVI] = BBVM_GET_HANDLER_REF(divi);
    BBVM_handler_table[OP_MOD] = BBVM_GET_HANDLER_REF(mod);
    BBVM_handler_table[OP_MODI] = BBVM_GET_HANDLER_REF(modi);
    BBVM_handler_table[OP_AND] = BBVM_GET_HANDLER_REF(and);
    BBVM_handler_table[OP_ANDI] = BBVM_GET_HANDLER_REF(andi);
    BBVM_handler_table[OP_OR] = BBVM_GET_HANDLER_REF(or);
    BBVM_handler_table[OP_ORI] = BBVM_GET_HANDLER_REF(ori);
   
	BBVM_handler_table[OP_FLSS] = BBVM_GET_HANDLER_REF(flss);
    BBVM_handler_table[OP_FLSSI] = BBVM_GET_HANDLER_REF(flssi);
    BBVM_handler_table[OP_FLEQ] = BBVM_GET_HANDLER_REF(fleq);
    BBVM_handler_table[OP_FLEQI] = BBVM_GET_HANDLER_REF(fleqi);
    BBVM_handler_table[OP_FGTR] = BBVM_GET_HANDLER_REF(fgtr);
    BBVM_handler_table[OP_FGTRI] = BBVM_GET_HANDLER_REF(fgtri);
    BBVM_handler_table[OP_FGEQ] = BBVM_GET_HANDLER_REF(fgeq);
    BBVM_handler_table[OP_FGEQI] = BBVM_GET_HANDLER_REF(fgeqi);
    BBVM_handler_table[OP_FEQU] = BBVM_GET_HANDLER_REF(fequ);
    BBVM_handler_table[OP_FEQUI] = BBVM_GET_HANDLER_REF(fequi);
    BBVM_handler_table[OP_FNEQ] = BBVM_GET_HANDLER_REF(fneq);
    BBVM_handler_table[OP_FNEQI] = BBVM_GET_HANDLER_REF(fneqi);
    
    BBVM_handler_table[OP_FADD] = BBVM_GET_HANDLER_REF(fadd);
    BBVM_handler_table[OP_FADDI] = BBVM_GET_HANDLER_REF(faddi);
    BBVM_handler_table[OP_FSUB] = BBVM_GET_HANDLER_REF(fsub);
    BBVM_handler_table[OP_FSUBI] = BBVM_GET_HANDLER_REF(fsubi);
    BBVM_handler_table[OP_FMUL] = BBVM_GET_HANDLER_REF(fmul);
    BBVM_handler_table[OP_FMULI] = BBVM_GET_HANDLER_REF(fmuli);
    BBVM_handler_table[OP_FDIV] = BBVM_GET_HANDLER_REF(fdiv);
    BBVM_handler_table[OP_FDIVI] = BBVM_GET_HANDLER_REF(fdivi);
	
    BBVM_handler_table[OP_PRINT] = BBVM_GET_HANDLER_REF(print);
    BBVM_handler_table[OP_FPRINT] = BBVM_GET_HANDLER_REF(fprint);
}

void BBVMInterp_main_loop(BBVirtualMachine * VM) {
    register BBVM_inst_handler handler = BBVM_GET_HANDLER(INST_GET_OPCODE(*VM->PC));
    
    while (handler != BBVM_GET_HANDLER_REF(escape)) {
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
        handler = (BBVM_inst_handler)handler(VM, *VM->PC);
    }
}
