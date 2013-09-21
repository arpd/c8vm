#include "def.h"
#include "iset.h"
#include <random>
#include <iostream>
#include "stdlib.h"

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
    return;
}

// ----------------------------------------------------------------------------
void iset::clear_screen(vmstate* state) {
    /* Opcode: 00E0
     * Call the routine at address NNN
     */
    for (unsigned int i = 0; i < GFX_SIZE; ++i)
        state->gfx_buffer[i] = 0x0;
}

// ----------------------------------------------------------------------------
void iset::ret_routine(vmstate* state) {
    /* Opcode: 00EE
     * Return from a routine
     */
    state->ip = state->stack[state->sp];
    --state->sp;
}

// ----------------------------------------------------------------------------
void iset::jump(vmstate* state) {
    /* Opcode: 1NNN
     * Jump to address NNN
     */
    state->ip = state->curr_opcode & 0x0FFF;
    return;
}

// ----------------------------------------------------------------------------
void iset::call_routine(vmstate* state) {
    /* Opcode: 2NNN
     * Call the routine at address NNN
     */
    // save this stack frame, and increase stack pointer for routine being
    // called
    state->stack[state->sp] = state->ip;
    ++state->sp;
    iset::jump(state);
}

// ----------------------------------------------------------------------------
void iset::skip_if_equal(vmstate* state) {
    /* Opcode: 3XNN
     * Skip the next instruction if register X is equal to NN
     */
    c8register comparison = state->curr_opcode & 0x00FF;
    byte reg = (byte) state->curr_opcode & 0x0F00 >> 8;
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
    c8register comparison = state->curr_opcode & 0x00FF;
    byte reg = (byte) state->curr_opcode & 0x0F00 >> 8;
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
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
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
    byte reg = (byte) state->curr_opcode & 0x0F00 >> 8;
    c8register val = state->curr_opcode & 0x00FF;
    state->registers[reg] = val;
}

// ----------------------------------------------------------------------------
void iset::add_reg(vmstate* state) {
    /* Opcode: 7XNN
     * Add NN to the register X
     */
    byte reg = (byte) state->curr_opcode & 0x0F00 >> 8;
    c8register val = state->curr_opcode & 0x00FF;
    state->registers[reg] += val;
}

// ----------------------------------------------------------------------------
void iset::set_regx_regy(vmstate* state) {
    /* Opcode: 8XY0
     * Set register X to the value of register Y
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] = state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_or_regy(vmstate* state) {
    /* Opcode: 8XY1
     * Set register X to register X | register Y
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] |= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_and_regy(vmstate* state) {
    /* Opcode: 8XY2
     * Set register X to register X & register Y
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] &= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_xor_regy(vmstate* state) {
    /* Opcode: 8XY3
     * Set register X to register X ^ register Y
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] ^= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_add_regy(vmstate* state) {
    /* Opcode: 8XY4
     * Set register X to register X + register Y
     *      [!] register F is set to 1 if there is a carry, else 0
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
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
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
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
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8;
    state->registers[0xF] = state->registers[regx] & 0x00F;

    state->registers[regx] >>= 1;
}

// ----------------------------------------------------------------------------
void iset::set_regx_regy_sub_regx(vmstate* state) {
    /* Opcode: 8XY7
     * Set register X to register Y - register X
     *      [!] register F is set to 0 if there is a borrow, else 0
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
    if (state->registers[regy] > state->registers[regx])
        state->registers[0xF] = 1; // no borrow
    else
        state->registers[0xF] = 0;

    state->registers[regx] -= state->registers[regy];
}

// ----------------------------------------------------------------------------
void iset::set_regx_lshift(vmstate* state) {
    /* Opcode: 8XYE
     * Set register X to register X << 1
     *      [!] register F is set to the value of the MSB before the shift
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8;
    state->registers[0xF] = (state->registers[regx] & 0xF00) >> 8;

    state->registers[regx] <<= 1;
}

// ----------------------------------------------------------------------------
void iset::skip_if_not_equal_regs(vmstate* state) {
    /* Opcode: 9XY0
     * Skip the next instruction if register X is not equal to register Y
     */
    byte regx = (byte) state->curr_opcode & 0x0F00 >> 8,
        regy = (byte) state->curr_opcode & 0x00F0 >> 4;
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

    iset::jump(state);
}

// ----------------------------------------------------------------------------
void iset::set_reg_rand_masked(vmstate* state) {
    /* Opcode: CXNN
     * Set VX to a random number and (& mask) NN.
     */
    byte mask = state->curr_opcode & 0x00FF;
    byte val  = (byte) std::rand();
    state->registers[0] = val & mask;
}

// ----------------------------------------------------------------------------
void iset::draw_sprite(vmstate* state) {
    /* Opcode: DXYN
     * Set the index register to NNN
     */
    std::cerr << "[-] FAILURE: draw_sprite not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::skip_if_key_pressed(vmstate* state) {
    /* Opcode: EX9E
     * Skip the next instruction if the key in register X is pressed
     */
    std::cerr << "[-] FAILURE: skip_if_key_pressed not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::skip_if_key_not_pressed(vmstate* state) {
    /* Opcode: EXA1
     * Skip the next instruction if the key in register X is not pressed
     */
    std::cerr << "[-] FAILURE: skip_if_key_not_pressed not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::set_reg_delay(vmstate* state) {
    /* Opcode: FX07
     * Set the register X to the value of the delay timer
     */
    std::cerr << "[-] FAILURE: set_reg_delay not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::wait_key_press_store(vmstate* state) {
    /* Opcode: FX0A
     * Wait for a key press, then store in the register X
     */
    std::cerr << "[-] FAILURE: wait_key_press_store not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::set_delay_regx(vmstate* state) {
    /* Opcode: FX15
     * Set the delay timer to register X
     */
    std::cerr << "[-] FAILURE: set_delay_regx not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::set_sound_regx(vmstate* state) {
    /* Opcode: FX18
     * Set the sound timer to register X
     */
    std::cerr << "[-] FAILURE: set_sound_regx not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::add_regx_to_index(vmstate* state) {
    /* Opcode: FX1E
     * Add the value from register X to the index register
     */
    std::cerr << "[-] FAILURE: add_regx_to_index not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::get_sprite_regx(vmstate* state) {
    /* Opcode: FX29
     * Set the index register to the location of the sprite for the character
     * in register X
     */
    std::cerr << "[-] FAILURE: get_sprite_regx not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::split_decimal(vmstate* state) {
    /* Opcode: FX33
     * Treating register X as a decimal, put the hundreds digit in [index], the
     * tens digit in [index+1] and the ones digit at [index+2]
     */
    std::cerr << "[-] FAILURE: split_decimal not implemented" << std::endl;
    abort();
}

// ----------------------------------------------------------------------------
void iset::dump_regs_to_regx(vmstate* state) {
    /* Opcode: FX55
     * Dump values of registers from register 0 -> register X in the memory
     * starting at [index]
     */
    std::cerr << "[-] FAILURE: dump_regs_to_regx not implemented" << std::endl;
    abort();
}


// ----------------------------------------------------------------------------
void iset::slurp_regs_to_regx(vmstate* state) {
    /* Opcode: FX65
     * Set values of registers from register 0 -> register X to values in
     * memory starting at [index]
     */
    std::cerr << "[-] FAILURE: slurp_regs_to_regx not implemented" << std::endl;
    abort();
}
