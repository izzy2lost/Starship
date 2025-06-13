#ifdef __ANDROID__
#include "AndroidImpl.h"
#include <SDL2/SDL.h>
#include "public/bridge/consolevariablebridge.h"

#include <ImGui/imgui_internal.h>

static bool isUsingTouchscreenControls = true;
static bool isShowingVirtualKeyboard = true;

void LUS::Android::ImGuiProcessEvent(bool wantsTextInput) {
    ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetActiveID());

    if (wantsTextInput) {
        if (!isShowingVirtualKeyboard) {
            state->ClearText();

            isShowingVirtualKeyboard = true;
            SDL_StartTextInput();
        }
    } else {
        if (isShowingVirtualKeyboard) {
            isShowingVirtualKeyboard = false;
            SDL_StopTextInput();
        }
    }
}

void LUS::Android::adjustGyro(float gyroData[3]){
    float gyroX = gyroData[0];
    float gyroY = gyroData[1];
    switch(SDL_GetDisplayOrientation(0)){
        case(SDL_ORIENTATION_PORTRAIT):
            // nothing to do
            break;
        case(SDL_ORIENTATION_PORTRAIT_FLIPPED):
            gyroData[0] = -gyroX;
            gyroData[1] = -gyroY;
            break;
        case(SDL_ORIENTATION_LANDSCAPE):
            gyroData[0] = -gyroY;
            gyroData[1] = gyroX;
            break;
        case(SDL_ORIENTATION_LANDSCAPE_FLIPPED):
            gyroData[0] = gyroY;
            gyroData[1] = -gyroX;
            break;
        case(SDL_ORIENTATION_UNKNOWN):
            // nothing to do
            break;
    }
}

#include <SDL_gamecontroller.h>
#include <jni.h>

bool LUS::Android::IsUsingTouchscreenControls(){
    return isUsingTouchscreenControls;
}

void LUS::Android::EnableTouchArea(){
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject javaObject = (jobject)SDL_AndroidGetActivity();
    jclass javaClass = env->GetObjectClass(javaObject);
    jmethodID enabletoucharea = env->GetMethodID(javaClass, "EnableTouchArea", "()V");
    env->CallVoidMethod(javaObject, enabletoucharea);
}

void LUS::Android::DisableTouchArea(){
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject javaObject = (jobject)SDL_AndroidGetActivity();
    jclass javaClass = env->GetObjectClass(javaObject);
    jmethodID disabletoucharea = env->GetMethodID(javaClass, "DisableTouchArea", "()V");
    env->CallVoidMethod(javaObject, disabletoucharea);
}

static int virtual_joystick_id = -1;
static SDL_Joystick *virtual_joystick = nullptr;
extern "C" void JNICALL Java_com_dishii_soh_MainActivity_attachController(JNIEnv* env, jobject obj) {
    virtual_joystick_id = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 6, 18, 0);
    if (virtual_joystick_id == -1) {
            SDL_Log("Could not create overlay virtual controller");
        return;
    }
    virtual_joystick = SDL_JoystickOpen(virtual_joystick_id);
    if (virtual_joystick == nullptr)
        SDL_Log("Could not create virtual joystick");
    isUsingTouchscreenControls = true;
}

extern "C" void JNICALL Java_com_dishii_soh_MainActivity_setButton(JNIEnv *env, jobject jobj, jint button, jboolean value) {
    if(button < 0){
        SDL_JoystickSetVirtualAxis(virtual_joystick,-button, value ? SDL_MAX_SINT16 : -SDL_MAX_SINT16); // This should be 0 when false, but I think there's a bug in SDL
    }else{
        SDL_JoystickSetVirtualButton(virtual_joystick, button, value);
    }
}

#include "Context.h"

extern "C" void JNICALL Java_com_dishii_soh_MainActivity_setAxis(JNIEnv *env, jobject jobj, jint axis, jshort value) {
    SDL_JoystickSetVirtualAxis(virtual_joystick, axis, value);
}

extern "C" void JNICALL Java_com_dishii_soh_MainActivity_detachController(JNIEnv *env, jobject jobj) {
    SDL_JoystickClose(virtual_joystick);
    SDL_JoystickDetachVirtual(virtual_joystick_id);
    virtual_joystick = nullptr;
    virtual_joystick_id = -1;
}


#endif
