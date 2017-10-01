//
//  main.c
//  BBVM
//
//  Created by Brandon Kammerdiener on 8/30/17.
//  Copyright © 2017 me. All rights reserved.
//

#include "VM.h"
#include "BytecodeBuilder.h"

extern FILE* yyin;
int yyparse();

void foo() {
    printf("This is foo()\n");
}

int fib(int n) {
    if (n <= 1)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int main(int argc, const char * argv[]) {
    BBVMInstBuilder * builder = BBVMInstBuilder_create();
    BBVM_FFI_TYPE puts_arg_types[1] = { BBVM_FFI_PTR };
    BBVMInstBuilder_add_ffi_symbol(builder, BBVM_FFI_create_symbolinfo("puts", puts_arg_types, 1, 0, BBVM_FFI_INT));

    BBVMBasicBlockIdx entry = BBVMInstBuilder_begin_new_basic_block(builder, "entry");
	BBVMBasicBlockIdx exit_bb = BBVMInstBuilder_begin_new_basic_block(builder, "exit");
    BBVMInstBuilder_push_escape_inst(builder);
	BBVMInstBuilder_set_current_basic_block(builder, entry);

    BBVMGlobalValueIdx hw = BBVMInstBuilder_add_global_string(builder, "Hello, world!");
	BBVMSSAValIdx arg = BBVMInstBuilder_push_getglobal_inst(builder, hw);
    BBVMInstBuilder_push_arg_inst(builder, arg);
    BBVMInstBuilder_push_ffi_call_inst(builder, "puts");
	
	BBVMSSAValIdx zero = BBVMInstBuilder_push_alloc_inst(builder, 8);
	BBVMInstBuilder_push_storei_inst(builder, zero, 0);
	BBVMSSAValIdx lzero = BBVMInstBuilder_push_load_inst(builder, zero);
	BBVMInstBuilder_push_brc_inst(builder, lzero, exit_bb);
    BBVMSSAValIdx a = BBVMInstBuilder_push_alloc_inst(builder, 8);
    BBVMSSAValIdx five = BBVMInstBuilder_push_alloc_inst(builder, 8);
    BBVMInstBuilder_push_fstorei_inst(builder, a, 1.2345);
    BBVMInstBuilder_push_fstorei_inst(builder, five, 5.0);
    BBVMSSAValIdx b = BBVMInstBuilder_push_fload_inst(builder, a);
    BBVMSSAValIdx c = BBVMInstBuilder_push_faddi_inst(builder, b, 4.5678);
    BBVMSSAValIdx d = BBVMInstBuilder_push_fmuli_inst(builder, c, 10.1);
    BBVMSSAValIdx e = BBVMInstBuilder_push_fload_inst(builder, five);
    BBVMSSAValIdx f = BBVMInstBuilder_push_fdiv_inst(builder, d, e);
    BBVMInstBuilder_push_fprint_inst(builder, f);
    BBVMSSAValIdx g = BBVMInstBuilder_push_fequi_inst(builder, e, 5.0);
    BBVMInstBuilder_push_print_inst(builder, g);
    BBVMSSAValIdx h = BBVMInstBuilder_push_fequi_inst(builder, e, 6.0);
    BBVMInstBuilder_push_print_inst(builder, h);
    
    
    /*
    BBVM_FFI_TYPE puts_arg_types[1] = { BBVM_FFI_PTR };
    BBVM_FFI_TYPE fib_arg_types[1] = { BBVM_FFI_INT };
    BBVMInstBuilder_add_ffi_symbol(builder, BBVM_FFI_create_symbolinfo("puts", puts_arg_types, 1, 0, BBVM_FFI_INT));
    BBVMInstBuilder_add_ffi_symbol(builder, BBVM_FFI_create_symbolinfo("getchar", NULL, 0, 0, BBVM_FFI_INT));
    BBVMInstBuilder_add_ffi_symbol(builder, BBVM_FFI_create_symbolinfo("foo", NULL, 0, 0, BBVM_FFI_VOID));
    BBVMInstBuilder_add_ffi_symbol(builder, BBVM_FFI_create_symbolinfo("fib", fib_arg_types, 1, 0, BBVM_FFI_INT));
    BBVMGlobalValueIdx message = BBVMInstBuilder_add_global_string(builder, "Hello, BBVM!\n");
    BBVMGlobalValueIdx thanks = BBVMInstBuilder_add_global_string(builder, "Thanks for the character: ");
    BBVMGlobalValueIdx bye = BBVMInstBuilder_add_global_string(builder, "Goodbye");
    BBVMGlobalValueIdx ff = BBVMInstBuilder_add_global_string(builder, "Fast fib");
    BBVMGlobalValueIdx sf = BBVMInstBuilder_add_global_string(builder, "Slow fib");
    */
     
    /* BBVMBasicBlockIdx entry = BBVMInstBuilder_begin_new_basic_block(builder, "entry"); */
    
    /*
    BBVMSSAValIdx arg = BBVMInstBuilder_push_getglobal_inst(builder, message);
    BBVMInstBuilder_push_arg_inst(builder, arg);
    BBVMInstBuilder_push_ffi_call_inst(builder, "puts");
    
    BBVMSSAValIdx ret = BBVMInstBuilder_push_ffi_call_inst(builder, "getchar");
    BBVMSSAValIdx targ = BBVMInstBuilder_push_getglobal_inst(builder, thanks);
    BBVMInstBuilder_push_arg_inst(builder, targ);
    BBVMInstBuilder_push_ffi_call_inst(builder, "puts");
    BBVMInstBuilder_push_print_inst(builder, ret);
   
    BBVMInstBuilder_push_ffi_call_inst(builder, "foo");
    
    BBVMSSAValIdx arg2 = BBVMInstBuilder_push_getglobal_inst(builder, bye);
    BBVMInstBuilder_push_arg_inst(builder, arg2);
    BBVMInstBuilder_push_ffi_call_inst(builder, "puts");
    
    // BBVMInstBuilder_push_escape_inst(builder);
    
    BBVMSSAValIdx ffarg = BBVMInstBuilder_push_getglobal_inst(builder, ff);
    BBVMInstBuilder_push_arg_inst(builder, ffarg);
    BBVMInstBuilder_push_ffi_call_inst(builder, "puts");
    
    BBVMInstBuilder_push_argi_inst(builder, 40);
    BBVMSSAValIdx ff_ret = BBVMInstBuilder_push_ffi_call_inst(builder, "fib");
    BBVMInstBuilder_push_print_inst(builder, ff_ret);
    
    BBVMSSAValIdx sfarg = BBVMInstBuilder_push_getglobal_inst(builder, sf);
    BBVMInstBuilder_push_arg_inst(builder, sfarg);
    BBVMInstBuilder_push_ffi_call_inst(builder, "puts");
    */
   
	/*
    BBVMBasicBlockIdx fib = BBVMInstBuilder_begin_new_basic_block(builder, "fib");
    
    
    BBVMInstBuilder_set_current_basic_block(builder, entry);
    
    BBVMInstBuilder_push_argi_inst(builder, 92);
    BBVMInstBuilder_push_argi_inst(builder, 0);
    BBVMInstBuilder_push_argi_inst(builder, 1);
    BBVMSSAValIdx result = BBVMInstBuilder_push_call_inst(builder, fib);
    BBVMInstBuilder_push_print_inst(builder, result);
    
    BBVMInstBuilder_push_escape_inst(builder);
    
    
    
    BBVMInstBuilder_set_current_basic_block(builder, fib);
    
    BBVMBasicBlockIdx ret0 = BBVMInstBuilder_begin_new_basic_block(builder, "ret0");
    BBVMBasicBlockIdx ret1 = BBVMInstBuilder_begin_new_basic_block(builder, "ret1");
    BBVMInstBuilder_set_current_basic_block(builder, fib);
    
    BBVMSSAValIdx n = BBVMInstBuilder_push_getarg_inst(builder, 0);
    BBVMSSAValIdx a = BBVMInstBuilder_push_getarg_inst(builder, 1);
    BBVMSSAValIdx b = BBVMInstBuilder_push_getarg_inst(builder, 2);
    BBVMSSAValIdx cond0 = BBVMInstBuilder_push_equi_inst(builder, n, 0);
    BBVMInstBuilder_push_brc_inst(builder, cond0, ret0);
    BBVMSSAValIdx cond1 = BBVMInstBuilder_push_equi_inst(builder, n, 1);
    BBVMInstBuilder_push_brc_inst(builder, cond1, ret1);
    
    
    
    BBVMSSAValIdx _1 = BBVMInstBuilder_push_subi_inst(builder, n, 1);
    BBVMSSAValIdx _2 = BBVMInstBuilder_push_add_inst(builder, a, b);
    BBVMInstBuilder_push_arg_inst(builder, _1);
    BBVMInstBuilder_push_arg_inst(builder, b);
    BBVMInstBuilder_push_arg_inst(builder, _2);
    BBVMSSAValIdx _3 = BBVMInstBuilder_push_call_inst(builder, fib);
    
    BBVMInstBuilder_push_ret_inst(builder, _3);
    
    
    BBVMInstBuilder_set_current_basic_block(builder, ret0);
    BBVMInstBuilder_push_ret_inst(builder, a);
    BBVMInstBuilder_set_current_basic_block(builder, ret1);
    BBVMInstBuilder_push_ret_inst(builder, b);
   
	*/
    
 
    BBVMInstBuilder_finalize(builder);
    
    
    BBVMInst_dump_init();
    BBVMInstBuilder_dump_instructions(builder);
    
    BBVirtualMachine * VM = BBVM_bcreate(builder);
    BBVM_run(VM);

	yyin = fopen("example.bb", "r");
	yyparse();
	fclose(yyin);

    return 0;
}
