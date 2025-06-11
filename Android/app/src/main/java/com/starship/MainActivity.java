package com.starship;

import org.libsdl.app.SDLActivity;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import android.Manifest;
import android.content.pm.PackageManager;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.os.Build;
import android.widget.Toast;
import android.util.Log;

public class MainActivity extends SDLActivity {

    static {
        System.loadLibrary("native-lib");
    }

    private static final int PICK_ROM_REQUEST = 1;
    private static final int STORAGE_PERMISSION_REQUEST_CODE = 2296;

    SharedPreferences preferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        preferences = getSharedPreferences("com.starship.prefs", Context.MODE_PRIVATE);

        if (hasStoragePermission()) {
            setupFiles();
            doVersionCheck();
            pickRomIfNeeded();
        } else {
            requestStoragePermission();
        }

        // TODO: setupControllerOverlay(); // Add this if you implement touch/controller overlay logic
    }

    // Version check and asset cleanup
    private void doVersionCheck(){
        int currentVersion = BuildConfig.VERSION_CODE;
        int storedVersion = preferences.getInt("appVersion", 1);
        if (currentVersion > storedVersion) {
            deleteOutdatedAssets();
            preferences.edit().putInt("appVersion", currentVersion).apply();
        }
    }

    private void deleteOutdatedAssets() {
        File rootFolder = new File(Environment.getExternalStorageDirectory(), "Starship");
        File romFile = new File(rootFolder, "baserom.z64");
        romFile.delete();
        File assetsFolder = new File(rootFolder, "assets");
        deleteRecursive(assetsFolder);
    }

    private void deleteRecursive(File fileOrDirectory) {
        if (fileOrDirectory != null && fileOrDirectory.exists()) {
            if (fileOrDirectory.isDirectory()) {
                for (File child : fileOrDirectory.listFiles()) {
                    deleteRecursive(child);
                }
            }
            fileOrDirectory.delete();
        }
    }

    // Permission helpers
    private boolean hasStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return Environment.isExternalStorageManager();
        }
        return ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                == PackageManager.PERMISSION_GRANTED &&
                ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)
                        == PackageManager.PERMISSION_GRANTED;
    }

    private void requestStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivityForResult(intent, STORAGE_PERMISSION_REQUEST_CODE);
            } else {
                setupFiles();
                pickRomIfNeeded();
            }
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            ActivityCompat.requestPermissions(this,
                    new String[]{
                            Manifest.permission.READ_EXTERNAL_STORAGE,
                            Manifest.permission.WRITE_EXTERNAL_STORAGE
                    },
                    STORAGE_PERMISSION_REQUEST_CODE);
        } else {
            setupFiles();
            pickRomIfNeeded();
        }
    }

    // File setup logic (can be extended to copy assets from APK if desired)
    private void setupFiles() {
        File targetRootFolder = new File(Environment.getExternalStorageDirectory(), "Starship");
        if (!targetRootFolder.exists()) {
            boolean created = targetRootFolder.mkdirs();
            if (!created) {
                Log.e("setupFiles", "Failed to create Starship folder");
                return;
            }
            Toast.makeText(this, "Setting up files in /storage/emulated/0/Starship...", Toast.LENGTH_SHORT).show();
        } else {
            Log.i("setupFiles", "Starship folder already exists.");
        }
    }

    // ROM picker logic
    private void pickRomIfNeeded() {
        File romFile = new File(getFilesDir(), "baserom.z64");
        if (!romFile.exists()) {
            pickRom();
        } else {
            nativeInit(romFile.getAbsolutePath());
        }
    }

    private void pickRom() {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.setType("*/*");
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        startActivityForResult(intent, PICK_ROM_REQUEST);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == PICK_ROM_REQUEST && resultCode == RESULT_OK && data != null) {
            Uri uri = data.getData();
            try {
                InputStream inputStream = getContentResolver().openInputStream(uri);
                File romFile = new File(getFilesDir(), "baserom.z64");
                FileOutputStream outputStream = new FileOutputStream(romFile);

                byte[] buffer = new byte[4096];
                int length;
                while ((length = inputStream.read(buffer)) > 0) {
                    outputStream.write(buffer, 0, length);
                }

                inputStream.close();
                outputStream.close();

                nativeInit(romFile.getAbsolutePath());

            } catch (Exception e) {
                e.printStackTrace();
                Toast.makeText(this, "Failed to load ROM", Toast.LENGTH_LONG).show();
            }
        } else if (requestCode == STORAGE_PERMISSION_REQUEST_CODE) {
            if (hasStoragePermission()) {
                setupFiles();
                pickRomIfNeeded();
            } else {
                Toast.makeText(this, "Storage permission is required to access files.", Toast.LENGTH_LONG).show();
            }
        }
    }

    public native void nativeInit(String romPath);
}
