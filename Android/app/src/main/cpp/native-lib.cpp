#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" int main(int argc, char* argv[]);

extern "C" JNIEXPORT void JNICALL
Java_com_starship_RomPickerActivity_nativeInit(JNIEnv* env, jobject, jstring romPath) {
    const char* path = env->GetStringUTFChars(romPath, nullptr);
    char* argv[] = { const_cast<char*>("starship"), const_cast<char*>(path) };
    main(2, argv);
    env->ReleaseStringUTFChars(romPath, path);
}

extern "C" JNIEXPORT void JNICALL
Java_com_starship_RomVersionSelectorActivity_nativeInit(JNIEnv* env, jobject, jstring romPath, jstring versionOverride) {
    const char* path = env->GetStringUTFChars(romPath, nullptr);
    const char* override = versionOverride ? env->GetStringUTFChars(versionOverride, nullptr) : nullptr;

    char* argv[3] = { const_cast<char*>("starship"), const_cast<char*>(path), nullptr };
    int argc = 2;

    if (override) {
        argv[2] = const_cast<char*>(override);
        argc = 3;
    }

    main(argc, argv);

    env->ReleaseStringUTFChars(romPath, path);
    if (override) env->ReleaseStringUTFChars(versionOverride, override);
}
