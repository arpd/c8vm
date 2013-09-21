#include "c8.h"
#include "iset.h"

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#endif

// ----------------------------------------------------------------------------
C8VM::C8VM() {
    init();
}

// ----------------------------------------------------------------------------
void C8VM::do_cycle() {
    fetch_opcode();
    byte first  = ((state.curr_opcode & 0xF000) >> 12),
         second = ((state.curr_opcode & 0x0F00) >> 8);
    // decode current opcode
    switch (first) {
        case 0x0: iset::call_prog(&state); break;
        default: break; // TODO: Failure here
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
    std::cerr << std::showbase
              << std::internal
              << std::setfill('0')
              << std::hex;
    std::cerr << "read opcode: " << std::setw(6) << opcode << std::endl;
    std::cerr << std::dec;
    std::cerr << "  [ip: ]" << state.ip << std::endl;
#endif
}

// ----------------------------------------------------------------------------
void C8VM::init() {
    state.ip          = PROG_START;
    state.sp          = 0x0;
    state.index       = 0x0;
    state.curr_opcode = 0x0;
    for (unsigned int i = 0; i < GFX_SIZE; ++i)
        state.gfx_buffer[i] = 0x0;
    for (unsigned int i = 0; i < STACK_SIZE; ++i)
        state.stack[i] = 0x0;
    for (unsigned int i = 0; i < NUM_REGISTERS; ++i)
        state.registers[i] = 0x0;
    for (unsigned int i = 0; i < MEM_SIZE; ++i)
        state.memory[i] = 0x0;
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
    while (state.on)
        do_cycle();
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
