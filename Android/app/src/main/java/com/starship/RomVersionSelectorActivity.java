package com.starship;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

public class RomVersionSelectorActivity extends Activity {
    static {
        System.loadLibrary("native-lib");
    }

    private Spinner versionSpinner;
    private String romPath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_rom_version_selector);

        romPath = getIntent().getStringExtra("romPath");

        TextView detectedText = findViewById(R.id.detected_version);
        detectedText.setText("Detected: Auto"); // Optionally read detection result

        versionSpinner = findViewById(R.id.version_spinner);
        String[] versions = {
            "Auto Detect",
            "us/rev0",
            "us/rev1",
            "jp/rev0",
            "jp/rev1",
            "eu/rev0"
        };
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, versions);
        versionSpinner.setAdapter(adapter);

        Button launchButton = findViewById(R.id.launch_button);
        launchButton.setOnClickListener(v -> {
            String selected = versionSpinner.getSelectedItem().toString();
            String override = selected.equals("Auto Detect") ? null : selected;
            launchGame(romPath, override);
        });
    }

    private void launchGame(String path, String versionOverride) {
        nativeInit(path, versionOverride);
    }

    public native void nativeInit(String romPath, String versionOverride);
}
