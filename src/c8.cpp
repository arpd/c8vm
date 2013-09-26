#include "c8.h"
#include "iset.h"

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#include "debug.h"
#endif

extern void draw_buf(const byte* gfx_buf, const unsigned int size);

// ----------------------------------------------------------------------------
C8VM::C8VM() {
    init();
}

// ----------------------------------------------------------------------------
void C8VM::do_cycle() {
    if (state.delay_timer > 0)
        --state.delay_timer;
    if (state.sound_timer > 0) {
#if DEBUG
        std::cerr << "BEEP!" << std::endl;
#endif
        --state.sound_timer;
    }
    fetch_opcode();
    byte first  = ((state.curr_opcode & 0xF000) >> 12),
         second = ((state.curr_opcode & 0x0F00) >> 8),
         third  = ((state.curr_opcode & 0x00F0) >> 4),
         fourth =  (state.curr_opcode & 0x000F);
    // decode current opcode
    switch (first) {
        case 0x0:
            if (third == 0xE && fourth == 0x0)
                iset::clear_screen(&state);
            else if (third == 0xE && fourth == 0xE)
                iset::ret_routine(&state);
            else
                iset::call_prog(&state);
            break;
        case 0x1:
            iset::jump(&state);
            break;
        case 0x2:
            iset::call_routine(&state);
            break;
        case 0x3:
            iset::skip_if_equal(&state);
            break;
        case 0x4:
            iset::skip_if_not_equal(&state);
            break;
        case 0x5:
            iset::skip_if_equal_regs(&state);
            break;
        case 0x6:
            iset::set_reg(&state);
            break;
        case 0x7:
            iset::add_reg(&state);
            break;
        case 0x8:
            switch(fourth) {
                case 0x0:
                    iset::set_regx_regy(&state);
                    break;
                case 0x1:
                    iset::set_regx_or_regy(&state);
                    break;
                case 0x2:
                    iset::set_regx_and_regy(&state);
                    break;
                case 0x3:
                    iset::set_regx_xor_regy(&state);
                    break;
                case 0x4:
                    iset::set_regx_add_regy(&state);
                    break;
                case 0x5:
                    iset::set_regx_sub_regy(&state);
                    break;
                case 0x6:
                    iset::set_regx_rshift(&state);
                    break;
                case 0x7:
                    iset::set_regx_regy_sub_regx(&state);
                    break;
                case 0xE:
                    iset::set_regx_lshift(&state);
                    break;
            }
            break;
        case 0x9:
            iset::skip_if_not_equal_regs(&state);
            break;
        case 0xA:
            iset::set_index(&state);
            break;
        case 0xB:
            iset::jump_offset(&state);
            break;
        case 0xC:
            iset::set_reg_rand_masked(&state);
            break;
        case 0xD:
            iset::draw_sprite(&state);
            break;
        case 0xE:
            if (third == 0x9 && fourth == 0xE)
                iset::skip_if_key_pressed(&state);
            else if (third == 0xA && fourth == 0x1)
                iset::skip_if_key_not_pressed(&state);
            break;
        case 0xF:
            switch (third) {
                case 0x0:
                    if (fourth == 0x7)
                        iset::set_reg_delay(&state);
                    else if (fourth == 0xA)
                        iset::wait_key_press_store(&state);
                    break;
                case 0x1:
                    if (fourth == 0x5)
                        iset::set_delay_regx(&state);
                    else if (fourth == 0x8)
                        iset::set_sound_regx(&state);
                    else if (fourth == 0xE)
                        iset::add_regx_to_index(&state);
                    break;
                case 0x2:
                    iset::get_sprite_regx(&state);
                    break;
                case 0x3:
                    iset::split_decimal(&state);
                    break;
                case 0x5:
                    iset::dump_regs_to_regx(&state);
                    break;
                case 0x6:
                    iset::slurp_regs_to_regx(&state);
                    break;
                default:
                    break; //TODO: failure here
            }
            break;
        default:
            break; // TODO: Failure here
    }
    state.cycles++;

    if (state.ip > MEM_SIZE - 2)
        state.on = false;
}

// ----------------------------------------------------------------------------
void C8VM::fetch_opcode() {
    /* fetch the next instruction, increasing the instruction pointer
     * appropriately, and store it in `state.curr_opcode`
     */
    c8opcode opcode = state.memory[state.ip++] << 8;
    opcode         |= state.memory[state.ip++];
    state.curr_opcode = opcode;
#ifdef DEBUG
    std::cerr << "read opcode: "; print_hex(std::cerr, opcode);
    std::cerr << "  [ip: "; print_hex(std::cerr, state.ip);
    std::cerr << "]" << std::endl;
#endif
}

// ----------------------------------------------------------------------------
void C8VM::init() {
    state.ip          = PROG_START;
    state.sp          = 0x0;
    state.index       = 0x0;
    state.curr_opcode = 0x0;
    state.gfx_stale   = true;
    state.on          = false;
    for (unsigned int i = 0; i < GFX_SIZE; ++i)
        state.gfx_buffer[i] = 0x0;
    for (unsigned int i = 0; i < STACK_SIZE; ++i)
        state.stack[i] = 0x0;
    for (unsigned int i = 0; i < NUM_REGISTERS; ++i)
        state.registers[i] = 0x0;
    for (unsigned int i = 0; i < MEM_SIZE; ++i)
        state.memory[i] = 0x0;

    // load fontset in to memory
    byte  font_offset = 0xA;
    byte* font_pos    = &state.memory[0x0 + font_offset];

    *font_pos   = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0x90; // *  *
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0x20; //  *
    *++font_pos = 0x60; // **
    *++font_pos = 0x20; //  *
    *++font_pos = 0x20; //  *
    *++font_pos = 0x70; // ***

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0x90; // *  *
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0x10; //    *

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0x20; //   *
    *++font_pos = 0x40; //  *
    *++font_pos = 0x40; //  *

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x10; //    *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x90; // *  *
    *++font_pos = 0x90; // *  *

    *++font_pos = 0xE0; // ***
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xE0; // ***
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xE0; // ***

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0x80; // *
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xE0; // ***
    *++font_pos = 0x90; // *  *
    *++font_pos = 0x90; // *  *
    *++font_pos = 0x90; // *  *
    *++font_pos = 0xE0; // ***

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****

    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0xF0; // ****
    *++font_pos = 0x80; // *
    *++font_pos = 0x80; // *
}

// ----------------------------------------------------------------------------
void C8VM::load(const std::string& bin) {
    if (bin.size() == 0) {
#ifdef DEBUG
        std::cerr << "load called with empty image" << std::endl;
#endif
        return;
    }
    for (unsigned int i = 0; i < bin.size(); ++i)
        state.memory[PROG_START + i] = bin.at(i);
#ifdef DEBUG
    std::cerr << "loaded image (" << bin.size() << " bytes)" << std::endl;
#endif
}

// ----------------------------------------------------------------------------
void C8VM::clean() {
}

// ----------------------------------------------------------------------------
void C8VM::start() {
    state.on = true;
}

// ----------------------------------------------------------------------------
void C8VM::stop() {
    state.on = false;
}

// ----------------------------------------------------------------------------
void C8VM::pause() {
}

// ----------------------------------------------------------------------------
void C8VM::reset() {
}

// ----------------------------------------------------------------------------
byte* C8VM::get_keys() {
    return state.key;
}

// ----------------------------------------------------------------------------

bool C8VM::is_on() {
    return state.on;
}

// ----------------------------------------------------------------------------

byte* C8VM::get_gfx_buf() {
    return state.gfx_buffer;
}

// ----------------------------------------------------------------------------

void C8VM::set_gfx_stale(bool v) {
    state.gfx_stale = v;
}

// ----------------------------------------------------------------------------

bool C8VM::get_gfx_stale() {
    return state.gfx_stale;
}
