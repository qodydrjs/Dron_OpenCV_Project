package com.example.kccistc.android_client;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.ByteArrayInputStream;
import java.net.Socket;

/**
 * @author Waleed Sarwar
 * @since March 30, 2016 12:34 PM
 */
public class PageFragment extends Fragment {
    public static final String ARG_PAGE = "ARG_PAGE";
    private int mPageNo;

    private String default_ip = "192.168.1.61";
    private int default_Port = 5001;
    //Member
    private TextView et;
    private EditText ip, port,et_sendMessage;
    private Button btn_start,btn_reset,btn_send;
    private ImageView imageView;
    private Client socket2;

    private View views;

    public static PageFragment newInstance(int pageNo) {

        Bundle args = new Bundle();
        args.putInt(ARG_PAGE, pageNo);
        PageFragment fragment = new PageFragment();
        fragment.setArguments(args);
        return fragment;
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mPageNo = getArguments().getInt(ARG_PAGE);
    }
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        views = inflater.inflate(R.layout.fragment_page, container, false);

        TextView textView = (TextView) views.findViewById(R.id.text_view);
        textView.setText(getString(R.string.server_ip));

        // viewPager.setAdapter(getSupportFragmentManager());
        et = views.findViewById(R.id.tx_log);
        btn_start = views.findViewById(R.id.btn_start);
        et_sendMessage = views.findViewById(R.id.ed_sendMessage);
        et_sendMessage.setOnKeyListener(new View.OnKeyListener() {
            @Override
            public boolean onKey(View v, int keyCode, KeyEvent event) {
                try {
                    Log.d("SimpleSocket", ""+keyCode);
                    if (keyCode == 66) {
                        EditText et_msg = (EditText) v;
                        String msg = et_msg.getText().toString();
                        if(msg.length() > 0) {
                            socket2.send(msg);
                            et_msg.setText("");
                        }
                    }
                }catch (Exception e){e.printStackTrace();}

                return false;
            }
        });


        ///메세지 SEND 핸들러..
        final Handler getMessageHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                byte[] data = msg.getData().getByteArray("msg");
                ((ImageView) views.findViewById(R.id.imageView)).setImageBitmap(BitmapFactory.decodeByteArray(data, 0, data.length));
                et.append(msg.getData().get("msg") +"\r\n");
            }
        };

        btn_start.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                ip = views.findViewById(R.id.ed_ip);
                port = views.findViewById(R.id.ed_port);

//                Toast t = Toast.makeText(getApplicationContext(),ip.getText().toString() + "" +port.getText().toString(),Toast.LENGTH_SHORT);
//                t.show();
                try {
                    if (ip.getText().toString().length() > 5 && port.getText().toString().length() > 2) {
                        socket2 = new Client(ip.getText().toString(), Integer.parseInt(port.getText().toString()));
                        setClientCallback(socket2, getMessageHandler);
                    } else {
                        socket2 = new Client(default_ip, default_Port);
                        setClientCallback(socket2, getMessageHandler);
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

        return views;
    }

    private void setClientCallback(final Client c_socket,final Handler getMessageHandler){
        c_socket.setClientCallback(new Client.ClientCallback () {
            @Override
            public void onMessage(final byte[] message) {
                //Main UI 쓰레드에서 처리 해야하므로 생성 다른 쓰레드에서 View 변경시 오류.
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Message msg = getMessageHandler.obtainMessage();
                        Bundle b = new Bundle();
                        b.putByteArray("msg",message);
                        //b.putByte("msg",message);
                        msg.setData(b);

                        //imageView = views.findViewById(R.id.imageView);
                        try {
                          //  ByteArrayInputStream ins =null;
                            //ins.read(message);

                       // Bitmap bitmap = BitmapFactory.decodeStream(ins);

                       // imageView.setImageBitmap(bitmap);
                        getMessageHandler.sendMessage(msg);
                        }catch (Exception e){}

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
