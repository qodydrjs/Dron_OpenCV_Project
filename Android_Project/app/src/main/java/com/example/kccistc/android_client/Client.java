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
            sizebuffer = new byte[0];
            byte stayBuffer[] = new byte[0];
            byte tempBuffer[] = new byte[0];
            buffer = new byte[buff_size];
            byte buffer_u[] = new byte[0];
            imagebuffer = null;
            boolean check = false;
            int check_num =-1;
            int read;

            byte temp[];

            try {
                while(mRun) {
                    // synchronized (this) {
                    while ((read = socketInputStream.read(buffer)) > 0 && mRun) {

                        if(stayBuffer.length >=1){
                            temp = new byte[stayBuffer.length+read];
                            System.arraycopy(stayBuffer,0,temp,0,stayBuffer.length);
                            System.arraycopy(buffer,0,temp,stayBuffer.length,read);
                            buffer_u  =  temp.clone();
                        }else{
                            buffer_u  = new byte[read];
                            System.arraycopy(buffer,0,buffer_u,0,read);
                        }

                        for(int i=0; i<buffer_u.length; i++){

                            ////////////////
                            ////////////////
                            if(buffer_u[i]==36)//$=36
                                if(  buffer_u.length > i+3 )
                                    if (buffer_u[i + 1] == 100) //d=100
                                        if (buffer_u[i + 2] == 100) //d =100
                                          if(buffer_u[ i + 3 ]==33) {//!=33
                                              check = true;
                                              check_num = (i + 3) + 1;
                                              tempBuffer = new byte[check_num];  //처리할 데이터 버퍼
                                              System.arraycopy(buffer_u, 0, tempBuffer, 0, check_num);
                                              if (buffer_u.length > check_num) {
                                                  stayBuffer = new byte[buffer_u.length - check_num];
                                                  System.arraycopy(buffer_u, check_num, stayBuffer, 0, buffer_u.length - check_num);
                                              } else {
                                                  stayBuffer = new byte[0];
                                              }
                                              break;
                                          }
                            ////////////////////////
                            ///////////////////////
                            if(i == buffer_u.length-1) {
                                stayBuffer = buffer_u.clone();
                            }
                        }

                        if(check == true){ //데이터가 END 이면 할 처리
                            //sizebuffer = tempBuffer.clone();
                            if (!isSize) {
                                stringTokenizer = new StringTokenizer(new String(tempBuffer), "$dd!");
                                while (stringTokenizer.hasMoreTokens()) {
                                    String token =  stringTokenizer.nextToken();
                                    if (token.equals("#i")) {
                                        isImage = true;
                                        //System.out.println("#i :::::::::::::::::::");
                                        check = false;
                                    } else if (!isSize) {
                                        //System.out.println("Size :::::::::::::::::::");
                                        String str_token = token;
                                        size = Integer.parseInt(str_token);
                                        //System.out.println(size);
                                        imagebuffer = new byte[0];
                                        isSize = true;
                                        check = false;
                                    }
                                }
                            } else {
                                    if (tempBuffer != null)
                                        if (tempBuffer.length >= size) {
                                            listener.onMessage(tempBuffer);
                                            isImage = false;
                                            isSize = false;
                                            imagebuffer = null;
                                            sizebuffer = null;
                                            preimagebuffer = null;
                                            check = false;
                                        }
                                    buffer = new byte[buff_size];
                                }
                          //  }

                        }else{
//////////////////////////////////////
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