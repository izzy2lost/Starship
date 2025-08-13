package com.starship.android;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;

public class DialogActivity extends Activity {
    public static final String EXTRA_TITLE = "title";
    public static final String EXTRA_MESSAGE = "message";
    public static final String EXTRA_POSITIVE_BUTTON = "positive_button";
    public static final String EXTRA_NEGATIVE_BUTTON = "negative_button";
    public static final String EXTRA_CANCELABLE = "cancelable";
    public static final String EXTRA_DIALOG_TYPE = "dialog_type";
    
    public static final int DIALOG_TYPE_FOLDER_PROMPT = 1;
    public static final int DIALOG_TYPE_FILE_NOT_FOUND = 2;
    public static final int DIALOG_TYPE_COPY_COMPLETE = 3;
    public static final int DIALOG_TYPE_FILE_READY = 4;
    
    public static final int RESULT_POSITIVE = RESULT_OK;
    public static final int RESULT_NEGATIVE = RESULT_CANCELED;
    public static final int RESULT_FOLDER_PICKER = 100;
    public static final int RESULT_TORCH_DOWNLOAD = 101;
    public static final int RESULT_FILE_PICKER = 102;
    public static final int RESULT_RESTART = 103;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        Intent intent = getIntent();
        String title = intent.getStringExtra(EXTRA_TITLE);
        String message = intent.getStringExtra(EXTRA_MESSAGE);
        String positiveButton = intent.getStringExtra(EXTRA_POSITIVE_BUTTON);
        String negativeButton = intent.getStringExtra(EXTRA_NEGATIVE_BUTTON);
        boolean cancelable = intent.getBooleanExtra(EXTRA_CANCELABLE, true);
        int dialogType = intent.getIntExtra(EXTRA_DIALOG_TYPE, 0);
        
        AlertDialog.Builder builder = new AlertDialog.Builder(this, R.style.RoundedDialog);
        builder.setTitle(title);
        builder.setMessage(message);
        builder.setCancelable(cancelable);
        
        // Set up buttons based on dialog type
        switch (dialogType) {
            case DIALOG_TYPE_FOLDER_PROMPT:
                builder.setPositiveButton("Select Folder", (d, w) -> {
                    setResult(RESULT_FOLDER_PICKER);
                    finish();
                });
                break;
                
            case DIALOG_TYPE_FILE_NOT_FOUND:
                builder.setPositiveButton("Download Torch App", (d, w) -> {
                    android.util.Log.i("DialogActivity", "Torch download button pressed");
                    setResult(RESULT_TORCH_DOWNLOAD);
                    finish();
                });
                builder.setNegativeButton("Select sf64.o2r File", (d, w) -> {
                    android.util.Log.i("DialogActivity", "File picker button pressed");
                    setResult(RESULT_FILE_PICKER);
                    finish();
                });
                break;
                
            case DIALOG_TYPE_COPY_COMPLETE:
                builder.setPositiveButton("Restart", (d, w) -> {
                    setResult(RESULT_RESTART);
                    finish();
                });
                builder.setNegativeButton("Later", (d, w) -> {
                    setResult(RESULT_NEGATIVE);
                    finish();
                });
                break;
                
            case DIALOG_TYPE_FILE_READY:
                builder.setPositiveButton("Restart", (d, w) -> {
                    setResult(RESULT_RESTART);
                    finish();
                });
                break;
                
            default:
                builder.setPositiveButton(positiveButton != null ? positiveButton : "OK", (d, w) -> {
                    setResult(RESULT_POSITIVE);
                    finish();
                });
                if (negativeButton != null) {
                    builder.setNegativeButton(negativeButton, (d, w) -> {
                        setResult(RESULT_NEGATIVE);
                        finish();
                    });
                }
                break;
        }
        
        AlertDialog dialog = builder.create();
        dialog.setOnDismissListener(d -> finish());
        dialog.show();
    }
}