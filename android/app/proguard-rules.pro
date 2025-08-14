# Starship Android ProGuard Rules

# Keep SDL classes - critical for native integration
-keep class org.libsdl.app.** { *; }

# Keep all native methods and JNI callbacks
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep MainActivity and its native methods
-keep class com.starship.android.MainActivity {
    public static java.lang.String getSaveDir();
    public static void waitForSetupFromNative();
    public native void attachController();
    public native void detachController();
    public native void setButton(int, boolean);
    public native void setCameraState(int, float);
    public native void setAxis(int, short);
}

# Keep DialogActivity
-keep class com.starship.android.DialogActivity { *; }

# Keep ControllerButtons constants
-keep class com.starship.android.ControllerButtons { *; }

# Keep DocumentFile for SAF
-keep class androidx.documentfile.provider.DocumentFile { *; }

# Keep all classes that might be accessed via JNI
-keep class * {
    public static <methods>;
    public static <fields>;
}

# Keep enums - often used by native code
-keepclassmembers enum * {
    public static **[] values();
    public static ** valueOf(java.lang.String);
}

# Keep Serializable classes
-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}

# Less aggressive optimization for native apps
-optimizations !code/simplification/arithmetic,!code/simplification/cast,!field/*,!class/merging/*,!code/removal/advanced,!code/removal/simple
-optimizationpasses 3
-allowaccessmodification
-dontpreverify

# Don't remove logging completely - keep error logs for crash reports
-assumenosideeffects class android.util.Log {
    public static boolean isLoggable(java.lang.String, int);
    public static int v(...);
    public static int i(...);
    public static int w(...);
    public static int d(...);
}

# Keep line numbers and source file for crash reports
-keepattributes SourceFile,LineNumberTable,*Annotation*,Signature

# Rename source file attribute
-renamesourcefileattribute SourceFile

# Don't warn about missing classes that might be platform-specific
-dontwarn javax.**
-dontwarn java.awt.**
-dontwarn org.slf4j.**
