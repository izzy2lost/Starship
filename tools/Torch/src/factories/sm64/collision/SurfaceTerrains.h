#pragma once

// Surface Types
enum class SurfaceType {
    SURFACE_DEFAULT                  = 0x0000, // Environment default
    SURFACE_BURNING                  = 0x0001, // Lava / Frostbite (in SL), but is used mostly for Lava
    SURFACE_0004                     = 0x0004, // Unused, has no function and has parameters
    SURFACE_HANGABLE                 = 0x0005, // Ceiling that Mario can climb on
    SURFACE_SLOW                     = 0x0009, // Slow down Mario, unused
    SURFACE_DEATH_PLANE              = 0x000A, // Death floor
    SURFACE_CLOSE_CAMERA             = 0x000B, // Close camera
    SURFACE_WATER                    = 0x000D, // Water, has no action, used on some waterboxes below
    SURFACE_FLOWING_WATER            = 0x000E, // Water (flowing), has parameters
    SURFACE_INTANGIBLE               = 0x0012, // Intangible (Separates BBH mansion from merry-go-round, for room usage)
    SURFACE_VERY_SLIPPERY            = 0x0013, // Very slippery, mostly used for slides
    SURFACE_SLIPPERY                 = 0x0014, // Slippery
    SURFACE_NOT_SLIPPERY             = 0x0015, // Non-slippery, climbable
    SURFACE_TTM_VINES                = 0x0016, // TTM vines, has no action defined
    SURFACE_MGR_MUSIC                = 0x001A, // Plays the Merry go round music, see handle_merry_go_round_music in bbh_merry_go_round.inc.c for more details
    SURFACE_INSTANT_WARP_1B          = 0x001B, // Instant warp to another area, used to warp between areas in WDW and the endless stairs to warp back
    SURFACE_INSTANT_WARP_1C          = 0x001C, // Instant warp to another area, used to warp between areas in WDW
    SURFACE_INSTANT_WARP_1D          = 0x001D, // Instant warp to another area, used to warp between areas in DDD, SSL and TTM
    SURFACE_INSTANT_WARP_1E          = 0x001E, // Instant warp to another area, used to warp between areas in DDD, SSL and TTM
    SURFACE_SHALLOW_QUICKSAND        = 0x0021, // Shallow Quicksand (depth of 10 units)
    SURFACE_DEEP_QUICKSAND           = 0x0022, // Quicksand (lethal, slow, depth of 160 units)
    SURFACE_INSTANT_QUICKSAND        = 0x0023, // Quicksand (lethal, instant)
    SURFACE_DEEP_MOVING_QUICKSAND    = 0x0024, // Moving quicksand (flowing, depth of 160 units)
    SURFACE_SHALLOW_MOVING_QUICKSAND = 0x0025, // Moving quicksand (flowing, depth of 25 units)
    SURFACE_QUICKSAND                = 0x0026, // Moving quicksand (60 units)
    SURFACE_MOVING_QUICKSAND         = 0x0027, // Moving quicksand (flowing, depth of 60 units)
    SURFACE_WALL_MISC                = 0x0028, // Used for some walls, Cannon to adjust the camera, and some objects like Warp Pipe
    SURFACE_NOISE_DEFAULT            = 0x0029, // Default floor with noise
    SURFACE_NOISE_SLIPPERY           = 0x002A, // Slippery floor with noise
    SURFACE_HORIZONTAL_WIND          = 0x002C, // Horizontal wind, has parameters
    SURFACE_INSTANT_MOVING_QUICKSAND = 0x002D, // Quicksand (lethal, flowing)
    SURFACE_ICE                      = 0x002E, // Slippery Ice, in snow levels and THI's water floor
    SURFACE_LOOK_UP_WARP             = 0x002F, // Look up and warp (Wing cap entrance)
    SURFACE_HARD                     = 0x0030, // Hard floor (Always has fall damage)
    SURFACE_WARP                     = 0x0032, // Surface warp
    SURFACE_TIMER_START              = 0x0033, // Timer start (Peach's secret slide)
    SURFACE_TIMER_END                = 0x0034, // Timer stop (Peach's secret slide)
    SURFACE_HARD_SLIPPERY            = 0x0035, // Hard and slippery (Always has fall damage)
    SURFACE_HARD_VERY_SLIPPERY       = 0x0036, // Hard and very slippery (Always has fall damage)
    SURFACE_HARD_NOT_SLIPPERY        = 0x0037, // Hard and Non-slippery (Always has fall damage)
    SURFACE_VERTICAL_WIND            = 0x0038, // Death at bottom with vertical wind
    SURFACE_BOSS_FIGHT_CAMERA        = 0x0065, // Wide camera for BoB and WF bosses
    SURFACE_CAMERA_FREE_ROAM         = 0x0066, // Free roam camera for THI and TTC
    SURFACE_THI3_WALLKICK            = 0x0068, // Surface where there's a wall kick section in THI 3rd area, has no action defined
    SURFACE_CAMERA_8_DIR             = 0x0069, // Surface that enables far camera for platforms, used in THI
    SURFACE_CAMERA_MIDDLE            = 0x006E, // Surface camera that returns to the middle, used on the 4 pillars of SSL
    SURFACE_CAMERA_ROTATE_RIGHT      = 0x006F, // Surface camera that rotates to the right (Bowser 1 & THI)
    SURFACE_CAMERA_ROTATE_LEFT       = 0x0070, // Surface camera that rotates to the left (BoB & TTM)
    SURFACE_CAMERA_BOUNDARY          = 0x0072, // Intangible Area, only used to restrict camera movement
    SURFACE_NOISE_VERY_SLIPPERY_73   = 0x0073, // Very slippery floor with noise, unused
    SURFACE_NOISE_VERY_SLIPPERY_74   = 0x0074, // Very slippery floor with noise, unused
    SURFACE_NOISE_VERY_SLIPPERY      = 0x0075, // Very slippery floor with noise, used in CCM
    SURFACE_NO_CAM_COLLISION         = 0x0076, // Surface with no cam collision flag
    SURFACE_NO_CAM_COLLISION_77      = 0x0077, // Surface with no cam collision flag, unused
    SURFACE_NO_CAM_COL_VERY_SLIPPERY = 0x0078, // Surface with no cam collision flag, very slippery with noise (THI)
    SURFACE_NO_CAM_COL_SLIPPERY      = 0x0079, // Surface with no cam collision flag, slippery with noise (CCM, PSS and TTM slides)
    SURFACE_SWITCH                   = 0x007A, // Surface with no cam collision flag, non-slippery with noise, used by switches and Dorrie
    SURFACE_VANISH_CAP_WALLS         = 0x007B, // Vanish cap walls, pass through them with Vanish Cap
    SURFACE_PAINTING_WOBBLE_A6       = 0x00A6, // Painting wobble (BoB Left)
    SURFACE_PAINTING_WOBBLE_A7       = 0x00A7, // Painting wobble (BoB Middle)
    SURFACE_PAINTING_WOBBLE_A8       = 0x00A8, // Painting wobble (BoB Right)
    SURFACE_PAINTING_WOBBLE_A9       = 0x00A9, // Painting wobble (CCM Left)
    SURFACE_PAINTING_WOBBLE_AA       = 0x00AA, // Painting wobble (CCM Middle)
    SURFACE_PAINTING_WOBBLE_AB       = 0x00AB, // Painting wobble (CCM Right)
    SURFACE_PAINTING_WOBBLE_AC       = 0x00AC, // Painting wobble (WF Left)
    SURFACE_PAINTING_WOBBLE_AD       = 0x00AD, // Painting wobble (WF Middle)
    SURFACE_PAINTING_WOBBLE_AE       = 0x00AE, // Painting wobble (WF Right)
    SURFACE_PAINTING_WOBBLE_AF       = 0x00AF, // Painting wobble (JRB Left)
    SURFACE_PAINTING_WOBBLE_B0       = 0x00B0, // Painting wobble (JRB Middle)
    SURFACE_PAINTING_WOBBLE_B1       = 0x00B1, // Painting wobble (JRB Right)
    SURFACE_PAINTING_WOBBLE_B2       = 0x00B2, // Painting wobble (LLL Left)
    SURFACE_PAINTING_WOBBLE_B3       = 0x00B3, // Painting wobble (LLL Middle)
    SURFACE_PAINTING_WOBBLE_B4       = 0x00B4, // Painting wobble (LLL Right)
    SURFACE_PAINTING_WOBBLE_B5       = 0x00B5, // Painting wobble (SSL Left)
    SURFACE_PAINTING_WOBBLE_B6       = 0x00B6, // Painting wobble (SSL Middle)
    SURFACE_PAINTING_WOBBLE_B7       = 0x00B7, // Painting wobble (SSL Right)
    SURFACE_PAINTING_WOBBLE_B8       = 0x00B8, // Painting wobble (Unused - Left)
    SURFACE_PAINTING_WOBBLE_B9       = 0x00B9, // Painting wobble (Unused - Middle)
    SURFACE_PAINTING_WOBBLE_BA       = 0x00BA, // Painting wobble (Unused - Right)
    SURFACE_PAINTING_WOBBLE_BB       = 0x00BB, // Painting wobble (DDD - Left), makes the painting wobble if touched
    SURFACE_PAINTING_WOBBLE_BC       = 0x00BC, // Painting wobble (Unused, DDD - Middle)
    SURFACE_PAINTING_WOBBLE_BD       = 0x00BD, // Painting wobble (Unused, DDD - Right)
    SURFACE_PAINTING_WOBBLE_BE       = 0x00BE, // Painting wobble (WDW Left)
    SURFACE_PAINTING_WOBBLE_BF       = 0x00BF, // Painting wobble (WDW Middle)
    SURFACE_PAINTING_WOBBLE_C0       = 0x00C0, // Painting wobble (WDW Right)
    SURFACE_PAINTING_WOBBLE_C1       = 0x00C1, // Painting wobble (THI Tiny - Left)
    SURFACE_PAINTING_WOBBLE_C2       = 0x00C2, // Painting wobble (THI Tiny - Middle)
    SURFACE_PAINTING_WOBBLE_C3       = 0x00C3, // Painting wobble (THI Tiny - Right)
    SURFACE_PAINTING_WOBBLE_C4       = 0x00C4, // Painting wobble (TTM Left)
    SURFACE_PAINTING_WOBBLE_C5       = 0x00C5, // Painting wobble (TTM Middle)
    SURFACE_PAINTING_WOBBLE_C6       = 0x00C6, // Painting wobble (TTM Right)
    SURFACE_PAINTING_WOBBLE_C7       = 0x00C7, // Painting wobble (Unused, TTC - Left)
    SURFACE_PAINTING_WOBBLE_C8       = 0x00C8, // Painting wobble (Unused, TTC - Middle)
    SURFACE_PAINTING_WOBBLE_C9       = 0x00C9, // Painting wobble (Unused, TTC - Right)
    SURFACE_PAINTING_WOBBLE_CA       = 0x00CA, // Painting wobble (Unused, SL - Left)
    SURFACE_PAINTING_WOBBLE_CB       = 0x00CB, // Painting wobble (Unused, SL - Middle)
    SURFACE_PAINTING_WOBBLE_CC       = 0x00CC, // Painting wobble (Unused, SL - Right)
    SURFACE_PAINTING_WOBBLE_CD       = 0x00CD, // Painting wobble (THI Huge - Left)
    SURFACE_PAINTING_WOBBLE_CE       = 0x00CE, // Painting wobble (THI Huge - Middle)
    SURFACE_PAINTING_WOBBLE_CF       = 0x00CF, // Painting wobble (THI Huge - Right)
    SURFACE_PAINTING_WOBBLE_D0       = 0x00D0, // Painting wobble (HMC & COTMC - Left), makes the painting wobble if touched
    SURFACE_PAINTING_WOBBLE_D1       = 0x00D1, // Painting wobble (Unused, HMC & COTMC - Middle)
    SURFACE_PAINTING_WOBBLE_D2       = 0x00D2, // Painting wobble (Unused, HMC & COTMC - Right)
    SURFACE_PAINTING_WARP_D3         = 0x00D3, // Painting warp (BoB Left)
    SURFACE_PAINTING_WARP_D4         = 0x00D4, // Painting warp (BoB Middle)
    SURFACE_PAINTING_WARP_D5         = 0x00D5, // Painting warp (BoB Right)
    SURFACE_PAINTING_WARP_D6         = 0x00D6, // Painting warp (CCM Left)
    SURFACE_PAINTING_WARP_D7         = 0x00D7, // Painting warp (CCM Middle)
    SURFACE_PAINTING_WARP_D8         = 0x00D8, // Painting warp (CCM Right)
    SURFACE_PAINTING_WARP_D9         = 0x00D9, // Painting warp (WF Left)
    SURFACE_PAINTING_WARP_DA         = 0x00DA, // Painting warp (WF Middle)
    SURFACE_PAINTING_WARP_DB         = 0x00DB, // Painting warp (WF Right)
    SURFACE_PAINTING_WARP_DC         = 0x00DC, // Painting warp (JRB Left)
    SURFACE_PAINTING_WARP_DD         = 0x00DD, // Painting warp (JRB Middle)
    SURFACE_PAINTING_WARP_DE         = 0x00DE, // Painting warp (JRB Right)
    SURFACE_PAINTING_WARP_DF         = 0x00DF, // Painting warp (LLL Left)
    SURFACE_PAINTING_WARP_E0         = 0x00E0, // Painting warp (LLL Middle)
    SURFACE_PAINTING_WARP_E1         = 0x00E1, // Painting warp (LLL Right)
    SURFACE_PAINTING_WARP_E2         = 0x00E2, // Painting warp (SSL Left)
    SURFACE_PAINTING_WARP_E3         = 0x00E3, // Painting warp (SSL Medium)
    SURFACE_PAINTING_WARP_E4         = 0x00E4, // Painting warp (SSL Right)
    SURFACE_PAINTING_WARP_E5         = 0x00E5, // Painting warp (Unused - Left)
    SURFACE_PAINTING_WARP_E6         = 0x00E6, // Painting warp (Unused - Medium)
    SURFACE_PAINTING_WARP_E7         = 0x00E7, // Painting warp (Unused - Right)
    SURFACE_PAINTING_WARP_E8         = 0x00E8, // Painting warp (DDD - Left)
    SURFACE_PAINTING_WARP_E9         = 0x00E9, // Painting warp (DDD - Middle)
    SURFACE_PAINTING_WARP_EA         = 0x00EA, // Painting warp (DDD - Right)
    SURFACE_PAINTING_WARP_EB         = 0x00EB, // Painting warp (WDW Left)
    SURFACE_PAINTING_WARP_EC         = 0x00EC, // Painting warp (WDW Middle)
    SURFACE_PAINTING_WARP_ED         = 0x00ED, // Painting warp (WDW Right)
    SURFACE_PAINTING_WARP_EE         = 0x00EE, // Painting warp (THI Tiny - Left)
    SURFACE_PAINTING_WARP_EF         = 0x00EF, // Painting warp (THI Tiny - Middle)
    SURFACE_PAINTING_WARP_F0         = 0x00F0, // Painting warp (THI Tiny - Right)
    SURFACE_PAINTING_WARP_F1         = 0x00F1, // Painting warp (TTM Left)
    SURFACE_PAINTING_WARP_F2         = 0x00F2, // Painting warp (TTM Middle)
    SURFACE_PAINTING_WARP_F3         = 0x00F3, // Painting warp (TTM Right)
    SURFACE_TTC_PAINTING_1           = 0x00F4, // Painting warp (TTC Left)
    SURFACE_TTC_PAINTING_2           = 0x00F5, // Painting warp (TTC Medium)
    SURFACE_TTC_PAINTING_3           = 0x00F6, // Painting warp (TTC Right)
    SURFACE_PAINTING_WARP_F7         = 0x00F7, // Painting warp (SL Left)
    SURFACE_PAINTING_WARP_F8         = 0x00F8, // Painting warp (SL Middle)
    SURFACE_PAINTING_WARP_F9         = 0x00F9, // Painting warp (SL Right)
    SURFACE_PAINTING_WARP_FA         = 0x00FA, // Painting warp (THI Tiny - Left)
    SURFACE_PAINTING_WARP_FB         = 0x00FB, // Painting warp (THI Tiny - Middle)
    SURFACE_PAINTING_WARP_FC         = 0x00FC, // Painting warp (THI Tiny - Right)
    SURFACE_WOBBLING_WARP            = 0x00FD, // Pool warp (HMC & DDD)
    SURFACE_TRAPDOOR                 = 0x00FF, // Bowser Left trapdoor, has no action defined
};

inline std::ostream& operator<<(std::ostream& out, const SurfaceType& surfaceType) {
    std::string output;
    switch (surfaceType) {
        case SurfaceType::SURFACE_DEFAULT:
            output = "SURFACE_DEFAULT";
            break;
        case SurfaceType::SURFACE_BURNING:
            output = "SURFACE_BURNING";
            break;
        case SurfaceType::SURFACE_0004:
            output = "SURFACE_0004";
            break;
        case SurfaceType::SURFACE_HANGABLE:
            output = "SURFACE_HANGABLE";
            break;
        case SurfaceType::SURFACE_SLOW:
            output = "SURFACE_SLOW";
            break;
        case SurfaceType::SURFACE_DEATH_PLANE:
            output = "SURFACE_DEATH_PLANE";
            break;
        case SurfaceType::SURFACE_CLOSE_CAMERA:
            output = "SURFACE_CLOSE_CAMERA";
            break;
        case SurfaceType::SURFACE_WATER:
            output = "SURFACE_WATER";
            break;
        case SurfaceType::SURFACE_FLOWING_WATER:
            output = "SURFACE_FLOWING_WATER";
            break;
        case SurfaceType::SURFACE_INTANGIBLE:
            output = "SURFACE_INTANGIBLE";
            break;
        case SurfaceType::SURFACE_VERY_SLIPPERY:
            output = "SURFACE_VERY_SLIPPERY";
            break;
        case SurfaceType::SURFACE_SLIPPERY:
            output = "SURFACE_SLIPPERY";
            break;
        case SurfaceType::SURFACE_NOT_SLIPPERY:
            output = "SURFACE_NOT_SLIPPERY";
            break;
        case SurfaceType::SURFACE_TTM_VINES:
            output = "SURFACE_TTM_VINES";
            break;
        case SurfaceType::SURFACE_MGR_MUSIC:
            output = "SURFACE_MGR_MUSIC";
            break;
        case SurfaceType::SURFACE_INSTANT_WARP_1B:
            output = "SURFACE_INSTANT_WARP_1B";
            break;
        case SurfaceType::SURFACE_INSTANT_WARP_1C:
            output = "SURFACE_INSTANT_WARP_1C";
            break;
        case SurfaceType::SURFACE_INSTANT_WARP_1D:
            output = "SURFACE_INSTANT_WARP_1D";
            break;
        case SurfaceType::SURFACE_INSTANT_WARP_1E:
            output = "SURFACE_INSTANT_WARP_1E";
            break;
        case SurfaceType::SURFACE_SHALLOW_QUICKSAND:
            output = "SURFACE_SHALLOW_QUICKSAND";
            break;
        case SurfaceType::SURFACE_DEEP_QUICKSAND:
            output = "SURFACE_DEEP_QUICKSAND";
            break;
        case SurfaceType::SURFACE_INSTANT_QUICKSAND:
            output = "SURFACE_INSTANT_QUICKSAND";
            break;
        case SurfaceType::SURFACE_DEEP_MOVING_QUICKSAND:
            output = "SURFACE_DEEP_MOVING_QUICKSAND";
            break;
        case SurfaceType::SURFACE_SHALLOW_MOVING_QUICKSAND:
            output = "SURFACE_SHALLOW_MOVING_QUICKSAND";
            break;
        case SurfaceType::SURFACE_QUICKSAND:
            output = "SURFACE_QUICKSAND";
            break;
        case SurfaceType::SURFACE_MOVING_QUICKSAND:
            output = "SURFACE_MOVING_QUICKSAND";
            break;
        case SurfaceType::SURFACE_WALL_MISC:
            output = "SURFACE_WALL_MISC";
            break;
        case SurfaceType::SURFACE_NOISE_DEFAULT:
            output = "SURFACE_NOISE_DEFAULT";
            break;
        case SurfaceType::SURFACE_NOISE_SLIPPERY:
            output = "SURFACE_NOISE_SLIPPERY";
            break;
        case SurfaceType::SURFACE_HORIZONTAL_WIND:
            output = "SURFACE_HORIZONTAL_WIND";
            break;
        case SurfaceType::SURFACE_INSTANT_MOVING_QUICKSAND:
            output = "SURFACE_INSTANT_MOVING_QUICKSAND";
            break;
        case SurfaceType::SURFACE_ICE:
            output = "SURFACE_ICE";
            break;
        case SurfaceType::SURFACE_LOOK_UP_WARP:
            output = "SURFACE_LOOK_UP_WARP";
            break;
        case SurfaceType::SURFACE_HARD:
            output = "SURFACE_HARD";
            break;
        case SurfaceType::SURFACE_WARP:
            output = "SURFACE_WARP";
            break;
        case SurfaceType::SURFACE_TIMER_START:
            output = "SURFACE_TIMER_START";
            break;
        case SurfaceType::SURFACE_TIMER_END:
            output = "SURFACE_TIMER_END";
            break;
        case SurfaceType::SURFACE_HARD_SLIPPERY:
            output = "SURFACE_HARD_SLIPPERY";
            break;
        case SurfaceType::SURFACE_HARD_VERY_SLIPPERY:
            output = "SURFACE_HARD_VERY_SLIPPERY";
            break;
        case SurfaceType::SURFACE_HARD_NOT_SLIPPERY:
            output = "SURFACE_HARD_NOT_SLIPPERY";
            break;
        case SurfaceType::SURFACE_VERTICAL_WIND:
            output = "SURFACE_VERTICAL_WIND";
            break;
        case SurfaceType::SURFACE_BOSS_FIGHT_CAMERA:
            output = "SURFACE_BOSS_FIGHT_CAMERA";
            break;
        case SurfaceType::SURFACE_CAMERA_FREE_ROAM:
            output = "SURFACE_CAMERA_FREE_ROAM";
            break;
        case SurfaceType::SURFACE_THI3_WALLKICK:
            output = "SURFACE_THI3_WALLKICK";
            break;
        case SurfaceType::SURFACE_CAMERA_8_DIR:
            output = "SURFACE_CAMERA_8_DIR";
            break;
        case SurfaceType::SURFACE_CAMERA_MIDDLE:
            output = "SURFACE_CAMERA_MIDDLE";
            break;
        case SurfaceType::SURFACE_CAMERA_ROTATE_RIGHT:
            output = "SURFACE_CAMERA_ROTATE_RIGHT";
            break;
        case SurfaceType::SURFACE_CAMERA_ROTATE_LEFT:
            output = "SURFACE_CAMERA_ROTATE_LEFT";
            break;
        case SurfaceType::SURFACE_CAMERA_BOUNDARY:
            output = "SURFACE_CAMERA_BOUNDARY";
            break;
        case SurfaceType::SURFACE_NOISE_VERY_SLIPPERY_73:
            output = "SURFACE_NOISE_VERY_SLIPPERY_73";
            break;
        case SurfaceType::SURFACE_NOISE_VERY_SLIPPERY_74:
            output = "SURFACE_NOISE_VERY_SLIPPERY_74";
            break;
        case SurfaceType::SURFACE_NOISE_VERY_SLIPPERY:
            output = "SURFACE_NOISE_VERY_SLIPPERY";
            break;
        case SurfaceType::SURFACE_NO_CAM_COLLISION:
            output = "SURFACE_NO_CAM_COLLISION";
            break;
        case SurfaceType::SURFACE_NO_CAM_COLLISION_77:
            output = "SURFACE_NO_CAM_COLLISION_77";
            break;
        case SurfaceType::SURFACE_NO_CAM_COL_VERY_SLIPPERY:
            output = "SURFACE_NO_CAM_COL_VERY_SLIPPERY";
            break;
        case SurfaceType::SURFACE_NO_CAM_COL_SLIPPERY:
            output = "SURFACE_NO_CAM_COL_SLIPPERY";
            break;
        case SurfaceType::SURFACE_SWITCH:
            output = "SURFACE_SWITCH";
            break;
        case SurfaceType::SURFACE_VANISH_CAP_WALLS:
            output = "SURFACE_VANISH_CAP_WALLS";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_A6:
            output = "SURFACE_PAINTING_WOBBLE_A6";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_A7:
            output = "SURFACE_PAINTING_WOBBLE_A7";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_A8:
            output = "SURFACE_PAINTING_WOBBLE_A8";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_A9:
            output = "SURFACE_PAINTING_WOBBLE_A9";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_AA:
            output = "SURFACE_PAINTING_WOBBLE_AA";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_AB:
            output = "SURFACE_PAINTING_WOBBLE_AB";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_AC:
            output = "SURFACE_PAINTING_WOBBLE_AC";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_AD:
            output = "SURFACE_PAINTING_WOBBLE_AD";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_AE:
            output = "SURFACE_PAINTING_WOBBLE_AE";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_AF:
            output = "SURFACE_PAINTING_WOBBLE_AF";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B0:
            output = "SURFACE_PAINTING_WOBBLE_B0";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B1:
            output = "SURFACE_PAINTING_WOBBLE_B1";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B2:
            output = "SURFACE_PAINTING_WOBBLE_B2";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B3:
            output = "SURFACE_PAINTING_WOBBLE_B3";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B4:
            output = "SURFACE_PAINTING_WOBBLE_B4";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B5:
            output = "SURFACE_PAINTING_WOBBLE_B5";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B6:
            output = "SURFACE_PAINTING_WOBBLE_B6";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B7:
            output = "SURFACE_PAINTING_WOBBLE_B7";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B8:
            output = "SURFACE_PAINTING_WOBBLE_B8";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_B9:
            output = "SURFACE_PAINTING_WOBBLE_B9";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_BA:
            output = "SURFACE_PAINTING_WOBBLE_BA";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_BB:
            output = "SURFACE_PAINTING_WOBBLE_BB";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_BC:
            output = "SURFACE_PAINTING_WOBBLE_BC";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_BD:
            output = "SURFACE_PAINTING_WOBBLE_BD";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_BE:
            output = "SURFACE_PAINTING_WOBBLE_BE";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_BF:
            output = "SURFACE_PAINTING_WOBBLE_BF";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C0:
            output = "SURFACE_PAINTING_WOBBLE_C0";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C1:
            output = "SURFACE_PAINTING_WOBBLE_C1";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C2:
            output = "SURFACE_PAINTING_WOBBLE_C2";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C3:
            output = "SURFACE_PAINTING_WOBBLE_C3";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C4:
            output = "SURFACE_PAINTING_WOBBLE_C4";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C5:
            output = "SURFACE_PAINTING_WOBBLE_C5";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C6:
            output = "SURFACE_PAINTING_WOBBLE_C6";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C7:
            output = "SURFACE_PAINTING_WOBBLE_C7";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C8:
            output = "SURFACE_PAINTING_WOBBLE_C8";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_C9:
            output = "SURFACE_PAINTING_WOBBLE_C9";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_CA:
            output = "SURFACE_PAINTING_WOBBLE_CA";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_CB:
            output = "SURFACE_PAINTING_WOBBLE_CB";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_CC:
            output = "SURFACE_PAINTING_WOBBLE_CC";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_CD:
            output = "SURFACE_PAINTING_WOBBLE_CD";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_CE:
            output = "SURFACE_PAINTING_WOBBLE_CE";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_CF:
            output = "SURFACE_PAINTING_WOBBLE_CF";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_D0:
            output = "SURFACE_PAINTING_WOBBLE_D0";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_D1:
            output = "SURFACE_PAINTING_WOBBLE_D1";
            break;
        case SurfaceType::SURFACE_PAINTING_WOBBLE_D2:
            output = "SURFACE_PAINTING_WOBBLE_D2";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D3:
            output = "SURFACE_PAINTING_WARP_D3";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D4:
            output = "SURFACE_PAINTING_WARP_D4";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D5:
            output = "SURFACE_PAINTING_WARP_D5";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D6:
            output = "SURFACE_PAINTING_WARP_D6";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D7:
            output = "SURFACE_PAINTING_WARP_D7";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D8:
            output = "SURFACE_PAINTING_WARP_D8";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_D9:
            output = "SURFACE_PAINTING_WARP_D9";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_DA:
            output = "SURFACE_PAINTING_WARP_DA";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_DB:
            output = "SURFACE_PAINTING_WARP_DB";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_DC:
            output = "SURFACE_PAINTING_WARP_DC";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_DD:
            output = "SURFACE_PAINTING_WARP_DD";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_DE:
            output = "SURFACE_PAINTING_WARP_DE";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_DF:
            output = "SURFACE_PAINTING_WARP_DF";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E0:
            output = "SURFACE_PAINTING_WARP_E0";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E1:
            output = "SURFACE_PAINTING_WARP_E1";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E2:
            output = "SURFACE_PAINTING_WARP_E2";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E3:
            output = "SURFACE_PAINTING_WARP_E3";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E4:
            output = "SURFACE_PAINTING_WARP_E4";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E5:
            output = "SURFACE_PAINTING_WARP_E5";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E6:
            output = "SURFACE_PAINTING_WARP_E6";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E7:
            output = "SURFACE_PAINTING_WARP_E7";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E8:
            output = "SURFACE_PAINTING_WARP_E8";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_E9:
            output = "SURFACE_PAINTING_WARP_E9";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_EA:
            output = "SURFACE_PAINTING_WARP_EA";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_EB:
            output = "SURFACE_PAINTING_WARP_EB";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_EC:
            output = "SURFACE_PAINTING_WARP_EC";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_ED:
            output = "SURFACE_PAINTING_WARP_ED";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_EE:
            output = "SURFACE_PAINTING_WARP_EE";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_EF:
            output = "SURFACE_PAINTING_WARP_EF";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F0:
            output = "SURFACE_PAINTING_WARP_F0";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F1:
            output = "SURFACE_PAINTING_WARP_F1";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F2:
            output = "SURFACE_PAINTING_WARP_F2";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F3:
            output = "SURFACE_PAINTING_WARP_F3";
            break;
        case SurfaceType::SURFACE_TTC_PAINTING_1:
            output = "SURFACE_TTC_PAINTING_1";
            break;
        case SurfaceType::SURFACE_TTC_PAINTING_2:
            output = "SURFACE_TTC_PAINTING_2";
            break;
        case SurfaceType::SURFACE_TTC_PAINTING_3:
            output = "SURFACE_TTC_PAINTING_3";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F7:
            output = "SURFACE_PAINTING_WARP_F7";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F8:
            output = "SURFACE_PAINTING_WARP_F8";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_F9:
            output = "SURFACE_PAINTING_WARP_F9";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_FA:
            output = "SURFACE_PAINTING_WARP_FA";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_FB:
            output = "SURFACE_PAINTING_WARP_FB";
            break;
        case SurfaceType::SURFACE_PAINTING_WARP_FC:
            output = "SURFACE_PAINTING_WARP_FC";
            break;
        case SurfaceType::SURFACE_WOBBLING_WARP:
            output = "SURFACE_WOBBLING_WARP";
            break;
        case SurfaceType::SURFACE_TRAPDOOR:
            output = "SURFACE_TRAPDOOR";
            break;
        default:
            throw std::runtime_error("Unknown Surface Type");
    }
    return out << output;
}

#define SURFACE_IS_QUICKSAND(cmd)     (cmd >= 0x21 && cmd < 0x28)   // Doesn't include SURFACE_INSTANT_MOVING_QUICKSAND
#define SURFACE_IS_NOT_HARD(cmd)      (cmd != SurfaceType::SURFACE_HARD && \
                                     !(cmd >= 0x35 && cmd <= 0x37))
#define SURFACE_IS_PAINTING_WARP(cmd) (cmd >= 0xD3 && cmd < 0xFD)

#define SURFACE_CLASS_DEFAULT       0x0000
#define SURFACE_CLASS_VERY_SLIPPERY 0x0013
#define SURFACE_CLASS_SLIPPERY      0x0014
#define SURFACE_CLASS_NOT_SLIPPERY  0x0015

#define SURFACE_FLAG_DYNAMIC          (1 << 0)
#define SURFACE_FLAG_NO_CAM_COLLISION (1 << 1)
#define SURFACE_FLAG_X_PROJECTION     (1 << 3)

// These are effectively unique "surface" types like those defined higher
// And they are used as collision commands to load certain functions
#define TERRAIN_LOAD_VERTICES    0x0040 // Begins vertices list for collision triangles
#define TERRAIN_LOAD_CONTINUE    0x0041 // Stop loading vertices but continues to load other collision commands
#define TERRAIN_LOAD_END         0x0042 // End the collision list
#define TERRAIN_LOAD_OBJECTS     0x0043 // Loads in certain objects for level start
#define TERRAIN_LOAD_ENVIRONMENT 0x0044 // Loads water/HMC gas

#define TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(cmd)  (cmd < 0x40)
#define TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(cmd) (cmd >= 0x65)

// Terrain types defined by the level script command terrain_type (cmd_31)
#define TERRAIN_GRASS  0x0000
#define TERRAIN_STONE  0x0001
#define TERRAIN_SNOW   0x0002
#define TERRAIN_SAND   0x0003
#define TERRAIN_SPOOKY 0x0004
#define TERRAIN_WATER  0x0005
#define TERRAIN_SLIDE  0x0006
#define TERRAIN_MASK   0x0007

// These collision commands are unique "surface" types like those defined higher

// Collision Data Routine Initiate
#define COL_INIT() TERRAIN_LOAD_VERTICES

// Collision Vertices Read Initiate
#define COL_VERTEX_INIT(vtxNum) vtxNum

// Collision Vertex
#define COL_VERTEX(x, y, z) x, y, z

// Collision Tris Initiate
#define COL_TRI_INIT(surfType, triNum) surfType, triNum

// Collision Tri
#define COL_TRI(v1, v2, v3) v1, v2, v3

// Collision Tri With Special Params
#define COL_TRI_SPECIAL(v1, v2, v3, param) v1, v2, v3, param

// Collision Tris Stop Loading
#define COL_TRI_STOP() TERRAIN_LOAD_CONTINUE

// End Collision Data
#define COL_END() TERRAIN_LOAD_END

// Special Object Initiate
#define COL_SPECIAL_INIT(num) TERRAIN_LOAD_OBJECTS, num

// Water Boxes Initiate
#define COL_WATER_BOX_INIT(num) TERRAIN_LOAD_ENVIRONMENT, num

// Water Box
#define COL_WATER_BOX(id, x1, z1, x2, z2, y) id, x1, z1, x2, z2, y
