package com.starship;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.provider.DocumentsContract;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class RomPickerActivity extends Activity {
    static {
        System.loadLibrary("native-lib");
    }

    private static final int PICK_ROM_REQUEST = 1;
    private ProgressBar progressBar;
    private TextView loadingText;
    private Button selectRomButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rom_picker);

        progressBar = findViewById(R.id.progress_bar);
        loadingText = findViewById(R.id.loading_text);
        selectRomButton = findViewById(R.id.select_rom_button);

        selectRomButton.setOnClickListener(v -> pickRom());
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
            progressBar.setVisibility(View.VISIBLE);
            loadingText.setVisibility(View.VISIBLE);
            selectRomButton.setEnabled(false);

            new Thread(() -> {
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

                    Intent intent = new Intent(this, RomVersionSelectorActivity.class);
intent.putExtra("romPath", romFile.getAbsolutePath());
startActivity(intent);

                } catch (Exception e) {
                    e.printStackTrace();
                    runOnUiThread(() -> Toast.makeText(this, "Failed to load ROM", Toast.LENGTH_LONG).show());
                }
            }).start();
        }
    }

    public native void nativeInit(String romPath);
}
