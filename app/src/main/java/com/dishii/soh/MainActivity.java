
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

//This class is the main SDLActivity
public class MainActivity extends SDLActivity{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //Check if folder/file exists and if it doesn't, create it
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
