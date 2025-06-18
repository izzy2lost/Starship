#pragma once

#include "endianness.h"
#include "n64/CommandMacros.h"
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include <lib/binarytools/BinaryWriter.h>
#include <types/Vec3D.h>

enum class GeoOpcode {
    BranchAndLink,
    End,
    Branch,
    Return,
    OpenNode,
    CloseNode,
    AssignAsView,
    UpdateNodeFlags,
    NodeRoot,
    NodeOrthoProjection,
    NodePerspective,
    NodeStart,
    NodeMasterList,
    NodeLevelOfDetail,
    NodeSwitchCase,
    NodeCamera,
    NodeTranslationRotation,
    NodeTranslation,
    NodeRotation,
    NodeAnimatedPart,
    NodeBillboard,
    NodeDisplayList,
    NodeShadow,
    NodeObjectParent,
    NodeAsm,
    NodeBackground,
    NOP,
    CopyView,
    NodeHeldObj,
    NodeScale,
    NOP2,
    NOP3,
    NodeCullingRadius,
};

inline std::ostream& operator<<(std::ostream& out, const GeoOpcode& opcode) {
    std::string output;
    switch (opcode) {
        case GeoOpcode::BranchAndLink:
            output = "GEO_BRANCH_AND_LINK";
            break;
        case GeoOpcode::End:
            output = "GEO_END";
            break;
        case GeoOpcode::Branch:
            output = "GEO_BRANCH";
            break;
        case GeoOpcode::Return:
            output = "GEO_RETURN";
            break;
        case GeoOpcode::OpenNode:
            output = "GEO_OPEN_NODE";
            break;
        case GeoOpcode::CloseNode:
            output = "GEO_CLOSE_NODE";
            break;
        case GeoOpcode::AssignAsView:
            output = "GEO_ASSIGN_AS_VIEW";
            break;
        case GeoOpcode::UpdateNodeFlags:
            output = "GEO_UPDATE_NODE_FLAGS";
            break;
        case GeoOpcode::NodeRoot:
            output = "GEO_NODE_SCREEN_AREA";
            break;
        case GeoOpcode::NodeOrthoProjection:
            output = "GEO_NODE_ORTHO";
            break;
        case GeoOpcode::NodePerspective:
            output = "GEO_CAMERA_FRUSTUM";
            break;
        case GeoOpcode::NodeStart:
            output = "GEO_NODE_START";
            break;
        case GeoOpcode::NodeMasterList:
            output = "GEO_ZBUFFER";
            break;
        case GeoOpcode::NodeLevelOfDetail:
            output = "GEO_RENDER_RANGE";
            break;
        case GeoOpcode::NodeSwitchCase:
            output = "GEO_SWITCH_CASE";
            break;
        case GeoOpcode::NodeCamera:
            output = "GEO_CAMERA";
            break;
        case GeoOpcode::NodeTranslationRotation:
            output = "GEO_TRANSLATE_ROTATE";
            break;
        case GeoOpcode::NodeTranslation:
            output = "GEO_TRANSLATE";
            break;
        case GeoOpcode::NodeRotation:
            output = "GEO_ROTATE";
            break;
        case GeoOpcode::NodeAnimatedPart:
            output = "GEO_ANIMATED_PART";
            break;
        case GeoOpcode::NodeBillboard:
            output = "GEO_BILLBOARD";
            break;
        case GeoOpcode::NodeDisplayList:
            output = "GEO_DISPLAY_LIST";
            break;
        case GeoOpcode::NodeShadow:
            output = "GEO_SHADOW";
            break;
        case GeoOpcode::NodeObjectParent:
            output = "GEO_RENDER_OBJ";
            break;
        case GeoOpcode::NodeAsm:
            output = "GEO_ASM";
            break;
        case GeoOpcode::NodeBackground:
            output = "GEO_BACKGROUND";
            break;
        case GeoOpcode::NOP:
            output = "GEO_NOP_1A";
            break;
        case GeoOpcode::CopyView:
            output = "GEO_COPY_VIEW";
            break;
        case GeoOpcode::NodeHeldObj:
            output = "GEO_HELD_OBJECT";
            break;
        case GeoOpcode::NodeScale:
            output = "GEO_SCALE";
            break;
        case GeoOpcode::NOP2:
            output = "GEO_NOP_1E";
            break;
        case GeoOpcode::NOP3:
            output = "GEO_NOP_1F";
            break;
        case GeoOpcode::NodeCullingRadius:
            output = "GEO_CULLING_RADIUS";
            break;
        default:
            throw std::runtime_error("Unknown Opcode");
    }

    return out << output;
}

#define cur_geo_cmd_u8(offset) \
    (cmd[CMD_PROCESS_OFFSET(offset)])

#define cur_geo_cmd_s16(offset) \
    (int16_t)BSWAP16((*(int16_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_geo_cmd_s32(offset) \
    (s32)BSWAP32((*(s32 *) &cmd[CMD_PROCESS_OFFSET(offset)]))

#define cur_geo_cmd_u32(offset) \
    (uint32_t) BSWAP32((*(uint32_t *) &cmd[CMD_PROCESS_OFFSET(offset)]))

class GeoLayoutCommand {
public:
    virtual void Print(std::ostream& out) = 0;
    virtual void Write(LUS::BinaryWriter& write) = 0;
};