package com.faip.piggy.djinni;

public class ServerMessageGetterImpl extends ServerMessageGetter {

    public static void init() {
        try {
            System.loadLibrary("server_message_getter_lib");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
