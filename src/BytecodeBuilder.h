//
//  BytecodeBuilder.h
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/3/17.
//  Copyright © 2017 me. All rights reserved.
//

#ifndef BytecodeBuilder_h
#define BytecodeBuilder_h

#include "Bytecode.h"
#include "BasicBlock.h"
#include "VM.h"
#include "FFI.h"

typedef struct BBVMGlobalObject {
    off_t GSP_off;
    size_t bytes;
} BBVMGlobalObject;

typedef struct BBVMBasicBlockTag {
    BBVMInst * start;
    size_t n_instructions;
    char * name;
} BBVMBasicBlockTag;

typedef struct BBVMInstBuilder {
    BBVMInst * instructions;
    BBVMBasicBlock ** basic_blocks, * current_basic_block;
    BBVMBasicBlockTag * bb_tags;
    uint8_t globaldata[GLOBAL_SECTION_SIZE], * GSP;
    BBVMGlobalObject * global_objects;
    BBVM_FFI_symbolinfo ** ffi_sym_infos;
    size_t n_basic_blocks, _bb_capacity, n_instructions, n_global_objects, _globals_capacity, n_ffi_infos, _ffi_info_capacity;
    BBVMSSAValIdx SSA_val_tracker;
} BBVMInstBuilder;

BBVMInstBuilder * BBVMInstBuilder_create(void);
void BBVMInstBuilder_finalize(BBVMInstBuilder * builder);

BBVMGlobalValueIdx BBVMInstBuilder_add_global_value(BBVMInstBuilder * builder, size_t bytes, void * data);
BBVMGlobalValueIdx BBVMInstBuilder_add_global_string(BBVMInstBuilder * builder, const char * str);


BBVMFFISymEntry BBVMInstBuilder_get_ffi_symbol(BBVMInstBuilder * builder, const char * symbol);
BBVMFFISymEntry BBVMInstBuilder_add_ffi_symbol(BBVMInstBuilder * builder, BBVM_FFI_symbolinfo * sym_info);

BBVMBasicBlockIdx BBVMInstBuilder_begin_new_basic_block(BBVMInstBuilder * builder, const char * name);
void BBVMInstBuilder_set_current_basic_block(BBVMInstBuilder * builder, BBVMBasicBlockIdx id);


void BBVMInstBuilder_push_escape_inst(BBVMInstBuilder * builder);
BBVMSSAValIdx BBVMInstBuilder_push_alloc_inst(BBVMInstBuilder * builder, size_t n_bytes);
BBVMSSAValIdx BBVMInstBuilder_push_load_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx);
BBVMSSAValIdx BBVMInstBuilder_push_fload_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx);
void BBVMInstBuilder_push_store_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val, BBVMSSAValIdx SSA_idx_2_dest);
void BBVMInstBuilder_push_storei_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_2_dest, uint32_t val);
void BBVMInstBuilder_push_fstore_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val, BBVMSSAValIdx SSA_idx_2_dest);
void BBVMInstBuilder_push_fstorei_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_2_dest, float val);
BBVMSSAValIdx BBVMInstBuilder_push_getglobal_inst(BBVMInstBuilder * builder, BBVMGlobalValueIdx idx);
void BBVMInstBuilder_push_arg_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val);
void BBVMInstBuilder_push_argi_inst(BBVMInstBuilder * builder, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_getarg_inst(BBVMInstBuilder * builder, uint32_t val);


void BBVMInstBuilder_push_br_inst(BBVMInstBuilder * builder, BBVMBasicBlockIdx bb_idx);
void BBVMInstBuilder_push_brc_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx, BBVMBasicBlockIdx bb_idx);
BBVMSSAValIdx BBVMInstBuilder_push_call_inst(BBVMInstBuilder * builder, BBVMBasicBlockIdx bb_idx);
void BBVMInstBuilder_push_vret_inst(BBVMInstBuilder * builder);
void BBVMInstBuilder_push_ret_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx);
void BBVMInstBuilder_push_reti_inst(BBVMInstBuilder * builder, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_ffi_call_inst(BBVMInstBuilder * builder, const char * symbol);



BBVMSSAValIdx BBVMInstBuilder_push_lss_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_lssi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_leq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_leqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_gtr_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_gtri_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_geq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_geqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_equ_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_equi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_neq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_neqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);

BBVMSSAValIdx BBVMInstBuilder_push_add_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_addi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_sub_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_subi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_mul_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_muli_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_div_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_divi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_mod_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_modi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_and_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_andi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);
BBVMSSAValIdx BBVMInstBuilder_push_or_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_ori_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, uint32_t val);

BBVMSSAValIdx BBVMInstBuilder_push_flss_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_flssi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fleq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fleqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fgtr_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fgtri_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fgeq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fgeqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fequ_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fequi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fneq_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fneqi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);

BBVMSSAValIdx BBVMInstBuilder_push_fadd_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_faddi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fsub_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fsubi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fmul_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fmuli_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);
BBVMSSAValIdx BBVMInstBuilder_push_fdiv_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, BBVMSSAValIdx SSA_idx_2);
BBVMSSAValIdx BBVMInstBuilder_push_fdivi_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1, float val);

void BBVMInstBuilder_push_print_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val);
void BBVMInstBuilder_push_fprint_inst(BBVMInstBuilder * builder, BBVMSSAValIdx SSA_idx_1_val);


void BBVMInstBuilder_dump_instructions(BBVMInstBuilder * builder);




#endif /* BytecodeBuilder_h */
