package org.oaknut.main;

import android.app.Application;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Looper;
import android.preference.PreferenceManager;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import static java.nio.charset.StandardCharsets.UTF_8;


public class App extends Application {

    static App app;
    static SharedPreferences prefs;
    static Handler handler;
    static {
        handler = new Handler(Looper.getMainLooper());
    }



    public App() {
        app = this;
    }

    @Override
    public void onCreate() {
      super.onCreate();
      prefs = PreferenceManager.getDefaultSharedPreferences(this);
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

    public static byte[] getPath(int i) {
        return app.getFilesDir().getAbsolutePath().getBytes(UTF_8);
    }

    public static int getPrefsInt(byte[] key, int defaultVal) {
        return prefs.getInt(new String(key, UTF_8), defaultVal);
    }
    public static void setPrefsInt(byte[] key, int val) {
        prefs.edit().putInt(new String(key, UTF_8),val).commit();
    }
    public static byte[] getPrefsString(byte[] key, byte[] defaultVal) {
        return prefs.getString(new String(key, UTF_8), new String(defaultVal, UTF_8)).getBytes(UTF_8);
    }
    public static void setPrefsString(byte[] key, byte[] val) {
        prefs.edit().putString(new String(key, UTF_8), new String(val, UTF_8)).commit();
    }
}
