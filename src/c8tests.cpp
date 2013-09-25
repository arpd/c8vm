#include "c8tests.h"
#include "iset.h"
#include <sstream>

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
    result->pass = result->expected.compare(result->actual) == 0;
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
    result->pass = result->actual.compare(result->expected) == 0;
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
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::call_routine(vmstate* state, result* result) {
    std::stringstream actual, expected;
    state->sp = 0;
    // calling from:
    state->ip = 0xABCD;
    // calling function at:
    state->curr_opcode = 0x2DED;
    word expected_sp = 1,
         expected_sp_follow = 0xABCD,
         expected_ip = 0x0DED;
    iset::call_routine(state);
    word actual_sp = state->sp,
         actual_sp_follow = state->stack[state->sp],
         actual_ip = state->ip;
    expected << "sp = " << expected_sp << std::endl
             << "sp follow = " << expected_sp_follow << std::endl
             << "ip = " << expected_ip << std::endl;
    actual << "sp = " << actual_sp << std::endl
           << "sp follow = " << actual_sp_follow << std::endl
           << "ip = " << actual_ip << std::endl;
    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::skip_if_equal(vmstate* state, result* result) {
    std::stringstream actual, expected;
    state->ip = 0x2;
    /* the opcode from 0x2-0x4 should be skipped if reg A == 0xDD */
    state->curr_opcode = 0x3EAD;
    state->registers[0xE] = 0xFF;
    /* which it doesn't, so we shouldn't skip here */
    word expected_ip = 0x2;
    iset::skip_if_equal(state);
    word actual_ip   = state->ip;
    expected << "ip = " << expected_ip << std::endl;
    actual << "ip = " << actual_ip << std::endl;
    /* however, this time it should ...
       n.b we don't have to reset state here, as the instruction functions
       don't actually alter the instruction pointer (i.e. this is driven
       by `do_cycle`) */
    expected_ip = 0x4;
    state->registers[0xE] = 0xAD;
    iset::skip_if_equal(state);
    actual_ip = state->ip;
    expected << "ip = " << expected_ip << std::endl;
    actual << "ip = " << actual_ip << std::endl;
    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::skip_if_not_equal(vmstate* state, result* result) {
    /* basically the inverse of the last test */
    std::stringstream actual, expected;
    state->ip = 0x2;
    /* the opcode from 0x2-0x4 should be skipped if reg A != 0xDD */
    state->curr_opcode = 0x4EAD;
    state->registers[0xE] = 0xFF;
    /* which it doesn't, so we shouldn't skip here */
    word expected_ip = 0x4;
    iset::skip_if_not_equal(state);
    word actual_ip   = state->ip;
    expected << "ip = " << expected_ip << std::endl;
    actual << "ip = " << actual_ip << std::endl;
    /* however, this time it should ...
       n.b we don't have to reset state here, as the instruction functions
       don't actually alter the instruction pointer (i.e. this is driven
       by `do_cycle`) */
    expected_ip = 0x4;
    state->registers[0xE] = 0xAD;
    iset::skip_if_not_equal(state);
    actual_ip = state->ip;
    expected << "ip = " << expected_ip << std::endl;
    actual << "ip = " << actual_ip << std::endl;
    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;

}

// ----------------------------------------------------------------------------

void c8tests::skip_if_equal_regs(vmstate* state, result* result) {
    std::stringstream actual, expected;
    state->ip = 0x2;
    state->curr_opcode    = 0x55A0;

    state->registers[0x5] = 0xBA;
    state->registers[0xA] = 0xDA;
    word expected_ip = 0x2;
    iset::skip_if_equal_regs(state);
    word actual_ip = state->ip;
    expected << "ip = " << expected_ip << std::endl;
    actual   << "ip = " << actual_ip   << std::endl;

    state->registers[0x5] = 0xDA;
    expected_ip = 0x4;
    iset::skip_if_equal_regs(state);
    actual_ip = state->ip;

    expected << "ip = " << expected_ip << std::endl;
    actual << "ip = " << actual_ip << std::endl;
    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::set_reg(vmstate* state, result* result) {
    std::stringstream actual, expected;
    state->registers[0xA] = 0xCC;
    state->curr_opcode    = 0x6ADD;

    word expected_reg = 0xDD;
    iset::set_reg(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::add_reg(vmstate* state, result* result) {
    std::stringstream actual, expected;
    state->registers[0xA] = 0xCC;
    state->curr_opcode    = 0x7A11;

    word expected_reg = 0xDD;
    iset::add_reg(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;

    // TODO: Check overflow
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_regy(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xCC;
    state->registers[0xB] = 0xFF;
    state->curr_opcode    = 0x8AB0;

    word expected_reg     = 0xFF;
    iset::set_regx_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_or_regy(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->registers[0xB] = 0xCD;
    state->curr_opcode    = 0x8AB1;

    word expected_reg     = 0xAB | 0xCD;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;

}

// ----------------------------------------------------------------------------
void c8tests::set_regx_and_regy(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->registers[0xB] = 0xCD;
    state->curr_opcode    = 0x8AB2;

    word expected_reg     = 0xAB & 0xCD;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_xor_regy(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->registers[0xB] = 0xCD;
    state->curr_opcode    = 0x8AB3;

    word expected_reg     = 0xAB ^ 0xCD;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_add_regy(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->registers[0xB] = 0xCD;
    state->curr_opcode    = 0x8AB4;

    word expected_reg     = 0xAB + 0xCD;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;

    // TODO: overflow
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_sub_regy(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->registers[0xB] = 0xCD;
    state->curr_opcode    = 0x8AB5;

    word expected_reg     = 0xAB - 0xCD;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_rshift(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->registers[0xB] = 0xCD;
    state->curr_opcode    = 0x8AB7;

    word expected_reg     = 0xCD - 0xAB;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
void c8tests::set_regx_regy_sub_regx(vmstate* state, result* result) { }

// ----------------------------------------------------------------------------
void c8tests::set_regx_lshift(vmstate* state, result* result) { 
    std::stringstream actual, expected;
    state->registers[0xA] = 0xAB;
    state->curr_opcode    = 0x8ABE;

    word expected_reg     = 0xAB << 1;
    iset::set_regx_or_regy(state);
    word actual_reg = state->registers[0xA];
    expected << "registers[0xA] = " << expected_reg << std::endl;
    actual   << "registers[0xA] = " << actual_reg   << std::endl;

    result->expected = expected.str();
    result->actual   = actual.str();
    result->pass = result->actual.compare(result->expected) == 0;
}

// ----------------------------------------------------------------------------
