#include "ControllerDefaultMappings.h"
#include "libultraship/libultra/controller.h"

namespace Ship {

ControllerDefaultMappings::ControllerDefaultMappings(
    const std::unordered_map<CONTROLLERBUTTONS_T, std::unordered_set<KbScancode>>& defaultKeyboardKeyToButtonMappings,
    const std::unordered_map<StickIndex, std::vector<std::pair<Direction, KbScancode>>>& defaultKeyboardKeyToAxisDirectionMappings,
    const std::unordered_map<CONTROLLERBUTTONS_T, std::unordered_set<SDL_GameControllerButton>>& defaultSDLButtonToButtonMappings,
    const std::unordered_map<StickIndex, std::vector<std::pair<Direction, SDL_GameControllerButton>>>& defaultSDLButtonToAxisDirectionMappings,
    const std::unordered_map<CONTROLLERBUTTONS_T, std::vector<std::pair<SDL_GameControllerAxis, int32_t>>>& defaultSDLAxisDirectionToButtonMappings,
    const std::unordered_map<StickIndex, std::vector<std::pair<Direction, std::pair<SDL_GameControllerAxis, int32_t>>>>& defaultSDLAxisDirectionToAxisDirectionMappings
) {
    SetDefaultKeyboardKeyToButtonMappings(defaultKeyboardKeyToButtonMappings);
    SetDefaultKeyboardKeyToAxisDirectionMappings(defaultKeyboardKeyToAxisDirectionMappings);
    SetDefaultSDLButtonToButtonMappings(defaultSDLButtonToButtonMappings);
    SetDefaultSDLButtonToAxisDirectionMappings(defaultSDLButtonToAxisDirectionMappings);
    SetDefaultSDLAxisDirectionToButtonMappings(defaultSDLAxisDirectionToButtonMappings);
    SetDefaultSDLAxisDirectionToAxisDirectionMappings(defaultSDLAxisDirectionToAxisDirectionMappings);
}

ControllerDefaultMappings::ControllerDefaultMappings()
    : ControllerDefaultMappings(
        {}, {}, {}, {}, {}, {}
    ) {}

ControllerDefaultMappings::~ControllerDefaultMappings() = default;

// --- KEYBOARD BUTTON MAPPINGS ---

std::unordered_map<CONTROLLERBUTTONS_T, std::unordered_set<KbScancode>>
ControllerDefaultMappings::GetDefaultKeyboardKeyToButtonMappings() {
    return mDefaultKeyboardKeyToButtonMappings;
}

void ControllerDefaultMappings::SetDefaultKeyboardKeyToButtonMappings(
    const std::unordered_map<CONTROLLERBUTTONS_T, std::unordered_set<KbScancode>>& map
) {
    if (!map.empty()) {
        mDefaultKeyboardKeyToButtonMappings = map;
        return;
    }

    mDefaultKeyboardKeyToButtonMappings = {
        { BTN_A,      { KbScancode::LUS_KB_X } },
        { BTN_B,      { KbScancode::LUS_KB_C } },
        { BTN_L,      { KbScancode::LUS_KB_E } },
        { BTN_R,      { KbScancode::LUS_KB_R } },
        { BTN_Z,      { KbScancode::LUS_KB_Z } },
        { BTN_START,  { KbScancode::LUS_KB_SPACE } },
        { BTN_CUP,    { KbScancode::LUS_KB_ARROWKEY_UP } },
        { BTN_CDOWN,  { KbScancode::LUS_KB_ARROWKEY_DOWN } },
        { BTN_CLEFT,  { KbScancode::LUS_KB_ARROWKEY_LEFT } },
        { BTN_CRIGHT, { KbScancode::LUS_KB_ARROWKEY_RIGHT } },
        { BTN_DUP,    { KbScancode::LUS_KB_T } },
        { BTN_DDOWN,  { KbScancode::LUS_KB_G } },
        { BTN_DLEFT,  { KbScancode::LUS_KB_F } },
        { BTN_DRIGHT, { KbScancode::LUS_KB_H } },
    };
}

// --- KEYBOARD AXIS MAPPINGS ---

std::unordered_map<StickIndex, std::vector<std::pair<Direction, KbScancode>>>
ControllerDefaultMappings::GetDefaultKeyboardKeyToAxisDirectionMappings() {
    return mDefaultKeyboardKeyToAxisDirectionMappings;
}

void ControllerDefaultMappings::SetDefaultKeyboardKeyToAxisDirectionMappings(
    const std::unordered_map<StickIndex, std::vector<std::pair<Direction, KbScancode>>>& map
) {
    if (!map.empty()) {
        mDefaultKeyboardKeyToAxisDirectionMappings = map;
        return;
    }

    mDefaultKeyboardKeyToAxisDirectionMappings = {
        { LEFT_STICK, {
            { LEFT,  KbScancode::LUS_KB_A },
            { RIGHT, KbScancode::LUS_KB_D },
            { UP,    KbScancode::LUS_KB_W },
            { DOWN,  KbScancode::LUS_KB_S }
        } }
    };
}

// --- SDL BUTTON TO BUTTON ---

std::unordered_map<CONTROLLERBUTTONS_T, std::unordered_set<SDL_GameControllerButton>>
ControllerDefaultMappings::GetDefaultSDLButtonToButtonMappings() {
    return mDefaultSDLButtonToButtonMappings;
}

void ControllerDefaultMappings::SetDefaultSDLButtonToButtonMappings(
    const std::unordered_map<CONTROLLERBUTTONS_T, std::unordered_set<SDL_GameControllerButton>>& map
) {
    if (!map.empty()) {
        mDefaultSDLButtonToButtonMappings = map;
        return;
    }

    mDefaultSDLButtonToButtonMappings = {
        { BTN_A,     { SDL_CONTROLLER_BUTTON_A } },
        { BTN_B,     { SDL_CONTROLLER_BUTTON_B } },
        { BTN_L,     { SDL_CONTROLLER_BUTTON_LEFTSHOULDER } },
        { BTN_START, { SDL_CONTROLLER_BUTTON_START } },
        { BTN_DUP,   { SDL_CONTROLLER_BUTTON_DPAD_UP } },
        { BTN_DDOWN, { SDL_CONTROLLER_BUTTON_DPAD_DOWN } },
        { BTN_DLEFT, { SDL_CONTROLLER_BUTTON_DPAD_LEFT } },
        { BTN_DRIGHT,{ SDL_CONTROLLER_BUTTON_DPAD_RIGHT } }
    };
}

// --- SDL BUTTON TO AXIS DIRECTION ---

std::unordered_map<StickIndex, std::vector<std::pair<Direction, SDL_GameControllerButton>>>
ControllerDefaultMappings::GetDefaultSDLButtonToAxisDirectionMappings() {
    return mDefaultSDLButtonToAxisDirectionMappings;
}

void ControllerDefaultMappings::SetDefaultSDLButtonToAxisDirectionMappings(
    const std::unordered_map<StickIndex, std::vector<std::pair<Direction, SDL_GameControllerButton>>>& map
) {
    if (!map.empty()) {
        mDefaultSDLButtonToAxisDirectionMappings = map;
        return;
    }
    // Add defaults if needed
}

// --- SDL AXIS TO BUTTON ---

std::unordered_map<CONTROLLERBUTTONS_T, std::vector<std::pair<SDL_GameControllerAxis, int32_t>>>
ControllerDefaultMappings::GetDefaultSDLAxisDirectionToButtonMappings() {
    return mDefaultSDLAxisDirectionToButtonMappings;
}

void ControllerDefaultMappings::SetDefaultSDLAxisDirectionToButtonMappings(
    const std::unordered_map<CONTROLLERBUTTONS_T, std::vector<std::pair<SDL_GameControllerAxis, int32_t>>>& map
) {
    if (!map.empty()) {
        mDefaultSDLAxisDirectionToButtonMappings = map;
        return;
    }

    mDefaultSDLAxisDirectionToButtonMappings = {
        { BTN_R,      { { SDL_CONTROLLER_AXIS_TRIGGERRIGHT, 1 } } },
        { BTN_Z,      { { SDL_CONTROLLER_AXIS_TRIGGERLEFT,  1 } } },
        { BTN_CUP,    { { SDL_CONTROLLER_AXIS_RIGHTY, -1 } } },
        { BTN_CDOWN,  { { SDL_CONTROLLER_AXIS_RIGHTY,  1 } } },
        { BTN_CLEFT,  { { SDL_CONTROLLER_AXIS_RIGHTX, -1 } } },
        { BTN_CRIGHT, { { SDL_CONTROLLER_AXIS_RIGHTX,  1 } } }
    };
}

// --- SDL AXIS TO AXIS DIRECTION ---

std::unordered_map<StickIndex, std::vector<std::pair<Direction, std::pair<SDL_GameControllerAxis, int32_t>>>>
ControllerDefaultMappings::GetDefaultSDLAxisDirectionToAxisDirectionMappings() {
    return mDefaultSDLAxisDirectionToAxisDirectionMappings;
}

void ControllerDefaultMappings::SetDefaultSDLAxisDirectionToAxisDirectionMappings(
    const std::unordered_map<StickIndex, std::vector<std::pair<Direction, std::pair<SDL_GameControllerAxis, int32_t>>>>& map
) {
    if (!map.empty()) {
        mDefaultSDLAxisDirectionToAxisDirectionMappings = map;
        return;
    }

    mDefaultSDLAxisDirectionToAxisDirectionMappings = {
        { LEFT_STICK, {
            { LEFT,  { SDL_CONTROLLER_AXIS_LEFTX,  -1 } },
            { RIGHT, { SDL_CONTROLLER_AXIS_LEFTX,   1 } },
            { UP,    { SDL_CONTROLLER_AXIS_LEFTY,  -1 } },
            { DOWN,  { SDL_CONTROLLER_AXIS_LEFTY,   1 } }
        } },
        { RIGHT_STICK, {
            { LEFT,  { SDL_CONTROLLER_AXIS_RIGHTX, -1 } },
            { RIGHT, { SDL_CONTROLLER_AXIS_RIGHTX,  1 } },
            { UP,    { SDL_CONTROLLER_AXIS_RIGHTY, -1 } },
            { DOWN,  { SDL_CONTROLLER_AXIS_RIGHTY,  1 } }
        } }
    };
}

} // namespace Ship
