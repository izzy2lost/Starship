#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

enum class MacroPresets {
    macro_yellow_coin,
    macro_yellow_coin_2,
    macro_moving_blue_coin,
    macro_sliding_blue_coin,
    macro_red_coin,
    macro_empty_5,
    macro_coin_line_horizontal,
    macro_coin_ring_horizontal,
    macro_coin_arrow,
    macro_coin_line_horizontal_flying,
    macro_coin_line_vertical,
    macro_coin_ring_horizontal_flying,
    macro_coin_ring_vertical,
    macro_coin_arrow_flying,
    macro_hidden_star_trigger,
    macro_empty_15,
    macro_empty_16,
    macro_empty_17,
    macro_empty_18,
    macro_empty_19,
    macro_fake_star,
    macro_wooden_signpost,
    macro_cannon_closed,
    macro_bobomb_buddy_opens_cannon,
    macro_butterfly,
    macro_bouncing_fireball_copy,
    macro_fish_group_3,
    macro_fish_group,
    macro_unknown_28,
    macro_hidden_1up_in_pole,
    macro_huge_goomba,
    macro_tiny_goomba,
    macro_goomba_triplet_spawner,
    macro_goomba_quintuplet_spawner,
    macro_sign_on_wall,
    macro_chuckya,
    macro_cannon_open,
    macro_goomba,
    macro_homing_amp,
    macro_circling_amp,
    macro_unknown_40,
    macro_unknown_41,
    macro_free_bowling_ball,
    macro_snufit,
    macro_recovery_heart,
    macro_1up_sliding,
    macro_1up,
    macro_1up_jump_on_approach,
    macro_hidden_1up,
    macro_hidden_1up_trigger,
    macro_1up_2,
    macro_1up_3,
    macro_empty_52,
    macro_blue_coin_switch,
    macro_hidden_blue_coin,
    macro_wing_cap_switch,
    macro_metal_cap_switch,
    macro_vanish_cap_switch,
    macro_yellow_cap_switch,
    macro_unknown_59,
    macro_box_wing_cap,
    macro_box_metal_cap,
    macro_box_vanish_cap,
    macro_box_koopa_shell,
    macro_box_one_coin,
    macro_box_three_coins,
    macro_box_ten_coins,
    macro_box_1up,
    macro_box_star_1,
    macro_breakable_box_no_coins,
    macro_breakable_box_three_coins,
    macro_pushable_metal_box,
    macro_breakable_box_small,
    macro_floor_switch_hidden_objects,
    macro_hidden_box,
    macro_hidden_object_2,
    macro_hidden_object_3,
    macro_breakable_box_giant,
    macro_koopa_shell_underwater,
    macro_box_1up_running_away,
    macro_empty_80,
    macro_bullet_bill_cannon,
    macro_heave_ho,
    macro_empty_83,
    macro_thwomp,
    macro_fire_spitter,
    macro_fire_fly_guy,
    macro_jumping_box,
    macro_butterfly_triplet,
    macro_butterfly_triplet_2,
    macro_empty_90,
    macro_empty_91,
    macro_empty_92,
    macro_bully,
    macro_bully_2,
    macro_empty_95,
    macro_unknown_96,
    macro_bouncing_fireball,
    macro_flamethrower,
    macro_empty_99,
    macro_empty_100,
    macro_empty_101,
    macro_empty_102,
    macro_empty_103,
    macro_empty_104,
    macro_empty_105,
    macro_wooden_post,
    macro_water_bomb_spawner,
    macro_enemy_lakitu,
    macro_bob_koopa_the_quick,
    macro_koopa_race_endpoint,
    macro_bobomb,
    macro_water_bomb_cannon_copy,
    macro_bobomb_buddy_opens_cannon_copy,
    macro_water_bomb_cannon,
    macro_bobomb_still,
    macro_empty_116,
    macro_empty_117,
    macro_empty_118,
    macro_empty_119,
    macro_empty_120,
    macro_empty_121,
    macro_empty_122,
    macro_unknown_123,
    macro_empty_124,
    macro_unagi,
    macro_sushi,
    macro_empty_127,
    macro_empty_128,
    macro_empty_129,
    macro_empty_130,
    macro_empty_131,
    macro_empty_132,
    macro_empty_133,
    macro_empty_134,
    macro_empty_135,
    macro_empty_136,
    macro_unknown_137,
    macro_tornado,
    macro_pokey,
    macro_pokey_copy,
    macro_tox_box,
    macro_empty_142,
    macro_empty_143,
    macro_empty_144,
    macro_empty_145,
    macro_empty_146,
    macro_empty_147,
    macro_empty_148,
    macro_empty_149,
    macro_empty_150,
    macro_monty_mole_2,
    macro_monty_mole,
    macro_monty_mole_hole,
    macro_fly_guy,
    macro_empty_155,
    macro_wiggler,
    macro_empty_157,
    macro_empty_158,
    macro_empty_159,
    macro_empty_160,
    macro_empty_161,
    macro_empty_162,
    macro_empty_163,
    macro_empty_164,
    macro_spindrift,
    macro_mr_blizzard,
    macro_mr_blizzard_copy,
    macro_empty_168,
    macro_small_penguin,
    macro_tuxies_mother,
    macro_tuxies_mother_copy,
    macro_mr_blizzard_2,
    macro_empty_173,
    macro_empty_174,
    macro_empty_175,
    macro_empty_176,
    macro_empty_177,
    macro_empty_178,
    macro_empty_179,
    macro_empty_180,
    macro_empty_181,
    macro_empty_182,
    macro_empty_183,
    macro_empty_184,
    macro_empty_185,
    macro_empty_186,
    macro_empty_187,
    macro_empty_188,
    macro_haunted_chair_copy,
    macro_haunted_chair,
    macro_haunted_chair_copy2,
    macro_boo,
    macro_boo_copy,
    macro_boo_group,
    macro_boo_with_cage,
    macro_beta_key,
    macro_empty_197,
    macro_empty_198,
    macro_empty_199,
    macro_empty_200,
    macro_empty_201,
    macro_empty_202,
    macro_empty_203,
    macro_empty_204,
    macro_empty_205,
    macro_empty_206,
    macro_empty_207,
    macro_empty_208,
    macro_empty_209,
    macro_empty_210,
    macro_empty_211,
    macro_empty_212,
    macro_empty_213,
    macro_empty_214,
    macro_empty_215,
    macro_empty_216,
    macro_empty_217,
    macro_empty_218,
    macro_empty_219,
    macro_empty_220,
    macro_empty_221,
    macro_empty_222,
    macro_empty_223,
    macro_empty_224,
    macro_empty_225,
    macro_empty_226,
    macro_empty_227,
    macro_empty_228,
    macro_empty_229,
    macro_empty_230,
    macro_empty_231,
    macro_empty_232,
    macro_empty_233,
    macro_chirp_chirp,
    macro_seaweed_bundle,
    macro_beta_chest,
    macro_water_mine,
    macro_fish_group_4,
    macro_fish_group_2,
    macro_jet_stream_ring_spawner,
    macro_jet_stream_ring_spawner_copy,
    macro_skeeter,
    macro_clam_shell,
    macro_empty_244,
    macro_empty_245,
    macro_empty_246,
    macro_empty_247,
    macro_empty_248,
    macro_empty_249,
    macro_empty_250,
    macro_ukiki,
    macro_ukiki_2,
    macro_piranha_plant,
    macro_empty_254,
    macro_whomp,
    macro_chain_chomp,
    macro_empty_257,
    macro_koopa,
    macro_koopa_shellless,
    macro_wooden_post_copy,
    macro_fire_piranha_plant,
    macro_fire_piranha_plant_2,
    macro_thi_koopa_the_quick,
    macro_empty_264,
    macro_empty_265,
    macro_empty_266,
    macro_empty_267,
    macro_empty_268,
    macro_empty_269,
    macro_empty_270,
    macro_empty_271,
    macro_empty_272,
    macro_empty_273,
    macro_empty_274,
    macro_empty_275,
    macro_empty_276,
    macro_empty_277,
    macro_empty_278,
    macro_empty_279,
    macro_empty_280,
    macro_moneybag,
    macro_empty_282,
    macro_empty_283,
    macro_empty_284,
    macro_empty_285,
    macro_empty_286,
    macro_empty_287,
    macro_empty_288,
    macro_swoop,
    macro_swoop_2,
    macro_mr_i,
    macro_scuttlebug_spawner,
    macro_scuttlebug,
    macro_empty_294,
    macro_empty_295,
    macro_empty_296,
    macro_empty_297,
    macro_empty_298,
    macro_empty_299,
    macro_empty_300,
    macro_empty_301,
    macro_empty_302,
    macro_unknown_303,
    macro_empty_304,
    macro_empty_305,
    macro_empty_306,
    macro_empty_307,
    macro_empty_308,
    macro_empty_309,
    macro_empty_310,
    macro_empty_311,
    macro_empty_312,
    macro_ttc_rotating_cube,
    macro_ttc_rotating_prism,
    macro_ttc_pendulum,
    macro_ttc_large_treadmill,
    macro_ttc_small_treadmill,
    macro_ttc_push_block,
    macro_ttc_rotating_hexagon,
    macro_ttc_rotating_triangle,
    macro_ttc_pit_block,
    macro_ttc_pit_block_2,
    macro_ttc_elevator_platform,
    macro_ttc_clock_hand,
    macro_ttc_spinner,
    macro_ttc_small_gear,
    macro_ttc_large_gear,
    macro_ttc_large_treadmill_2,
    macro_ttc_small_treadmill_2,
    macro_empty_330,
    macro_empty_331,
    macro_empty_332,
    macro_empty_333,
    macro_empty_334,
    macro_empty_335,
    macro_empty_336,
    macro_empty_337,
    macro_empty_338,
    macro_box_star_2,
    macro_box_star_3,
    macro_box_star_4,
    macro_box_star_5,
    macro_box_star_6,
    macro_empty_344,
    macro_empty_345,
    macro_empty_346,
    macro_empty_347,
    macro_empty_348,
    macro_empty_349,
    macro_bits_sliding_platform,
    macro_bits_twin_sliding_platforms,
    macro_bits_unknown_352,
    macro_bits_octagonal_platform,
    macro_bits_staircase,
    macro_empty_355,
    macro_empty_356,
    macro_bits_ferris_wheel_axle,
    macro_bits_arrow_platform,
    macro_bits_seesaw_platform,
    macro_bits_tilting_w_platform,
    macro_empty_361,
    macro_empty_362,
    macro_empty_363,
    macro_empty_364,
    macro_empty_365,
    macro_invalid = -1
};

inline std::ostream& operator<<(std::ostream& out, const MacroPresets& preset) {
    std::string output;
    switch (preset) {
        case MacroPresets::macro_yellow_coin:
            output = "macro_yellow_coin";
            break;
        case MacroPresets::macro_yellow_coin_2:
            output = "macro_yellow_coin_2";
            break;
        case MacroPresets::macro_moving_blue_coin:
            output = "macro_moving_blue_coin";
            break;
        case MacroPresets::macro_sliding_blue_coin:
            output = "macro_sliding_blue_coin";
            break;
        case MacroPresets::macro_red_coin:
            output = "macro_red_coin";
            break;
        case MacroPresets::macro_empty_5:
            output = "macro_empty_5";
            break;
        case MacroPresets::macro_coin_line_horizontal:
            output = "macro_coin_line_horizontal";
            break;
        case MacroPresets::macro_coin_ring_horizontal:
            output = "macro_coin_ring_horizontal";
            break;
        case MacroPresets::macro_coin_arrow:
            output = "macro_coin_arrow";
            break;
        case MacroPresets::macro_coin_line_horizontal_flying:
            output = "macro_coin_line_horizontal_flying";
            break;
        case MacroPresets::macro_coin_line_vertical:
            output = "macro_coin_line_vertical";
            break;
        case MacroPresets::macro_coin_ring_horizontal_flying:
            output = "macro_coin_ring_horizontal_flying";
            break;
        case MacroPresets::macro_coin_ring_vertical:
            output = "macro_coin_ring_vertical";
            break;
        case MacroPresets::macro_coin_arrow_flying:
            output = "macro_coin_arrow_flying";
            break;
        case MacroPresets::macro_hidden_star_trigger:
            output = "macro_hidden_star_trigger";
            break;
        case MacroPresets::macro_empty_15:
            output = "macro_empty_15";
            break;
        case MacroPresets::macro_empty_16:
            output = "macro_empty_16";
            break;
        case MacroPresets::macro_empty_17:
            output = "macro_empty_17";
            break;
        case MacroPresets::macro_empty_18:
            output = "macro_empty_18";
            break;
        case MacroPresets::macro_empty_19:
            output = "macro_empty_19";
            break;
        case MacroPresets::macro_fake_star:
            output = "macro_fake_star";
            break;
        case MacroPresets::macro_wooden_signpost:
            output = "macro_wooden_signpost";
            break;
        case MacroPresets::macro_cannon_closed:
            output = "macro_cannon_closed";
            break;
        case MacroPresets::macro_bobomb_buddy_opens_cannon:
            output = "macro_bobomb_buddy_opens_cannon";
            break;
        case MacroPresets::macro_butterfly:
            output = "macro_butterfly";
            break;
        case MacroPresets::macro_bouncing_fireball_copy:
            output = "macro_bouncing_fireball_copy";
            break;
        case MacroPresets::macro_fish_group_3:
            output = "macro_fish_group_3";
            break;
        case MacroPresets::macro_fish_group:
            output = "macro_fish_group";
            break;
        case MacroPresets::macro_unknown_28:
            output = "macro_unknown_28";
            break;
        case MacroPresets::macro_hidden_1up_in_pole:
            output = "macro_hidden_1up_in_pole";
            break;
        case MacroPresets::macro_huge_goomba:
            output = "macro_huge_goomba";
            break;
        case MacroPresets::macro_tiny_goomba:
            output = "macro_tiny_goomba";
            break;
        case MacroPresets::macro_goomba_triplet_spawner:
            output = "macro_goomba_triplet_spawner";
            break;
        case MacroPresets::macro_goomba_quintuplet_spawner:
            output = "macro_goomba_quintuplet_spawner";
            break;
        case MacroPresets::macro_sign_on_wall:
            output = "macro_sign_on_wall";
            break;
        case MacroPresets::macro_chuckya:
            output = "macro_chuckya";
            break;
        case MacroPresets::macro_cannon_open:
            output = "macro_cannon_open";
            break;
        case MacroPresets::macro_goomba:
            output = "macro_goomba";
            break;
        case MacroPresets::macro_homing_amp:
            output = "macro_homing_amp";
            break;
        case MacroPresets::macro_circling_amp:
            output = "macro_circling_amp";
            break;
        case MacroPresets::macro_unknown_40:
            output = "macro_unknown_40";
            break;
        case MacroPresets::macro_unknown_41:
            output = "macro_unknown_41";
            break;
        case MacroPresets::macro_free_bowling_ball:
            output = "macro_free_bowling_ball";
            break;
        case MacroPresets::macro_snufit:
            output = "macro_snufit";
            break;
        case MacroPresets::macro_recovery_heart:
            output = "macro_recovery_heart";
            break;
        case MacroPresets::macro_1up_sliding:
            output = "macro_1up_sliding";
            break;
        case MacroPresets::macro_1up:
            output = "macro_1up";
            break;
        case MacroPresets::macro_1up_jump_on_approach:
            output = "macro_1up_jump_on_approach";
            break;
        case MacroPresets::macro_hidden_1up:
            output = "macro_hidden_1up";
            break;
        case MacroPresets::macro_hidden_1up_trigger:
            output = "macro_hidden_1up_trigger";
            break;
        case MacroPresets::macro_1up_2:
            output = "macro_1up_2";
            break;
        case MacroPresets::macro_1up_3:
            output = "macro_1up_3";
            break;
        case MacroPresets::macro_empty_52:
            output = "macro_empty_52";
            break;
        case MacroPresets::macro_blue_coin_switch:
            output = "macro_blue_coin_switch";
            break;
        case MacroPresets::macro_hidden_blue_coin:
            output = "macro_hidden_blue_coin";
            break;
        case MacroPresets::macro_wing_cap_switch:
            output = "macro_wing_cap_switch";
            break;
        case MacroPresets::macro_metal_cap_switch:
            output = "macro_metal_cap_switch";
            break;
        case MacroPresets::macro_vanish_cap_switch:
            output = "macro_vanish_cap_switch";
            break;
        case MacroPresets::macro_yellow_cap_switch:
            output = "macro_yellow_cap_switch";
            break;
        case MacroPresets::macro_unknown_59:
            output = "macro_unknown_59";
            break;
        case MacroPresets::macro_box_wing_cap:
            output = "macro_box_wing_cap";
            break;
        case MacroPresets::macro_box_metal_cap:
            output = "macro_box_metal_cap";
            break;
        case MacroPresets::macro_box_vanish_cap:
            output = "macro_box_vanish_cap";
            break;
        case MacroPresets::macro_box_koopa_shell:
            output = "macro_box_koopa_shell";
            break;
        case MacroPresets::macro_box_one_coin:
            output = "macro_box_one_coin";
            break;
        case MacroPresets::macro_box_three_coins:
            output = "macro_box_three_coins";
            break;
        case MacroPresets::macro_box_ten_coins:
            output = "macro_box_ten_coins";
            break;
        case MacroPresets::macro_box_1up:
            output = "macro_box_1up";
            break;
        case MacroPresets::macro_box_star_1:
            output = "macro_box_star_1";
            break;
        case MacroPresets::macro_breakable_box_no_coins:
            output = "macro_breakable_box_no_coins";
            break;
        case MacroPresets::macro_breakable_box_three_coins:
            output = "macro_breakable_box_three_coins";
            break;
        case MacroPresets::macro_pushable_metal_box:
            output = "macro_pushable_metal_box";
            break;
        case MacroPresets::macro_breakable_box_small:
            output = "macro_breakable_box_small";
            break;
        case MacroPresets::macro_floor_switch_hidden_objects:
            output = "macro_floor_switch_hidden_objects";
            break;
        case MacroPresets::macro_hidden_box:
            output = "macro_hidden_box";
            break;
        case MacroPresets::macro_hidden_object_2:
            output = "macro_hidden_object_2";
            break;
        case MacroPresets::macro_hidden_object_3:
            output = "macro_hidden_object_3";
            break;
        case MacroPresets::macro_breakable_box_giant:
            output = "macro_breakable_box_giant";
            break;
        case MacroPresets::macro_koopa_shell_underwater:
            output = "macro_koopa_shell_underwater";
            break;
        case MacroPresets::macro_box_1up_running_away:
            output = "macro_box_1up_running_away";
            break;
        case MacroPresets::macro_empty_80:
            output = "macro_empty_80";
            break;
        case MacroPresets::macro_bullet_bill_cannon:
            output = "macro_bullet_bill_cannon";
            break;
        case MacroPresets::macro_heave_ho:
            output = "macro_heave_ho";
            break;
        case MacroPresets::macro_empty_83:
            output = "macro_empty_83";
            break;
        case MacroPresets::macro_thwomp:
            output = "macro_thwomp";
            break;
        case MacroPresets::macro_fire_spitter:
            output = "macro_fire_spitter";
            break;
        case MacroPresets::macro_fire_fly_guy:
            output = "macro_fire_fly_guy";
            break;
        case MacroPresets::macro_jumping_box:
            output = "macro_jumping_box";
            break;
        case MacroPresets::macro_butterfly_triplet:
            output = "macro_butterfly_triplet";
            break;
        case MacroPresets::macro_butterfly_triplet_2:
            output = "macro_butterfly_triplet_2";
            break;
        case MacroPresets::macro_empty_90:
            output = "macro_empty_90";
            break;
        case MacroPresets::macro_empty_91:
            output = "macro_empty_91";
            break;
        case MacroPresets::macro_empty_92:
            output = "macro_empty_92";
            break;
        case MacroPresets::macro_bully:
            output = "macro_bully";
            break;
        case MacroPresets::macro_bully_2:
            output = "macro_bully_2";
            break;
        case MacroPresets::macro_empty_95:
            output = "macro_empty_95";
            break;
        case MacroPresets::macro_unknown_96:
            output = "macro_unknown_96";
            break;
        case MacroPresets::macro_bouncing_fireball:
            output = "macro_bouncing_fireball";
            break;
        case MacroPresets::macro_flamethrower:
            output = "macro_flamethrower";
            break;
        case MacroPresets::macro_empty_99:
            output = "macro_empty_99";
            break;
        case MacroPresets::macro_empty_100:
            output = "macro_empty_100";
            break;
        case MacroPresets::macro_empty_101:
            output = "macro_empty_101";
            break;
        case MacroPresets::macro_empty_102:
            output = "macro_empty_102";
            break;
        case MacroPresets::macro_empty_103:
            output = "macro_empty_103";
            break;
        case MacroPresets::macro_empty_104:
            output = "macro_empty_104";
            break;
        case MacroPresets::macro_empty_105:
            output = "macro_empty_105";
            break;
        case MacroPresets::macro_wooden_post:
            output = "macro_wooden_post";
            break;
        case MacroPresets::macro_water_bomb_spawner:
            output = "macro_water_bomb_spawner";
            break;
        case MacroPresets::macro_enemy_lakitu:
            output = "macro_enemy_lakitu";
            break;
        case MacroPresets::macro_bob_koopa_the_quick:
            output = "macro_bob_koopa_the_quick";
            break;
        case MacroPresets::macro_koopa_race_endpoint:
            output = "macro_koopa_race_endpoint";
            break;
        case MacroPresets::macro_bobomb:
            output = "macro_bobomb";
            break;
        case MacroPresets::macro_water_bomb_cannon_copy:
            output = "macro_water_bomb_cannon_copy";
            break;
        case MacroPresets::macro_bobomb_buddy_opens_cannon_copy:
            output = "macro_bobomb_buddy_opens_cannon_copy";
            break;
        case MacroPresets::macro_water_bomb_cannon:
            output = "macro_water_bomb_cannon";
            break;
        case MacroPresets::macro_bobomb_still:
            output = "macro_bobomb_still";
            break;
        case MacroPresets::macro_empty_116:
            output = "macro_empty_116";
            break;
        case MacroPresets::macro_empty_117:
            output = "macro_empty_117";
            break;
        case MacroPresets::macro_empty_118:
            output = "macro_empty_118";
            break;
        case MacroPresets::macro_empty_119:
            output = "macro_empty_119";
            break;
        case MacroPresets::macro_empty_120:
            output = "macro_empty_120";
            break;
        case MacroPresets::macro_empty_121:
            output = "macro_empty_121";
            break;
        case MacroPresets::macro_empty_122:
            output = "macro_empty_122";
            break;
        case MacroPresets::macro_unknown_123:
            output = "macro_unknown_123";
            break;
        case MacroPresets::macro_empty_124:
            output = "macro_empty_124";
            break;
        case MacroPresets::macro_unagi:
            output = "macro_unagi";
            break;
        case MacroPresets::macro_sushi:
            output = "macro_sushi";
            break;
        case MacroPresets::macro_empty_127:
            output = "macro_empty_127";
            break;
        case MacroPresets::macro_empty_128:
            output = "macro_empty_128";
            break;
        case MacroPresets::macro_empty_129:
            output = "macro_empty_129";
            break;
        case MacroPresets::macro_empty_130:
            output = "macro_empty_130";
            break;
        case MacroPresets::macro_empty_131:
            output = "macro_empty_131";
            break;
        case MacroPresets::macro_empty_132:
            output = "macro_empty_132";
            break;
        case MacroPresets::macro_empty_133:
            output = "macro_empty_133";
            break;
        case MacroPresets::macro_empty_134:
            output = "macro_empty_134";
            break;
        case MacroPresets::macro_empty_135:
            output = "macro_empty_135";
            break;
        case MacroPresets::macro_empty_136:
            output = "macro_empty_136";
            break;
        case MacroPresets::macro_unknown_137:
            output = "macro_unknown_137";
            break;
        case MacroPresets::macro_tornado:
            output = "macro_tornado";
            break;
        case MacroPresets::macro_pokey:
            output = "macro_pokey";
            break;
        case MacroPresets::macro_pokey_copy:
            output = "macro_pokey_copy";
            break;
        case MacroPresets::macro_tox_box:
            output = "macro_tox_box";
            break;
        case MacroPresets::macro_empty_142:
            output = "macro_empty_142";
            break;
        case MacroPresets::macro_empty_143:
            output = "macro_empty_143";
            break;
        case MacroPresets::macro_empty_144:
            output = "macro_empty_144";
            break;
        case MacroPresets::macro_empty_145:
            output = "macro_empty_145";
            break;
        case MacroPresets::macro_empty_146:
            output = "macro_empty_146";
            break;
        case MacroPresets::macro_empty_147:
            output = "macro_empty_147";
            break;
        case MacroPresets::macro_empty_148:
            output = "macro_empty_148";
            break;
        case MacroPresets::macro_empty_149:
            output = "macro_empty_149";
            break;
        case MacroPresets::macro_empty_150:
            output = "macro_empty_150";
            break;
        case MacroPresets::macro_monty_mole_2:
            output = "macro_monty_mole_2";
            break;
        case MacroPresets::macro_monty_mole:
            output = "macro_monty_mole";
            break;
        case MacroPresets::macro_monty_mole_hole:
            output = "macro_monty_mole_hole";
            break;
        case MacroPresets::macro_fly_guy:
            output = "macro_fly_guy";
            break;
        case MacroPresets::macro_empty_155:
            output = "macro_empty_155";
            break;
        case MacroPresets::macro_wiggler:
            output = "macro_wiggler";
            break;
        case MacroPresets::macro_empty_157:
            output = "macro_empty_157";
            break;
        case MacroPresets::macro_empty_158:
            output = "macro_empty_158";
            break;
        case MacroPresets::macro_empty_159:
            output = "macro_empty_159";
            break;
        case MacroPresets::macro_empty_160:
            output = "macro_empty_160";
            break;
        case MacroPresets::macro_empty_161:
            output = "macro_empty_161";
            break;
        case MacroPresets::macro_empty_162:
            output = "macro_empty_162";
            break;
        case MacroPresets::macro_empty_163:
            output = "macro_empty_163";
            break;
        case MacroPresets::macro_empty_164:
            output = "macro_empty_164";
            break;
        case MacroPresets::macro_spindrift:
            output = "macro_spindrift";
            break;
        case MacroPresets::macro_mr_blizzard:
            output = "macro_mr_blizzard";
            break;
        case MacroPresets::macro_mr_blizzard_copy:
            output = "macro_mr_blizzard_copy";
            break;
        case MacroPresets::macro_empty_168:
            output = "macro_empty_168";
            break;
        case MacroPresets::macro_small_penguin:
            output = "macro_small_penguin";
            break;
        case MacroPresets::macro_tuxies_mother:
            output = "macro_tuxies_mother";
            break;
        case MacroPresets::macro_tuxies_mother_copy:
            output = "macro_tuxies_mother_copy";
            break;
        case MacroPresets::macro_mr_blizzard_2:
            output = "macro_mr_blizzard_2";
            break;
        case MacroPresets::macro_empty_173:
            output = "macro_empty_173";
            break;
        case MacroPresets::macro_empty_174:
            output = "macro_empty_174";
            break;
        case MacroPresets::macro_empty_175:
            output = "macro_empty_175";
            break;
        case MacroPresets::macro_empty_176:
            output = "macro_empty_176";
            break;
        case MacroPresets::macro_empty_177:
            output = "macro_empty_177";
            break;
        case MacroPresets::macro_empty_178:
            output = "macro_empty_178";
            break;
        case MacroPresets::macro_empty_179:
            output = "macro_empty_179";
            break;
        case MacroPresets::macro_empty_180:
            output = "macro_empty_180";
            break;
        case MacroPresets::macro_empty_181:
            output = "macro_empty_181";
            break;
        case MacroPresets::macro_empty_182:
            output = "macro_empty_182";
            break;
        case MacroPresets::macro_empty_183:
            output = "macro_empty_183";
            break;
        case MacroPresets::macro_empty_184:
            output = "macro_empty_184";
            break;
        case MacroPresets::macro_empty_185:
            output = "macro_empty_185";
            break;
        case MacroPresets::macro_empty_186:
            output = "macro_empty_186";
            break;
        case MacroPresets::macro_empty_187:
            output = "macro_empty_187";
            break;
        case MacroPresets::macro_empty_188:
            output = "macro_empty_188";
            break;
        case MacroPresets::macro_haunted_chair_copy:
            output = "macro_haunted_chair_copy";
            break;
        case MacroPresets::macro_haunted_chair:
            output = "macro_haunted_chair";
            break;
        case MacroPresets::macro_haunted_chair_copy2:
            output = "macro_haunted_chair_copy2";
            break;
        case MacroPresets::macro_boo:
            output = "macro_boo";
            break;
        case MacroPresets::macro_boo_copy:
            output = "macro_boo_copy";
            break;
        case MacroPresets::macro_boo_group:
            output = "macro_boo_group";
            break;
        case MacroPresets::macro_boo_with_cage:
            output = "macro_boo_with_cage";
            break;
        case MacroPresets::macro_beta_key:
            output = "macro_beta_key";
            break;
        case MacroPresets::macro_empty_197:
            output = "macro_empty_197";
            break;
        case MacroPresets::macro_empty_198:
            output = "macro_empty_198";
            break;
        case MacroPresets::macro_empty_199:
            output = "macro_empty_199";
            break;
        case MacroPresets::macro_empty_200:
            output = "macro_empty_200";
            break;
        case MacroPresets::macro_empty_201:
            output = "macro_empty_201";
            break;
        case MacroPresets::macro_empty_202:
            output = "macro_empty_202";
            break;
        case MacroPresets::macro_empty_203:
            output = "macro_empty_203";
            break;
        case MacroPresets::macro_empty_204:
            output = "macro_empty_204";
            break;
        case MacroPresets::macro_empty_205:
            output = "macro_empty_205";
            break;
        case MacroPresets::macro_empty_206:
            output = "macro_empty_206";
            break;
        case MacroPresets::macro_empty_207:
            output = "macro_empty_207";
            break;
        case MacroPresets::macro_empty_208:
            output = "macro_empty_208";
            break;
        case MacroPresets::macro_empty_209:
            output = "macro_empty_209";
            break;
        case MacroPresets::macro_empty_210:
            output = "macro_empty_210";
            break;
        case MacroPresets::macro_empty_211:
            output = "macro_empty_211";
            break;
        case MacroPresets::macro_empty_212:
            output = "macro_empty_212";
            break;
        case MacroPresets::macro_empty_213:
            output = "macro_empty_213";
            break;
        case MacroPresets::macro_empty_214:
            output = "macro_empty_214";
            break;
        case MacroPresets::macro_empty_215:
            output = "macro_empty_215";
            break;
        case MacroPresets::macro_empty_216:
            output = "macro_empty_216";
            break;
        case MacroPresets::macro_empty_217:
            output = "macro_empty_217";
            break;
        case MacroPresets::macro_empty_218:
            output = "macro_empty_218";
            break;
        case MacroPresets::macro_empty_219:
            output = "macro_empty_219";
            break;
        case MacroPresets::macro_empty_220:
            output = "macro_empty_220";
            break;
        case MacroPresets::macro_empty_221:
            output = "macro_empty_221";
            break;
        case MacroPresets::macro_empty_222:
            output = "macro_empty_222";
            break;
        case MacroPresets::macro_empty_223:
            output = "macro_empty_223";
            break;
        case MacroPresets::macro_empty_224:
            output = "macro_empty_224";
            break;
        case MacroPresets::macro_empty_225:
            output = "macro_empty_225";
            break;
        case MacroPresets::macro_empty_226:
            output = "macro_empty_226";
            break;
        case MacroPresets::macro_empty_227:
            output = "macro_empty_227";
            break;
        case MacroPresets::macro_empty_228:
            output = "macro_empty_228";
            break;
        case MacroPresets::macro_empty_229:
            output = "macro_empty_229";
            break;
        case MacroPresets::macro_empty_230:
            output = "macro_empty_230";
            break;
        case MacroPresets::macro_empty_231:
            output = "macro_empty_231";
            break;
        case MacroPresets::macro_empty_232:
            output = "macro_empty_232";
            break;
        case MacroPresets::macro_empty_233:
            output = "macro_empty_233";
            break;
        case MacroPresets::macro_chirp_chirp:
            output = "macro_chirp_chirp";
            break;
        case MacroPresets::macro_seaweed_bundle:
            output = "macro_seaweed_bundle";
            break;
        case MacroPresets::macro_beta_chest:
            output = "macro_beta_chest";
            break;
        case MacroPresets::macro_water_mine:
            output = "macro_water_mine";
            break;
        case MacroPresets::macro_fish_group_4:
            output = "macro_fish_group_4";
            break;
        case MacroPresets::macro_fish_group_2:
            output = "macro_fish_group_2";
            break;
        case MacroPresets::macro_jet_stream_ring_spawner:
            output = "macro_jet_stream_ring_spawner";
            break;
        case MacroPresets::macro_jet_stream_ring_spawner_copy:
            output = "macro_jet_stream_ring_spawner_copy";
            break;
        case MacroPresets::macro_skeeter:
            output = "macro_skeeter";
            break;
        case MacroPresets::macro_clam_shell:
            output = "macro_clam_shell";
            break;
        case MacroPresets::macro_empty_244:
            output = "macro_empty_244";
            break;
        case MacroPresets::macro_empty_245:
            output = "macro_empty_245";
            break;
        case MacroPresets::macro_empty_246:
            output = "macro_empty_246";
            break;
        case MacroPresets::macro_empty_247:
            output = "macro_empty_247";
            break;
        case MacroPresets::macro_empty_248:
            output = "macro_empty_248";
            break;
        case MacroPresets::macro_empty_249:
            output = "macro_empty_249";
            break;
        case MacroPresets::macro_empty_250:
            output = "macro_empty_250";
            break;
        case MacroPresets::macro_ukiki:
            output = "macro_ukiki";
            break;
        case MacroPresets::macro_ukiki_2:
            output = "macro_ukiki_2";
            break;
        case MacroPresets::macro_piranha_plant:
            output = "macro_piranha_plant";
            break;
        case MacroPresets::macro_empty_254:
            output = "macro_empty_254";
            break;
        case MacroPresets::macro_whomp:
            output = "macro_whomp";
            break;
        case MacroPresets::macro_chain_chomp:
            output = "macro_chain_chomp";
            break;
        case MacroPresets::macro_empty_257:
            output = "macro_empty_257";
            break;
        case MacroPresets::macro_koopa:
            output = "macro_koopa";
            break;
        case MacroPresets::macro_koopa_shellless:
            output = "macro_koopa_shellless";
            break;
        case MacroPresets::macro_wooden_post_copy:
            output = "macro_wooden_post_copy";
            break;
        case MacroPresets::macro_fire_piranha_plant:
            output = "macro_fire_piranha_plant";
            break;
        case MacroPresets::macro_fire_piranha_plant_2:
            output = "macro_fire_piranha_plant_2";
            break;
        case MacroPresets::macro_thi_koopa_the_quick:
            output = "macro_thi_koopa_the_quick";
            break;
        case MacroPresets::macro_empty_264:
            output = "macro_empty_264";
            break;
        case MacroPresets::macro_empty_265:
            output = "macro_empty_265";
            break;
        case MacroPresets::macro_empty_266:
            output = "macro_empty_266";
            break;
        case MacroPresets::macro_empty_267:
            output = "macro_empty_267";
            break;
        case MacroPresets::macro_empty_268:
            output = "macro_empty_268";
            break;
        case MacroPresets::macro_empty_269:
            output = "macro_empty_269";
            break;
        case MacroPresets::macro_empty_270:
            output = "macro_empty_270";
            break;
        case MacroPresets::macro_empty_271:
            output = "macro_empty_271";
            break;
        case MacroPresets::macro_empty_272:
            output = "macro_empty_272";
            break;
        case MacroPresets::macro_empty_273:
            output = "macro_empty_273";
            break;
        case MacroPresets::macro_empty_274:
            output = "macro_empty_274";
            break;
        case MacroPresets::macro_empty_275:
            output = "macro_empty_275";
            break;
        case MacroPresets::macro_empty_276:
            output = "macro_empty_276";
            break;
        case MacroPresets::macro_empty_277:
            output = "macro_empty_277";
            break;
        case MacroPresets::macro_empty_278:
            output = "macro_empty_278";
            break;
        case MacroPresets::macro_empty_279:
            output = "macro_empty_279";
            break;
        case MacroPresets::macro_empty_280:
            output = "macro_empty_280";
            break;
        case MacroPresets::macro_moneybag:
            output = "macro_moneybag";
            break;
        case MacroPresets::macro_empty_282:
            output = "macro_empty_282";
            break;
        case MacroPresets::macro_empty_283:
            output = "macro_empty_283";
            break;
        case MacroPresets::macro_empty_284:
            output = "macro_empty_284";
            break;
        case MacroPresets::macro_empty_285:
            output = "macro_empty_285";
            break;
        case MacroPresets::macro_empty_286:
            output = "macro_empty_286";
            break;
        case MacroPresets::macro_empty_287:
            output = "macro_empty_287";
            break;
        case MacroPresets::macro_empty_288:
            output = "macro_empty_288";
            break;
        case MacroPresets::macro_swoop:
            output = "macro_swoop";
            break;
        case MacroPresets::macro_swoop_2:
            output = "macro_swoop_2";
            break;
        case MacroPresets::macro_mr_i:
            output = "macro_mr_i";
            break;
        case MacroPresets::macro_scuttlebug_spawner:
            output = "macro_scuttlebug_spawner";
            break;
        case MacroPresets::macro_scuttlebug:
            output = "macro_scuttlebug";
            break;
        case MacroPresets::macro_empty_294:
            output = "macro_empty_294";
            break;
        case MacroPresets::macro_empty_295:
            output = "macro_empty_295";
            break;
        case MacroPresets::macro_empty_296:
            output = "macro_empty_296";
            break;
        case MacroPresets::macro_empty_297:
            output = "macro_empty_297";
            break;
        case MacroPresets::macro_empty_298:
            output = "macro_empty_298";
            break;
        case MacroPresets::macro_empty_299:
            output = "macro_empty_299";
            break;
        case MacroPresets::macro_empty_300:
            output = "macro_empty_300";
            break;
        case MacroPresets::macro_empty_301:
            output = "macro_empty_301";
            break;
        case MacroPresets::macro_empty_302:
            output = "macro_empty_302";
            break;
        case MacroPresets::macro_unknown_303:
            output = "macro_unknown_303";
            break;
        case MacroPresets::macro_empty_304:
            output = "macro_empty_304";
            break;
        case MacroPresets::macro_empty_305:
            output = "macro_empty_305";
            break;
        case MacroPresets::macro_empty_306:
            output = "macro_empty_306";
            break;
        case MacroPresets::macro_empty_307:
            output = "macro_empty_307";
            break;
        case MacroPresets::macro_empty_308:
            output = "macro_empty_308";
            break;
        case MacroPresets::macro_empty_309:
            output = "macro_empty_309";
            break;
        case MacroPresets::macro_empty_310:
            output = "macro_empty_310";
            break;
        case MacroPresets::macro_empty_311:
            output = "macro_empty_311";
            break;
        case MacroPresets::macro_empty_312:
            output = "macro_empty_312";
            break;
        case MacroPresets::macro_ttc_rotating_cube:
            output = "macro_ttc_rotating_cube";
            break;
        case MacroPresets::macro_ttc_rotating_prism:
            output = "macro_ttc_rotating_prism";
            break;
        case MacroPresets::macro_ttc_pendulum:
            output = "macro_ttc_pendulum";
            break;
        case MacroPresets::macro_ttc_large_treadmill:
            output = "macro_ttc_large_treadmill";
            break;
        case MacroPresets::macro_ttc_small_treadmill:
            output = "macro_ttc_small_treadmill";
            break;
        case MacroPresets::macro_ttc_push_block:
            output = "macro_ttc_push_block";
            break;
        case MacroPresets::macro_ttc_rotating_hexagon:
            output = "macro_ttc_rotating_hexagon";
            break;
        case MacroPresets::macro_ttc_rotating_triangle:
            output = "macro_ttc_rotating_triangle";
            break;
        case MacroPresets::macro_ttc_pit_block:
            output = "macro_ttc_pit_block";
            break;
        case MacroPresets::macro_ttc_pit_block_2:
            output = "macro_ttc_pit_block_2";
            break;
        case MacroPresets::macro_ttc_elevator_platform:
            output = "macro_ttc_elevator_platform";
            break;
        case MacroPresets::macro_ttc_clock_hand:
            output = "macro_ttc_clock_hand";
            break;
        case MacroPresets::macro_ttc_spinner:
            output = "macro_ttc_spinner";
            break;
        case MacroPresets::macro_ttc_small_gear:
            output = "macro_ttc_small_gear";
            break;
        case MacroPresets::macro_ttc_large_gear:
            output = "macro_ttc_large_gear";
            break;
        case MacroPresets::macro_ttc_large_treadmill_2:
            output = "macro_ttc_large_treadmill_2";
            break;
        case MacroPresets::macro_ttc_small_treadmill_2:
            output = "macro_ttc_small_treadmill_2";
            break;
        case MacroPresets::macro_empty_330:
            output = "macro_empty_330";
            break;
        case MacroPresets::macro_empty_331:
            output = "macro_empty_331";
            break;
        case MacroPresets::macro_empty_332:
            output = "macro_empty_332";
            break;
        case MacroPresets::macro_empty_333:
            output = "macro_empty_333";
            break;
        case MacroPresets::macro_empty_334:
            output = "macro_empty_334";
            break;
        case MacroPresets::macro_empty_335:
            output = "macro_empty_335";
            break;
        case MacroPresets::macro_empty_336:
            output = "macro_empty_336";
            break;
        case MacroPresets::macro_empty_337:
            output = "macro_empty_337";
            break;
        case MacroPresets::macro_empty_338:
            output = "macro_empty_338";
            break;
        case MacroPresets::macro_box_star_2:
            output = "macro_box_star_2";
            break;
        case MacroPresets::macro_box_star_3:
            output = "macro_box_star_3";
            break;
        case MacroPresets::macro_box_star_4:
            output = "macro_box_star_4";
            break;
        case MacroPresets::macro_box_star_5:
            output = "macro_box_star_5";
            break;
        case MacroPresets::macro_box_star_6:
            output = "macro_box_star_6";
            break;
        case MacroPresets::macro_empty_344:
            output = "macro_empty_344";
            break;
        case MacroPresets::macro_empty_345:
            output = "macro_empty_345";
            break;
        case MacroPresets::macro_empty_346:
            output = "macro_empty_346";
            break;
        case MacroPresets::macro_empty_347:
            output = "macro_empty_347";
            break;
        case MacroPresets::macro_empty_348:
            output = "macro_empty_348";
            break;
        case MacroPresets::macro_empty_349:
            output = "macro_empty_349";
            break;
        case MacroPresets::macro_bits_sliding_platform:
            output = "macro_bits_sliding_platform";
            break;
        case MacroPresets::macro_bits_twin_sliding_platforms:
            output = "macro_bits_twin_sliding_platforms";
            break;
        case MacroPresets::macro_bits_unknown_352:
            output = "macro_bits_unknown_352";
            break;
        case MacroPresets::macro_bits_octagonal_platform:
            output = "macro_bits_octagonal_platform";
            break;
        case MacroPresets::macro_bits_staircase:
            output = "macro_bits_staircase";
            break;
        case MacroPresets::macro_empty_355:
            output = "macro_empty_355";
            break;
        case MacroPresets::macro_empty_356:
            output = "macro_empty_356";
            break;
        case MacroPresets::macro_bits_ferris_wheel_axle:
            output = "macro_bits_ferris_wheel_axle";
            break;
        case MacroPresets::macro_bits_arrow_platform:
            output = "macro_bits_arrow_platform";
            break;
        case MacroPresets::macro_bits_seesaw_platform:
            output = "macro_bits_seesaw_platform";
            break;
        case MacroPresets::macro_bits_tilting_w_platform:
            output = "macro_bits_tilting_w_platform";
            break;
        case MacroPresets::macro_empty_361:
            output = "macro_empty_361";
            break;
        case MacroPresets::macro_empty_362:
            output = "macro_empty_362";
            break;
        case MacroPresets::macro_empty_363:
            output = "macro_empty_363";
            break;
        case MacroPresets::macro_empty_364:
            output = "macro_empty_364";
            break;
        case MacroPresets::macro_empty_365:
            output = "macro_empty_365";
            break;
    }
    return out << output;
}