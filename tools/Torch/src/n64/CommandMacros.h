#pragma once

#include <cstdint>

#define _SHIFTL(v, s, w) ((uint32_t)(((uint32_t)(v) & ((0x01 << (w)) - 1)) << (s)))
#define _SHIFTR(v, s, w) ((uint32_t)(((uint32_t)(v) >> (s)) & ((0x01 << (w)) - 1)))

#define CMD_SIZE_SHIFT (sizeof(uint32_t) >> 3)
#define CMD_PROCESS_OFFSET(offset) (((offset) & 3) | (((offset) & ~3) << CMD_SIZE_SHIFT))

#define CMD_BBBB(a, b, c, d) (_SHIFTL(a, 0, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(c, 16, 8) | _SHIFTL(d, 24, 8))
#define CMD_BBH(a, b, c) (_SHIFTL(a, 0, 8) | _SHIFTL(b, 8, 8) | _SHIFTL(c, 16, 16))
#define CMD_HH(a, b) (_SHIFTL(a, 0, 16) | _SHIFTL(b, 16, 16))
#define CMD_W(a) (a)
#define CMD_PTR(a) ((uintptr_t)(a))

#define CMD_HHHHHH(a, b, c, d, e, f) CMD_HH(a, b), CMD_HH(c, d), CMD_HH(e, f)
