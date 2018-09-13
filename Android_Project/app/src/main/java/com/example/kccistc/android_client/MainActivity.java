package com.example.kccistc.android_client;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.design.widget.BottomNavigationView;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.net.Socket;


public class MainActivity extends AppCompatActivity {

    private String default_ip = "192.168.1.61";
    private int default_Port = 5001;
    //Member
    private TextView et;
    private EditText ip, port;
    private Button btn_start,btn_reset,btn_send;
    private BottomNavigationView bottomNavigationView;
    private ViewPager viewPager;

    Client socket2;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        viewPager = findViewById(R.id.ViewPager1);
        viewPager.setAdapter(getSupportFragmentManager());
        et = findViewById(R.id.tx_log);
        btn_start = findViewById(R.id.btn_start);
        bottomNavigationView = findViewById(R.id.bottom_navigation);
        //bottomNavigationView 리스너 설정
        bottomNavigationView.setOnNavigationItemSelectedListener(new BottomNavigationView.OnNavigationItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                switch(item.getItemId()){
                    case R.id.navigation_1:
                        return true;
                    case R.id.navigation_2:
                        return true;
                    case R.id.navigation_3:
                        return  true;
                }
                return false;
            }
        });

        ///메세지 SEND 핸들러..
        final Handler getMessageHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                et.setText("Message 받음"+msg.getData().get("msg"));
            }
        };

        btn_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ip = findViewById(R.id.ed_ip);
                port = findViewById(R.id.ed_port);

//                Toast t = Toast.makeText(getApplicationContext(),ip.getText().toString() + "" +port.getText().toString(),Toast.LENGTH_SHORT);
//                t.show();
                try {
                    if (ip.getText().toString().length() > 5 && port.getText().toString().length() > 2) {
                        socket2 = new Client(ip.getText().toString(), Integer.parseInt(port.getText().toString()));
                    } else {
                        socket2 = new Client(default_ip, default_Port);
                    }
                    if (!socket2.equals(null)) {
                        setClientCallback(socket2, getMessageHandler);
                        socket2.connect();
                    }
                }catch (Exception e){
                    try {
                        socket2.disconnect();
                    }catch (Exception es){es.printStackTrace();}
                    e.printStackTrace();
                }

            }
        });

    }

    private void setClientCallback(final Client c_socket,final Handler getMessageHandler){
        c_socket.setClientCallback(new Client.ClientCallback () {
            @Override
            public void onMessage(final String message) {
                //Main UI 쓰레드에서 처리 해야하므로 생성 다른 쓰레드에서 View 변경시 오류.
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Message msg = getMessageHandler.obtainMessage();
                        Bundle b = new Bundle();
                        b.putString("msg",message);
                        msg.setData(b);
                        getMessageHandler.sendMessage(msg);
                    }
                }).start();
            }

            @Override
            public void onConnect(Socket socket) {
                c_socket.send("안드로이드에서 접속함");
                // socket.disconnect();
            }

            @Override
            public void onDisconnect(Socket socket, String message) {

            }

            @Override
            public void onConnectError(Socket socket, String message) {

            }
        });
    }
}
