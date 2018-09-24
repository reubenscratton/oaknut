package org.oaknut.main;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.text.Editable;
import android.text.InputType;
import android.text.SpannableStringBuilder;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Choreographer;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.CompletionInfo;
import android.view.inputmethod.CorrectionInfo;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.ExtractedText;
import android.view.inputmethod.ExtractedTextRequest;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputContentInfo;
import android.view.inputmethod.InputMethodManager;



public class MainActivity extends Activity implements SurfaceHolder.Callback2, ViewTreeObserver.OnGlobalLayoutListener, Choreographer.FrameCallback {

    static {
        System.loadLibrary("oaknutapp");
    }

    private static final String KEY_NATIVE_SAVED_STATE = "android:native_state";

    private long nativePtr;
    private NativeView nativeView;

    private SurfaceHolder mCurSurfaceHolder;

    final int[] mLocation = new int[2];
    int mLastContentX;
    int mLastContentY;
    int mLastContentWidth;
    int mLastContentHeight;

    private boolean mDestroyed;

    private native long onCreateNative(AssetManager assetManager, float screenScale, float statusBarHeight, float navigationBarHeight, long rootVC);
    private native void onStartNative(long nativePtr);
    private native void onResumeNative(long nativePtr);
    private native byte[] onSaveInstanceStateNative(long nativePtr);
    private native void onPauseNative(long nativePtr);
    private native void onStopNative(long nativePtr);
    private native void onConfigurationChangedNative(long nativePtr);
    private native void onWindowFocusChangedNative(long nativePtr, boolean focused);
    private native void onSurfaceCreatedNative(long nativePtr, Surface surface);
    private native void onSurfaceChangedNative(long nativePtr, Surface surface, int format, int width, int height);
    private native void onSurfaceRedrawNeededNative(long nativePtr, Surface surface);
    private native void redrawNative(long nativePtr);
    private native void onSurfaceDestroyedNative(long nativePtr);
    private native void onContentRectChangedNative(long nativePtr, int x, int y, int w, int h);
    private native boolean onKeyEventNative(long nativePtr, boolean isDown, int keyCode, int charCode);
    private native void onTouchEventNative(long nativePtr, int pointer, int action, long time, float x, float y);
    private native boolean onBackPressedNative(long nativePtr);
    private native void onDestroyNative(long nativePtr);

    private native boolean textInputIsFocused(long nativePtr);
    private native int textInputGetInputType(long nativePtr);
    private native int textInputGetSelStart(long nativePtr);
    private native int textInputGetSelEnd(long nativePtr);
    private native byte[] textInputGetText(long nativePtr);
    private native void textInputSetText(long nativePtr, byte[] text, int newCursorPosition);
    private native void textInputActionPressed(long nativePtr);

    //Editable editable = new SpannableStringBuilder();
    NativeInputConnection currentInputConnection;

    /*void updateEditable() {
        byte[] textBytes = textInputGetText(nativePtr);
        editable.clear();
        if (textBytes != null) {
            editable.append(new String(textBytes, App.UTF_8));
        }
    }*/


    String getCurrentText() {
        byte[] textBytes = textInputGetText(nativePtr);
        String str = "";
        if (textBytes != null) {
            str = new String(textBytes, App.UTF_8);
        }
        return str;
    }


    /**
     * Important lesson re IMEs: sometimes the IME will call sendKeyEvent() cos you pressed a key
     * and other times it will call commitText() with the text to insert or replace.
     */

    private class NativeInputConnection implements InputConnection {


        @Override
        public CharSequence getTextBeforeCursor(int n, int flags) {
            return null;
        }

        @Override
        public CharSequence getTextAfterCursor(int n, int flags) {
            return null;
        }

        @Override
        public CharSequence getSelectedText(int flags) {
            String str = getCurrentText();
            int s = textInputGetSelStart(nativePtr);
            int e = textInputGetSelEnd(nativePtr);
            s = Math.min(s, str.length());
            e = Math.min(e, str.length());
            if (e<s) {
                int tmp = e;
                e = s;
                s = tmp;
            }
            return str.substring(s,e);
        }

        @Override
        public int getCursorCapsMode(int reqModes) {
            return 0;
        }

        @Override
        public ExtractedText getExtractedText(ExtractedTextRequest request, int flags) {
            return null;
        }

        @Override
        public boolean deleteSurroundingText(int beforeLength, int afterLength) {
            return false;
        }

        @Override
        public boolean deleteSurroundingTextInCodePoints(int beforeLength, int afterLength) {
            return false;
        }

        @Override
        public boolean setComposingText(CharSequence text, int newCursorPosition) {
            return false;
        }

        @Override
        public boolean setComposingRegion(int start, int end) {
            return false;
        }

        @Override
        public boolean finishComposingText() {
            return false;
        }

        @Override
        public boolean commitText(CharSequence text, int newCursorPosition) {
            textInputSetText(nativePtr, text.toString().getBytes(App.UTF_8), newCursorPosition);
            return true;
        }

        @Override
        public boolean commitCompletion(CompletionInfo text) {
            return false;
        }

        @Override
        public boolean commitCorrection(CorrectionInfo correctionInfo) {
            return false;
        }

        @Override
        public boolean setSelection(int start, int end) {
            return false;
        }

        @Override
        public boolean performEditorAction(int editorAction) {
            textInputActionPressed(nativePtr);
            return true;
        }

        @Override
        public boolean performContextMenuAction(int id) {
            return false;
        }

        @Override
        public boolean beginBatchEdit() {
            return false;
        }

        @Override
        public boolean endBatchEdit() {
            return false;
        }

        @Override
        public boolean sendKeyEvent(KeyEvent event) {
            return dispatchKeyEvent(event);
        }

        @Override
        public boolean clearMetaKeyStates(int states) {
            return false;
        }

        @Override
        public boolean reportFullscreenMode(boolean enabled) {
            return false;
        }

        @Override
        public boolean performPrivateCommand(String action, Bundle data) {
            return false;
        }

        @Override
        public boolean requestCursorUpdates(int cursorUpdateMode) {
            return false;
        }

        @Override
        public Handler getHandler() {
            return null;
        }

        @Override
        public void closeConnection() {

        }

        @Override
        public boolean commitContent(@NonNull InputContentInfo inputContentInfo, int flags, @Nullable Bundle opts) {
            return false;
        }
    }


    private class NativeView extends View {
        NativeView(Context context) {
            super(context);
            setFocusableInTouchMode(true);
        }

        @Override
        public boolean onCheckIsTextEditor() {
            return textInputIsFocused(nativePtr);
        }

        @Override
        public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
            if (!textInputIsFocused(nativePtr)) {
                currentInputConnection = null;
            } else {
                outAttrs.inputType = textInputGetInputType(nativePtr);
                outAttrs.initialSelStart = textInputGetSelStart(nativePtr);
                outAttrs.initialSelEnd = textInputGetSelEnd(nativePtr);
                currentInputConnection = new NativeInputConnection();
            }
            return currentInputConnection;
        }
    }

    void keyboardNotifyTextChanged() {
        InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.restartInput(nativeView);
    }
    void keyboardNotifyTextSelectionChanged() {
        InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        int selStart = textInputGetSelStart(nativePtr);
        int selEnd = textInputGetSelEnd(nativePtr);
        imm.updateSelection(nativeView, selStart, selEnd, 0, 0);
        //currentInputConnection.setSelection(selStart, selEnd);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        Window window = getWindow();
        window.takeSurface(this);
        window.setFormat(PixelFormat.RGBA_8888);
        window.setSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_UNSPECIFIED | WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);

        window.getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);

        nativeView = new NativeView(this);
        setContentView(nativeView);
        //nativeView.requestFocus();
        nativeView.getViewTreeObserver().addOnGlobalLayoutListener(this);


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


        long rootVC = getIntent().getLongExtra("rootVC", 0);
        nativePtr = onCreateNative(getAssets(), realDisplayMetrics.density, statusBarHeight, navigationBarHeight, rootVC);

        super.onCreate(savedInstanceState);

    }


    @Override
    protected void onDestroy() {
        mDestroyed = true;
        if (mCurSurfaceHolder != null) {
            onSurfaceDestroyedNative(nativePtr);
            mCurSurfaceHolder = null;
        }
        onDestroyNative(nativePtr);
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        super.onPause();
        onPauseNative(nativePtr);
    }

    @Override
    protected void onResume() {
        super.onResume();
        onResumeNative(nativePtr);
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        byte[] state = onSaveInstanceStateNative(nativePtr);
        if (state != null) {
            outState.putByteArray(KEY_NATIVE_SAVED_STATE, state);
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        onStartNative(nativePtr);
    }

    @Override
    protected void onStop() {
        super.onStop();
        onStopNative(nativePtr);
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        if (!mDestroyed) {
            onConfigurationChangedNative(nativePtr);
        }
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (!mDestroyed) {
            onWindowFocusChangedNative(nativePtr, hasFocus);
        }
    }

    public void surfaceCreated(SurfaceHolder holder) {
        if (!mDestroyed) {
            mCurSurfaceHolder = holder;
            Surface surface = holder.getSurface();
            onSurfaceCreatedNative(nativePtr, surface);
            Choreographer.getInstance().postFrameCallback(this);
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (!mDestroyed) {
            mCurSurfaceHolder = holder;
            onSurfaceChangedNative(nativePtr, holder.getSurface(), format, width, height);
        }
    }

    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        if (!mDestroyed) {
            mCurSurfaceHolder = holder;
            onSurfaceRedrawNeededNative(nativePtr, holder.getSurface());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        mCurSurfaceHolder = null;
        if (!mDestroyed) {
            onSurfaceDestroyedNative(nativePtr);
        }
    }

    public void onGlobalLayout() {
        nativeView.getLocationInWindow(mLocation);
        int w = nativeView.getWidth();
        int h = nativeView.getHeight();
        if (mLocation[0] != mLastContentX || mLocation[1] != mLastContentY
                || w != mLastContentWidth || h != mLastContentHeight) {
            mLastContentX = mLocation[0];
            mLastContentY = mLocation[1];
            mLastContentWidth = w;
            mLastContentHeight = h;
            if (!mDestroyed) {
                onContentRectChangedNative(nativePtr, mLastContentX,
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
        onTouchEventNative(nativePtr, finger, action, event.getEventTime(), x, y);
        return true;
    }

    void setWindowFlags(int flags, int mask) {
        getWindow().setFlags(flags, mask);
    }

    void setWindowFormat(int format) {
        getWindow().setFormat(format);
    }


    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        onKeyEventNative(nativePtr,true, keyCode, event.getUnicodeChar());
        return false; // always return false, otherwise we don't get compound key events eg Shift+etc
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode==KeyEvent.KEYCODE_BACK) {
            onBackPressed();
            return true;
        }
        return onKeyEventNative(nativePtr,false, keyCode, event.getUnicodeChar());
    }

    @Override
    public void doFrame(long frameTimeNanos) {
        redrawNative(nativePtr);
        Choreographer.getInstance().postFrameCallback(this);
    }

    @Override
    public void onBackPressed() {
        if (!onBackPressedNative(nativePtr)) {
            super.onBackPressed();
        }
    }

    public void showKeyboard(boolean show) {
        InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        if (show) {
            nativeView.requestFocus();
            imm.showSoftInput(nativeView, InputMethodManager.SHOW_IMPLICIT);
            imm.restartInput(nativeView);
        } else {
            imm.hideSoftInputFromWindow(nativeView.getWindowToken(), InputMethodManager.HIDE_IMPLICIT_ONLY);
        }
    }

}
