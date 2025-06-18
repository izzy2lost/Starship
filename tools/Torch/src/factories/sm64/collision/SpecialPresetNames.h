#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

enum class SpecialPresetTypes {
    SPTYPE_NO_YROT_OR_PARAMS,
    SPTYPE_YROT_NO_PARAMS,
    SPTYPE_PARAMS_AND_YROT,
    SPTYPE_UNKNOWN,
    SPTYPE_DEF_PARAM_AND_YROT,
};

inline std::ostream& operator<<(std::ostream& out, const SpecialPresetTypes& type) {
    std::string output;
    switch (type) {
        case SpecialPresetTypes::SPTYPE_NO_YROT_OR_PARAMS:
            output = "SPTYPE_NO_YROT_OR_PARAMS";
            break;
        case SpecialPresetTypes::SPTYPE_YROT_NO_PARAMS:
            output = "SPTYPE_YROT_NO_PARAMS";
            break;
        case SpecialPresetTypes::SPTYPE_PARAMS_AND_YROT:
            output = "SPTYPE_PARAMS_AND_YROT";
            break;
        case SpecialPresetTypes::SPTYPE_UNKNOWN:
            output = "SPTYPE_UNKNOWN";
            break;
        case SpecialPresetTypes::SPTYPE_DEF_PARAM_AND_YROT:
            output = "SPTYPE_DEF_PARAM_AND_YROT";
            break;
        default:
            throw std::runtime_error("Unknown Special Preset Type");
    }
    return out << output;
}

enum class SpecialPresets {
    special_null_start,
    special_yellow_coin,
    special_yellow_coin_2,
    special_unknown_3,
    special_boo,
    special_unknown_5,
    special_lll_moving_octagonal_mesh_platform,
    special_snow_ball,
    special_lll_drawbridge_spawner,
    special_empty_9,
    special_lll_rotating_block_with_fire_bars,
    special_lll_floating_wood_bridge,
    special_tumbling_platform,
    special_lll_rotating_hexagonal_ring,
    special_lll_sinking_rectangular_platform,
    special_lll_sinking_square_platforms,
    special_lll_tilting_square_platform,
    special_lll_bowser_puzzle,
    special_mr_i,
    special_small_bully,
    special_big_bully,
    special_empty_21,
    special_empty_22,
    special_empty_23,
    special_empty_24,
    special_empty_25,
    special_moving_blue_coin,
    special_jrb_chest,
    special_water_ring,
    special_mine,
    special_empty_30,
    special_empty_31,
    special_butterfly,
    special_bowser,
    special_wf_rotating_wooden_platform,
    special_small_bomp,
    special_wf_sliding_platform,
    special_tower_platform_group,
    special_rotating_counter_clockwise,
    special_wf_tumbling_bridge,
    special_large_bomp,

    special_level_geo_03 = 0x65,
    special_level_geo_04,
    special_level_geo_05,
    special_level_geo_06,
    special_level_geo_07,
    special_level_geo_08,
    special_level_geo_09,
    special_level_geo_0A,
    special_level_geo_0B,
    special_level_geo_0C,
    special_level_geo_0D,
    special_level_geo_0E,
    special_level_geo_0F,
    special_level_geo_10,
    special_level_geo_11,
    special_level_geo_12,
    special_level_geo_13,
    special_level_geo_14,
    special_level_geo_15,
    special_level_geo_16,
    special_bubble_tree,
    special_spiky_tree,
    special_snow_tree,
    special_unknown_tree,
    special_palm_tree,
    special_wooden_door,
    special_haunted_door = special_wooden_door,
    special_unknown_door,
    special_metal_door,
    special_hmc_door,
    special_unknown2_door,
    special_wooden_door_warp,
    special_unknown1_door_warp,
    special_metal_door_warp,
    special_unknown2_door_warp,
    special_unknown3_door_warp,
    special_castle_door_warp,
    special_castle_door,
    special_0stars_door,
    special_1star_door,
    special_3star_door,
    special_key_door,

    special_null_end = 0xFF
};

inline std::ostream& operator<<(std::ostream& out, const SpecialPresets& preset) {
    std::string output;
    switch (preset) {
        case SpecialPresets::special_null_start:
            output = "special_null_start";
            break;
        case SpecialPresets::special_yellow_coin:
            output = "special_yellow_coin";
            break;
        case SpecialPresets::special_yellow_coin_2:
            output = "special_yellow_coin_2";
            break;
        case SpecialPresets::special_unknown_3:
            output = "special_unknown_3";
            break;
        case SpecialPresets::special_boo:
            output = "special_boo";
            break;
        case SpecialPresets::special_unknown_5:
            output = "special_unknown_5";
            break;
        case SpecialPresets::special_lll_moving_octagonal_mesh_platform:
            output = "special_lll_moving_octagonal_mesh_platform";
            break;
        case SpecialPresets::special_snow_ball:
            output = "special_snow_ball";
            break;
        case SpecialPresets::special_lll_drawbridge_spawner:
            output = "special_lll_drawbridge_spawner";
            break;
        case SpecialPresets::special_empty_9:
            output = "special_empty_9";
            break;
        case SpecialPresets::special_lll_rotating_block_with_fire_bars:
            output = "special_lll_rotating_block_with_fire_bars";
            break;
        case SpecialPresets::special_lll_floating_wood_bridge:
            output = "special_lll_floating_wood_bridge";
            break;
        case SpecialPresets::special_tumbling_platform:
            output = "special_tumbling_platform";
            break;
        case SpecialPresets::special_lll_rotating_hexagonal_ring:
            output = "special_lll_rotating_hexagonal_ring";
            break;
        case SpecialPresets::special_lll_sinking_rectangular_platform:
            output = "special_lll_sinking_rectangular_platform";
            break;
        case SpecialPresets::special_lll_sinking_square_platforms:
            output = "special_lll_sinking_square_platforms";
            break;
        case SpecialPresets::special_lll_tilting_square_platform:
            output = "special_lll_tilting_square_platform";
            break;
        case SpecialPresets::special_lll_bowser_puzzle:
            output = "special_lll_bowser_puzzle";
            break;
        case SpecialPresets::special_mr_i:
            output = "special_mr_i";
            break;
        case SpecialPresets::special_small_bully:
            output = "special_small_bully";
            break;
        case SpecialPresets::special_big_bully:
            output = "special_big_bully";
            break;
        case SpecialPresets::special_empty_21:
            output = "special_empty_21";
            break;
        case SpecialPresets::special_empty_22:
            output = "special_empty_22";
            break;
        case SpecialPresets::special_empty_23:
            output = "special_empty_23";
            break;
        case SpecialPresets::special_empty_24:
            output = "special_empty_24";
            break;
        case SpecialPresets::special_empty_25:
            output = "special_empty_25";
            break;
        case SpecialPresets::special_moving_blue_coin:
            output = "special_moving_blue_coin";
            break;
        case SpecialPresets::special_jrb_chest:
            output = "special_jrb_chest";
            break;
        case SpecialPresets::special_water_ring:
            output = "special_water_ring";
            break;
        case SpecialPresets::special_mine:
            output = "special_mine";
            break;
        case SpecialPresets::special_empty_30:
            output = "special_empty_30";
            break;
        case SpecialPresets::special_empty_31:
            output = "special_empty_31";
            break;
        case SpecialPresets::special_butterfly:
            output = "special_butterfly";
            break;
        case SpecialPresets::special_bowser:
            output = "special_bowser";
            break;
        case SpecialPresets::special_wf_rotating_wooden_platform:
            output = "special_wf_rotating_wooden_platform";
            break;
        case SpecialPresets::special_small_bomp:
            output = "special_small_bomp";
            break;
        case SpecialPresets::special_wf_sliding_platform:
            output = "special_wf_sliding_platform";
            break;
        case SpecialPresets::special_tower_platform_group:
            output = "special_tower_platform_group";
            break;
        case SpecialPresets::special_rotating_counter_clockwise:
            output = "special_rotating_counter_clockwise";
            break;
        case SpecialPresets::special_wf_tumbling_bridge:
            output = "special_wf_tumbling_bridge";
            break;
        case SpecialPresets::special_large_bomp:
            output = "special_large_bomp";
            break;
        case SpecialPresets::special_level_geo_03:
            output = "special_level_geo_03";
            break;
        case SpecialPresets::special_level_geo_04:
            output = "special_level_geo_04";
            break;
        case SpecialPresets::special_level_geo_05:
            output = "special_level_geo_05";
            break;
        case SpecialPresets::special_level_geo_06:
            output = "special_level_geo_06";
            break;
        case SpecialPresets::special_level_geo_07:
            output = "special_level_geo_07";
            break;
        case SpecialPresets::special_level_geo_08:
            output = "special_level_geo_08";
            break;
        case SpecialPresets::special_level_geo_09:
            output = "special_level_geo_09";
            break;
        case SpecialPresets::special_level_geo_0A:
            output = "special_level_geo_0A";
            break;
        case SpecialPresets::special_level_geo_0B:
            output = "special_level_geo_0B";
            break;
        case SpecialPresets::special_level_geo_0C:
            output = "special_level_geo_0C";
            break;
        case SpecialPresets::special_level_geo_0D:
            output = "special_level_geo_0D";
            break;
        case SpecialPresets::special_level_geo_0E:
            output = "special_level_geo_0E";
            break;
        case SpecialPresets::special_level_geo_0F:
            output = "special_level_geo_0F";
            break;
        case SpecialPresets::special_level_geo_10:
            output = "special_level_geo_10";
            break;
        case SpecialPresets::special_level_geo_11:
            output = "special_level_geo_11";
            break;
        case SpecialPresets::special_level_geo_12:
            output = "special_level_geo_12";
            break;
        case SpecialPresets::special_level_geo_13:
            output = "special_level_geo_13";
            break;
        case SpecialPresets::special_level_geo_14:
            output = "special_level_geo_14";
            break;
        case SpecialPresets::special_level_geo_15:
            output = "special_level_geo_15";
            break;
        case SpecialPresets::special_level_geo_16:
            output = "special_level_geo_16";
            break;
        case SpecialPresets::special_bubble_tree:
            output = "special_bubble_tree";
            break;
        case SpecialPresets::special_spiky_tree:
            output = "special_spiky_tree";
            break;
        case SpecialPresets::special_snow_tree:
            output = "special_snow_tree";
            break;
        case SpecialPresets::special_unknown_tree:
            output = "special_unknown_tree";
            break;
        case SpecialPresets::special_palm_tree:
            output = "special_palm_tree";
            break;
        case SpecialPresets::special_wooden_door:
            output = "special_wooden_door";
            break;
        case SpecialPresets::special_unknown_door:
            output = "special_unknown_door";
            break;
        case SpecialPresets::special_metal_door:
            output = "special_metal_door";
            break;
        case SpecialPresets::special_hmc_door:
            output = "special_hmc_door";
            break;
        case SpecialPresets::special_unknown2_door:
            output = "special_unknown2_door";
            break;
        case SpecialPresets::special_wooden_door_warp:
            output = "special_wooden_door_warp";
            break;
        case SpecialPresets::special_unknown1_door_warp:
            output = "special_unknown1_door_warp";
            break;
        case SpecialPresets::special_metal_door_warp:
            output = "special_metal_door_warp";
            break;
        case SpecialPresets::special_unknown2_door_warp:
            output = "special_unknown2_door_warp";
            break;
        case SpecialPresets::special_unknown3_door_warp:
            output = "special_unknown3_door_warp";
            break;
        case SpecialPresets::special_castle_door_warp:
            output = "special_castle_door_warp";
            break;
        case SpecialPresets::special_castle_door:
            output = "special_castle_door";
            break;
        case SpecialPresets::special_0stars_door:
            output = "special_0stars_door";
            break;
        case SpecialPresets::special_1star_door:
            output = "special_1star_door";
            break;
        case SpecialPresets::special_3star_door:
            output = "special_3star_door";
            break;
        case SpecialPresets::special_key_door:
            output = "special_key_door";
            break;
        case SpecialPresets::special_null_end:
            output = "special_null_end";
            break;
        default:
            throw std::runtime_error("Unknown Special Preset");
    }

    return out << output;
}
