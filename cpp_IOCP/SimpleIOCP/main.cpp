#include "EchoServer.h"
#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 11021;
const UINT16 MAX_CLIENT = 100;		//총 접속할수 있는 클라이언트 수

int main()
{
	EchoServer server;
	//IOCompletionPort ioCompletionPort;

	//소켓을 초기화
	//ioCompletionPort.InitSocket();
	server.InitSocket();

	//소켓과 서버 주소를 연결하고 등록 시킨다.
	//ioCompletionPort.BindandListen(SERVER_PORT);
	server.BindandListen(SERVER_PORT);

	//ioCompletionPort.StartServer(MAX_CLIENT);
	server.StartServer(MAX_CLIENT);
	
	printf("아무 키나 누를 때까지 대기합니다\n");
	getchar();

	/*printf("quit을 입력할때까지 대기합니다\n");
	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}*/

	//ioCompletionPort.DestroyThread();

	server.DestroyThread();
	return 0;
}

