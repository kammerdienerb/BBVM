//
//  Bytecode.h
//  BBVM
//
//  Created by Brandon Kammerdiener on 8/30/17.
//  Copyright © 2017 me. All rights reserved.
//

#ifndef Bytecode_h
#define Bytecode_h

#include <stdint.h>

typedef uint64_t BBVMInst;

void BBVMInst_dump_init(void);

struct BBVMBasicBlockTag;
struct BBVM_FFI_symbolinfo;

void BBVMInst_dump(BBVMInst inst, struct BBVMBasicBlockTag * tags, struct BBVM_FFI_symbolinfo ** ffi_infos);

#define INST_GET_OPCODE(inst)           ((inst)             & 0x00000000000000FF)
#define INST_GET_DEST_IDX(inst)         ((uint32_t)(((inst) & 0x000000000000FF00) >> 8))
#define INST_GET_OP1_IDX(inst)          ((uint32_t)(((inst) & 0x0000000000FF0000) >> 16))
#define INST_GET_OP2_IDX(inst)          ((uint32_t)(((inst) & 0x00000000FF000000) >> 24))
#define INST_GET_IMMEDIATE(inst)        ((uint32_t)(((inst) & 0xFFFFFFFF00000000) >> 32))

#define INST_SET_OPCODE(inst, val)      (((inst) &= 0xFFFFFFFFFFFFFF00), ((inst) |= (uint64_t)(val)))
#define INST_SET_DEST_IDX(inst, val)    (((inst) &= 0xFFFFFFFFFFFF00FF), ((inst) |= ((((uint64_t)(val)) & 0x0000000000FFFFFF) << 8)))
#define INST_SET_OP1_IDX(inst, val)     (((inst) &= 0xFFFFFFFFFF00FFFF), ((inst) |= ((((uint64_t)(val)) & 0x0000000000FFFFFF) << 16)))
#define INST_SET_OP2_IDX(inst, val)     (((inst) &= 0xFFFFFFFF00FFFFFF), ((inst) |= ((((uint64_t)(val)) & 0x0000000000FFFFFF) << 24)))
#define INST_SET_IMMEDIATE(inst, val)   (((inst) &= 0x00000000FFFFFFFF), ((inst) |= ((((uint64_t)(val)) & 0x00000000FFFFFFFF) << 32)))

/* opcodes */

#define OP_ESCAPE       ((uint64_t)0x01)

#define OP_ALLOC        ((uint64_t)0x10)
#define OP_LOAD         ((uint64_t)0x11)
#define OP_FLOAD        ((uint64_t)0x12)
#define OP_STORE        ((uint64_t)0x13)
#define OP_STOREI       ((uint64_t)0x14)
#define OP_FSTORE       ((uint64_t)0x15)
#define OP_FSTOREI      ((uint64_t)0x16)
#define OP_GETGLOBAL    ((uint64_t)0x17)
#define OP_ARG          ((uint64_t)0x18)
#define OP_ARGI         ((uint64_t)0x19)
#define OP_GETARG       ((uint64_t)0x1A)

#define OP_BR           ((uint64_t)0x20)
#define OP_BRC          ((uint64_t)0x21)
#define OP_CALL         ((uint64_t)0x22)
#define OP_CALLI        ((uint64_t)0x23)
#define OP_VRET         ((uint64_t)0x24)
#define OP_RET          ((uint64_t)0x25)
#define OP_RETI         ((uint64_t)0x26)    
#define OP_FFI_CALL     ((uint64_t)0x27)

#define OP_LSS          ((uint64_t)0x30)
#define OP_LSSI         ((uint64_t)0x31)
#define OP_LEQ          ((uint64_t)0x32)
#define OP_LEQI         ((uint64_t)0x33)
#define OP_GTR          ((uint64_t)0x34)
#define OP_GTRI         ((uint64_t)0x35)
#define OP_GEQ          ((uint64_t)0x36)
#define OP_GEQI         ((uint64_t)0x37)
#define OP_EQU          ((uint64_t)0x38)
#define OP_EQUI         ((uint64_t)0x39)
#define OP_NEQ          ((uint64_t)0x3A)
#define OP_NEQI         ((uint64_t)0x3B)

#define OP_ADD          ((uint64_t)0x40)
#define OP_ADDI         ((uint64_t)0x41)
#define OP_SUB          ((uint64_t)0x42)
#define OP_SUBI         ((uint64_t)0x43)
#define OP_MUL          ((uint64_t)0x44)
#define OP_MULI         ((uint64_t)0x45)
#define OP_DIV          ((uint64_t)0x46)
#define OP_DIVI         ((uint64_t)0x47)
#define OP_MOD          ((uint64_t)0x48)
#define OP_MODI         ((uint64_t)0x49)
#define OP_AND          ((uint64_t)0x4A)
#define OP_ANDI         ((uint64_t)0x4B)
#define OP_OR           ((uint64_t)0x4C)
#define OP_ORI          ((uint64_t)0x4D)

#define OP_FLSS         ((uint64_t)0x50)
#define OP_FLSSI        ((uint64_t)0x51)
#define OP_FLEQ         ((uint64_t)0x52)
#define OP_FLEQI        ((uint64_t)0x53)
#define OP_FGTR         ((uint64_t)0x54)
#define OP_FGTRI        ((uint64_t)0x55)
#define OP_FGEQ         ((uint64_t)0x56)
#define OP_FGEQI        ((uint64_t)0x57)
#define OP_FEQU         ((uint64_t)0x58)
#define OP_FEQUI        ((uint64_t)0x59)
#define OP_FNEQ         ((uint64_t)0x5A)
#define OP_FNEQI        ((uint64_t)0x5B)

#define OP_FADD         ((uint64_t)0x60)
#define OP_FADDI        ((uint64_t)0x61)
#define OP_FSUB         ((uint64_t)0x62)
#define OP_FSUBI        ((uint64_t)0x63)
#define OP_FMUL         ((uint64_t)0x64)
#define OP_FMULI        ((uint64_t)0x65)
#define OP_FDIV         ((uint64_t)0x66)
#define OP_FDIVI        ((uint64_t)0x67)

#define OP_PRINT        ((uint64_t)0x70)
#define OP_FPRINT       ((uint64_t)0x71)

#endif /* Bytecode_h */
