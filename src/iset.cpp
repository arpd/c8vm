#include "def.h"
#include "iset.h"
#include <random>
#include <iostream>
#include "stdlib.h"
#include "debug.h"

/* CHIP-8 has 35 opcodes, which are all two bytes long. The most significant
 * byte is stored first. The opcodes are listed below, in hexadecimal and with
 * the following symbols:
 *
 *     NNN: address
 *     NN: 8-bit constant
 *     N: 4-bit constant
 *     X and Y: 4-bit register identifier
 */
void iset::call_prog(vmstate* state) {
    /* Opcode: 0NNN
     * Calls RCA 1802 program at address NNN
     */
#ifdef DEBUG
    debug(iset_decode, state, "call_prog (0NNN)");
#endif
    return;
}

// ----------------------------------------------------------------------------
void iset::clear_screen(vmstate* state) {
    /* Opcode: 00E0
     * Call the routine at address NNN
     */
#ifdef DEBUG
    debug(iset_decode, state, "clear_screen (00E0)");
#endif
    for (unsigned int i = 0; i < GFX_SIZE; ++i) {
        state->gfx_buffer[i] = 0x0;
        state->gfx_stale = true;
    }
}

// ----------------------------------------------------------------------------
void iset::ret_routine(vmstate* state) {
    /* Opcode: 00EE
     * Return from a routine
     */
#ifdef DEBUG
    debug(iset_decode, state, "ret (00EE)");
#endif
    --state->sp;
    state->ip = state->stack[state->sp];
}

// ----------------------------------------------------------------------------
void iset::jump(vmstate* state) {
    /* Opcode: 1NNN
     * Jump to address NNN
     */
#ifdef DEBUG
    debug(iset_decode, state, "jmp (1NNN)");
#endif
    word addr = state->curr_opcode & 0x0FFF;
    state->ip = addr;
    return;
}

// ----------------------------------------------------------------------------
void iset::call_routine(vmstate* state) {
    /* Opcode: 2NNN
     * Call the routine at address NNN
     */
    // save this stack frame, and increase stack pointer for routine being
    // called
#ifdef DEBUG
    debug(iset_decode, state, "call (2NNN)");
#endif
    state->stack[state->sp] = state->ip;
    ++state->sp;
    word addr = state->curr_opcode & 0x0FFF;
    state->ip = addr;
}

// ----------------------------------------------------------------------------
void iset::skip_if_equal(vmstate* state) {
    /* Opcode: 3XNN
     * Skip the next instruction if register X is equal to NN
     */
#ifdef DEBUG
    debug(iset_decode, state, "skip_if_equal (3XNN)");
#endif
    byte comparison = state->curr_opcode & 0x00FF;
    byte reg = (state->curr_opcode & 0x0F00) >> 8;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[reg] == comparison)
        state->ip += 0x2;

}

// ----------------------------------------------------------------------------
void iset::skip_if_not_equal(vmstate* state) {
    /* Opcode: 4XNN
     * Skip the next instruction if register X is not equal to NN
     */
#ifdef DEBUG
    debug(iset_decode, state, "skip_if_not_equal (4XNN)");
#endif
    c8register comparison = state->curr_opcode & 0x00FF;
    byte reg = (state->curr_opcode & 0x0F00) >> 8;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[reg] != comparison)
        state->ip += 0x2;
}

// ----------------------------------------------------------------------------
void iset::skip_if_equal_regs(vmstate* state) {
    /* Opcode: 5XY0
     * Skip the next instruction if register X is equal to register Y
     */
#ifdef DEBUG
    debug(iset_decode, state, "skip_if_equal_regs (5XY0)");
#endif
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[regx] == state->registers[regy])
        state->ip += 0x2;
}

// ----------------------------------------------------------------------------
void iset::set_reg(vmstate* state) {
    /* Opcode: 6XNN
     * Set the register X to NN
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_reg (6XNN)");
#endif
    byte reg = (state->curr_opcode & 0x0F00) >> 8;
    c8register val = state->curr_opcode & 0x00FF;
    state->registers[reg] = val;
}

// ----------------------------------------------------------------------------
void iset::add_reg(vmstate* state) {
    /* Opcode: 7XNN
     * Add NN to the register X
     */
#ifdef DEBUG
    debug(iset_decode, state, "add_reg (7XNN)");
#endif
    byte reg = (state->curr_opcode & 0x0F00) >> 8;
    c8register val = state->curr_opcode & 0x00FF;
    state->registers[reg] += val;
}

// ----------------------------------------------------------------------------
void iset::set_regx_regy(vmstate* state) {
    /* Opcode: 8XY0
     * Set register X to the value of register Y
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_regx_regy (8XY0)");
#endif
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    state->registers[regx] = state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_or_regy(vmstate* state) {
    /* Opcode: 8XY1
     * Set register X to register X | register Y
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_regx_or_regy (8XY1)");
#endif
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    state->registers[regx] |= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_and_regy(vmstate* state) {
    /* Opcode: 8XY2
     * Set register X to register X & register Y
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_regx_and_regy (8XY2)");
#endif
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    state->registers[regx] &= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_xor_regy(vmstate* state) {
    /* Opcode: 8XY3
     * Set register X to register X ^ register Y
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_regx_xor_regy (8XY3)");
#endif
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    state->registers[regx] ^= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_add_regy(vmstate* state) {
    /* Opcode: 8XY4
     * Set register X to register X + register Y
     *      [!] register F is set to 1 if there is a carry, else 0
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_regx_add_regy (8XY4)");
#endif
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    if (state->registers[regy] >
        0xFF - state->registers[regx])
        state->registers[0xF] = 1; // carry
    else
        state->registers[0xF] = 0;

    state->registers[regx] += state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_sub_regy(vmstate* state) {
    /* Opcode: 8XY5
     * Set register X to register X - register Y
     *      [!] register F is set to 0 if there is a borrow, else 0
     */
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    if (state->registers[regy] < state->registers[regx])
        state->registers[0xF] = 1; // no borrow
    else
        state->registers[0xF] = 0;

    state->registers[regx] -= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_rshift(vmstate* state) {
    /* Opcode: 8XY6
     * Set register X to register X >> 1
     *      [!] register F is set to the value of the LSB before the shift
     */
    byte regx = (state->curr_opcode & 0x0F00) >> 8;
    state->registers[0xF] = state->registers[regx] & 0x00F;

    state->registers[regx] >>= 1;
}

// ----------------------------------------------------------------------------
void iset::set_regx_regy_sub_regx(vmstate* state) {
    /* Opcode: 8XY7
     * Set register X to register Y - register X
     *      [!] register F is set to 0 if there is a borrow, else 0
     */
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    if (state->registers[regy] > state->registers[regx])
        state->registers[0xF] = 1; // no borrow
    else
        state->registers[0xF] = 0;

    state->registers[regx] = state->registers[regy] - state->registers[regx];
}

// ----------------------------------------------------------------------------
void iset::set_regx_lshift(vmstate* state) {
    /* Opcode: 8XYE
     * Set register X to register X << 1
     *      [!] register F is set to the value of the MSB before the shift
     */
    byte regx = (state->curr_opcode & 0x0F00) >> 8;
    state->registers[0xF] = (state->registers[regx] & 0xF00) >> 8;

    state->registers[regx] <<= 1;
}

// ----------------------------------------------------------------------------
void iset::skip_if_not_equal_regs(vmstate* state) {
    /* Opcode: 9XY0
     * Skip the next instruction if register X is not equal to register Y
     */
    byte regx = (state->curr_opcode & 0x0F00) >> 8,
         regy = (state->curr_opcode & 0x00F0) >> 4;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[regx] != state->registers[regy])
        state->ip += 0x2;
}

// ----------------------------------------------------------------------------
void iset::set_index(vmstate* state) {
    /* Opcode: ANNN
     * Set the index register to NNN
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_index (ANNN)");
#endif
    word addr = state->curr_opcode & 0x0FFF;

    state->index = addr;
}

// ----------------------------------------------------------------------------
void iset::jump_offset(vmstate* state) {
    /* Opcode: BNNN
     * Jump to the address NNN + register 0
     */
    word addr = state->curr_opcode & 0x0FFF;
    addr += state->registers[0];

    state->ip = addr;
}

// ----------------------------------------------------------------------------
void iset::set_reg_rand_masked(vmstate* state) {
    /* Opcode: CXNN
     * Set VX to a random number and (& mask) NN.
     */
#ifdef DEBUG
    debug(iset_decode, state, "set_reg_rand_masked (CXNN)");
#endif
    byte mask = state->curr_opcode & 0x00FF;
    byte val  = (byte) std::rand();
    state->registers[0] = val & mask;
}

// ----------------------------------------------------------------------------
void iset::draw_sprite(vmstate* state) {
    /* Opcode: DXYN
     * Draw the sprite found at [index] to the coordinates taken from
     * [X],[Y]. The sprite is N rows tall. If there is collision, set
     * register F to 1.
     */
#ifdef DEBUG
    debug(iset_decode, state, "draw_sprite (DXYN)");
#endif
    state->registers[0xF] = 0;
    c8register x = state->registers[(state->curr_opcode & 0x0F00) >> 8],
               y = state->registers[(state->curr_opcode & 0x00F0) >> 4];
    int n = state->curr_opcode & 0x000F;
    byte pixel_row;
    for (byte yoff = 0; yoff < n; ++yoff) {
        pixel_row = state->memory[state->index + yoff];
        for (byte xoff = 0; xoff < 8; ++xoff) {
            if ((pixel_row & (0x80 >> xoff)) != 0) {
                if (state->gfx_buffer[(x + xoff + ((y + yoff) * 64))] == 1)
                    state->registers[0xF] = 1;
                state->gfx_buffer[x + xoff + ((y + yoff) * 64)] ^= 1;
            }
        }
    }
    state->gfx_stale = true; // let the vm know to redraw the screen
}

// ----------------------------------------------------------------------------
void iset::skip_if_key_pressed(vmstate* state) {
    /* Opcode: EX9E
     * Skip the next instruction if the key in register X is pressed
     */
    word key = (state->curr_opcode & 0x0F00) >> 8;
    if (state->key[key])
        state->ip += 0x2;
}

// ----------------------------------------------------------------------------
void iset::skip_if_key_not_pressed(vmstate* state) {
    /* Opcode: EXA1
     * Skip the next instruction if the key in register X is not pressed
     */
    word key = (state->curr_opcode & 0x0F00) >> 8;
    if (!state->key[key])
        state->ip += 0x2;
}

// ----------------------------------------------------------------------------
void iset::set_reg_delay(vmstate* state) {
    /* Opcode: FX07
     * Set the register X to the value of the delay timer
     */
    c8register* x = &state->registers[(state->curr_opcode & 0x0F00) >> 8];
    *x = state->delay_timer;
}

// ----------------------------------------------------------------------------
void iset::wait_key_press_store(vmstate* state) {
    /* Opcode: FX0A
     * Wait for a key press, then store in the register X
     */
    bool key_press = false;
    for (int i = 0; i < 0xF; ++i) {
        if (state->key[i]) {
            key_press = true;
            break;
        }
    }
    if (!key_press) // just execute another cycle rather than busy waiting
        state->ip -= 0x2;
}

// ----------------------------------------------------------------------------
void iset::set_delay_regx(vmstate* state) {
    /* Opcode: FX15
     * Set the delay timer to register X
     */
    c8register* x = &state->registers[(state->curr_opcode & 0x0F00) >> 8];
    state->delay_timer = *x;
}

// ----------------------------------------------------------------------------
void iset::set_sound_regx(vmstate* state) {
    /* Opcode: FX18
     * Set the sound timer to register X
     */
    c8register* x = &state->registers[(state->curr_opcode & 0x0F00) >> 8];
    state->sound_timer = *x;
}

// ----------------------------------------------------------------------------
void iset::add_regx_to_index(vmstate* state) {
    /* Opcode: FX1E
     * Add the value from register X to the index register
     */
    c8register* x = &state->registers[(state->curr_opcode & 0x0F00) >> 8];
    state->index += *x;
}

// ----------------------------------------------------------------------------
void iset::get_sprite_regx(vmstate* state) {
    /* Opcode: FX29
     * Set the index register to the location of the sprite for the character
     * in register X. (i.e. there is a fontset in memory somewhere, so point
     * index at the character of that fontset as stored in register X)
     */
    c8register* x = &state->registers[(state->curr_opcode & 0x0F00) >> 8];
    state->index = *x * 0x5;
}

// ----------------------------------------------------------------------------
void iset::split_decimal(vmstate* state) {
    /* Opcode: FX33
     * Treating register X as a decimal, put the hundreds digit in [index], the
     * tens digit in [index+1] and the ones digit at [index+2]
     */
    c8register* x = &state->registers[(state->curr_opcode & 0x0F00) >> 8];
    word ones = *x % 10,
         tens = (*x % 100) / 10,
         hundreds = (*x % 1000) / 100;
    state->memory[state->index]   = hundreds;
    state->memory[state->index+1] = tens;
    state->memory[state->index+2] = ones;
}

// ----------------------------------------------------------------------------
void iset::dump_regs_to_regx(vmstate* state) {
    /* Opcode: FX55
     * Dump values of registers from register 0 -> register X in the memory
     * starting at [index]
     */
    word start_loc = state->index;
    int end_reg = (state->curr_opcode & 0x0F00) >> 8;
    word loc = start_loc;
    for (int reg = 0; reg < end_reg; ++reg, ++loc)
        state->memory[loc] = state->registers[reg];
}


// ----------------------------------------------------------------------------
void iset::slurp_regs_to_regx(vmstate* state) {
    /* Opcode: FX65
     * Set values of registers from register 0 -> register X to values in
     * memory starting at [index]
     */
    word start_loc = state->index;
    int end_reg = (state->curr_opcode & 0x0F00) >> 8;
    word loc = start_loc;
    for (int reg = 0; reg < end_reg; ++reg, ++loc)
        state->registers[reg] = state->memory[loc] ;
}
