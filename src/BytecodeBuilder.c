//
//  BytecodeBuilder.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/3/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "BytecodeBuilder.h"


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BBVM_INST_BUILDER_INIT_BB_CAPACITY          (32)
#define BBVM_INST_BUILDER_INIT_FFI_SYMS_CAPACITY    (8)
#define BBVM_INST_BUILDER_INIT_GLOBALS_CAPACITY     (32)


#define BB_TAGGED_IS_FN(ptr)    (BBVMInst*)((uintptr_t)(ptr) | 0x01)
#define BB_UNTAGGED_IS_FN(ptr)  (BBVMInst*)((uintptr_t)(ptr) | 0x00)
#define BB_IS_FN(ptr)           (BBVMInst*)((uintptr_t)(ptr) & 0x01)


BBVMInstBuilder * BBVMInstBuilder_create() {
    BBVMInstBuilder * builder = malloc(sizeof(BBVMInstBuilder));

    builder->instructions = NULL;
    builder->n_instructions = 0;
    
    builder->_bb_capacity = BBVM_INST_BUILDER_INIT_BB_CAPACITY;
    builder->basic_blocks = malloc(BBVM_INST_BUILDER_INIT_BB_CAPACITY * sizeof(BBVMBasicBlock*));
    builder->n_basic_blocks = 0;
    builder->_ffi_info_capacity = BBVM_INST_BUILDER_INIT_FFI_SYMS_CAPACITY;
    builder->ffi_sym_infos = malloc(BBVM_INST_BUILDER_INIT_FFI_SYMS_CAPACITY * sizeof(BBVM_FFI_symbolinfo*));
    builder->n_ffi_infos = 0;
    builder->_globals_capacity = BBVM_INST_BUILDER_INIT_GLOBALS_CAPACITY;
    builder->global_objects = malloc(BBVM_INST_BUILDER_INIT_GLOBALS_CAPACITY * sizeof(BBVMGlobalObject));
    builder->n_global_objects = 0;
    builder->GSP = builder->globaldata;
    builder->current_basic_block = NULL;
    builder->SSA_val_tracker = 0;
    
    return builder;
}

void BBVMInstBuilder_finalize(BBVMInstBuilder * builder) {
    int i;
    BBVMInst * insert = NULL;
    for (i = 0; i < builder->n_basic_blocks; i += 1)
        builder->n_instructions += builder->basic_blocks[i]->n_instructions;
    builder->instructions = malloc(builder->n_instructions * sizeof(BBVMInst));
    insert = builder->instructions;
    builder->bb_tags = malloc(builder->n_basic_blocks * sizeof(BBVMBasicBlockTag));
    for (i = 0; i < builder->n_basic_blocks; i += 1) {
        BBVMBasicBlock * bb = builder->basic_blocks[i];
        BBVMBasicBlockTag * tag = &builder->bb_tags[i];
        
        tag->start = insert;
        tag->n_instructions = bb->n_instructions;
        tag->name = malloc(strlen(bb->name));
        strcpy(tag->name, bb->name);
        
        memcpy(insert, bb->instructions, bb->n_instructions * sizeof(BBVMInst));
        insert += bb->n_instructions;
        BBVMBasicBlock_destroy(bb);
    }
}

static void BBVMInstBuilder_grow_global_values(BBVMInstBuilder * builder) {
    builder->_globals_capacity *= 2;
    builder->global_objects = realloc(builder->global_objects, builder->_globals_capacity * sizeof(BBVMGlobalObject));
}

BBVMGlobalValueIdx BBVMInstBuilder_add_global_value(BBVMInstBuilder * builder, size_t bytes, void * data) {
    if (builder->n_global_objects == builder->_globals_capacity)
        BBVMInstBuilder_grow_global_values(builder);
    
    BBVMGlobalValueIdx i = (BBVMGlobalValueIdx)builder->n_global_objects++;
    
    BBVMGlobalObject * gobj = &builder->global_objects[i];
    gobj->bytes = bytes;
    gobj->GSP_off = builder->GSP - builder->globaldata;
    
    memcpy(builder->GSP, data, bytes);
    
    builder->GSP += bytes;
    
    return i;
}

BBVMGlobalValueIdx BBVMInstBuilder_add_global_string(BBVMInstBuilder * builder, const char * str) {
    size_t bytes = strlen(str) + 1;
    return BBVMInstBuilder_add_global_value(builder, bytes, (void*)str);
}

BBVMFFISymEntry BBVMInstBuilder_get_ffi_symbol(BBVMInstBuilder * builder, const char * symbol) {
    unsigned int i;
    for (i = 0; i < builder->n_ffi_infos; i += 1) {
        if (strcmp(symbol, builder->ffi_sym_infos[i]->symbol) == 0)
            return i;
    }
    return -1;
}

static void BBVMInstBuilder_grow_ffi_symbols(BBVMInstBuilder * builder) {
    builder->_ffi_info_capacity *= 2;
    builder->ffi_sym_infos = realloc(builder->ffi_sym_infos, builder->_ffi_info_capacity * sizeof(const char *));
}

BBVMFFISymEntry BBVMInstBuilder_add_ffi_symbol(BBVMInstBuilder * builder, BBVM_FFI_symbolinfo * info) {
    BBVMFFISymEntry i;
    i = BBVMInstBuilder_get_ffi_symbol(builder, info->symbol);
    if (i != -1) {
        BBVM_FFI_destroy_symbolinfo(builder->ffi_sym_infos[i]);
        builder->ffi_sym_infos[i] = info;
        return i;
    }
    if (builder->n_ffi_infos == builder->_ffi_info_capacity)
        BBVMInstBuilder_grow_ffi_symbols(builder);
    i = (BBVMFFISymEntry)builder->n_ffi_infos++;
    builder->ffi_sym_infos[i] = info;
    return i;
}

void BBVMInstBuilder_set_current_basic_block(BBVMInstBuilder * builder, BBVMBasicBlockIdx id) {
    builder->current_basic_block = builder->basic_blocks[id];
}

static void BBVMInstBuilder_grow_basic_blocks(BBVMInstBuilder * builder) {
    builder->_bb_capacity *= 2;
    builder->basic_blocks = realloc(builder->basic_blocks, builder->_bb_capacity * sizeof(BBVMBasicBlock*));
}

static BBVMBasicBlockIdx BBVMInstBuilder_push_basic_block(BBVMInstBuilder * builder, BBVMBasicBlock * bb) {
    if (builder->n_basic_blocks == builder->_bb_capacity)
        BBVMInstBuilder_grow_basic_blocks(builder);
    builder->basic_blocks[builder->n_basic_blocks++] = bb;
    return (BBVMBasicBlockIdx)builder->n_basic_blocks - 1;
}

BBVMBasicBlockIdx BBVMInstBuilder_begin_new_basic_block(BBVMInstBuilder * builder, const char * name) {
    BBVMBasicBlock * bb = BBVMBasicBlock_create(name);
    BBVMInstBuilder_push_basic_block(builder, bb);
    BBVMBasicBlockIdx id = builder->n_basic_blocks - 1;
    BBVMInstBuilder_set_current_basic_block(builder, id);
    
    return id;
}

void BBVMInstBuilder_push_escape_inst(BBVMInstBuilder * builder) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_ESCAPE);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

BBVMSSAValIdx BBVMInstBuilder_push_alloc_inst(BBVMInstBuilder * builder, size_t n_bytes) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_ALLOC);
    INST_SET_IMMEDIATE(inst, n_bytes);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}

BBVMSSAValIdx BBVMInstBuilder_push_load_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_LOAD);
    INST_SET_OP1_IDX(inst, SSA_idx);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}

BBVMSSAValIdx BBVMInstBuilder_push_fload_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_FLOAD);
    INST_SET_OP1_IDX(inst, SSA_idx);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}


void BBVMInstBuilder_push_store_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_dest, BBVMSSAValIdx SSA_idx_2_val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_STORE);
    INST_SET_OP1_IDX(inst, SSA_idx_1_dest);
    INST_SET_OP2_IDX(inst, SSA_idx_2_val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}


void BBVMInstBuilder_push_storei_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_dest, uint32_t val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_STOREI);
    INST_SET_OP1_IDX(inst, SSA_idx_1_dest);
    INST_SET_IMMEDIATE(inst, val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

void BBVMInstBuilder_push_fstore_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_dest, BBVMSSAValIdx SSA_idx_2_val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_FSTORE);
    INST_SET_OP1_IDX(inst, SSA_idx_1_dest);
    INST_SET_OP2_IDX(inst, SSA_idx_2_val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}


void BBVMInstBuilder_push_fstorei_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_dest, float val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_FSTOREI);
    INST_SET_OP1_IDX(inst, SSA_idx_1_dest);
    INST_SET_IMMEDIATE(inst, *((uint32_t*)&val));
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

BBVMSSAValIdx BBVMInstBuilder_push_getglobal_inst(BBVMInstBuilder * builder, BBVMGlobalValueIdx idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_GETGLOBAL);
    INST_SET_IMMEDIATE(inst, idx);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    return dest_idx;
}

void BBVMInstBuilder_push_arg_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_ARG);
    INST_SET_OP1_IDX(inst, SSA_idx_1_val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}


void BBVMInstBuilder_push_argi_inst(BBVMInstBuilder * builder, uint32_t val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_ARGI);
    INST_SET_IMMEDIATE(inst, val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

BBVMSSAValIdx BBVMInstBuilder_push_getarg_inst(BBVMInstBuilder * builder, uint32_t val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_GETARG);
    INST_SET_IMMEDIATE(inst, val);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    return dest_idx;
}




void BBVMInstBuilder_push_br_inst(BBVMInstBuilder * builder, BBVMBasicBlockIdx bb_idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_BR);
    INST_SET_OP1_IDX(inst, bb_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

void BBVMInstBuilder_push_brc_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx, BBVMBasicBlockIdx bb_idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_BRC);
    INST_SET_OP1_IDX(inst, SSA_idx);
    INST_SET_OP2_IDX(inst, bb_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}




BBVMSSAValIdx BBVMInstBuilder_push_call_inst(BBVMInstBuilder * builder, BBVMBasicBlockIdx bb_idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_CALL);
    INST_SET_IMMEDIATE(inst, bb_idx);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}

void BBVMInstBuilder_push_vret_inst(BBVMInstBuilder * builder) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_VRET);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

void BBVMInstBuilder_push_ret_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_RET);
    INST_SET_OP1_IDX(inst, SSA_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

void BBVMInstBuilder_push_reti_inst(BBVMInstBuilder * builder, uint32_t val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_RET);
    INST_SET_IMMEDIATE(inst, val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

BBVMSSAValIdx BBVMInstBuilder_push_ffi_call_inst(BBVMInstBuilder * builder, const char * symbol) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_FFI_CALL);
    BBVMFFISymEntry idx = BBVMInstBuilder_get_ffi_symbol(builder, symbol);
    assert(idx != -1 && "FFI symbol info not found!");
    INST_SET_IMMEDIATE(inst, idx);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    return dest_idx;
}




static BBVMSSAValIdx BBVMInstBuilder_push_binary_inst(BBVMInstBuilder * builder, uint8_t opcode, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, opcode);
    INST_SET_OP1_IDX(inst, SSA_idx_1);
    INST_SET_OP2_IDX(inst, SSA_idx_2);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}

static BBVMSSAValIdx BBVMInstBuilder_push_binaryi_inst(BBVMInstBuilder * builder, uint8_t opcode, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, opcode);
    INST_SET_OP1_IDX(inst, SSA_idx_1);
    INST_SET_IMMEDIATE(inst, val);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}

BBVMSSAValIdx BBVMInstBuilder_push_lss_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_LSS, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_lssi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_LSSI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_leq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_LEQ, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_leqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_LEQI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_gtr_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_GTR, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_gtri_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_GTRI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_geq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_GEQ, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_geqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_GEQI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_equ_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_EQU, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_equi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_EQUI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_neq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_NEQ, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_neqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_NEQI, SSA_idx_1, val);
}






BBVMSSAValIdx BBVMInstBuilder_push_add_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_ADD, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_addi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_ADDI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_sub_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_SUB, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_subi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_SUBI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_mul_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_MUL, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_muli_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_MULI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_div_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_DIV, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_divi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_DIVI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_mod_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_MOD, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_modi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_MODI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_and_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_AND, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_andi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_ANDI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_or_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_binary_inst(builder, OP_OR, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_ori_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val) {
    return BBVMInstBuilder_push_binaryi_inst(builder, OP_ORI, SSA_idx_1, val);
}





static BBVMSSAValIdx BBVMInstBuilder_push_fbinary_inst(BBVMInstBuilder * builder, uint8_t opcode, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, opcode);
    INST_SET_OP1_IDX(inst, SSA_idx_1);
    INST_SET_OP2_IDX(inst, SSA_idx_2);
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}

static BBVMSSAValIdx BBVMInstBuilder_push_fbinaryi_inst(BBVMInstBuilder * builder, uint8_t opcode, BBVMSSAValIdx SSA_idx_1, float val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, opcode);
    INST_SET_OP1_IDX(inst, SSA_idx_1);
    INST_SET_IMMEDIATE(inst, *((uint32_t*)(&val)));
    BBVMSSAValIdx dest_idx = builder->SSA_val_tracker++;
    INST_SET_DEST_IDX(inst, dest_idx);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
    
    return dest_idx;
}


BBVMSSAValIdx BBVMInstBuilder_push_flss_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FLSS, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_flssi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FLSSI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fleq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FLEQ, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fleqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FLEQI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fgtr_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FGTR, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fgtri_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FGTRI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fgeq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FGEQ, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fgeqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FGEQI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fequ_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FEQU, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fequi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FEQUI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fneq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FNEQ, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fneqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FNEQI, SSA_idx_1, val);
}




BBVMSSAValIdx BBVMInstBuilder_push_fadd_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FADD, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_faddi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FADDI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fsub_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FSUB, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fsubi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FSUBI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fmul_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FMUL, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fmuli_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FMULI, SSA_idx_1, val);
}

BBVMSSAValIdx BBVMInstBuilder_push_fdiv_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2) {
    return BBVMInstBuilder_push_fbinary_inst(builder, OP_FDIV, SSA_idx_1, SSA_idx_2);
}

BBVMSSAValIdx BBVMInstBuilder_push_fdivi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val) {
    return BBVMInstBuilder_push_fbinaryi_inst(builder, OP_FDIVI, SSA_idx_1, val);
}


void BBVMInstBuilder_push_print_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_PRINT);
    INST_SET_OP1_IDX(inst, SSA_idx_1_val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

void BBVMInstBuilder_push_fprint_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val) {
    BBVMInst inst = 0;
    INST_SET_OPCODE(inst, OP_FPRINT);
    INST_SET_OP1_IDX(inst, SSA_idx_1_val);
    BBVMBasicBlock_push_instruction(builder->current_basic_block, inst);
}

void BBVMInstBuilder_dump_instructions(BBVMInstBuilder * builder) {
    int i, j;
    
    for (i = 0; i < builder->n_global_objects; i += 1)
        printf("GLOBAL\t%d:\tGSP + %llx\n", i, builder->global_objects[i].GSP_off);
    
    printf("\n");
    
    for (i = 0; i < builder->n_ffi_infos; i += 1) {
        BBVM_FFI_symbolinfo * info = builder->ffi_sym_infos[i];
        printf("FFI\t%s\n", info->symbol);
    }
    
    printf("\n");
    
    for (i = 0; i < builder->n_basic_blocks; i += 1) {
        BBVMBasicBlockTag * tag = &builder->bb_tags[i];
        BBVMInst * inst;
        
        printf("%s:\n", tag->name);
        
        for (j = 0, inst = tag->start; j < tag->n_instructions; j++, inst++){
            BBVMInst_dump(*inst, builder->bb_tags, builder->ffi_sym_infos);
        }
    }
}


