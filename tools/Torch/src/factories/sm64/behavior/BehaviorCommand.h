#pragma once

#include "endianness.h"
#include "n64/CommandMacros.h"
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <lib/binarytools/BinaryWriter.h>
#include <types/Vec3D.h>

enum class BehaviorOpcode {
    BEGIN,
    DELAY,
    CALL,
    RETURN,
    GOTO,
    BEGIN_REPEAT,
    END_REPEAT,
    END_REPEAT_CONTINUE,
    BEGIN_LOOP,
    END_LOOP,
    BREAK,
    BREAK_UNUSED,
    CALL_NATIVE,
    ADD_FLOAT,
    SET_FLOAT,
    ADD_INT,
    SET_INT,
    OR_INT,
    BIT_CLEAR,
    SET_INT_RAND_RSHIFT,
    SET_RANDOM_FLOAT,
    SET_RANDOM_INT,
    ADD_RANDOM_FLOAT,
    ADD_INT_RAND_RSHIFT,
    CMD_NOP_1,
    CMD_NOP_2,
    CMD_NOP_3,
    SET_MODEL,
    SPAWN_CHILD,
    DEACTIVATE,
    DROP_TO_FLOOR,
    SUM_FLOAT,
    SUM_INT,
    BILLBOARD,
    HIDE,
    SET_HITBOX,
    CMD_NOP_4,
    DELAY_VAR,
    BEGIN_REPEAT_UNUSED,
    LOAD_ANIMATIONS,
    ANIMATE,
    SPAWN_CHILD_WITH_PARAM,
    LOAD_COLLISION_DATA,
    SET_HITBOX_WITH_OFFSET,
    SPAWN_OBJ,
    SET_HOME,
    SET_HURTBOX,
    SET_INTERACT_TYPE,
    SET_OBJ_PHYSICS,
    SET_INTERACT_SUBTYPE,
    SCALE,
    PARENT_BIT_CLEAR,
    ANIMATE_TEXTURE,
    DISABLE_RENDERING,
    SET_INT_UNUSED,
    SPAWN_WATER_DROPLET,
};

inline std::ostream& operator<<(std::ostream& out, const BehaviorOpcode& opcode) {
    std::string output;
    switch (opcode) {
        case BehaviorOpcode::BEGIN:
            output = "BEGIN";
            break;
        case BehaviorOpcode::DELAY:
            output = "DELAY";
            break;
        case BehaviorOpcode::CALL:
            output = "CALL";
            break;
        case BehaviorOpcode::RETURN:
            output = "RETURN";
            break;
        case BehaviorOpcode::GOTO:
            output = "GOTO";
            break;
        case BehaviorOpcode::BEGIN_REPEAT:
            output = "BEGIN_REPEAT";
            break;
        case BehaviorOpcode::END_REPEAT:
            output = "END_REPEAT";
            break;
        case BehaviorOpcode::END_REPEAT_CONTINUE:
            output = "END_REPEAT_CONTINUE";
            break;
        case BehaviorOpcode::BEGIN_LOOP:
            output = "BEGIN_LOOP";
            break;
        case BehaviorOpcode::END_LOOP:
            output = "END_LOOP";
            break;
        case BehaviorOpcode::BREAK:
            output = "BREAK";
            break;
        case BehaviorOpcode::BREAK_UNUSED:
            output = "BREAK_UNUSED";
            break;
        case BehaviorOpcode::CALL_NATIVE:
            output = "CALL_NATIVE";
            break;
        case BehaviorOpcode::ADD_FLOAT:
            output = "ADD_FLOAT";
            break;
        case BehaviorOpcode::SET_FLOAT:
            output = "SET_FLOAT";
            break;
        case BehaviorOpcode::ADD_INT:
            output = "ADD_INT";
            break;
        case BehaviorOpcode::SET_INT:
            output = "SET_INT";
            break;
        case BehaviorOpcode::OR_INT:
            output = "OR_INT";
            break;
        case BehaviorOpcode::BIT_CLEAR:
            output = "BIT_CLEAR";
            break;
        case BehaviorOpcode::SET_INT_RAND_RSHIFT:
            output = "SET_INT_RAND_RSHIFT";
            break;
        case BehaviorOpcode::SET_RANDOM_FLOAT:
            output = "SET_RANDOM_FLOAT";
            break;
        case BehaviorOpcode::SET_RANDOM_INT:
            output = "SET_RANDOM_INT";
            break;
        case BehaviorOpcode::ADD_RANDOM_FLOAT:
            output = "ADD_RANDOM_FLOAT";
            break;
        case BehaviorOpcode::ADD_INT_RAND_RSHIFT:
            output = "ADD_INT_RAND_RSHIFT";
            break;
        case BehaviorOpcode::CMD_NOP_1:
            output = "CMD_NOP_1";
            break;
        case BehaviorOpcode::CMD_NOP_2:
            output = "CMD_NOP_2";
            break;
        case BehaviorOpcode::CMD_NOP_3:
            output = "CMD_NOP_3";
            break;
        case BehaviorOpcode::SET_MODEL:
            output = "SET_MODEL";
            break;
        case BehaviorOpcode::SPAWN_CHILD:
            output = "SPAWN_CHILD";
            break;
        case BehaviorOpcode::DEACTIVATE:
            output = "DEACTIVATE";
            break;
        case BehaviorOpcode::DROP_TO_FLOOR:
            output = "DROP_TO_FLOOR";
            break;
        case BehaviorOpcode::SUM_FLOAT:
            output = "SUM_FLOAT";
            break;
        case BehaviorOpcode::SUM_INT:
            output = "SUM_INT";
            break;
        case BehaviorOpcode::BILLBOARD:
            output = "BILLBOARD";
            break;
        case BehaviorOpcode::HIDE:
            output = "HIDE";
            break;
        case BehaviorOpcode::SET_HITBOX:
            output = "SET_HITBOX";
            break;
        case BehaviorOpcode::CMD_NOP_4:
            output = "CMD_NOP_4";
            break;
        case BehaviorOpcode::DELAY_VAR:
            output = "DELAY_VAR";
            break;
        case BehaviorOpcode::BEGIN_REPEAT_UNUSED:
            output = "BEGIN_REPEAT_UNUSED";
            break;
        case BehaviorOpcode::LOAD_ANIMATIONS:
            output = "LOAD_ANIMATIONS";
            break;
        case BehaviorOpcode::ANIMATE:
            output = "ANIMATE";
            break;
        case BehaviorOpcode::SPAWN_CHILD_WITH_PARAM:
            output = "SPAWN_CHILD_WITH_PARAM";
            break;
        case BehaviorOpcode::LOAD_COLLISION_DATA:
            output = "LOAD_COLLISION_DATA";
            break;
        case BehaviorOpcode::SET_HITBOX_WITH_OFFSET:
            output = "SET_HITBOX_WITH_OFFSET";
            break;
        case BehaviorOpcode::SPAWN_OBJ:
            output = "SPAWN_OBJ";
            break;
        case BehaviorOpcode::SET_HOME:
            output = "SET_HOME";
            break;
        case BehaviorOpcode::SET_HURTBOX:
            output = "SET_HURTBOX";
            break;
        case BehaviorOpcode::SET_INTERACT_TYPE:
            output = "SET_INTERACT_TYPE";
            break;
        case BehaviorOpcode::SET_OBJ_PHYSICS:
            output = "SET_OBJ_PHYSICS";
            break;
        case BehaviorOpcode::SET_INTERACT_SUBTYPE:
            output = "SET_INTERACT_SUBTYPE";
            break;
        case BehaviorOpcode::SCALE:
            output = "SCALE";
            break;
        case BehaviorOpcode::PARENT_BIT_CLEAR:
            output = "PARENT_BIT_CLEAR";
            break;
        case BehaviorOpcode::ANIMATE_TEXTURE:
            output = "ANIMATE_TEXTURE";
            break;
        case BehaviorOpcode::DISABLE_RENDERING:
            output = "DISABLE_RENDERING";
            break;
        case BehaviorOpcode::SET_INT_UNUSED:
            output = "SET_INT_UNUSED";
            break;
        case BehaviorOpcode::SPAWN_WATER_DROPLET:
            output = "SPAWN_WATER_DROPLET";
            break;
        default:
            throw std::runtime_error("Unknown Behavior Opcode");
    }

    return out << output;
}

#define cur_behavior_cmd_u8(offset) \
    (cmd[CMD_PROCESS_OFFSET(offset)])

#define cur_behavior_cmd_s16(offset) \
    (int16_t)BSWAP16((*(int16_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_behavior_cmd_s32(offset) \
    (int32_t)BSWAP32((*(int32_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_behavior_cmd_u32(offset) \
    (uint32_t) BSWAP32((*(uint32_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_behavior_cmd_f32(offset) \
    (float) BSWAP32((*(float *) &cmd[CMD_PROCESS_OFFSET(offset)]))
