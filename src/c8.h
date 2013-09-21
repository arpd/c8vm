#ifndef __C8_H__
#define __C8_H__

#include "def.h"
#include <string>

class C8VM {
    vmstate state;

    public:
    C8VM();
    C8VM(unsigned int);
    //~C8VM();
    void start();
    void stop();
    void pause();
    void reset();
    void load(const std::string&);

    private:
    void do_cycle();
    void fetch_opcode();
    void init();
    void clean();
};
#endif
