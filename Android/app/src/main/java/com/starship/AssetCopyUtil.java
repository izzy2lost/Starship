package com.starship.android;

import android.content.Context;
import android.content.res.AssetManager;
import java.io.*;

public class AssetCopyUtil {
    public static void copyAssetFolder(Context context, String srcDir, String dstDir) throws IOException {
        AssetManager assetManager = context.getAssets();
        String[] assets = assetManager.list(srcDir);
        if (assets == null || assets.length == 0) {
            copyAsset(context, srcDir, dstDir);
        } else {
            File dir = new File(dstDir);
            if (!dir.exists()) dir.mkdirs();
            for (String asset : assets) {
                copyAssetFolder(context, srcDir + "/" + asset, dstDir + "/" + asset);
            }
        }
    }

    private static void copyAsset(Context context, String srcFile, String dstFile) throws IOException {
        AssetManager assetManager = context.getAssets();
        InputStream in = assetManager.open(srcFile);
        OutputStream out = new FileOutputStream(dstFile);
        byte[] buffer = new byte[1024];
        int read;
        while ((read = in.read(buffer)) != -1) out.write(buffer, 0, read);
        in.close();
        out.flush();
        out.close();
    }
}
