package org.oaknut.main;

import android.app.Application;
import android.os.Handler;
import android.os.Looper;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;


public class App extends Application {

    static App app;
    static Handler handler;
    static {
        handler = new Handler(Looper.getMainLooper());
    }



    public App() {
        app = this;
    }



    public static byte[] loadAsset(String assetPath) {
        try {
            InputStream inputStream = app.getAssets().open(assetPath);
            byte[] buffer = new byte[8192];
            int bytesRead;
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            while ((bytesRead = inputStream.read(buffer)) != -1) {
                output.write(buffer, 0, bytesRead);
            }
            inputStream.close();
            return output.toByteArray();
        }
        catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public static String getDocsPath() {
        return app.getFilesDir().getAbsolutePath();
    }

}
