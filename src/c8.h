#ifndef __C8_H__
#define __C8_H__

#include "def.h"

class C8VM {
    vmstate state;

    public:
    C8VM();
    C8VM(unsigned int);
    //~C8VM();
    bool start();
    bool stop();
    bool pause();
    bool reset();

    private:
    void do_cycle();
    void fetch_opcode();
    void init();
    void clean();
};
#endif
