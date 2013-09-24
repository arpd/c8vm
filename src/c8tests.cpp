#include "c8tests.h"
#include "iset.h"

void c8tests::clear_result(result* r) {
    r->pass = false;
    r->expected = "DEFAULT";
    r->actual   = "DEFAULT";
    r->name     = "DEFAULT";
}

// ----------------------------------------------------------------------------
void c8tests::call_prog(vmstate* state, result* result) {
    result->pass = true;
    result->expected = "";
    result->actual   = "";
}

// ----------------------------------------------------------------------------
void c8tests::clear_screen(vmstate* state, result* result) {
    iset::clear_screen(state);
    result->expected = "cleared";
    result->actual   = "cleared";
    for (int i = 0; i < 64 * 32; ++i) {
        if (state->gfx_buffer[i] != 0) {
            result->actual = "not cleared";
            break;
        }
    }
    result->pass = result->expected == result->actual;
}

// ----------------------------------------------------------------------------
void c8tests::ret_routine(vmstate* state, result* result) {
    word addr = 0xBAA;
    state->stack[0] = addr;
    state->sp = 1;
    state->ip = 1;
    result->expected = "ip = 0xBAA";
    result->actual   = "ip = 0xBAA";
    iset::ret_routine(state);
    if (state->ip != 0xBAA)
        result->actual   = "ip = " + std::to_string(state->ip);
    result->pass = result->actual == result->expected;
}

// ----------------------------------------------------------------------------
void c8tests::jump(vmstate* state, result* result) {
    state->ip = 0xABCD;
    state->curr_opcode = 0x1DED;
    result->expected = "ip = 0x0DED";
    result->actual   = "ip = 0x0DED";
    iset::jump(state);
    if (state->ip != 0xDED)
        result->actual = "ip = " + std::to_string(state->ip);
    result->pass = result->actual == result->expected;
}

// ----------------------------------------------------------------------------
