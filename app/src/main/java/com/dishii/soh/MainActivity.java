
package com.dishii.soh;
import org.libsdl.app.SDLActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileOutputStream;
import android.Manifest;
import android.content.pm.PackageManager;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import android.util.Log;

//This class is the main SDLActivity and just sets up a bunch of default files
public class MainActivity extends SDLActivity{

    private static final int STORAGE_PERMISSION_REQUEST_CODE = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Check if storage permissions are granted
        if (Environment.isExternalStorageManager()) {
            setupFiles();
        } else {
            requestStoragePermission();
        }
    }

    // Request storage permission
    private void requestStoragePermission() {
        try {
            Uri uri = Uri.parse("package:" + BuildConfig.APPLICATION_ID);
            Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION, uri);
            startActivity(intent);
        } catch (Exception ex){
            Intent intent = new Intent();
            intent.setAction(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION);
            startActivity(intent);
        }
    }

    // Handle permission request result
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == STORAGE_PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                setupFiles();
            } else {
                // Permission denied, handle accordingly (e.g., show a message)
            }
        }
    }

    private void setupFiles(){
        File externalAssetsDir = new File(getExternalFilesDir(null), "assets");
        if (!externalAssetsDir.exists()) {
            try {
                externalAssetsDir.mkdirs();
                AssetCopyUtil.copyAssetsToExternal(this, "assets", externalAssetsDir.getAbsolutePath());
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        File externalSohOtrFile = new File(getExternalFilesDir(null), "soh.otr");
        if (!externalSohOtrFile.exists()) {
            try {
                InputStream in = getAssets().open("soh.otr");
                OutputStream out = new FileOutputStream(externalSohOtrFile);

                byte[] buffer = new byte[1024];
                int read;
                while ((read = in.read(buffer)) != -1) {
                    out.write(buffer, 0, read);
                }

                in.close();
                out.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}
