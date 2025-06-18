#pragma once

#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace FZX {
enum class GhostType {
    /* 0 */ GHOST_NONE,
    /* 1 */ GHOST_PLAYER,
    /* 2 */ GHOST_STAFF,
    /* 3 */ GHOST_CELEBRITY,
    /* 4 */ GHOST_CHAMP,
};

enum class Character {
    /* 00 */ CAPTAIN_FALCON,
    /* 01 */ DR_STEWART,
    /* 02 */ PICO,
    /* 03 */ SAMURAI_GOROH,
    /* 04 */ JODY_SUMMER,
    /* 05 */ MIGHTY_GAZELLE,
    /* 06 */ MR_EAD,
    /* 07 */ BABA,
    /* 08 */ OCTOMAN,
    /* 09 */ GOMAR_AND_SHIOH,
    /* 10 */ KATE_ALEN,
    /* 11 */ ROGER_BUSTER,
    /* 12 */ JAMES_MCCLOUD,
    /* 13 */ LEON,
    /* 14 */ ANTONIO_GUSTER,
    /* 15 */ BLACK_SHADOW,
    /* 16 */ MICHAEL_CHAIN,
    /* 17 */ JACK_LEVIN,
    /* 18 */ SUPER_ARROW,
    /* 19 */ MRS_ARROW,
    /* 20 */ JOHN_TANAKA,
    /* 21 */ BEASTMAN,
    /* 22 */ ZODA,
    /* 23 */ DR_CLASH,
    /* 24 */ SILVER_NEELSEN,
    /* 25 */ BIO_REX,
    /* 26 */ DRAQ,
    /* 27 */ BILLY,
    /* 28 */ THE_SKULL,
    /* 29 */ BLOOD_FALCON,
};

enum class CustomType {
    /* 0 */ CUSTOM_MACHINE_DEFAULT,
    /* 1 */ CUSTOM_MACHINE_EDITED,
    /* 2 */ CUSTOM_MACHINE_SUPER_FALCON,
    /* 3 */ CUSTOM_MACHINE_SUPER_STINGRAY,
    /* 4 */ CUSTOM_MACHINE_SUPER_CAT,
};

enum class FrontType {
    /* 0 */ FRONT_0,
    /* 1 */ FRONT_1,
    /* 2 */ FRONT_2,
    /* 3 */ FRONT_3,
    /* 4 */ FRONT_4,
    /* 5 */ FRONT_5,
    /* 6 */ FRONT_6,
};

enum class RearType {
    /* 0 */ REAR_0,
    /* 1 */ REAR_1,
    /* 2 */ REAR_2,
    /* 3 */ REAR_3,
    /* 4 */ REAR_4,
    /* 5 */ REAR_5,
    /* 6 */ REAR_6,
};

enum class WingType {
    /* 0 */ WING_NONE,
    /* 1 */ WING_1,
    /* 2 */ WING_2,
    /* 3 */ WING_3,
    /* 4 */ WING_4,
    /* 5 */ WING_5,
    /* 6 */ WING_6,
};

enum class Logo {
    /* 0 */ LOGO_SHIELD,
    /* 1 */ LOGO_ARROW_PLANE,
    /* 2 */ LOGO_CIRCLE,
    /* 3 */ LOGO_SKULL,
    /* 4 */ LOGO_YELLOW_GREEN,
    /* 5 */ LOGO_KANJI,
    /* 6 */ LOGO_X,
    /* 7 */ LOGO_N64,
};

enum class Decal {
    /* 0 */ DECAL_STRIPE,
    /* 1 */ DECAL_THIN_STRIPE,
    /* 2 */ DECAL_DOUBLE_STRIPE,
    /* 3 */ DECAL_TRIPLE_STRIPE_UNEVEN,
    /* 4 */ DECAL_BLOCK,
};


inline std::ostream& operator<<(std::ostream& out, const GhostType& ghostType) {
    std::string output;
    switch (ghostType) {
        case GhostType::GHOST_NONE:
            output = "GHOST_NONE";
            break;
        case GhostType::GHOST_PLAYER:
            output = "GHOST_PLAYER";
            break;
        case GhostType::GHOST_STAFF:
            output = "GHOST_STAFF";
            break;
        case GhostType::GHOST_CELEBRITY:
            output = "GHOST_CELEBRITY";
            break;
        case GhostType::GHOST_CHAMP:
            output = "GHOST_CHAMP";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const Character& character) {
    std::string output;
    switch (character) {
        case Character::CAPTAIN_FALCON:
            output = "CAPTAIN_FALCON";
            break;
        case Character::DR_STEWART:
            output = "DR_STEWART";
            break;
        case Character::PICO:
            output = "PICO";
            break;
        case Character::SAMURAI_GOROH:
            output = "SAMURAI_GOROH";
            break;
        case Character::JODY_SUMMER:
            output = "JODY_SUMMER";
            break;
        case Character::MIGHTY_GAZELLE:
            output = "MIGHTY_GAZELLE";
            break;
        case Character::MR_EAD:
            output = "MR_EAD";
            break;
        case Character::BABA:
            output = "BABA";
            break;
        case Character::OCTOMAN:
            output = "OCTOMAN";
            break;
        case Character::GOMAR_AND_SHIOH:
            output = "GOMAR_AND_SHIOH";
            break;
        case Character::KATE_ALEN:
            output = "KATE_ALEN";
            break;
        case Character::ROGER_BUSTER:
            output = "ROGER_BUSTER";
            break;
        case Character::JAMES_MCCLOUD:
            output = "JAMES_MCCLOUD";
            break;
        case Character::LEON:
            output = "LEON";
            break;
        case Character::ANTONIO_GUSTER:
            output = "ANTONIO_GUSTER";
            break;
        case Character::BLACK_SHADOW:
            output = "BLACK_SHADOW";
            break;
        case Character::MICHAEL_CHAIN:
            output = "MICHAEL_CHAIN";
            break;
        case Character::JACK_LEVIN:
            output = "JACK_LEVIN";
            break;
        case Character::SUPER_ARROW:
            output = "SUPER_ARROW";
            break;
        case Character::MRS_ARROW:
            output = "MRS_ARROW";
            break;
        case Character::JOHN_TANAKA:
            output = "JOHN_TANAKA";
            break;
        case Character::BEASTMAN:
            output = "BEASTMAN";
            break;
        case Character::ZODA:
            output = "ZODA";
            break;
        case Character::DR_CLASH:
            output = "DR_CLASH";
            break;
        case Character::SILVER_NEELSEN:
            output = "SILVER_NEELSEN";
            break;
        case Character::BIO_REX:
            output = "BIO_REX";
            break;
        case Character::DRAQ:
            output = "DRAQ";
            break;
        case Character::BILLY:
            output = "BILLY";
            break;
        case Character::THE_SKULL:
            output = "THE_SKULL";
            break;
        case Character::BLOOD_FALCON:
            output = "BLOOD_FALCON";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const CustomType& customType) {
    std::string output;
    switch (customType) {
        case CustomType::CUSTOM_MACHINE_DEFAULT:
            output = "CUSTOM_MACHINE_DEFAULT";
            break;
        case CustomType::CUSTOM_MACHINE_EDITED:
            output = "CUSTOM_MACHINE_EDITED";
            break;
        case CustomType::CUSTOM_MACHINE_SUPER_FALCON:
            output = "CUSTOM_MACHINE_SUPER_FALCON";
            break;
        case CustomType::CUSTOM_MACHINE_SUPER_STINGRAY:
            output = "CUSTOM_MACHINE_SUPER_STINGRAY";
            break;
        case CustomType::CUSTOM_MACHINE_SUPER_CAT:
            output = "CUSTOM_MACHINE_SUPER_CAT";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const FrontType& frontType) {
    std::string output;
    switch (frontType) {
        case FrontType::FRONT_0:
            output = "FRONT_0";
            break;
        case FrontType::FRONT_1:
            output = "FRONT_1";
            break;
        case FrontType::FRONT_2:
            output = "FRONT_2";
            break;
        case FrontType::FRONT_3:
            output = "FRONT_3";
            break;
        case FrontType::FRONT_4:
            output = "FRONT_4";
            break;
        case FrontType::FRONT_5:
            output = "FRONT_5";
            break;
        case FrontType::FRONT_6:
            output = "FRONT_6";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const RearType rearType) {
    std::string output;
    switch (rearType) {
        case RearType::REAR_0:
            output = "REAR_0";
            break;
        case RearType::REAR_1:
            output = "REAR_1";
            break;
        case RearType::REAR_2:
            output = "REAR_2";
            break;
        case RearType::REAR_3:
            output = "REAR_3";
            break;
        case RearType::REAR_4:
            output = "REAR_4";
            break;
        case RearType::REAR_5:
            output = "REAR_5";
            break;
        case RearType::REAR_6:
            output = "REAR_6";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const WingType& wingType) {
    std::string output;
    switch (wingType) {
        case WingType::WING_NONE:
            output = "WING_NONE";
            break;
        case WingType::WING_1:
            output = "WING_1";
            break;
        case WingType::WING_2:
            output = "WING_2";
            break;
        case WingType::WING_3:
            output = "WING_3";
            break;
        case WingType::WING_4:
            output = "WING_4";
            break;
        case WingType::WING_5:
            output = "WING_5";
            break;
        case WingType::WING_6:
            output = "WING_6";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const Logo& logo) {
    std::string output;
    switch (logo) {
        case Logo::LOGO_SHIELD:
            output = "LOGO_SHIELD";
            break;
        case Logo::LOGO_ARROW_PLANE:
            output = "LOGO_ARROW_PLANE";
            break;
        case Logo::LOGO_CIRCLE:
            output = "LOGO_CIRCLE";
            break;
        case Logo::LOGO_SKULL:
            output = "LOGO_SKULL";
            break;
        case Logo::LOGO_YELLOW_GREEN:
            output = "LOGO_YELLOW_GREEN";
            break;
        case Logo::LOGO_KANJI:
            output = "LOGO_KANJI";
            break;
        case Logo::LOGO_X:
            output = "LOGO_X";
            break;
        case Logo::LOGO_N64:
            output = "LOGO_N64";
            break;
    }
    return out << output;
}

inline std::ostream& operator<<(std::ostream& out, const Decal& decal) {
    std::string output;
    switch (decal) {
        case Decal::DECAL_STRIPE:
            output = "DECAL_STRIPE";
            break;
        case Decal::DECAL_THIN_STRIPE:
            output = "DECAL_THIN_STRIPE";
            break;
        case Decal::DECAL_DOUBLE_STRIPE:
            output = "DECAL_DOUBLE_STRIPE";
            break;
        case Decal::DECAL_TRIPLE_STRIPE_UNEVEN:
            output = "DECAL_TRIPLE_STRIPE_UNEVEN";
            break;
        case Decal::DECAL_BLOCK:
            output = "DECAL_BLOCK";
            break;
    }
    return out << output;
}

}
