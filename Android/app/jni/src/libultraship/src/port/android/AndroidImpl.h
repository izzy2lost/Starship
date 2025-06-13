#pragma once

#include <cstdint>
#include <string>

#include <ImGui/imgui.h>

namespace LUS {

class Android {
  public:
    static void ImGuiProcessEvent(bool wantsTextInput);
    static void adjustGyro(float gyroData[3]);
    static bool IsUsingTouchscreenControls();
    static void EnableTouchArea();
    static void DisableTouchArea();
};
};
