package com.example.depthai_android_api;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.depthai_android_api.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'depthai_android_api' library on application startup.
    static {
        System.loadLibrary("depthai_android_api");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'depthai_android_api' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}