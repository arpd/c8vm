#ifndef __ISET_H__
#define __ISET_H__
namespace iset {
    void call_prog(vmstate* state);
    void clear_screen(vmstate* state);
    void ret_routine(vmstate* state);
    void call_routine(vmstate* state);
    void skip_if_equal(vmstate* state);
    void skip_if_not_equal(vmstate* state);
    void skip_if_equal_regs(vmstate* state);
    void set_reg(vmstate* state);
    void add_reg(vmstate* state);
    void set_regx_regy(vmstate* state);
    void set_regx_or_regy(vmstate* state);
    void set_regx_and_regy(vmstate* state);
    void set_regx_xor_regy(vmstate* state);
    void set_regx_add_regy(vmstate* state);
    void set_regx_sub_regy(vmstate* state);
    void set_regx_rshift(vmstate* state);
    void set_regx_regy_sub_regx(vmstate* state);
    void set_regx_lshift(vmstate* state);
    void skip_if_not_equal_regs(vmstate* state);
    void set_index(vmstate* state);
};
#endif
