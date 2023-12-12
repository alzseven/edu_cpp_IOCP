#include "EchoServer.h"
#include <string>
#include <iostream>

const UINT16 SERVER_PORT = 11021;
const UINT16 MAX_CLIENT = 3;		//�� �����Ҽ� �ִ� Ŭ���̾�Ʈ ��
const UINT32 MAX_IO_WORKER_THREAD = 4;  //������ Ǯ�� ���� ������ ��

int main()
{
	EchoServer server;

	//������ �ʱ�ȭ
	server.InitSocket(MAX_IO_WORKER_THREAD);

	//���ϰ� ���� �ּҸ� �����ϰ� ��� ��Ų��.
	server.BindandListen(SERVER_PORT);

	//server.StartServer(MAX_CLIENT);
	server.Run(MAX_CLIENT);

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

	//server.DestroyThread();
	server.End();
	return 0;
}

