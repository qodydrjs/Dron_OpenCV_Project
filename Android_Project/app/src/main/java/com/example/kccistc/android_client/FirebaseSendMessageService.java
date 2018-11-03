package com.example.kccistc.android_client;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

import com.google.firebase.FirebaseApp;
import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;

public class FirebaseSendMessageService extends FirebaseMessagingService {
    public FirebaseSendMessageService() {
        super();
    }

    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        super.onMessageReceived(remoteMessage);
        System.out.println(remoteMessage.getNotification().getBody());
    }
}
