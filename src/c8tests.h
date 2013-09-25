#ifndef __C8TESTS_H__
#define __C8TESTS_H__
#include "def.h"
#include <string>

namespace c8tests {
    typedef struct result {
        bool pass;
        std::string name, expected, actual;
    };

    void clear_result(result* result);
    void call_prog(vmstate* state, result* result);
    void clear_screen(vmstate* state, result* result);
    void ret_routine(vmstate* state, result* result);
    void jump(vmstate* state, result* result);
    void call_routine(vmstate* state, result* result);
    void skip_if_equal(vmstate* state, result* result);
    void skip_if_not_equal(vmstate* state, result* result);
    void skip_if_equal_regs(vmstate* state, result* result);
    void set_reg(vmstate* state, result* result);
    void add_reg(vmstate* state, result* result);
    void set_regx_regy(vmstate* state, result* result);
    void set_regx_or_regy(vmstate* state, result* result);
    void set_regx_and_regy(vmstate* state, result* result);
    void set_regx_xor_regy(vmstate* state, result* result);
    void set_regx_add_regy(vmstate* state, result* result);
    void set_regx_sub_regy(vmstate* state, result* result);
    void set_regx_rshift(vmstate* state, result* result);
    void set_regx_regy_sub_regx(vmstate* state, result* result);
    void set_regx_lshift(vmstate* state, result* result);
};
#endif
