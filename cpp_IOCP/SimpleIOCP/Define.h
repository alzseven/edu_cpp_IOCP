#pragma once
#include <winsock2.h>
#include <Ws2tcpip.h>

#define MAX_SOCKBUF 1024	//��Ŷ ũ��
#define MAX_WORKERTHREAD 4  //������ Ǯ�� ���� ������ ��



enum class IOOperation
{
	RECV,
	SEND
};

//WSAOVERLAPPED����ü�� Ȯ�� ���Ѽ� �ʿ��� ������ �� �־���.
struct stOverlappedEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O����ü
	SOCKET		m_socketClient;			//Ŭ���̾�Ʈ ����
	WSABUF		m_wsaBuf;				//Overlapped I/O�۾� ����
	IOOperation m_eOperation;			//�۾� ���� ����
};

//Ŭ���̾�Ʈ ������ ������� ����ü
struct stClientInfo
{
	SOCKET			m_socketClient;			//Cliet�� ����Ǵ� ����
	stOverlappedEx	m_stRecvOverlappedEx;	//RECV Overlapped I/O�۾��� ���� ����
	stOverlappedEx	m_stSendOverlappedEx;	//SEND Overlapped I/O�۾��� ���� ����
	char			m_szBuf[MAX_SOCKBUF];	//������ ����

	stClientInfo()
	{
		ZeroMemory(&m_stRecvOverlappedEx, sizeof(stOverlappedEx));
		ZeroMemory(&m_stSendOverlappedEx, sizeof(stOverlappedEx));
		m_socketClient = INVALID_SOCKET;
	}
};
