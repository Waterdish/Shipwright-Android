package com.dishii.soh;

import android.content.Context;
import android.content.res.AssetManager;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class AssetCopyUtil {

    public static void copyAssetsToExternal(Context context, String assetsFolderPath, String externalFolderPath) throws IOException {
        AssetManager assetManager = context.getAssets();
        String[] assetFiles = assetManager.list(assetsFolderPath);

        for (String assetFile : assetFiles) {
            String assetPath = assetsFolderPath + File.separator + assetFile;
            String externalPath = externalFolderPath + File.separator + assetFile;

            if (assetManager.list(assetPath).length > 0) {
                // It's a directory
                // Check if the directory exists in the external storage
                File externalDir = new File(externalPath);
                if (!externalDir.exists()) {
                    externalDir.mkdirs(); // Create the directory if it doesn't exist
                }

                // Recursively copy contents of the directory
                copyAssetsToExternal(context, assetPath, externalPath);
            } else {
                // It's a file
                File externalFile = new File(externalPath);
                if (!externalFile.exists()) {
                    // Check if the file exists in the external storage
                    InputStream in = null;
                    OutputStream out = null;

                    try {
                        in = assetManager.open(assetPath);
                        out = new FileOutputStream(externalPath);

                        byte[] buffer = new byte[1024];
                        int read;
                        while ((read = in.read(buffer)) != -1) {
                            out.write(buffer, 0, read);
                        }

                    } finally {
                        if (in != null) {
                            in.close();
                        }
                        if (out != null) {
                            out.close();
                        }
                    }
                }
            }
        }
    }
}
