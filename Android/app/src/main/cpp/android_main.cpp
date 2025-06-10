#include <android_native_app_glue.h>
#include "libultraship/Core.h"

class AndroidPlatform : public Ship::Platform {
public:
    AndroidPlatform(android_app* app) : mApp(app) {}
    void Init() override {}
    void Exit() override {}
    void* GetInstance() override { return mApp; }
    
private:
    android_app* mApp;
};

void android_main(android_app* app) {
    // Initialize libultraship
    Ship::RegisterInterfaces();
    Ship::RegisterResourceFactories();
    Ship::Core::Init(app->activity->internalDataPath);

    // Configure platform
    auto platform = std::make_shared<AndroidPlatform>(app);
    Ship::Core::GetInstance()->GetWindow()->SetPlatform(platform);

    // Main loop
    while (true) {
        // Handle Android events
        int events;
        android_poll_source* source;
        while (ALooper_pollAll(0, nullptr, &events, (void**)&source) >= 0) {
            if (source) source->process(app, source);
            if (app->destroyRequested) return;
        }

        // Run Starship frame
        Ship::Core::GetInstance()->Run();
    }
}