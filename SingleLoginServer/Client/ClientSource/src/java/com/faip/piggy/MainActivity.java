package com.faip.piggy;

import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;

import com.faip.piggy.djinni.ServerMessageGetterImpl;
import com.sirvar.robin.RobinActivity;

public class MainActivity extends RobinActivity {

    // 本地server的ip地址和端口号
    // 由于证书的关系只能使用域名，本地hosts文件要添加一条规则将本域名映射到正确的ip上
    private static final String SERVER_ADDRESS = "www.myserver.com:50051";

    private LoginRunnable mLoginRunnable;
    private Thread mThread;

    /**
     *
     * @param savedInstanceState
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ServerMessageGetterImpl.init();

        // 设置登录注册等页面的显示文本
        setLoginTitle("Sign in");
        setSignupTitle("Sign up");
        setForgotPasswordTitle("");
    }

    @Override
    protected void onLogin(final String user_name, final String user_password) {
        if (TextUtils.isEmpty(user_name)) {
            ToastHelper.showToast(MainActivity.this, "Empty user name!");
            return;
        }

        if (TextUtils.isEmpty(user_password)) {
            ToastHelper.showToast(MainActivity.this, "Empty user password!");
            return;
        }

        // 用户名合法性判断
        if (!StringUtils.isStringOnlyNumberAndLetter(user_name)) {
            ToastHelper.showToast(
                    MainActivity.this, "User name only support number and letter!");
            return;
        }

        // 密码合法性判断
        if (!StringUtils.isStringOnlyNumberAndLetter(user_password)) {
            ToastHelper.showToast(
                    MainActivity.this, "User password only support number and letter!");
            return;
        }

        // 用户名最多支持到10位
        if (user_name.length() > 10) {
            ToastHelper.showToast(MainActivity.this, "User name len more than 10!");
            return;
        }

        // 用户密码最多支持到10位
        if (user_password.length() > 10) {
            ToastHelper.showToast(MainActivity.this, "User password len more than 10!");
            return;
        }

        // 每次点击登录按钮，先尝试关闭已有的登录状态查询线程
        if (mLoginRunnable != null) {
            mLoginRunnable.needExit = true;
        }
        if (mThread != null) {
            mThread.interrupt();
        }

        Log.d("Faip", "On login user = " + user_name);

        mLoginRunnable = new LoginRunnable() {
            @Override
            public void run() {
                super.run();

                // 获取登录结果，为空，则登录失败，登录成功返回登录成功后的session
                String reply = ServerMessageGetterImpl.tryLogin(SERVER_ADDRESS, user_name, user_password);

                Log.d("Faip", "Try login result = " + reply);

                if (TextUtils.isEmpty(reply)) {
                    ToastHelper.showToast(MainActivity.this, "Login fail");
                    return;
                } else {
                    ToastHelper.showToast(MainActivity.this, "Login success");
                }

                // 登录成功后，无限循环判断当前登录session是否被挤下线了，一秒检测一次
                while (!needExit && FunctionToggle.NEED_CHECK_STATUS) {
                    Log.d("Faip", "Check status loop start once!");
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    // 向服务器查询登录session是否已经被挤下线
                    // 返回非0的结果就是被挤下线了
                    String onlineResult = ServerMessageGetterImpl.tryStatus(SERVER_ADDRESS, user_name, reply);
                    Log.d("Faip", "Check status result = " + onlineResult);

                    // 如果被挤下线，弹toast提示，如果线程退出标志位被置为true，则不弹toast，防止自己挤自己下线时的多余toast提示
                    // 一旦查询到被踢下线，则退出无限循环轮询
                    if (!"0".equals(onlineResult) && !needExit) {
                        ToastHelper.showToast(MainActivity.this, "Kicked out!");
                        return;
                    }
                }
            }
        };

        mThread = new Thread(mLoginRunnable);
        mThread.start();
    }

    @Override
    protected void onSignup(final String name, String email, final String password) {
        if (TextUtils.isEmpty(name)) {
            ToastHelper.showToast(MainActivity.this, "Empty user name!");
            return;
        }

        if (TextUtils.isEmpty(password)) {
            ToastHelper.showToast(MainActivity.this, "Empty user password!");
            return;
        }

        // 用户名合法性判断
        if (!StringUtils.isStringOnlyNumberAndLetter(name)) {
            ToastHelper.showToast(
                    MainActivity.this, "User name only support number and letter!");
            return;
        }

        // 密码合法性判断
        if (!StringUtils.isStringOnlyNumberAndLetter(password)) {
            ToastHelper.showToast(
                    MainActivity.this, "User password only support number and letter!");
            return;
        }

        // 用户名最多支持到10位
        if (name.length() > 16) {
            ToastHelper.showToast(MainActivity.this, "User name len more than 16!");
            return;
        }

        // 用户名最多支持到10位
        if (name.length() < 8) {
            ToastHelper.showToast(MainActivity.this, "User name len less than 8!");
            return;
        }

        // 用户名最多支持到10位
        if (password.length() < 8) {
            ToastHelper.showToast(MainActivity.this, "Password len less than 8!");
            return;
        }

        // 用户密码最多支持到10位
        if (password.length() > 16) {
            ToastHelper.showToast(MainActivity.this, "User password len more than 16!");
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                // 访问服务器的注册接口，并toast提示注册结果
                String reply = ServerMessageGetterImpl.tryRegister(SERVER_ADDRESS, name, password);
                ToastHelper.showToast(MainActivity.this, reply);
            }
        }).start();
    }

    @Override
    protected void onForgotPassword(String email) {

    }

    @Override
    protected void onGoogleLogin() {

    }

    @Override
    protected void onFacebookLogin() {

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // 退出前尝试清理activity的资源
        if (mLoginRunnable != null) {
            mLoginRunnable.needExit = true;
        }

        if (mThread != null) {
            mThread.interrupt();
        }
    }
}
