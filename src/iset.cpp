#include "def.h"
#include "iset.h"

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

void iset::clear_screen(vmstate* state) {
    /* Opcode: 00E0
     * Call the routine at address NNN
     */
    for (unsigned int i = 0; i < MEM_SIZE; ++i)
        state->memory[i] = 0x0;

}

void iset::ret_routine(vmstate* state) {
    /* Opcode: 00EE
     * Return from a routine
     */
    state->ip = state->stack[state->sp];
    --state->sp;
}

void iset::call_routine(vmstate* state) {
    /* Opcode: 2NNN
     * Call the routine at address NNN
     */
    // save this stack frame, and increase stack pointer for routine being
    // called
    state->stack[state->sp] = state->ip;
    ++state->sp;
    // pull routine address from opcode
    state->ip = state->curr_opcode & 0x0FFF;
}

void iset::skip_if_equal(vmstate* state) {
    /* Opcode: 3XNN
     * Skip the next instruction if register X is equal to NN
     */
    c8register comparison = state->curr_opcode & 0x00FF;
    int reg = (int) state->curr_opcode & 0x0F00 >> 8;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[reg] == comparison)
        state->ip += 0x2;

}

void iset::skip_if_not_equal(vmstate* state) {
    /* Opcode: 4XNN
     * Skip the next instruction if register X is not equal to NN
     */
    c8register comparison = state->curr_opcode & 0x00FF;
    int reg = (int) state->curr_opcode & 0x0F00 >> 8;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[reg] != comparison)
        state->ip += 0x2;
}

void iset::skip_if_equal_regs(vmstate* state) {
    /* Opcode: 5XY0
     * Skip the next instruction if register X is equal to register Y
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[regx] == state->registers[regy])
        state->ip += 0x2;
}

void iset::set_reg(vmstate* state) {
    /* Opcode: 6XNN
     * Set the register X to NN
     */
    int reg = (int) state->curr_opcode & 0x0F00 >> 8;
    c8register val = state->curr_opcode & 0x00FF;
    state->registers[reg] = val;
}

void iset::add_reg(vmstate* state) {
    /* Opcode: 7XNN
     * Add NN to the register X
     */
    int reg = (int) state->curr_opcode & 0x0F00 >> 8;
    c8register val = state->curr_opcode & 0x00FF;
    state->registers[reg] += val;
}

void iset::set_regx_regy(vmstate* state) {
    /* Opcode: 8XY0
     * Set register X to the value of register Y
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] = state->registers[regy];
}

void iset::set_regx_or_regy(vmstate* state) {
    /* Opcode: 8XY1
     * Set register X to register X | register Y
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] |= state->registers[regy];
}

void iset::set_regx_and_regy(vmstate* state) {
    /* Opcode: 8XY2
     * Set register X to register X & register Y
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] &= state->registers[regy];
}

void iset::set_regx_xor_regy(vmstate* state) {
    /* Opcode: 8XY3
     * Set register X to register X ^ register Y
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    state->registers[regx] ^= state->registers[regy];
}

void iset::set_regx_add_regy(vmstate* state) {
    /* Opcode: 8XY4
     * Set register X to register X + register Y
     *      [!] Register F is set to 1 if there is a carry, else 0
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    if (state->registers[regy] >
        0xFF - state->registers[regx])
        state->registers[0xF] = 1; // carry
    else
        state->registers[0xF] = 0;

    state->registers[regx] += state->registers[regy];
}

void iset::set_regx_sub_regy(vmstate* state) {
    /* Opcode: 8XY5
     * Set register X to register X - register Y
     *      [!] Register F is set to 0 if there is a borrow, else 0
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    if (state->registers[regy] < state->registers[regx])
        state->registers[0xF] = 1; // no borrow
    else
        state->registers[0xF] = 0;

    state->registers[regx] -= state->registers[regy];
}

void iset::set_regx_rshift(vmstate* state) {
    /* Opcode: 8XY6
     * Set register X to register X >> 1
     *      [!] Register F is set to the value of the LSB before the shift
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8;
    state->registers[0xF] = state->registers[regx] & 0x00F;

    state->registers[regx] >>= 1;
}

void iset::set_regx_regy_sub_regx(vmstate* state) {
    /* Opcode: 8XY7
     * Set register X to register Y - register X
     *      [!] Register F is set to 0 if there is a borrow, else 0
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    if (state->registers[regy] > state->registers[regx])
        state->registers[0xF] = 1; // no borrow
    else
        state->registers[0xF] = 0;

    state->registers[regx] -= state->registers[regy];
}

void iset::set_regx_lshift(vmstate* state) {
    /* Opcode: 8XYE
     * Set register X to register X << 1
     *      [!] Register F is set to the value of the MSB before the shift
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8;
    state->registers[0xF] = (state->registers[regx] & 0xF00) >> 8;

    state->registers[regx] <<= 1;
}

void iset::skip_if_not_equal_regs(vmstate* state) {
    /* Opcode: 9XY0
     * Skip the next instruction if register X is not equal to register Y
     */
    int regx = (int) state->curr_opcode & 0x0F00 >> 8,
        regy = (int) state->curr_opcode & 0x00F0 >> 4;
    // we increment by two here as memory is an array of bytes and operands
    // are composed of two
    if (state->registers[regx] != state->registers[regy])
        state->ip += 0x2;
}

void iset::set_index(vmstate* state) {
    /* Opcode: ANNN
     * Set the index register to NNN
     */
    c8register addr = state->curr_opcode & 0x0FFF;

    state->index = addr;
}
