package com.example.kccistc.android_client;

import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

public class Client {
    private static int count = 1;
    private static boolean isImage = false;
    private static byte[] imagebuffer;
    private static byte[] sizebuffer;
    private static byte[] preimagebuffer;

    private Socket socket;
    private OutputStream socketOutput;
    private BufferedReader socketInput;
    private BufferedInputStream socketInputStream;
//    private ByteArrayOutputStream socktInputBuffer;
//    private ByteArrayInputStream socketBytein;

    private int buff_size = 1024;
    private byte buffer[] = new byte[buff_size];
    private boolean mRun = true;


    private String ip;
    private int port;
    private ClientCallback listener=null;

    public Client(String ip, int port){
        this.ip=ip;
        this.port=port;
    }

    public void connect(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                socket = new Socket();
                InetSocketAddress socketAddress = new InetSocketAddress(ip, port);
                try {
                    socket.connect(socketAddress);
                    socketOutput = socket.getOutputStream();
                    //socketInput = new BufferedReader(new InputStreamReader(socket.getInputStream(),"UTF-8"));
                    socketInputStream = new BufferedInputStream(socket.getInputStream());
                    new ReceiveThread().start();

                    if(listener!=null)
                        listener.onConnect(socket);
                } catch (IOException e) {
                    if(listener!=null)
                        listener.onConnectError(socket, e.getMessage());
                }
            }
        }).start();
    }


    public void disconnect()throws Exception{
        try {
            socket.close();
        } catch (IOException e) {
            if(listener!=null)
                listener.onDisconnect(socket, e.getMessage());
        }
    }

    public void send(String message){
        try {
            socketOutput.write(message.getBytes("EUC-KR"));
        } catch (IOException e) {
            if(listener!=null)
                listener.onDisconnect(socket, e.getMessage());
        }
    }

    private class ReceiveThread extends Thread implements Runnable{
        public void run(){
            String message;
            buffer = new byte[buff_size];
            imagebuffer = null;

            int read;
            try {
                while(mRun) {
                    int size =0;
                    synchronized (this) {
                        while ((read = socketInputStream.read(buffer)) != -1 && mRun) {
                            sizebuffer = new byte[read];
                            System.arraycopy(buffer, 0, sizebuffer, 0, read);
                          // Log.d("byte : ", "" + read + " " + new String(sizebuffer) + " c : "+count++);
                            if(new String(sizebuffer).equals("#i\0")){
                               // Log.d("byte : ",  "::::: " + new String(sizebuffer));
                               // Log.d("byte : ",  "::::: " + (imagebuffer==null) );
                                isImage = true;
                            }else if(isImage && imagebuffer == null){
                                size = 41070;
                                imagebuffer = new byte[read];
//                                Log.d("size : ",  "::size " + sizebuffer[0]);
//                                Log.d("size : ",  "::size " + sizebuffer[1]);
//                                Log.d("size : ",  "::size " + sizebuffer[2]);
//                                Log.d("size : ",  "::size " + sizebuffer[3]);
                            }else if(isImage && !(imagebuffer==null)){
                                preimagebuffer = sizebuffer.clone();
                                imagebuffer = new byte[read + preimagebuffer.length];
                                System.arraycopy(preimagebuffer,0,imagebuffer,0,preimagebuffer.length);
                                //Log.d("size11 : ",  "::size " + size);
                            }


                            if(imagebuffer.length >=size){
                                Log.d("image : ",  "::image " + new String(imagebuffer) +"len : "+ imagebuffer.length);
                                Log.d("image : ",  "len : "+ imagebuffer.length);
                                listener.onMessage(imagebuffer);
                            }

//                            imagebuffer=null;
//                            sizebuffer=null;
//                            preimagebuffer=null;
                            buffer = new byte[buff_size];
                            //

                        }
                    }
                }
            } catch (Exception e) {
                if(listener!=null)
                    listener.onDisconnect(socket, e.getMessage());
            }
        }
    }

    private int getInt(byte[] data) {
        int s1 = data[0] & 0xFF;
        int s2 = data[1] & 0xFF;
        int s3 = data[2] & 0xFF;
        int s4 = data[3] & 0xFF;

        return ((s1 << 24) + (s2 << 16) + (s3 << 8) + (s4 << 0));
    }

    public void setClientCallback(ClientCallback listener){
        this.listener=listener;
    }

    public void removeClientCallback(){
       // this.listener=null;
    }

    public interface ClientCallback {
        void onMessage(byte[] message);
        void onConnect(Socket socket);
        void onDisconnect(Socket socket, String message);
        void onConnectError(Socket socket, String message);
    }
}