//
//  FFI.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 9/7/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "FFI.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>

BBVM_FFI_symbolinfo * BBVM_FFI_create_symbolinfo(const char * symbol, BBVM_FFI_TYPE * arg_types, int n_arg_types, int is_vararg, BBVM_FFI_TYPE ret_type) {
    BBVM_FFI_symbolinfo * info = malloc(sizeof(BBVM_FFI_symbolinfo));
    info->symbol = strdup(symbol);
    info->arg_types = malloc(n_arg_types * sizeof(ffi_type*));
    memcpy(info->arg_types, arg_types, n_arg_types * sizeof(ffi_type*));
    info->n_arg_types = n_arg_types;
    info->is_vararg = is_vararg;
    info->ret_type = ret_type;
    info->cached_cif = NULL;
    return info;
}

void BBVM_FFI_destroy_symbolinfo(BBVM_FFI_symbolinfo * info) {
    free(info->arg_types);
    free((void*)info->symbol);
    free(info);
}

void BBVM_FFI_call(BBVirtualMachine * VM, BBVM_FFI_symbolinfo * info, void * ret_addr) {
    int i;
    void * ptr = dlsym(RTLD_DEFAULT, info->symbol);
    assert(ptr);

    BBVMStackFrame * f = BBVM_GET_CUR_STACK_FRAME(VM);
    ffi_cif * cif = NULL;

    void ** arg_values = malloc(info->n_arg_types * sizeof(void*));
    
    for (i = 0; i < f->n_out_args; i += 1)
        arg_values[i] = &f->args_out[i];

    if (info->cached_cif != NULL) {
        cif = info->cached_cif;
    } else {
        cif = malloc(sizeof(ffi_cif));
        ffi_type ** arg_types = malloc(info->n_arg_types * sizeof(ffi_type*));
        ffi_type * ret_type = NULL;

        for (i = 0; i < info->n_arg_types; i += 1)
            arg_types[i] = BBVM_FFI_get_type(info->arg_types[i]);
        
        ret_type = BBVM_FFI_get_type(info->ret_type);

        /* Initialize the cif */
        ffi_status status = ffi_prep_cif(cif, FFI_DEFAULT_ABI, info->n_arg_types, ret_type, arg_types);
        if (status != FFI_OK) {
            assert(false);
        }
        info->cached_cif = cif;
    }

    ffi_call(cif, FFI_FN(ptr), ret_addr, arg_values);
}

ffi_type * BBVM_FFI_get_type(BBVM_FFI_TYPE t) {
    switch (t) {
        case BBVM_FFI_VOID:     return &ffi_type_void;
        case BBVM_FFI_INT:      return &ffi_type_sint;
        case BBVM_FFI_PTR:      return &ffi_type_pointer;
        default: return NULL;
    }
}
