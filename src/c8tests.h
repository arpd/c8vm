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
};
#endif
