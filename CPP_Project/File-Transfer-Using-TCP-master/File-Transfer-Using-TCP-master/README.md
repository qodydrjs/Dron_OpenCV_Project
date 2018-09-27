# File-Transfer-Using-TCP

Making File Transfer Program based on object oriented programming using c++. This project implemented in VisualStudion 2015

FileSender는 TCP/IP 기반의 파일 전송 프로그램으로 가장 빠른 파일 전송을 목적으로 합니다. 이를 위하여 소켓 옵션을 전송에 최적화 시키고 Windows의 메모리 매핑 기법을 사용하여 속도를 향상시킵니다. 

## 개발 환경

#### Server
* OS : Windows 10 Pro 64bit
* CPU : Intel(R) Core(TM) i5-4590 CPU
* RAM : 8.00GB
* IDE : Microsoft Visual Studio Community 2015

#### Client
* OS : Windows 10 Pro 64bit
* CPU : Intel(R) Core(TM) i5-5200U CPU @ 2.20GHz 2.20 GHz
* RAM : 4.00GB
* IDE : Microsoft Visual Studio Community 2015

## 사용 기술

C++, Window API, Threading, WinSock2.0

## 부가 설명

 먼저 해당 프로그램은 서버에서 지정한 파일을 해당 서버에 접속한 클라이언트로 전송함을 원칙으로 한다.

 서버를 파일을 인자값으로 하여 실행하면 서버의 메인 소켓이 생성되고 바인드 과정을 거쳐 클라이언트 접속을 수락할 대기 상태가 된다. 이 때 최대 클라이언트의 접속은 3명으로 제한하고, 특정 클라이언트가 접속되면(connect) 해당 클라이언트에 대한 정보를 바탕으로 객체를 생성하고 이 객체를 활용하여 파일을 전송한다. 이 때 파일 전송을 위해 컴퓨터의 메모리 상에 파일을 매핑시키고 이로 하여금 메모리 상에서 파일을 컨트롤함에 따라 보조기억장치보다 상대적으로 높은 속도의 이점을 가질 수 있다.

 클라이언트는 대기상태의 서버에 서버의 IP주소와 Port 번호를 통해 접속하는데 해당 프로그램에서 서버의 포트 번호는 9090 으로 고정되어 있다. 클라이언트 역시 마찬가지로 데이터 수신에 필요한 receiver 클래스의 인스턴스를 생성하고 해당 인스턴스로 데이터 수신을 시작한다. 먼저 3개의 소켓을 생성하여 주소 정보를 할당하고, 각각 파일의 이름, 파일의 크기, 그리고 실 데이터를 수신하도록 한다. 각각의 소켓으로 대기중인 서버에 접속하면 서버는 해당 내용을 클라이언트로 수신하고 클라이언트의 File 클래스에 따라 받은 정보(파일의 이름, 크기)를 토대로 메모리에 해당 크기만큼 가상으로 매핑을 한다. 그리고 데이터 수신이 이루어지면 마찬가지로 메모리에 파일을 writing함으로서 파일 수신의 속도를 높인다. 

## 실행 방법

* 먼저 FileSender_server와 FileSender_client를 빌드한다.
* 이 후 FileSender_server.exe 파일의 위치에서 cmd를 실행하고
  FileSender_server.exe [보낼 파일 명] 을 입력한다.
* 다음으로 FileSender_client 폴더에서 마찬가지의 방법으로 cmd를 실행하고
  인자값으로 [서버의 ip] [서버의 PORT]를 입력한다.
  (서버의 포트 번호는 기본값으로 9090을 사용한다.)
* 보낼 파일은 FileSender_server.exe와 동일한 경로에 위치하도록 설정한다.

## 실행 결과

[![Demo Doccou alpha](https://j.gifs.com/3lnG5n.gif)](https://www.youtube.com/watch?v=j0UqMonOjWI)

(클릭 시 유튜브로 이동합니다)
