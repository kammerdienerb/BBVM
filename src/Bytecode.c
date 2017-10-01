//
//  Bytecode.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 8/30/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "Bytecode.h"
#include "BytecodeBuilder.h"

#include <stdio.h>
#include <stdlib.h>

const char ** inst_names_table = NULL;

void BBVMInst_dump_init() {
    inst_names_table = malloc(UINT8_MAX * sizeof(const char *));
    
    inst_names_table[OP_ENTRY   ] = "entry";
    inst_names_table[OP_ESCAPE  ] = "escape";
    
    inst_names_table[OP_ALLOC   ] = "alloc";
    inst_names_table[OP_LOAD    ] = "load";
    inst_names_table[OP_FLOAD   ] = "fload";
    inst_names_table[OP_STORE   ] = "store";
    inst_names_table[OP_STOREI  ] = "storei";
    inst_names_table[OP_FSTORE  ] = "fstore";
    inst_names_table[OP_FSTOREI ] = "fstorei";
    inst_names_table[OP_GETGLOBAL] = "getglobal";
    inst_names_table[OP_ARG     ] = "arg";
    inst_names_table[OP_ARGI    ] = "argi";
    inst_names_table[OP_GETARG  ] = "getarg";
    
    
    inst_names_table[OP_BR      ] = "br";
    inst_names_table[OP_BRC     ] = "brc";
    inst_names_table[OP_CALL    ] = "call";
    inst_names_table[OP_CALLI   ] = "calli";
    inst_names_table[OP_FFI_CALL]=  "fficall";
    inst_names_table[OP_VRET    ] = "vret";
    inst_names_table[OP_RET     ] = "ret";
    inst_names_table[OP_RETI    ] = "reti";
    
    
    inst_names_table[OP_LSS     ] = "lss";
    inst_names_table[OP_LSSI    ] = "lssi";
    inst_names_table[OP_LEQ     ] = "leq";
    inst_names_table[OP_LEQI    ] = "leqi";
    inst_names_table[OP_GTR     ] = "gtr";
    inst_names_table[OP_GTRI    ] = "gtri";
    inst_names_table[OP_GEQ     ] = "geq";
    inst_names_table[OP_GEQI    ] = "geqi";
    inst_names_table[OP_EQU     ] = "equ";
    inst_names_table[OP_EQUI    ] = "equi";
    inst_names_table[OP_NEQ     ] = "neq";
    inst_names_table[OP_NEQI    ] = "neqi";
    
    inst_names_table[OP_ADD     ] = "add";
    inst_names_table[OP_ADDI    ] = "addi";
    inst_names_table[OP_SUB     ] = "sub";
    inst_names_table[OP_SUBI    ] = "subi";
    inst_names_table[OP_MUL     ] = "mul";
    inst_names_table[OP_MULI    ] = "muli";
    inst_names_table[OP_DIV     ] = "div";
    inst_names_table[OP_DIVI    ] = "divi";
    inst_names_table[OP_MOD     ] = "mod";
    inst_names_table[OP_MODI    ] = "modi";
    inst_names_table[OP_AND     ] = "and";
    inst_names_table[OP_ANDI    ] = "andi";
    inst_names_table[OP_OR      ] = "or";
    inst_names_table[OP_ORI     ] = "ori";
   
	inst_names_table[OP_FLSS     ] = "flss";
    inst_names_table[OP_FLSSI    ] = "flssi";
    inst_names_table[OP_FLEQ     ] = "fleq";
    inst_names_table[OP_FLEQI    ] = "fleqi";
    inst_names_table[OP_FGTR     ] = "fgtr";
    inst_names_table[OP_FGTRI    ] = "fgtri";
    inst_names_table[OP_FGEQ     ] = "fgeq";
    inst_names_table[OP_FGEQI    ] = "fgeqi";
    inst_names_table[OP_FEQU     ] = "fequ";
    inst_names_table[OP_FEQUI    ] = "fequi";
    inst_names_table[OP_FNEQ     ] = "fneq";
    inst_names_table[OP_FNEQI    ] = "fneqi";
    
    inst_names_table[OP_FADD     ] = "fadd";
    inst_names_table[OP_FADDI    ] = "faddi";
    inst_names_table[OP_FSUB     ] = "fsub";
    inst_names_table[OP_FSUBI    ] = "fsubi";
    inst_names_table[OP_FMUL     ] = "fmul";
    inst_names_table[OP_FMULI    ] = "fmuli";
    inst_names_table[OP_FDIV     ] = "fdiv";
    inst_names_table[OP_FDIVI    ] = "fdivi";

    inst_names_table[OP_PRINT   ] = "print";
    inst_names_table[OP_FPRINT  ] = "fprint";
}

void BBVMInst_dump(BBVMInst inst, BBVMBasicBlockTag * tags, BBVM_FFI_symbolinfo ** ffi_infos) {
    uint8_t opcode = INST_GET_OPCODE(inst);
    
    printf("\t");
    
    switch (opcode) {
        case OP_ENTRY:
        case OP_ESCAPE:
            printf("%s", inst_names_table[opcode]);
            break;
        case OP_ALLOC:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
        case OP_LOAD:
        case OP_FLOAD:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u", INST_GET_OP1_IDX(inst));
            break;
        case OP_STORE:
        case OP_FSTORE:
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u\t", INST_GET_OP1_IDX(inst));
            printf("%%%u", INST_GET_OP2_IDX(inst));
            break;
        case OP_STOREI:
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u\t", INST_GET_OP1_IDX(inst));
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
        case OP_FSTOREI: {
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u\t", INST_GET_OP1_IDX(inst));
            
            uint32_t val32 = INST_GET_IMMEDIATE(inst);
            
            printf("%f", *((float*)&val32));
            break;
        }
        case OP_GETGLOBAL:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
        case OP_ARG:
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u", INST_GET_OP1_IDX(inst));
            break;
        case OP_ARGI:
            printf("%s\t", inst_names_table[opcode]);
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
        case OP_GETARG:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
        case OP_BR:
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%s", tags[INST_GET_OP1_IDX(inst)].name);
            break;
        case OP_CALL:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%s", tags[INST_GET_IMMEDIATE(inst)].name);
            break;
        case OP_FFI_CALL:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%s", ffi_infos[INST_GET_IMMEDIATE(inst)]->symbol);
            break;
        case OP_BRC: {
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u\t", INST_GET_OP1_IDX(inst));
            int idx = INST_GET_OP2_IDX(inst);
            BBVMBasicBlockTag * tag = &tags[idx];
            printf("%%%s", tags[INST_GET_OP2_IDX(inst)].name);
            break;
        }
        case OP_CALLI:
            break;
        case OP_VRET:
            printf("%s", inst_names_table[opcode]);
            break;
        case OP_RET:
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u", INST_GET_OP1_IDX(inst));
            break;
        case OP_RETI:
            printf("%s\t", inst_names_table[opcode]);
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
        case OP_LSS:
        case OP_LEQ:
        case OP_GTR:
        case OP_GEQ:
        case OP_EQU:
        case OP_NEQ:
        case OP_ADD:
        case OP_SUB:
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
        case OP_AND:
        case OP_OR:
        case OP_FLSS:
        case OP_FLEQ:
        case OP_FGTR:
        case OP_FGEQ:
        case OP_FEQU:
        case OP_FNEQ:
        case OP_FADD:
        case OP_FSUB:
        case OP_FMUL:
        case OP_FDIV:
   			printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u,\t", INST_GET_OP1_IDX(inst));
            printf("%%%u", INST_GET_OP2_IDX(inst));
            break;
        case OP_LSSI:
        case OP_LEQI:
        case OP_GTRI:
        case OP_GEQI:
        case OP_EQUI:
        case OP_NEQI:
        case OP_ADDI:
        case OP_SUBI:
        case OP_MULI:
        case OP_DIVI:
        case OP_MODI:
        case OP_ANDI:
        case OP_ORI:
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u,\t", INST_GET_OP1_IDX(inst));
            printf("%u", INST_GET_IMMEDIATE(inst));
            break;
		case OP_FLSSI:
        case OP_FLEQI:
        case OP_FGTRI:
        case OP_FGEQI:
        case OP_FEQUI:
        case OP_FNEQI:
        case OP_FADDI:
        case OP_FSUBI:
        case OP_FMULI:
        case OP_FDIVI: {
            printf("%%%u =\t", INST_GET_DEST_IDX(inst));
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u,\t", INST_GET_OP1_IDX(inst));
            
            uint32_t val32 = INST_GET_IMMEDIATE(inst);
            
            printf("%f", *((float*)&val32));
            break;
        }
        
        case OP_PRINT:
        case OP_FPRINT:
            printf("%s\t", inst_names_table[opcode]);
            printf("%%%u", INST_GET_OP1_IDX(inst));
            break;
    }
    printf("\n");
}








