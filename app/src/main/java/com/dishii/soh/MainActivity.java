
package com.dishii.soh;
import org.libsdl.app.SDLActivity;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.content.pm.PackageManager;
import android.provider.Settings;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.FrameLayout;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.ImageView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

//This class is the main SDLActivity and just sets up a bunch of default files
public class MainActivity extends SDLActivity {

    private static final int FILE_HANDLER_REQUEST_CODE = 0;
    private static final int SPECIAL_STORAGE_PERMISSION_REQUEST_CODE = 1;

    SharedPreferences preferences;

    private boolean hasSpecialExternalStoragePermission = false;

    private boolean permissionPopupIsOpen = false;

    // this is a case where I feel like it is actually clearer to have a double negative boolean,
    // than to have one named "permissionPopupChargeIsAvailable = true", but you can let me know
    // if you would prefer to reorganize this.
    private boolean permissionPopupWasDeclined = false;

    private boolean hasInstalledExternalAssetFiles = false;

    @Override
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            "soh"
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        preferences = getSharedPreferences("com.dishii.soh.prefs", Context.MODE_PRIVATE);

        doVersionCheck();

        setupControllerOverlay();
        attachController();
    }

    private void doVersionCheck(){
        int currentVersion = BuildConfig.VERSION_CODE; // Use your app's version code
        int storedVersion = preferences.getInt("appVersion", 1);

        if (currentVersion > storedVersion) {
            // I tend to just copy all assets on every app launch, overwriting the old ones,
            // so that I don't have to change appVersion every time I need to make sure that the newest assets
            // are guaranteed to always be present. Also, when /storage/emulated/0/com.dishii.soh is used, the assets
            // could be from a whole different version of the app and that wouldn't be detected (at least by this
            // particular SharedPreferences), so my way always overwrites those.
            // My way is also very slow to load at every startup, which is ok for
            // apps that have only a few external assets, but for apps like this one that have
            // a lot of external assets, the slowness is pretty severe. Let me know if that is not desirable
            // and you would prefer it to work differently.
            //deleteOutdatedAssets();
            preferences.edit().putInt("appVersion", currentVersion).apply();
        }
    }

    // called from native code through JNI where necessary
    public String getExternalAssetsPath() {
        // the original location, /storage/emulated/0/Android/data/com.dishii.soh/files,
        // can be the fallback if the user denies the permission
        String externalAssetsPath = getExternalFilesDir(null).getAbsolutePath();

        if (!permissionPopupWasDeclined) {
            requestSpecialExternalStoragePermission();
        }

        while(permissionPopupIsOpen) {
            // Do nothing until a permission is chosen
            try {
                Thread.sleep(250);
            } catch (InterruptedException e) {
                // do nothing
            }
        }

        if (hasSpecialExternalStoragePermission) {
            // /storage/emulated/0/com.dishii.soh, also mounted at /sdcard/com.dishii.soh
            externalAssetsPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/" + getApplicationContext().getPackageName();
        }

        if (!hasInstalledExternalAssetFiles) {
            setupFiles(externalAssetsPath);
        }

        return externalAssetsPath;
    }

    // Request the special external storage permission
    private void requestSpecialExternalStoragePermission() {
        // Android 5 or older
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.LOLLIPOP_MR1) {
            hasSpecialExternalStoragePermission = true;
            return;
        }

        // Android 10 or older
        if (Build.VERSION.SDK_INT <= Build.VERSION_CODES.Q) {
            if (checkSelfPermission("android.permission.WRITE_EXTERNAL_STORAGE") == PackageManager.PERMISSION_GRANTED) {
                hasSpecialExternalStoragePermission = true;
                return;
            }

            requestPermissions(new String[]{"android.permission.WRITE_EXTERNAL_STORAGE"}, SPECIAL_STORAGE_PERMISSION_REQUEST_CODE);
            permissionPopupIsOpen = true;
            return;
        }

        // Android 11 or newer
        if (Environment.isExternalStorageManager()) {
            hasSpecialExternalStoragePermission = true;
            return;
        }

        try {
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
            intent.addCategory("android.intent.category.DEFAULT");
            intent.setData(Uri.parse(String.format("package:%s", getApplicationContext().getPackageName())));
            startActivityForResult(intent, SPECIAL_STORAGE_PERMISSION_REQUEST_CODE);
        } catch (Exception e) {
            Intent intent = new Intent();
            intent.setAction(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
            startActivityForResult(intent, SPECIAL_STORAGE_PERMISSION_REQUEST_CODE);
        }

        permissionPopupIsOpen = true;
    }

    // Handle permission request result
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == SPECIAL_STORAGE_PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                hasSpecialExternalStoragePermission = true;
            } else {
                permissionPopupWasDeclined = true;
            }
            permissionPopupIsOpen = false;
        }
    }

    private void setupFiles(String externalAssetsPath) {
        AssetCopyUtil.copyAssetsToExternal(this, externalAssetsPath);
        hasInstalledExternalAssetFiles = true;
    }

    private native void nativeHandleSelectedFile(String filePath);

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == FILE_HANDLER_REQUEST_CODE && resultCode == RESULT_OK) {
            Uri selectedFileUri = data.getData();
            String fileName = "ZELOOTD.z64";
            String destinationDirectory = getExternalAssetsPath();
            File destinationFile = new File(destinationDirectory, fileName);

            if (destinationDirectory != null) {
                try {
                    InputStream in = getContentResolver().openInputStream(selectedFileUri);
                    OutputStream out = new FileOutputStream(destinationFile);

                    byte[] buffer = new byte[4096];
                    int bytesRead;
                    while ((bytesRead = in.read(buffer)) != -1) {
                        out.write(buffer, 0, bytesRead);
                    }

                    in.close();
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            nativeHandleSelectedFile(destinationFile.getPath());
        }

        if (requestCode == SPECIAL_STORAGE_PERMISSION_REQUEST_CODE) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R /* Android 11 or newer */) {
                if (Environment.isExternalStorageManager()) {
                    hasSpecialExternalStoragePermission = true;
                } else {
                    permissionPopupWasDeclined = true;
                }
            }
            permissionPopupIsOpen = false;
        }
    }

    // called from native code through JNI where necessary
    public void openFilePicker() {
        // Create an Intent to open the file picker dialog
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.setType("*/*");

        // Start the file picker dialog
        startActivityForResult(intent, 0);
    }

    public native void attachController();
    public native void detachController();
    // Native method for setting button state
    public native void setButton(int button, boolean value);
    public native void setCameraState(int axis, float value);

    // Native method for setting joystick axis value
    public native void setAxis(int axis, short value);

    private Button button1, button2, button3, button4;
    private Button buttonA, buttonB, buttonX, buttonY;
    private Button buttonDpadUp, buttonDpadDown, buttonDpadLeft, buttonDpadRight;
    private Button buttonLB, buttonRB, buttonZ, buttonStart, buttonBack;
    private Button buttonToggle;
    private FrameLayout leftJoystick;
    private ImageView leftJoystickKnob;
    private View overlayView;

    // Function to set up the controller overlay (inflate layout and initialize buttons)
    private void setupControllerOverlay() {
        // Inflate the touchcontrol_overlay layout
        LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);
        overlayView = inflater.inflate(R.layout.touchcontrol_overlay, null);

        // Set layout params for overlayView to control positioning and sizing
        FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT
        );
        overlayView.setLayoutParams(layoutParams);
        // Add overlay view to the main layout (you may need to add it to a container like FrameLayout)
        ViewGroup view = (ViewGroup) getContentView();
        view.addView(overlayView);

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

        buttonToggle = overlayView.findViewById(R.id.buttonToggle);

        // Initialize joysticks and joystick knobs from the inflated layout
        leftJoystick = overlayView.findViewById(R.id.left_joystick);
        leftJoystickKnob = overlayView.findViewById(R.id.left_joystick_knob);

        FrameLayout rightScreenArea = overlayView.findViewById(R.id.right_screen_area);

        // Set OnTouchListeners for the Xbox controller buttons
        addTouchListener(buttonA, ControllerButtons.BUTTON_A); // SDL Button 0 (A)
        addTouchListener(buttonB, ControllerButtons.BUTTON_B); // SDL Button 1 (B)
        addTouchListener(buttonX, ControllerButtons.BUTTON_X); // SDL Button 2 (X)
        addTouchListener(buttonY, ControllerButtons.BUTTON_Y); // SDL Button 3 (Y)

        setupCButtons(buttonDpadUp, ControllerButtons.AXIS_RY, 1); // SDL Button 10 (D-Pad Up)
        setupCButtons(buttonDpadDown, ControllerButtons.AXIS_RY , -1); // SDL Button 11 (D-Pad Down)
        setupCButtons(buttonDpadLeft, ControllerButtons.AXIS_RX, 1); // SDL Button 12 (D-Pad Left)
        setupCButtons(buttonDpadRight, ControllerButtons.AXIS_RX, -1); // SDL Button 13 (D-Pad Right)

        addTouchListener(buttonLB, ControllerButtons.BUTTON_LB); // SDL Button 4 (LB)
        addTouchListener(buttonRB, ControllerButtons.BUTTON_RB); // SDL Button 5 (RB)
        addTouchListener(buttonZ, ControllerButtons.AXIS_RT); // SDL Button 5 (Z)

        addTouchListener(buttonStart, ControllerButtons.BUTTON_START); // SDL Button 7 (Start)
        addTouchListener(buttonBack, ControllerButtons.BUTTON_BACK); // SDL Button 6 (Back)


        // Setup joystick movement
        setupJoystick(leftJoystick, leftJoystickKnob, true); // Left joystick

        setupLookAround(rightScreenArea);

        setupToggleButton(buttonToggle,buttonGroup);

    }

    private void setupToggleButton(Button button, ViewGroup uiGroup){
        boolean isHidden = preferences.getBoolean("controlsVisible", false); // Default to 'false' (visible)
        uiGroup.setVisibility(isHidden ? View.INVISIBLE : View.VISIBLE); // Set the initial visibility based on the saved state
        /*if(isHidden){
            detachController();
        }*/
        button.setOnClickListener(new View.OnClickListener() {
            boolean isHidden = false;
            @Override
            public void onClick(View v) {
                if (isHidden) {
                    uiGroup.setVisibility(View.VISIBLE); // Show UI elements
                    //attachController();
                } else {
                    uiGroup.setVisibility(View.INVISIBLE); // Hide UI elements
                    //detachController();
                }
                preferences.edit().putBoolean("controlsVisible", !isHidden).apply();
                isHidden = !isHidden; // Toggle state
            }
        });
    }

    // Function to set a touch listener for each button
    private void addTouchListener(Button button, int buttonNum) {
        button.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
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
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        setAxis(buttonNum, direction<0 ? Short.MAX_VALUE : Short.MIN_VALUE);
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

    boolean TouchAreaEnabled = true;

    void DisableTouchArea(){
        TouchAreaEnabled = false;
    }
    void EnableTouchArea(){
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
                        // Start tracking the finger's position
                        lastX = event.getX();
                        lastY = event.getY();
                        isTouching = true;
                        break;

                    case MotionEvent.ACTION_MOVE:
                        if (isTouching) {
                            // Calculate the change in position (delta)
                            float deltaX = event.getX() - lastX;
                            float deltaY = event.getY() - lastY;

                            // Update the last position
                            lastX = event.getX();
                            lastY = event.getY();

                            // Increase sensitivity by using a larger multiplier
                            // Adjust these multipliers to suit your needs
                            float sensitivityMultiplier = 15; // Higher value for more sensitivity
                            float rx = (deltaX * sensitivityMultiplier);
                            float ry = (deltaY * sensitivityMultiplier);

                            // Send the mapped values to the joystick axes
                            setCameraState(0, rx); // Right stick X axis
                            setCameraState(1, ry); // Right stick Y axis
                        }
                        break;

                    case MotionEvent.ACTION_UP:
                    case MotionEvent.ACTION_CANCEL:
                        // Stop tracking the finger's position and reset joystick input
                        isTouching = false;
                        setCameraState(0, 0.0f); // Reset right stick X axis
                        setCameraState(1, 0.0f); // Reset right stick Y axis
                        break;
                }
                return TouchAreaEnabled; // Event full handled
            }
        });
    }





    // Function to set joystick movement with reset to center when not touched
    private void setupJoystick(FrameLayout joystickLayout, ImageView joystickKnob, boolean isLeft) {
        joystickLayout.post(() -> {
            // Calculate the joystick center once, before any events
            final float joystickCenterX = joystickLayout.getWidth() / 2f;
            final float joystickCenterY = joystickLayout.getHeight() / 2f;

            joystickLayout.setOnTouchListener(new View.OnTouchListener() {
                @Override
                public boolean onTouch(View v, MotionEvent event) {
                    switch (event.getAction()) {
                        case MotionEvent.ACTION_DOWN:
                        case MotionEvent.ACTION_MOVE:
                            // Calculate the joystick movement and move the knob
                            float deltaX = event.getX() - joystickCenterX;
                            float deltaY = event.getY() - joystickCenterY;

                            // Clamp the joystick movement to prevent it from going outside the area
                            float maxRadius = joystickLayout.getWidth() / 2f - joystickKnob.getWidth() / 2f;
                            float distance = (float) Math.sqrt(deltaX * deltaX + deltaY * deltaY);
                            if (distance > maxRadius) {
                                float scale = maxRadius / distance;
                                deltaX *= scale;
                                deltaY *= scale;
                            }

                            joystickKnob.setX(joystickCenterX + deltaX - joystickKnob.getWidth() / 2f);
                            joystickKnob.setY(joystickCenterY + deltaY - joystickKnob.getHeight() / 2f);

                            // Send joystick values to native C code
                            short x = (short) (deltaX / maxRadius * Short.MAX_VALUE);
                            short y = (short) (deltaY / maxRadius * Short.MAX_VALUE);

                            // Send X-axis and Y-axis values
                            setAxis(isLeft ? ControllerButtons.AXIS_LX : ControllerButtons.AXIS_RX, x); // X-axis
                            setAxis(isLeft ? ControllerButtons.AXIS_LY : ControllerButtons.AXIS_RY, y); // Y-axis
                            break;

                        case MotionEvent.ACTION_UP:
                        case MotionEvent.ACTION_CANCEL:
                            // Reset joystick knob to the center position (ensure it's placed correctly)
                            joystickKnob.setX(joystickCenterX - joystickKnob.getWidth() / 2f);
                            joystickKnob.setY(joystickCenterY - joystickKnob.getHeight() / 2f);

                            // Reset joystick values to 0 when released or canceled
                            setAxis(isLeft ? ControllerButtons.AXIS_LX : ControllerButtons.AXIS_RX, (short) 0); // X-axis
                            setAxis(isLeft ? ControllerButtons.AXIS_LY : ControllerButtons.AXIS_RY, (short) 0); // Y-axis
                            break;
                    }
                    return true;
                }
            });
        });



    }

}
