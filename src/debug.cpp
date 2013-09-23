#include "debug.h"
#include <iomanip>

void print_hex(std::ostream& stream, c8opcode v) {
    stream << std::showbase
              << std::internal
              << std::setfill('0')
              << std::hex
              << std::setw(6)
              << v;
    stream << std::dec;
}

void debug(debug_kind db_type, vmstate* state, const std::string& str) {
    switch(db_type) {
        case iset_decode:
            std::cerr << " - opcode [";
            print_hex(std::cerr, state->curr_opcode);
            std::cerr << "] decoded as " << str << std::endl;
            break;
        default:
            break;
    };
}
