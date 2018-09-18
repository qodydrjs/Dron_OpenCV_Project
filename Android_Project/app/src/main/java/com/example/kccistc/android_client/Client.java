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
import java.nio.ByteBuffer;
import java.util.StringTokenizer;

public class Client {
    private static int count = 1;
    private static boolean isImage = false;
    private static boolean isSize = false;
    private static byte[] imagebuffer;
    private static byte[] sizebuffer;
    private static byte[] preimagebuffer;
    private static int size;

    private Socket socket;
    private OutputStream socketOutput;
   // private BufferedReader socketInput;
    private BufferedInputStream socketInputStream;
//    private ByteArrayOutputStream socktInputBuffer;
//    private ByteArrayInputStream socketBytein;

    private int buff_size = 1024;
    private byte buffer[] = new byte[buff_size];
    private boolean mRun = true;

    private StringTokenizer stringTokenizer;

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
                   // socketInput = new BufferedReader(new InputStreamReader(socket.getInputStream(),"UTF-8"));
                    socketInputStream = new BufferedInputStream(socket.getInputStream());
                  //  ByteBuffer b = new
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
                    // synchronized (this) {
                    while ((read = socketInputStream.read(buffer)) > 0 && mRun) {
                        sizebuffer = new byte[read];
                        System.arraycopy(buffer, 0, sizebuffer, 0, read);

                        if (!isSize) {
                            stringTokenizer = new StringTokenizer(new String(sizebuffer), "$^@");
                            while (stringTokenizer.hasMoreTokens()) {
                                if (stringTokenizer.nextToken().equals("#i")) {
                                    isImage = true;
                                    System.out.println("#i :::::::::::::::::::");
                                } else if (!isSize) {
                                    System.out.println("Size :::::::::::::::::::");
                                    String aa = stringTokenizer.nextToken();
                                    System.out.println(aa);
                                    byte buf_image_size[] = aa.getBytes();
                                    int s1 = buf_image_size[0] & 0xFF;
                                    int s2 = buf_image_size[1] & 0xFF;
                                    int s3 = buf_image_size[2] & 0xFF;
                                    int s4 = buf_image_size[3] & 0xFF;
                                    size = (s1 << 24) + (s2 << 16) + (s3 << 8) + (s4 << 0);
                                    System.out.println(size);
                                    imagebuffer = new byte[0];
                                    isSize = true;
                                } else if (isSize) {
                                    try {
                                        String aa = stringTokenizer.nextToken();
                                        System.out.println(aa);
                                        imagebuffer = aa.getBytes();
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                        imagebuffer = new byte[0];
                                    }
                                }

                            }
                        } else {


//                            //Server 에서 #i\0 을 보내면 Image 받기 시작
//                            if (new String(sizebuffer).equals("#i\0")) {
////                            isImage = true;
//                            }
//                            //#i\0을 받았으면 이어서 Image Size 값을 서버로부터 받는다.
//                            else if (!isSize && isImage && imagebuffer == null) {
//                            }
                            //Image Size를 알아냈으니 Image를 Byte 배열에 저장한다
                             if (isSize && (imagebuffer != null)) {
                                System.out.println(new String(sizebuffer));
                                if (imagebuffer.length == 0) {
                                    imagebuffer = new byte[sizebuffer.length];
                                    System.arraycopy(sizebuffer, 0, imagebuffer, 0, sizebuffer.length);
                                } else {
                                    preimagebuffer = imagebuffer.clone();
                                    imagebuffer = new byte[read + preimagebuffer.length];
                                    System.arraycopy(preimagebuffer, 0, imagebuffer, 0, preimagebuffer.length);
                                    System.arraycopy(sizebuffer, 0, imagebuffer, preimagebuffer.length, sizebuffer.length);
                                }

                                //Image를 Size만큼 받으면 리스너를 통해 ImageView에 Image byte[] 값을 넘겨줄수 있다.
                                if (imagebuffer != null)
                                    if (imagebuffer.length >= size) {
                                        listener.onMessage(imagebuffer);
                                        isImage = false;
                                        isSize = false;
                                        imagebuffer = null;
                                        sizebuffer = null;
                                        preimagebuffer = null;
                                    }

                                buffer = new byte[buff_size];

                            }
                        }
                    }
                }
            } catch (Exception e) {
                if(listener!=null)
                    listener.onDisconnect(socket, e.getMessage());
                e.printStackTrace();
                Log.d("error : ",  "::erroe "+e.getMessage());
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

    private void BinarySerch(){

    }
}