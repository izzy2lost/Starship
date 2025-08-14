# Starship Android ProGuard Rules

# Keep SDL classes
-keep class org.libsdl.app.** { *; }

# Keep native methods
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

# Optimization settings
-optimizations !code/simplification/arithmetic,!code/simplification/cast,!field/*,!class/merging/*
-optimizationpasses 5
-allowaccessmodification
-dontpreverify

# Remove logging in release builds
-assumenosideeffects class android.util.Log {
    public static boolean isLoggable(java.lang.String, int);
    public static int v(...);
    public static int i(...);
    public static int w(...);
    public static int d(...);
    public static int e(...);
}

# Keep line numbers for crash reports
-keepattributes SourceFile,LineNumberTable

# Rename source file attribute to something shorter
-renamesourcefileattribute SourceFile
