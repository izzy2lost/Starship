#pragma once

#include "endianness.h"
#include "n64/CommandMacros.h"
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <lib/binarytools/BinaryWriter.h>
#include <types/Vec3D.h>

enum class LevelOpcode {
    EXECUTE,
    EXIT_AND_EXECUTE,
    EXIT,
    SLEEP,
    SLEEP_BEFORE_EXIT,
    JUMP,
    JUMP_LINK,
    RETURN,
    JUMP_LINK_PUSH_ARG,
    JUMP_N_TIMES,
    LOOP_BEGIN,
    LOOP_UNTIL,
    JUMP_IF,
    JUMP_LINK_IF,
    SKIP_IF,
    SKIP,
    SKIP_NOP,
    CALL,
    CALL_LOOP,
    SET_REG,
    PUSH_POOL,
    POP_POOL,
    FIXED_LOAD,
    LOAD_RAW,
    LOAD_MIO0,
    LOAD_MARIO_HEAD,
    LOAD_MIO0_TEXTURE,
    INIT_LEVEL,
    CLEAR_LEVEL,
    ALLOC_LEVEL_POOL,
    FREE_LEVEL_POOL,
    AREA,
    END_AREA,
    LOAD_MODEL_FROM_DL,
    LOAD_MODEL_FROM_GEO,
    CMD23,
    OBJECT_WITH_ACTS,
    MARIO,
    WARP_NODE,
    PAINTING_WARP_NODE,
    INSTANT_WARP,
    LOAD_AREA,
    CMD2A,
    MARIO_POS,
    CMD2C,
    CMD2D,
    TERRAIN,
    ROOMS,
    SHOW_DIALOG,
    TERRAIN_TYPE,
    NOP,
    TRANSITION,
    BLACKOUT,
    GAMMA,
    SET_BACKGROUND_MUSIC,
    SET_MENU_MUSIC,
    STOP_MUSIC,
    MACRO_OBJECTS,
    CMD3A,
    WHIRLPOOL,
    GET_OR_SET,
};

inline std::ostream& operator<<(std::ostream& out, const LevelOpcode& opcode) {
    std::string output;
    switch (opcode) {
        case LevelOpcode::EXECUTE:
            output = "EXECUTE";
            break;
        case LevelOpcode::EXIT_AND_EXECUTE:
            output = "EXIT_AND_EXECUTE";
            break;
        case LevelOpcode::EXIT:
            output = "EXIT";
            break;
        case LevelOpcode::SLEEP:
            output = "SLEEP";
            break;
        case LevelOpcode::SLEEP_BEFORE_EXIT:
            output = "SLEEP_BEFORE_EXIT";
            break;
        case LevelOpcode::JUMP:
            output = "JUMP";
            break;
        case LevelOpcode::JUMP_LINK:
            output = "JUMP_LINK";
            break;
        case LevelOpcode::RETURN:
            output = "RETURN";
            break;
        case LevelOpcode::JUMP_LINK_PUSH_ARG:
            output = "JUMP_LINK_PUSH_ARG";
            break;
        case LevelOpcode::JUMP_N_TIMES:
            output = "JUMP_N_TIMES";
            break;
        case LevelOpcode::LOOP_BEGIN:
            output = "LOOP_BEGIN";
            break;
        case LevelOpcode::LOOP_UNTIL:
            output = "LOOP_UNTIL";
            break;
        case LevelOpcode::JUMP_IF:
            output = "JUMP_IF";
            break;
        case LevelOpcode::JUMP_LINK_IF:
            output = "JUMP_LINK_IF";
            break;
        case LevelOpcode::SKIP_IF:
            output = "SKIP_IF";
            break;
        case LevelOpcode::SKIP:
            output = "SKIP";
            break;
        case LevelOpcode::SKIP_NOP:
            output = "SKIP_NOP";
            break;
        case LevelOpcode::CALL:
            output = "CALL";
            break;
        case LevelOpcode::CALL_LOOP:
            output = "CALL_LOOP";
            break;
        case LevelOpcode::SET_REG:
            output = "SET_REG";
            break;
        case LevelOpcode::PUSH_POOL:
            output = "PUSH_POOL";
            break;
        case LevelOpcode::POP_POOL:
            output = "POP_POOL";
            break;
        case LevelOpcode::FIXED_LOAD:
            output = "FIXED_LOAD";
            break;
        case LevelOpcode::LOAD_RAW:
            output = "LOAD_RAW";
            break;
        case LevelOpcode::LOAD_MIO0:
            output = "LOAD_MIO0";
            break;
        case LevelOpcode::LOAD_MARIO_HEAD:
            output = "LOAD_MARIO_HEAD";
            break;
        case LevelOpcode::LOAD_MIO0_TEXTURE:
            output = "LOAD_MIO0_TEXTURE";
            break;
        case LevelOpcode::INIT_LEVEL:
            output = "INIT_LEVEL";
            break;
        case LevelOpcode::CLEAR_LEVEL:
            output = "CLEAR_LEVEL";
            break;
        case LevelOpcode::ALLOC_LEVEL_POOL:
            output = "ALLOC_LEVEL_POOL";
            break;
        case LevelOpcode::FREE_LEVEL_POOL:
            output = "FREE_LEVEL_POOL";
            break;
        case LevelOpcode::AREA:
            output = "AREA";
            break;
        case LevelOpcode::END_AREA:
            output = "END_AREA";
            break;
        case LevelOpcode::LOAD_MODEL_FROM_DL:
            output = "LOAD_MODEL_FROM_DL";
            break;
        case LevelOpcode::LOAD_MODEL_FROM_GEO:
            output = "LOAD_MODEL_FROM_GEO";
            break;
        case LevelOpcode::CMD23:
            output = "CMD23";
            break;
        case LevelOpcode::OBJECT_WITH_ACTS:
            output = "OBJECT_WITH_ACTS";
            break;
        case LevelOpcode::MARIO:
            output = "MARIO";
            break;
        case LevelOpcode::WARP_NODE:
            output = "WARP_NODE";
            break;
        case LevelOpcode::PAINTING_WARP_NODE:
            output = "PAINTING_WARP_NODE";
            break;
        case LevelOpcode::INSTANT_WARP:
            output = "INSTANT_WARP";
            break;
        case LevelOpcode::LOAD_AREA:
            output = "LOAD_AREA";
            break;
        case LevelOpcode::CMD2A:
            output = "CMD2A";
            break;
        case LevelOpcode::MARIO_POS:
            output = "MARIO_POS";
            break;
        case LevelOpcode::CMD2C:
            output = "CMD2C";
            break;
        case LevelOpcode::CMD2D:
            output = "CMD2D";
            break;
        case LevelOpcode::TERRAIN:
            output = "TERRAIN";
            break;
        case LevelOpcode::ROOMS:
            output = "ROOMS";
            break;
        case LevelOpcode::SHOW_DIALOG:
            output = "SHOW_DIALOG";
            break;
        case LevelOpcode::TERRAIN_TYPE:
            output = "TERRAIN_TYPE";
            break;
        case LevelOpcode::NOP:
            output = "NOP";
            break;
        case LevelOpcode::TRANSITION:
            output = "TRANSITION";
            break;
        case LevelOpcode::BLACKOUT:
            output = "BLACKOUT";
            break;
        case LevelOpcode::GAMMA:
            output = "GAMMA";
            break;
        case LevelOpcode::SET_BACKGROUND_MUSIC:
            output = "SET_BACKGROUND_MUSIC";
            break;
        case LevelOpcode::SET_MENU_MUSIC:
            output = "SET_MENU_MUSIC";
            break;
        case LevelOpcode::STOP_MUSIC:
            output = "STOP_MUSIC";
            break;
        case LevelOpcode::MACRO_OBJECTS:
            output = "MACRO_OBJECTS";
            break;
        case LevelOpcode::CMD3A:
            output = "CMD3A";
            break;
        case LevelOpcode::WHIRLPOOL:
            output = "WHIRLPOOL";
            break;
        case LevelOpcode::GET_OR_SET:
            output = "GET_OR_SET";
            break;
        default:
            throw std::runtime_error("Unknown Level Opcode");
    }

    return out << output;
}

#define cur_level_cmd_u8(offset) \
    (cmd[CMD_PROCESS_OFFSET(offset)])

#define cur_level_cmd_s16(offset) \
    (int16_t)BSWAP16((*(int16_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_level_cmd_s32(offset) \
    (int32_t)BSWAP32((*(int32_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_level_cmd_u32(offset) \
    (uint32_t) BSWAP32((*(uint32_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_level_cmd_f32(offset) \
    (float) BSWAP32((*(float *) &cmd[CMD_PROCESS_OFFSET(offset)]))
