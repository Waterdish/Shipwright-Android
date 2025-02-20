package com.dishii.soh;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class AssetCopyUtil {
    // based on https://stackoverflow.com/a/8366081/11708026
    // This is desirable because it bulk copies all assets indiscriminately, simplifying the code in MainActivity
    // the side effect is that, some people will see strange, unidentifiable files appearing in their destination folder.
    // those extra files do not come from the app itself. They originate from something inside the Android ROM
    // of the device they are using, and can be observed to vary between different models of devices.
    public static void copyAssetsToExternal(Context context, String externalFolderPath) {
        externalFolderPath = externalFolderPath + "/";
        copyFileOrDir(context, "", externalFolderPath); // copy all files in assets folder to the destination
    }

    private static void copyFileOrDir(Context context, String srcpath, String destpath) {
        AssetManager assetManager = context.getAssets();
        String assets[] = null;
        String tag = "AssetCopyUtil";
        try {
            Log.i(tag, "copyFileOrDir() " + srcpath);
            assets = assetManager.list(srcpath);
            if (assets.length == 0) {
                copyFile(context, srcpath, destpath);
            } else {
                String fullPath = destpath + srcpath;
                Log.i(tag, "path=" + fullPath);
                File dir = new File(fullPath);
                if (!dir.exists())
                    if (!dir.mkdirs())
                        Log.i(tag, "could not create dir " + fullPath);
                for (int i = 0; i < assets.length; ++i) {
                    String p;
                    if (srcpath.isEmpty())
                        p = "";
                    else
                        p = srcpath + "/";
                    copyFileOrDir(context,p + assets[i], destpath);
                }
            }
        } catch (IOException ex) {
            Log.e(tag, "I/O Exception", ex);
        }
    }

    private static void copyFile(Context context, String filename, String destpath) {
        AssetManager assetManager = context.getAssets();
        InputStream in = null;
        OutputStream out = null;
        String newFileName = null;
        String tag = "AssetCopyUtil";
        try {
            Log.i(tag, "copyFile() " + filename);
            in = assetManager.open(filename);
            newFileName = destpath + filename;
            out = new FileOutputStream(newFileName);
            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
        } catch (Exception e) {
            Log.e(tag, "Exception in copyFile() of " + newFileName);
            Log.e(tag, "Exception in copyFile() " + e.toString());
        }
    }
}
