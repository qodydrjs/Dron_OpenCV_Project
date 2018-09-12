package com.example.kccistc.android_client;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.net.Socket;


public class MainActivity extends AppCompatActivity {

    private String default_ip = "192.168.1.61";
    private int default_Port = 5000;
    //Member
    private TextView et;
    private EditText ip, port;
    private Button btn_start,btn_reset,btn_send;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        et = findViewById(R.id.tx_log);
        btn_start = findViewById(R.id.btn_start);

        btn_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ip = findViewById(R.id.ed_ip);
                port = findViewById(R.id.ed_port);
            }
        });

        Client socket = new Client("192.168.0.8", 1234);
        socket.setClientCallback(new Client.ClientCallback () {
            @Override
            public void onMessage(String message) {

            }

            @Override
            public void onConnect(Socket socket) {
                socket.send("Hello World!\n");
               // socket.disconnect();
            }

            @Override
            public void onDisconnect(Socket socket, String message) {
            }

            @Override
            public void onConnectError(Socket socket, String message) {
            }
        });


        socket.connect();
    }

}
