//
//  FFI.h
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/7/17.
//  Copyright © 2017 me. All rights reserved.
//

#ifndef FFI_h
#define FFI_h

#include "VM.h"

#include <ffi/ffi.h>

typedef enum {
    BBVM_FFI_VOID,
    BBVM_FFI_INT,
    BBVM_FFI_PTR
} BBVM_FFI_TYPE;

typedef struct BBVM_FFI_symbolinfo {
    const char * symbol;
    BBVM_FFI_TYPE * arg_types;
    int n_arg_types;
    int is_vararg;
    BBVM_FFI_TYPE ret_type;
    ffi_cif * cached_cif;
} BBVM_FFI_symbolinfo;

BBVM_FFI_symbolinfo * BBVM_FFI_create_symbolinfo(const char * symbol, BBVM_FFI_TYPE * arg_types, int n_arg_types, int is_vararg, BBVM_FFI_TYPE ret_type);
void BBVM_FFI_destroy_symbolinfo(BBVM_FFI_symbolinfo * info);

ffi_type * BBVM_FFI_get_type(BBVM_FFI_TYPE t);

void BBVM_FFI_call(BBVirtualMachine * VM, BBVM_FFI_symbolinfo * info, void * ret_addr);

#endif /* FFI_h */
