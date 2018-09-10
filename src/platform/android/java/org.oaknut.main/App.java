package org.oaknut.main;

import android.app.Application;
import android.content.SharedPreferences;
import android.os.Handler;
import android.os.Looper;
import android.preference.PreferenceManager;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.nio.charset.Charset;
import java.util.Base64;
import java.util.UUID;



public class App extends Application {

    static App app;
    static SharedPreferences prefs;
    static Handler handler;
    static {
        handler = new Handler(Looper.getMainLooper());
    }
    static final Charset UTF_8 = Charset.forName("UTF-8");
    static WeakReference<MainActivity> currentActivity;

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
        if (defaultVal == null) defaultVal=new byte[] {};
        String s = prefs.getString(new String(key, UTF_8), new String(defaultVal, UTF_8));
        return s.getBytes(UTF_8);
    }
    public static void setPrefsString(byte[] key, byte[] val) {
        prefs.edit().putString(new String(key, UTF_8), new String(val, UTF_8)).commit();
    }

    public static byte[] getPrefsByteArray(String key, byte[] defaultVal) {
        String s = prefs.getString(key, null);
        if (s == null) {
            return defaultVal;
        }
        return android.util.Base64.decode(s, 0);
    }
    public static void setPrefsByteArray(String key, byte[] val) {
        prefs.edit().putString(key, android.util.Base64.encodeToString(val, 0)).commit();
    }

    public static String createUUID() {
        return UUID.randomUUID().toString();
    }

    public static void showKeyboard(boolean show) {
        MainActivity activity = currentActivity.get();
        if (activity != null) {
            activity.showKeyboard(show);
        }
    }
}
