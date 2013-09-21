#include "c8.h"

C8VM::C8VM() { }

void C8VM::do_cycle() {
}
void C8VM::fetch_opcode() {
    /* fetch the next instruction, increasing the instruction pointer */
    c8opcode opcode = state.memory[state.ip++] << 8 |
                      state.memory[state.ip++];
    state.curr_opcode = opcode;
}
void C8VM::init() {
    state.ip          = PROG_START;
    state.sp          = NULL;
    state.index       = NULL;
    state.curr_opcode = NULL;
    for (int i = 0; i < GFX_SIZE; ++i)
        state.gfx_buffer[i] = NULL;
    for (int i = 0; i < STACK_SIZE; ++i)
        state.stack[i] = NULL;
    for (int i = 0; i < NUM_REGISTERS; ++i)
        state.registers[i] = NULL;
    for (int i = 0; i < MEM_SIZE; ++i)
        state.memory[i] = NULL;
}
void C8VM::clean() {
}
bool C8VM::start() {
}
bool C8VM::stop() {
}
bool C8VM::pause() {
}
bool C8VM::reset() {
}
