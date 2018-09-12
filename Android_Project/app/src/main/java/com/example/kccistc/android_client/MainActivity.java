package com.example.kccistc.android_client;

import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketAddress;

public class MainActivity extends AppCompatActivity {

    private String default_ip = "192.168.1.61";
    private int default_Port = 5000;
    //Member
    private TextView et;
    private EditText ip, port;
    private Button btn_start,btn_reset,btn_send;
    //Socket
    private Socket socket;
    private Handler mHandler;
    private BufferedReader networkReader;
    private BufferedWriter networkWriter;
    private String html = "";

    @Override
    protected void onStop() {
        super.onStop();
        try {
            socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mHandler = new Handler();

        et = findViewById(R.id.tx_log);
        et.setText("ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ");
        btn_start = findViewById(R.id.btn_start);
        btn_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ip = findViewById(R.id.ed_ip);
                port = findViewById(R.id.ed_port);
//                if( !ip.toString().equals(null) &&
//                        !port.toString().equals(null) ){
//                    try {
//                        setSocket(ip.toString(), Integer.parseInt(port.toString()),et);
//                    }catch (Exception e){e.printStackTrace();}
//                }else{
//                    try {
//                        setSocket(default_ip, default_Port,et);
//                    }catch (Exception e){e.printStackTrace();}
//                }
                try {
                    setSocket(default_ip, default_Port,et);
                }catch (Exception e){e.printStackTrace();}

                checkUpdate.start();
            }
        });

        btn_reset = findViewById(R.id.btn_clear);
        btn_reset.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try {
                    socket.close();
                }catch (Exception e){
                    et.setText(e.getMessage());
                }
            }
        });

        btn_send = findViewById(R.id.btn_send);
        btn_send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                PrintWriter out = new PrintWriter(networkWriter, true);
                String return_msg = et.getText().toString();
                out.println(return_msg);
            }
        });



    }

    private Thread checkUpdate = new Thread() {

        public void run() {
            try {
                String line;
                Log.w("ChattingStart", "Start Thread");
                while (true) {
                    Log.w("Chatting is running", "chatting is running");
                    line = networkReader.readLine();
                    html = line;
                    mHandler.post(showUpdate);
                }
            } catch (Exception e) {

            }
        }
    };


    private Runnable showUpdate = new Runnable() {

        public void run() {
            Toast.makeText(MainActivity.this, "Coming word: " + html, Toast.LENGTH_SHORT).show();
        }

    };

    public void setSocket(String ip, int port, TextView v) throws IOException {
        try {
            socket = new Socket(ip,port);
            networkWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
            networkReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        } catch (IOException e) {
            v.setText(e.getMessage());
            System.out.println(e);
            e.printStackTrace();
        }
    }


}
