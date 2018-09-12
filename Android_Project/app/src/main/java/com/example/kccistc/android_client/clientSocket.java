package com.example.kccistc.android_client;

import android.os.Message;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import android.os.Handler;

public class clientSocket extends Thread {
    private Socket client_scok;

    private BufferedReader buffRecv;
    private BufferedWriter buffSend;

    private String mAddr = "192.168.1.61";
    private int mPort = 5000;
    private boolean mConnected = false;
    private Handler mHandler = null;



    public static class MessageTypeClass {
        public static final int SIMSOCK_CONNECTED = 1;
        public static final int SIMSOCK_DATA = 2;
        public static final int SIMSOCK_DISCONNECTED = 3;
    };

    public enum MessageType { SIMSOCK_CONNECTED, SIMSOCK_DATA, SIMSOCK_DISCONNECTED };


    public clientSocket(String addr, int port, Handler handler){
        this.mAddr = addr;
        this.mPort = port;
        this.mHandler = handler;
    }

    private void makeMessage(MessageType what, Object obj)
    {
        Message msg = Message.obtain();
        msg.what = what.ordinal();
        msg.obj  = obj;
        mHandler.sendMessage(msg);
    }

    private boolean connect (String addr, int port)
    {
        try {
            InetSocketAddress socketAddress  = new InetSocketAddress (InetAddress.getByName(addr), port);
            client_scok = new Socket();
            client_scok.connect(socketAddress, 5000);
        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
            return false;
        }
        return true;
    }


    @Override
    public void run() {
        if(! connect(mAddr, mPort)) return; // connect failed
        if(client_scok == null)         return;

        try {
            buffRecv = new BufferedReader(new InputStreamReader(client_scok.getInputStream()));
            buffSend = new BufferedWriter(new OutputStreamWriter(client_scok.getOutputStream()));
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        mConnected = true;

        makeMessage(MessageType.SIMSOCK_CONNECTED, "");
        Log.d("SimpleSocket", "socket_thread loop started");

        String aLine = null;

        while( ! Thread.interrupted() ){ try {
            aLine = buffRecv.readLine();
            if(aLine != null) makeMessage(MessageType.SIMSOCK_DATA, aLine);
            else break;
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }}

        makeMessage(MessageType.SIMSOCK_DISCONNECTED, "");
        Log.d("SimpleSocket", "socket_thread loop terminated");

        try {
            buffRecv.close();
            buffSend.close();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        mConnected = false;
    }

    synchronized public boolean isConnected(){
        return mConnected;
    }

    public void sendString(String str){
        PrintWriter out = new PrintWriter(buffSend, true);
        out.println(str);
    }
}
