#ifndef __DEF_H__
#define __DEF_H__

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef word c8opcode;
typedef byte c8register;

const unsigned int NUM_REGISTERS = 16,
                   MEM_SIZE      = 4096,    // # of bytes
                   STACK_SIZE    = 16,
                   KEY_SIZE      = 16,
                   GFX_SIZE      = 64 * 32,
                   FREQUENCY     = 60,
                   PROG_START    = 0x200;
typedef struct vmstate {
    c8opcode curr_opcode;
    c8register registers[16];
    word ip, sp, index; // ip == pc
    word stack[16];
    byte memory[4096];
    byte gfx_buffer[64 * 32];
    byte delay_timer, sound_timer;
    byte key[16];
    unsigned int frequency;
    bool on;
    long cycles;
    bool gfx_stale;
}vmstate;

enum debug_kind {
    iset_decode,
};
#endif
