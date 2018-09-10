package org.oaknut.main;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.Choreographer;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewConfiguration;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;

import java.lang.ref.WeakReference;


public class MainActivity extends Activity implements SurfaceHolder.Callback2, ViewTreeObserver.OnGlobalLayoutListener, Choreographer.FrameCallback {

    static {
        System.loadLibrary("oaknutapp");
    }

    private static final String KEY_NATIVE_SAVED_STATE = "android:native_state";

    private View mNativeContentView;

    private SurfaceHolder mCurSurfaceHolder;

    final int[] mLocation = new int[2];
    int mLastContentX;
    int mLastContentY;
    int mLastContentWidth;
    int mLastContentHeight;

    private boolean mDestroyed;

    private native void onCreateNative(AssetManager assetManager, float screenScale, float statusBarHeight, float navigationBarHeight);
    private native void onStartNative();
    private native void onResumeNative();
    private native byte[] onSaveInstanceStateNative();
    private native void onPauseNative();
    private native void onStopNative();
    private native void onConfigurationChangedNative();
    private native void onWindowFocusChangedNative(boolean focused);
    private native void onSurfaceCreatedNative(Surface surface);
    private native void onSurfaceChangedNative(Surface surface, int format, int width, int height);
    private native void onSurfaceRedrawNeededNative(Surface surface);
    private native void redrawNative();
    private native void onSurfaceDestroyedNative();
    private native void onContentRectChangedNative(int x, int y, int w, int h);
    private native boolean onKeyEventNative(boolean isDown, int keyCode, int charCode);
    private native void onTouchEventNative(int pointer, int action, long time, float x, float y);
    private native boolean onBackPressedNative();
    private native void onDestroyNative();

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Window window = getWindow();
        window.takeSurface(this);
        window.setFormat(PixelFormat.RGBA_8888);
        window.setSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_UNSPECIFIED | WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);

        mNativeContentView = new View(this);
        setContentView(mNativeContentView);
        mNativeContentView.requestFocus();
        mNativeContentView.getViewTreeObserver().addOnGlobalLayoutListener(this);


        // Get status bar height. If it's non-zero then the top window inset is zero because
        // we can't render under it. If it's zero then work out what the bar height is
        // and use that as the inset.
        Rect rectangle = new Rect();
        window.getDecorView().getWindowVisibleDisplayFrame(rectangle);
        int statusBarHeight = rectangle.top;
        if (statusBarHeight == 0) {
            int resourceId = getResources().getIdentifier("status_bar_height", "dimen", "android");
            if (resourceId > 0) {
                statusBarHeight = getResources().getDimensionPixelSize(resourceId);
            }
        } else {
            statusBarHeight = 0;
        }

        // Detect nav bar
        Display d = getWindowManager().getDefaultDisplay();
        DisplayMetrics realDisplayMetrics = new DisplayMetrics();
        d.getRealMetrics(realDisplayMetrics);
        int realHeight = realDisplayMetrics.heightPixels;
        DisplayMetrics displayMetrics = new DisplayMetrics();
        d.getMetrics(displayMetrics);
        int displayHeight = displayMetrics.heightPixels;
        float navigationBarHeight =  (realHeight - displayHeight);


        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        onCreateNative(getAssets(), metrics.density, statusBarHeight, navigationBarHeight);

        super.onCreate(savedInstanceState);

    }


    @Override
    protected void onDestroy() {
        mDestroyed = true;
        if (mCurSurfaceHolder != null) {
            onSurfaceDestroyedNative();
            mCurSurfaceHolder = null;
        }
        onDestroyNative();
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        super.onPause();
        onPauseNative();
    }

    @Override
    protected void onResume() {
        super.onResume();
        App.currentActivity = new WeakReference<>(this);
        onResumeNative();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        byte[] state = onSaveInstanceStateNative();
        if (state != null) {
            outState.putByteArray(KEY_NATIVE_SAVED_STATE, state);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        onStartNative();
    }

    @Override
    protected void onStop() {
        super.onStop();
        onStopNative();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (!mDestroyed) {
            onConfigurationChangedNative();
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!mDestroyed) {
            onWindowFocusChangedNative(hasFocus);
        }
    }

    public void surfaceCreated(SurfaceHolder holder) {
        if (!mDestroyed) {
            mCurSurfaceHolder = holder;
            Surface surface = holder.getSurface();
            onSurfaceCreatedNative(surface);
            Choreographer.getInstance().postFrameCallback(this);
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (!mDestroyed) {
            mCurSurfaceHolder = holder;
            onSurfaceChangedNative(holder.getSurface(), format, width, height);
        }
    }

    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        if (!mDestroyed) {
            mCurSurfaceHolder = holder;
            onSurfaceRedrawNeededNative(holder.getSurface());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        mCurSurfaceHolder = null;
        if (!mDestroyed) {
            onSurfaceDestroyedNative();
        }
    }

    public void onGlobalLayout() {
        mNativeContentView.getLocationInWindow(mLocation);
        int w = mNativeContentView.getWidth();
        int h = mNativeContentView.getHeight();
        if (mLocation[0] != mLastContentX || mLocation[1] != mLastContentY
                || w != mLastContentWidth || h != mLastContentHeight) {
            mLastContentX = mLocation[0];
            mLastContentY = mLocation[1];
            mLastContentWidth = w;
            mLastContentHeight = h;
            if (!mDestroyed) {
                onContentRectChangedNative(mLastContentX,
                        mLastContentY, mLastContentWidth, mLastContentHeight);
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getAction();
        int finger = event.getPointerId(0);
        float x = event.getRawX();
        float y = event.getRawY();
        onTouchEventNative(finger, action, event.getEventTime(), x, y);
        return true;
    }

    void setWindowFlags(int flags, int mask) {
        getWindow().setFlags(flags, mask);
    }

    void setWindowFormat(int format) {
        getWindow().setFormat(format);
    }

    void showKeyboard(boolean show) {
        InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        if (show) {
            imm.showSoftInput(mNativeContentView, InputMethodManager.SHOW_FORCED);
        } else {
            imm.hideSoftInputFromWindow(mNativeContentView.getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);
        }
    }


    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return onKeyEventNative(true, keyCode, event.getUnicodeChar());
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode==KeyEvent.KEYCODE_BACK) {
            onBackPressed();
            return true;
        }
        return onKeyEventNative(false, keyCode, event.getUnicodeChar());
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        redrawNative();
        Choreographer.getInstance().postFrameCallback(this);
    }

    @Override
    public void onBackPressed() {
        if (!onBackPressedNative()) {
            super.onBackPressed();
        }
    }

}
