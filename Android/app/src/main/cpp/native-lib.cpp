#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" int main(int argc, char* argv[]);

extern "C" JNIEXPORT void JNICALL
Java_com_starship_MainActivity_nativeInit(JNIEnv* env, jobject /* this */, jstring romPath) {
    const char* path = env->GetStringUTFChars(romPath, nullptr);
    __android_log_print(ANDROID_LOG_INFO, "Starship", "ROM Path: %s", path);

    char* argv[] = {
        const_cast<char*>("starship"),
        const_cast<char*>(path),
    };

    main(2, argv);

    env->ReleaseStringUTFChars(romPath, path);
}
