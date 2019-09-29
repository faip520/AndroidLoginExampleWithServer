package com.faip.piggy;

import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;
import android.widget.Toast;

/**
 * 可以在非主线程弹出toast提示的工具类
 */
class ToastHelper {

    private static final Handler mainHandler = new Handler(Looper.getMainLooper());

    private static class ToastRunnable implements Runnable {

        private String message;
        private Context context;

        ToastRunnable(Context context, String message) {
            this.message = message;
            this.context = context;
        }

        @Override
        public void run() {
            if (context == null) {
                return;
            }

            if (TextUtils.isEmpty(message)) {
                return;
            }

            Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
        }
    }

    static void showToast(Context context, String msg) {
        mainHandler.post(new ToastRunnable(context, msg));
    }
}
