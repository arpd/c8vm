#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>
#include "def.h"

void print_hex(std::ostream& stream, c8opcode v);
void debug(debug_kind db_type, vmstate* state, const std::string& str);
void print_gfx_buf(const byte* gfx_buf);

#endif
