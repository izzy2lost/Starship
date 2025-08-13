package com.starship.android;

import org.libsdl.app.SDLActivity;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.util.Log;
import android.widget.Toast;
import android.app.AlertDialog;
import java.io.File;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.concurrent.CountDownLatch;
import androidx.documentfile.provider.DocumentFile;

import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;
import android.view.KeyEvent;
import android.content.pm.ActivityInfo;
import android.view.KeyEvent;

public class MainActivity extends SDLActivity {
static {
    System.loadLibrary("Starship");
}

// ===== Constants / Prefs =====
private static final String PREFS = "com.starship.android.prefs";
private static final String KEY_USER_FOLDER_URI = "user_folder_uri";
private static final String TAG = "MainActivity";
private static final int REQ_PICK_FOLDER = 1001;
private static final int REQ_PICK_SF64    = 1002;

// ===== State =====
SharedPreferences preferences;
private static final CountDownLatch setupLatch = new CountDownLatch(1);
private Uri userFolderUri; // Persisted SAF tree URI

// ===== Native methods =====
public native void attachController();
public native void detachController();
public native void setButton(int button, boolean value);
public native void setCameraState(int axis, float value);
public native void setAxis(int axis, short value);

// ===== Save dir for the engine (internal only; no extra subfolder) =====
public static String getSaveDir() {
    Context ctx = SDLActivity.getContext();
    File internal = ctx.getFilesDir(); // /data/data/<pkg>/files
    if (!internal.exists()) internal.mkdirs();
    Log.i(TAG, "getSaveDir -> " + internal.getAbsolutePath());
    return internal.getAbsolutePath();
}

private Uri getUserFolderUri() {
    String s = preferences.getString(KEY_USER_FOLDER_URI, null);
    return (s != null) ? Uri.parse(s) : null;
}

private void setUserFolderUri(Uri uri) {
    preferences.edit().putString(KEY_USER_FOLDER_URI, uri != null ? uri.toString() : null).apply();
}

private String getUserFolderPath() {
    if (userFolderUri == null) return null;
    try {
        String treeId = DocumentsContract.getTreeDocumentId(userFolderUri);
        // Check if it's external storage
        if (treeId.startsWith("primary:")) {
            String relativePath = treeId.substring("primary:".length());
            File externalStorage = Environment.getExternalStorageDirectory();
            return new File(externalStorage, relativePath).getAbsolutePath();
        }
        Log.w(TAG, "Could not extract path from URI: " + userFolderUri);
        return null;
    } catch (Exception e) {
        Log.e(TAG, "Error extracting path from URI", e);
        return null;
    }
}

private void syncModsFromUserFolder() {
    try {
        DocumentFile userRoot = DocumentFile.fromTreeUri(this, userFolderUri);
        if (userRoot == null) return;

        DocumentFile userModsFolder = userRoot.findFile("mods");
        if (userModsFolder == null || !userModsFolder.isDirectory()) {
            Log.i(TAG, "No mods folder found in user directory");
            return;
        }

        File internalModsFolder = new File(getFilesDir(), "mods");
        if (!internalModsFolder.exists()) {
            internalModsFolder.mkdirs();
        }

        // Clear existing mods in internal storage first
        clearDirectory(internalModsFolder);

        // Copy all files from user mods folder to internal mods folder
        copyModsRecursively(userModsFolder, internalModsFolder);
        Log.i(TAG, "Mods synced from user folder to internal storage");
    } catch (Exception e) {
        Log.e(TAG, "Error syncing mods from user folder", e);
    }
}

private void clearDirectory(File dir) {
    if (dir.exists() && dir.isDirectory()) {
        File[] files = dir.listFiles();
        if (files != null) {
            for (File file : files) {
                if (file.isDirectory()) {
                    clearDirectory(file);
                }
                file.delete();
            }
        }
    }
}

private void copyModsRecursively(DocumentFile sourceDir, File destDir) {
    try {
        DocumentFile[] files = sourceDir.listFiles();
        if (files == null) return;

        for (DocumentFile file : files) {
            String fileName = file.getName();
            if (fileName == null) continue;

            File destFile = new File(destDir, fileName);
            if (file.isDirectory()) {
                // Create directory and recurse
                destFile.mkdirs();
                copyModsRecursively(file, destFile);
            } else {
                // Copy file
                try (InputStream in = getContentResolver().openInputStream(file.getUri());
                     FileOutputStream out = new FileOutputStream(destFile)) {
                    byte[] buf = new byte[8192];
                    int r;
                    while ((r = in.read(buf)) != -1) { 
                        out.write(buf, 0, r); 
                    }
                    out.flush();
                    Log.i(TAG, "Copied mod file: " + fileName);
                } catch (IOException e) {
                    Log.e(TAG, "Failed to copy mod file: " + fileName, e);
                }
            }
        }
    } catch (Exception e) {
        Log.e(TAG, "Error in copyModsRecursively", e);
    }
}

// ===== Lifecycle =====
@Override
protected void onCreate(Bundle savedInstanceState) {
    preferences = getSharedPreferences(PREFS, Context.MODE_PRIVATE);
    userFolderUri = getUserFolderUri();

    super.onCreate(savedInstanceState);
    
        try { nativeSetAppDirs(getSaveDir()); } catch (Throwable t) { Log.w(TAG, "nativeSetAppDirs failed", t); }
setupControllerOverlay();
    attachController();

    // Seed internal directory with assets if they exist (optional)
    seedInternalFromAssetsIfPresent();

    File internal = getFilesDir();
    File external = getExternalFilesDir(null);
    Log.i(TAG, "Internal root: " + internal);
    Log.i(TAG, "External root: " + external);

    // Check for sf64.o2r in internal storage first
    File internalSf64 = new File(internal, "sf64.o2r");

    // If not in internal, check if it exists in user's chosen folder and copy it
    if (!internalSf64.exists() && userFolderUri != null) {
        DocumentFile userRoot = DocumentFile.fromTreeUri(this, userFolderUri);
        if (userRoot != null) {
            DocumentFile userSf64 = userRoot.findFile("sf64.o2r");
            if (userSf64 != null && userSf64.exists()) {
                Log.i(TAG, "Found sf64.o2r in user folder, copying to internal storage");
                try (InputStream in = getContentResolver().openInputStream(userSf64.getUri());
                     FileOutputStream out = new FileOutputStream(internalSf64)) {
                    byte[] buf = new byte[8192];
                    int r;
                    while ((r = in.read(buf)) != -1) { 
                        out.write(buf, 0, r); 
                    }
                    out.flush();
                    out.getFD().sync();
                    Log.i(TAG, "sf64.o2r copied from user folder to internal storage");
                
        try { nativeSetAppDirs(getSaveDir()); } catch (Throwable t) { Log.w(TAG, "nativeSetAppDirs failed", t); }
} catch (IOException e) {
                    Log.e(TAG, "Failed to copy sf64.o2r from user folder", e);
                }
            }
        }
    }

    // Always sync mods folder from user's chosen folder to internal storage
    if (userFolderUri != null) {
        syncModsFromUserFolder();
    }

    // Now check if sf64.o2r exists in internal storage
    if (!internalSf64.exists()) {
        Log.i(TAG, "sf64.o2r not found. Prompting for folder.");
        if (userFolderUri == null) {
            // First time - need to select folder
            promptForUserFolder();
        } else {
            // Folder already selected but no ROM - show file not found dialog
            showPortraitDialog("sf64.o2r not found in selected folder",
                "Pick an existing sf64.o2r file or use Torch to create one. It will be copied to your selected folder.",
                DialogActivity.DIALOG_TYPE_FILE_NOT_FOUND);
        }
    } else {
        Log.i(TAG, "sf64.o2r found in internal storage, game should start normally.");
    }
}

public static void waitForSetupFromNative() {
    // This method is called from C++ but we don't need it anymore
    // The C++ polling system handles waiting for the file
    Log.i(TAG, "waitForSetupFromNative() called from C++ - using polling system");
}

// ===== Asset seeding (optional, safe if assets not present) =====
private boolean assetExists(String name) {
    try { 
        getAssets().open(name).close(); 
        return true; 
    } catch (IOException e) { 
        return false; 
    }
}

private boolean assetDirExists(String dir) {
    try { 
        String[] list = getAssets().list(dir); 
        return list != null && list.length > 0; 
    } catch (IOException e) { 
        return false; 
    }
}

private void seedInternalFromAssetsIfPresent() {
    File internal = getFilesDir();

    // Prefer the real name: gamecontrollerdb.txt (but accept controllerdb.txt asset if that's what you ship)
    File gcdb = new File(internal, "gamecontrollerdb.txt");
    File cdb  = new File(internal, "controllerdb.txt");
    if (!gcdb.exists() && !cdb.exists()) {
        if (assetExists("gamecontrollerdb.txt")) {
            copyAssetFile("gamecontrollerdb.txt", gcdb);
        } else if (assetExists("controllerdb.txt")) {
            copyAssetFile("controllerdb.txt", cdb);
        } else {
            Log.i(TAG, "No controller DB asset shipped.");
        }
    }

    File starship = new File(internal, "starship.o2r");
    Log.i(TAG, "Checking starship.o2r - exists in internal: " + starship.exists() + ", exists in assets: " + assetExists("starship.o2r"));
    if (!starship.exists() && assetExists("starship.o2r")) {
        Log.i(TAG, "Copying starship.o2r from assets to internal");
        copyAssetFile("starship.o2r", starship);
    } else if (!starship.exists()) {
        Log.w(TAG, "starship.o2r not found in assets - this might be expected for development builds");
        // List available assets for debugging
        try {
            String[] assets = getAssets().list("");
            Log.i(TAG, "Available assets:");
            for (String asset : assets) {
                Log.i(TAG, "  - " + asset);
            }
        } catch (IOException e) {
            Log.e(TAG, "Error listing assets", e);
        }
    }

    File modsDir = new File(internal, "mods");
    if (!modsDir.exists() && assetDirExists("mods")) {
        copyAssetFolderRecursive("mods", modsDir);
    }
}

private void copyAssetFile(String assetName, File destFile) {
    try {
        File parent = destFile.getParentFile();
        if (parent != null && !parent.exists()) parent.mkdirs();

        try (InputStream in = getAssets().open(assetName);
             FileOutputStream out = new FileOutputStream(destFile)) {
            byte[] buffer = new byte[8192];
            int read;
            long total = 0;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
                total += read;
            }
            out.flush();
            out.getFD().sync();
            Log.i(TAG, "Seeded asset " + assetName + " (" + total + " bytes) -> " + destFile.getAbsolutePath());
        }
    } catch (IOException e) {
        Log.e(TAG, "copyAssetFile failed for " + assetName, e);
    }
}

private void copyAssetFolderRecursive(String assetDir, File destDir) {
    try {
        if (!destDir.exists()) destDir.mkdirs();

        String[] kids = getAssets().list(assetDir);
        if (kids == null) return;

        for (String name : kids) {
            String assetPath = assetDir + "/" + name;
            String[] sub = getAssets().list(assetPath);
            if (sub != null && sub.length > 0) {
                copyAssetFolderRecursive(assetPath, new File(destDir, name));
            } else {
                copyAssetFile(assetPath, new File(destDir, name));
            }
        }
        Log.i(TAG, "Seeded asset dir " + assetDir + " -> " + destDir.getAbsolutePath());
    } catch (IOException e) {
        Log.e(TAG, "copyAssetFolderRecursive failed for " + assetDir, e);
    }
}

// ===== UI helpers =====
private void showPortraitDialog(String title, String message, int dialogType) {
    Intent intent = new Intent(this, DialogActivity.class);
    intent.putExtra(DialogActivity.EXTRA_TITLE, title);
    intent.putExtra(DialogActivity.EXTRA_MESSAGE, message);
    intent.putExtra(DialogActivity.EXTRA_DIALOG_TYPE, dialogType);
    intent.putExtra(DialogActivity.EXTRA_CANCELABLE, false);
    startActivityForResult(intent, dialogType);
}

private void showToast(String msg) {
    runOnUiThread(() -> Toast.makeText(this, msg, Toast.LENGTH_SHORT).show());
}

private void restartApp() {
    try {
        Intent i = getPackageManager().getLaunchIntentForPackage(getPackageName());
        if (i != null) {
            i.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(i);
            finish();
            System.exit(0);
        }
    } catch (Exception e) {
        Log.e(TAG, "restartApp", e);
        showToast("Please restart the app manually");
    }
}

// ===== Folder / File pickers =====
private void promptForUserFolder() {
    showPortraitDialog("Choose Your Folder", 
        "Select a folder for your sf64.o2r file and mods location. This will be your main Starship folder where you can add mods and manage game files.",
        DialogActivity.DIALOG_TYPE_FOLDER_PROMPT);
}

public void openFolderPicker() {
    Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT_TREE);
    i.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION
        | Intent.FLAG_GRANT_WRITE_URI_PERMISSION
        | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION
        | Intent.FLAG_GRANT_PREFIX_URI_PERMISSION);
    startActivityForResult(i, REQ_PICK_FOLDER);
}

public void openFilePickerForSf64() {
    Intent i = new Intent(Intent.ACTION_OPEN_DOCUMENT);
    i.setType("*/*");
    i.addCategory(Intent.CATEGORY_OPENABLE);
    i.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION
        | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
    startActivityForResult(i, REQ_PICK_SF64);
}

private void openTorchDownload() {
    try {
        Intent browser = new Intent(Intent.ACTION_VIEW, Uri.parse("https://github.com/izzy2lost/Torch/releases"));
        Intent chooser = Intent.createChooser(browser, "Open Torch download page with:");
        chooser.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        startActivity(chooser);
        showToast("Opening Torch download. App will close so you can install Torch.");
        
        // Close the app after a short delay so user can install Torch
        new android.os.Handler(getMainLooper()).postDelayed(() -> {
            finish();
            System.exit(0);
        }, 2000);
    } catch (Exception e) {
        Log.e(TAG, "openTorchDownload", e);
        showToast("Visit: https://github.com/izzy2lost/Torch/releases");
    }
}

// ===== Activity result =====
@Override
protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    super.onActivityResult(requestCode, resultCode, data);

    Log.i(TAG, "onActivityResult: requestCode=" + requestCode + ", resultCode=" + resultCode);

    // Handle DialogActivity results
    if (requestCode == DialogActivity.DIALOG_TYPE_FOLDER_PROMPT && resultCode == DialogActivity.RESULT_FOLDER_PICKER) {
        Log.i(TAG, "Opening folder picker");
        openFolderPicker();
        return;
    }
    if (requestCode == DialogActivity.DIALOG_TYPE_FILE_NOT_FOUND) {
        if (resultCode == DialogActivity.RESULT_TORCH_DOWNLOAD) {
            Log.i(TAG, "Opening Torch download");
            openTorchDownload();
        } else if (resultCode == DialogActivity.RESULT_FILE_PICKER) {
            Log.i(TAG, "Opening file picker for sf64");
            openFilePickerForSf64();
        }
        return;
    }
    if ((requestCode == DialogActivity.DIALOG_TYPE_COPY_COMPLETE || requestCode == DialogActivity.DIALOG_TYPE_FILE_READY) 
        && resultCode == DialogActivity.RESULT_RESTART) {
        restartApp();
        return;
    }

    // Handle file/folder picker results
    if (resultCode != RESULT_OK || data == null) return;

    if (requestCode == REQ_PICK_FOLDER) {
        handleFolderSelection(data.getData(), data.getFlags());
    } else if (requestCode == REQ_PICK_SF64) {
        handleRomFileSelection(data.getData());
    }
}

// ===== Folder selection & copy (SAF) =====
private void handleFolderSelection(Uri treeUri, int returnedFlags) {
    if (treeUri == null) { 
        showToast("No folder selected."); 
        return; 
    }

    // Persist read/write — try returned flags; if 0, try explicit
    try {
        final int permsReturned = returnedFlags &
            (Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        if (permsReturned != 0) {
            getContentResolver().takePersistableUriPermission(treeUri, permsReturned);
        } else {
            getContentResolver().takePersistableUriPermission(treeUri,
                Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        }
    } catch (Exception e) {
        Log.w(TAG, "takePersistableUriPermission failed (continuing with transient perms): " + e);
    }

    setUserFolderUri(treeUri);
    userFolderUri = treeUri;
    showToast("Folder selected.");

    DocumentFile userRoot = DocumentFile.fromTreeUri(this, treeUri);
    if (userRoot == null) {
        showToast("Cannot access the selected folder.");
        return;
    }

    if (!verifyWritable(userRoot)) {
        showToast("Can't write there. Try Downloads or a folder you create under Internal storage.");
        return;
    }

    boolean anyCopied = copyFromBestSourceToSaf(userRoot);

    // Check if sf64.o2r exists in the user's chosen folder and copy it to internal storage
    DocumentFile sf64InUserFolder = userRoot.findFile("sf64.o2r");
    if (sf64InUserFolder != null && sf64InUserFolder.exists()) {
        // Copy sf64.o2r to internal storage
        File internalSf64 = new File(getFilesDir(), "sf64.o2r");
        try (InputStream in = getContentResolver().openInputStream(sf64InUserFolder.getUri());
             FileOutputStream out = new FileOutputStream(internalSf64)) {
            byte[] buf = new byte[8192];
            int r;
            while ((r = in.read(buf)) != -1) { 
                out.write(buf, 0, r); 
            }
            out.flush();
            out.getFD().sync();
            Log.i(TAG, "sf64.o2r copied from user folder to internal storage during folder selection");
            
            
        try { nativeSetAppDirs(getSaveDir()); } catch (Throwable t) { Log.w(TAG, "nativeSetAppDirs failed", t); }
// Also sync mods
            syncModsFromUserFolder();
            
            showToast("Files copied. Game should start now.");
        } catch (IOException e) {
            Log.e(TAG, "Failed to copy sf64.o2r during folder selection", e);
            showToast("Failed to copy sf64.o2r: " + e.getMessage());
        }
    } else {
        runOnUiThread(() -> showPortraitDialog("sf64.o2r not found in selected folder",
            "Pick an existing sf64.o2r file or use Torch to create one. It will be copied to your selected folder.",
            DialogActivity.DIALOG_TYPE_FILE_NOT_FOUND));
    }
}

private boolean verifyWritable(DocumentFile dir) {
    try {
        DocumentFile tmp = dir.createFile("application/octet-stream", ".saf_write_test");
        if (tmp == null) return false;
        OutputStream os = getContentResolver().openOutputStream(tmp.getUri(), "w");
        if (os == null) return false;
        os.write(1);
        os.flush();
        os.close();
        tmp.delete();
        return true;
    } catch (Exception e) {
        Log.w(TAG, "verifyWritable failed: " + e);
        return false;
    }
}

// Choose best source root:
// 1) Internal files dir
// 2) External files dir root (legacy)
// 3) External files dir /Starship (legacy leftover)
private File chooseBestSourceRoot() {
    File internal = getFilesDir();
    File external = getExternalFilesDir(null);
    File legacy = (external != null) ? new File(external, "Starship") : null;

    boolean hasInternal = hasAnyTarget(internal);
    boolean hasExternal = external != null && hasAnyTarget(external);
    boolean hasLegacy   = legacy != null && hasAnyTarget(legacy);

    Log.i(TAG, "Source check -> internal=" + hasInternal + ", external=" + hasExternal + ", legacy=" + hasLegacy);

    if (hasInternal) return internal;
    if (hasExternal) return external;
    if (hasLegacy)   return legacy;
    return internal; // default (will report not found later)
}

private boolean hasAnyTarget(File root) {
    if (root == null) return false;
    if (new File(root, "gamecontrollerdb.txt").exists()) return true;
    if (new File(root, "controllerdb.txt").exists())     return true; // tolerate alt name
    if (new File(root, "starship.o2r").exists())         return true;
    File mods = new File(root, "mods");
    return mods.exists() && mods.isDirectory() && mods.listFiles() != null && mods.listFiles().length > 0;
}

private boolean copyFromBestSourceToSaf(DocumentFile userRoot) {
    File srcRoot = chooseBestSourceRoot();
    Log.i(TAG, "Copying from srcRoot=" + srcRoot);
    int copied = 0;

    // gamecontrollerdb.txt (prefer this name), fall back to controllerdb.txt if that's what exists
    File gcdb = new File(srcRoot, "gamecontrollerdb.txt");
    File cdb  = new File(srcRoot, "controllerdb.txt");
    if (gcdb.exists()) { 
        if (copyFileToTree(gcdb, userRoot, "text/plain")) copied++; 
    } else if (cdb.exists()) { 
        if (copyFileToTree(cdb, userRoot, "text/plain")) copied++; 
    } else Log.w(TAG, "No controller DB at " + srcRoot);

    // starship.o2r
    File starship = new File(srcRoot, "starship.o2r");
    if (starship.exists()) { 
        if (copyFileToTree(starship, userRoot, "application/octet-stream")) copied++; 
    } else Log.w(TAG, "No starship.o2r at " + srcRoot);

    // mods
    File modsSrc = new File(srcRoot, "mods");
    if (modsSrc.exists() && modsSrc.isDirectory()) {
        if (copyFolderToTree(modsSrc, userRoot)) copied++;
    } else Log.w(TAG, "No mods folder at " + srcRoot);

    if (copied > 0) showToast("Copied " + copied + " item(s) to selected folder.");
    else showToast("Nothing copied. Make sure files exist in app storage.");
    return copied > 0;
}

// When user picks sf64.o2r via SAF, copy into INTERNAL (engine reads from here)
private void handleRomFileSelection(Uri selectedFileUri) {
    if (selectedFileUri == null) { 
        showToast("No sf64.o2r selected."); 
        return; 
    }

    File dest = new File(getFilesDir(), "sf64.o2r");
    showToast("Copying sf64.o2r...");

    try (InputStream in = getContentResolver().openInputStream(selectedFileUri);
         FileOutputStream out = new FileOutputStream(dest)) {
        byte[] buf = new byte[8192];
        int r;
        long total = 0;
        while ((r = in.read(buf)) != -1) { 
            out.write(buf, 0, r); 
            total += r; 
        }
        out.flush();
        out.getFD().sync();
        Log.i(TAG, "sf64.o2r copied to internal (" + total + " bytes): " + dest.getAbsolutePath());

        
        try { nativeSetAppDirs(getSaveDir()); } catch (Throwable t) { Log.w(TAG, "nativeSetAppDirs failed", t); }
runOnUiThread(() -> showPortraitDialog("sf64.o2r ready",
            "sf64.o2r copied. Restart to load the game.",
            DialogActivity.DIALOG_TYPE_FILE_READY));
    } catch (IOException e) {
        Log.e(TAG, "handleRomFileSelection", e);
        showToast("Failed to copy sf64.o2r: " + e.getMessage());
    }
}

// ===== SAF copy helpers =====
private boolean copyFileToTree(File src, DocumentFile dstParent, String mimeGuess) {
    try {
        // Overwrite if present
        DocumentFile existing = findChild(dstParent, src.getName());
        if (existing != null && existing.isFile()) existing.delete();

        String mime = (mimeGuess != null) ? mimeGuess : guessMime(src.getName());
        DocumentFile dest = dstParent.createFile(mime, src.getName());
        if (dest == null) {
            Log.e(TAG, "Failed to create file in tree: " + src.getName());
            return false;
        }

        try (InputStream in = new FileInputStream(src);
             OutputStream out = getContentResolver().openOutputStream(dest.getUri(), "w")) {
            if (out == null) throw new IOException("Null OutputStream from resolver");
            byte[] buf = new byte[8192];
            int r;
            long total = 0;
            while ((r = in.read(buf)) != -1) {
                out.write(buf, 0, r);
                total += r;
            }
            out.flush();
            Log.i(TAG, "Wrote " + total + " bytes → " + dest.getUri());
        }
        Log.i(TAG, "Copied to SAF: " + src.getAbsolutePath() + " → " + dest.getUri());
        return true;
    } catch (IOException e) {
        Log.e(TAG, "copyFileToTree " + src.getName(), e);
        showToast("Failed copying " + src.getName());
        return false;
    }
}

// Returns true if any item was copied
private boolean copyFolderToTree(File srcDir, DocumentFile dstParent) {
    boolean any = false;
    DocumentFile dstDir = ensureDirectory(dstParent, srcDir.getName());
    if (dstDir == null) return false;

    File[] kids = srcDir.listFiles();
    if (kids == null) return false;

    for (File kid : kids) {
        if (kid.isDirectory()) {
            if (copyFolderToTree(kid, dstDir)) any = true;
        } else {
            if (copyFileToTree(kid, dstDir, guessMime(kid.getName()))) any = true;
        }
    }
    return any;
}

private DocumentFile ensureDirectory(DocumentFile parent, String name) {
    DocumentFile existing = findChild(parent, name);
    if (existing != null && existing.isDirectory()) return existing;
    if (existing != null) existing.delete();
    return parent.createDirectory(name);
}

private DocumentFile findChild(DocumentFile parent, String name) {
    for (DocumentFile f : parent.listFiles()) {
        if (name.equals(f.getName())) return f;
    }
    return null;
}

private String guessMime(String name) {
    String n = name.toLowerCase();
    if (n.endsWith(".txt"))  return "text/plain";
    if (n.endsWith(".json")) return "application/json";
    return "application/octet-stream";
}

// ================= Controller overlay and touch handling =================
private Button buttonA, buttonB, buttonX, buttonY;
private Button buttonDpadUp, buttonDpadDown, buttonDpadLeft, buttonDpadRight;
private Button buttonLB, buttonRB, buttonZ, buttonStart, buttonBack, buttonToggle, buttonMenu;
private FrameLayout leftJoystick;
private ImageView leftJoystickKnob;
private View overlayView;

private void setupControllerOverlay() {
    LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
    overlayView = inflater.inflate(R.layout.touchcontrol_overlay, null);
    FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT,
            FrameLayout.LayoutParams.MATCH_PARENT
    );
    overlayView.setLayoutParams(layoutParams);
    ViewGroup rootView = (ViewGroup) this.getWindow().getDecorView().findViewById(android.R.id.content);
    rootView.addView(overlayView);

    final ViewGroup buttonGroup = overlayView.findViewById(R.id.button_group);

    buttonA = overlayView.findViewById(R.id.buttonA);
    buttonB = overlayView.findViewById(R.id.buttonB);
    buttonX = overlayView.findViewById(R.id.buttonX);
    buttonY = overlayView.findViewById(R.id.buttonY);

    buttonDpadUp = overlayView.findViewById(R.id.buttonDpadUp);
    buttonDpadDown = overlayView.findViewById(R.id.buttonDpadDown);
    buttonDpadLeft = overlayView.findViewById(R.id.buttonDpadLeft);
    buttonDpadRight = overlayView.findViewById(R.id.buttonDpadRight);

    buttonLB = overlayView.findViewById(R.id.buttonLB);
    buttonRB = overlayView.findViewById(R.id.buttonRB);
    buttonZ = overlayView.findViewById(R.id.buttonZ);

    buttonStart = overlayView.findViewById(R.id.buttonStart);
    buttonBack = overlayView.findViewById(R.id.buttonBack);
    buttonMenu = overlayView.findViewById(R.id.buttonMenu);

    buttonToggle = overlayView.findViewById(R.id.buttonToggle);

    leftJoystick = overlayView.findViewById(R.id.left_joystick);
    leftJoystickKnob = overlayView.findViewById(R.id.left_joystick_knob);

    FrameLayout rightScreenArea = overlayView.findViewById(R.id.right_screen_area);

    addTouchListener(buttonA, ControllerButtons.BUTTON_A);
    addTouchListener(buttonB, ControllerButtons.BUTTON_B);
    addTouchListener(buttonX, ControllerButtons.BUTTON_X);
    addTouchListener(buttonY, ControllerButtons.BUTTON_Y);

    setupCButtons(buttonDpadUp, ControllerButtons.AXIS_RY, 1);
    setupCButtons(buttonDpadDown, ControllerButtons.AXIS_RY, -1);
    setupCButtons(buttonDpadLeft, ControllerButtons.AXIS_RX, 1);
    setupCButtons(buttonDpadRight, ControllerButtons.AXIS_RX, -1);

    addTouchListener(buttonLB, ControllerButtons.BUTTON_LB);
    addTouchListener(buttonRB, ControllerButtons.BUTTON_RB);
    addTouchListener(buttonZ, ControllerButtons.AXIS_RT);

    addTouchListener(buttonStart, ControllerButtons.BUTTON_START);
    addTouchListener(buttonBack, ControllerButtons.BUTTON_BACK);
    setupMenuButton(buttonMenu);

    setupJoystick(leftJoystick, leftJoystickKnob, true);
    setupLookAround(rightScreenArea);
    setupToggleButton(buttonToggle, buttonGroup);
}

private void setupMenuButton(Button button) {
    button.setOnTouchListener(new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    onNativeKeyDown(KeyEvent.KEYCODE_F1);
                    button.setPressed(true);
                    // Toggle menu state and controls
                    MenuOpen = !MenuOpen;
                    if (MenuOpen) {
                        DisableAllControls();
                    } else {
                        EnableAllControls();
                    }
                    return true;
                case MotionEvent.ACTION_UP:
                    onNativeKeyUp(KeyEvent.KEYCODE_F1);
                    button.setPressed(false);
                    return true;
                case MotionEvent.ACTION_CANCEL:
                    onNativeKeyUp(KeyEvent.KEYCODE_F1);
                    return true;
            }
            return false;
        }
    });
}

private void setupToggleButton(Button button, ViewGroup uiGroup) {
    boolean isHidden = preferences.getBoolean("controlsVisible", false);
    uiGroup.setVisibility(isHidden ? View.INVISIBLE : View.VISIBLE);
    button.setOnClickListener(new View.OnClickListener() {
        boolean isHidden = false;
        @Override
        public void onClick(View v) {
            if (isHidden) {
                uiGroup.setVisibility(View.VISIBLE);
            } else {
                uiGroup.setVisibility(View.INVISIBLE);
            }
            preferences.edit().putBoolean("controlsVisible", !isHidden).apply();
            isHidden = !isHidden;
        }
    });
}

private void addTouchListener(Button button, int buttonNum) {
    button.setOnTouchListener(new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if (!AllControlsEnabled) return false;
            
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    setButton(buttonNum, true);
                    button.setPressed(true);
                    return true;
                case MotionEvent.ACTION_UP:
                    setButton(buttonNum, false);
                    button.setPressed(false);
                    return true;
                case MotionEvent.ACTION_CANCEL:
                    setButton(buttonNum, false);
                    return true;
            }
            return false;
        }
    });
}

private void setupCButtons(Button button, int buttonNum, int direction) {
    button.setOnTouchListener(new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            if (!AllControlsEnabled) return false;
            
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    setAxis(buttonNum, direction < 0 ? Short.MAX_VALUE : Short.MIN_VALUE);
                    button.setPressed(true);
                    return true;
                case MotionEvent.ACTION_UP:
                    setAxis(buttonNum, (short) 0);
                    button.setPressed(false);
                    return true;
                case MotionEvent.ACTION_CANCEL:
                    setAxis(buttonNum, (short) 0);
                    return true;
            }
            return false;
        }
    });
}

// Control state management
private boolean TouchAreaEnabled = true;
private boolean MenuOpen = false;
private boolean AllControlsEnabled = true;

private void DisableTouchArea() {
    TouchAreaEnabled = false;
}

private void EnableTouchArea() {
    TouchAreaEnabled = true;
}

private void DisableAllControls() {
    AllControlsEnabled = false;
    TouchAreaEnabled = false;
}

private void EnableAllControls() {
    AllControlsEnabled = true;
    TouchAreaEnabled = true;
}

private void setupLookAround(FrameLayout rightScreenArea) {
    rightScreenArea.setOnTouchListener(new View.OnTouchListener() {
        private float lastX = 0;
        private float lastY = 0;
        private boolean isTouching = false;

        @Override
        public boolean onTouch(View v, MotionEvent event) {
            switch (event.getAction()) {
                case MotionEvent.ACTION_DOWN:
                    lastX = event.getX();
                    lastY = event.getY();
                    isTouching = true;
                    break;
                case MotionEvent.ACTION_MOVE:
                    if (isTouching) {
                        float deltaX = event.getX() - lastX;
                        float deltaY = event.getY() - lastY;
                        lastX = event.getX();
                        lastY = event.getY();
                        float sensitivityMultiplier = 15;
                        float rx = (deltaX * sensitivityMultiplier);
                        float ry = (deltaY * sensitivityMultiplier);
                        setCameraState(0, rx);
                        setCameraState(1, ry);
                    }
                    break;
                case MotionEvent.ACTION_UP:
                case MotionEvent.ACTION_CANCEL:
                    isTouching = false;
                    setCameraState(0, 0.0f);
                    setCameraState(1, 0.0f);
                    break;
            }
            return TouchAreaEnabled && AllControlsEnabled;
        }
    });
}

private void setupJoystick(FrameLayout joystickLayout, ImageView joystickKnob, boolean isLeft) {
    joystickLayout.post(() -> {
        final float joystickCenterX = joystickLayout.getWidth() / 2f;
        final float joystickCenterY = joystickLayout.getHeight() / 2f;

        joystickLayout.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (!AllControlsEnabled) return false;
                
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                    case MotionEvent.ACTION_MOVE:
                        float deltaX = event.getX() - joystickCenterX;
                        float deltaY = event.getY() - joystickCenterY;
                        float maxRadius = joystickLayout.getWidth() / 2f - joystickKnob.getWidth() / 2f;
                        float distance = (float) Math.sqrt(deltaX * deltaX + deltaY * deltaY);
                        if (distance > maxRadius) {
                            float scale = maxRadius / distance;
                            deltaX *= scale;
                            deltaY *= scale;
                        }
                        joystickKnob.setX(joystickCenterX + deltaX - joystickKnob.getWidth() / 2f);
                        joystickKnob.setY(joystickCenterY + deltaY - joystickKnob.getHeight() / 2f);

                        short x = (short) (deltaX / maxRadius * Short.MAX_VALUE);
                        short y = (short) (deltaY / maxRadius * Short.MAX_VALUE);
                        setAxis(isLeft ? ControllerButtons.AXIS_LX : ControllerButtons.AXIS_RX, x);
                        setAxis(isLeft ? ControllerButtons.AXIS_LY : ControllerButtons.AXIS_RY, y);
                        break;
                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_CANCEL:
                        joystickKnob.setX(joystickCenterX - joystickKnob.getWidth() / 2f);
                        joystickKnob.setY(joystickCenterY - joystickKnob.getHeight() / 2f);
                        setAxis(isLeft ? ControllerButtons.AXIS_LX : ControllerButtons.AXIS_RX, (short) 0);
                        setAxis(isLeft ? ControllerButtons.AXIS_LY : ControllerButtons.AXIS_RY, (short) 0);
                        break;
                }
                return true;
            }
        });
    });
}
}