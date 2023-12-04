#include "EchoServer.h"
#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 11021;
const UINT16 MAX_CLIENT = 100;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��

int main()
{
	EchoServer server;
	//IOCompletionPort ioCompletionPort;

	//������ �ʱ�ȭ
	//ioCompletionPort.InitSocket();
	server.InitSocket();

	//���ϰ� ���� �ּҸ� �����ϰ� ��� ��Ų��.
	//ioCompletionPort.BindandListen(SERVER_PORT);
	server.BindandListen(SERVER_PORT);

	//ioCompletionPort.StartServer(MAX_CLIENT);
	server.StartServer(MAX_CLIENT);
	
	printf("�ƹ� Ű�� ���� ������ ����մϴ�\n");
	getchar();

	/*printf("quit�� �Է��Ҷ����� ����մϴ�\n");
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

