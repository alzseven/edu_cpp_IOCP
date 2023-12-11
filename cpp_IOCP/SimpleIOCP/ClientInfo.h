#pragma once

#include "Define.h"
#include <stdio.h>
#include <mutex>
#include <queue>

class ClientInfo
{
public:
	ClientInfo()
	{
		ZeroMemory(&mRecvOverlappedEx, sizeof(stOverlappedEx));
		//ZeroMemory(&mSendOverlappedEx, sizeof(stOverlappedEx));
		mSocket = INVALID_SOCKET;
	}

	~ClientInfo() = default;

	void Init(const UINT32 index) 
	{
		mIndex = index;
	}

	UINT32 GetIndex() { return mIndex; }

	bool IsConnected() { return mSocket != INVALID_SOCKET; }

	SOCKET GetSock() { return mSocket; }

	char* RecvBuffer() { return mRecvBuf; }

	bool OnConnect(HANDLE iocpHandle_, SOCKET socket_)
	{
		mSocket = socket_;

		Clear();

		//I/O Completion Port��ü�� ������ �����Ų��.
		if (BindIOCompletionPort(iocpHandle_) == false)
		{
			return false;
		}

		return BindRecv();
	}

	void Close(bool bIsForce = false)
	{
		struct linger stLinger = { 0, 0 };	// SO_DONTLINGER�� ����

		// bIsForce�� true�̸� SO_LINGER, timeout = 0���� �����Ͽ� ���� ���� ��Ų��. 
		// ���� : ������ �ս��� ������ ���� 
		if (true == bIsForce)
		{
			stLinger.l_onoff = 1;
		}

		//socketClose������ ������ �ۼ����� ��� �ߴ� ��Ų��.
		shutdown(mSocket, SD_BOTH);

		//���� �ɼ��� �����Ѵ�.
		setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&stLinger, sizeof(stLinger));

		//���� ������ ���� ��Ų��. 
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}

	void Clear()
	{
		//mSendPos = 0;
		//mIsSending = false;
	}

	bool BindIOCompletionPort(HANDLE iocpHandle_)
	{
		auto hIOCP = CreateIoCompletionPort((HANDLE)GetSock()
			, iocpHandle_
			, (ULONG_PTR)(this), 0);

		if (hIOCP == INVALID_HANDLE_VALUE)
		{
			printf("[����] CreateIoCompletionPort()�Լ� ����: %d\n", GetLastError());
			return false;
		}

		return true;
	}

	bool BindRecv()
	{
		DWORD dwFlag = 0;
		DWORD dwRecvNumBytes = 0;

		//Overlapped I/O�� ���� �� ������ ������ �ش�.
		mRecvOverlappedEx.m_wsaBuf.len = MAX_SOCKBUF;
		mRecvOverlappedEx.m_wsaBuf.buf = mRecvBuf;
		mRecvOverlappedEx.m_eOperation = IOOperation::RECV;

		int nRet = WSARecv(mSocket,
			&(mRecvOverlappedEx.m_wsaBuf),
			1,
			&dwRecvNumBytes,
			&dwFlag,
			(LPWSAOVERLAPPED) & (mRecvOverlappedEx),
			NULL);

		//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[����] WSARecv()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		return true;
	}

	// 1���� �����忡���� ȣ���ؾ� �Ѵ�!
	bool SendMsg(const UINT32 dataSize_, char* pMsg_)
	{
		auto sendOverlappedEx = new stOverlappedEx;
		ZeroMemory(sendOverlappedEx, sizeof(stOverlappedEx));
		sendOverlappedEx->m_wsaBuf.len = dataSize_;
		sendOverlappedEx->m_wsaBuf.buf = new char[dataSize_];
		CopyMemory(sendOverlappedEx->m_wsaBuf.buf, pMsg_, dataSize_);
		sendOverlappedEx->m_eOperation = IOOperation::SEND;

		std::lock_guard<std::mutex> guard(mSendLock);

		mSendDataqueue.push(sendOverlappedEx);

		if (mSendDataqueue.size() == 1)
		{
			SendIO();
		}

		return true;

		//if ((mSendPos + dataSize_) > MAX_SOCK_SENDBUF)
		//{
		//	mSendPos = 0;
		//}

		//auto pSendBuf = &mSendBuf[mSendPos];

		////���۵� �޼����� ����
		//CopyMemory(pSendBuf, pMsg_, dataSize_);
		//mSendPos += dataSize_;

		//return true;
	}
	
	bool SendIO()
	{
		auto sendOverlappedEx = mSendDataqueue.front();

		DWORD dwRecvNumBytes = 0;
		int nRet = WSASend(mSocket,
			&(sendOverlappedEx->m_wsaBuf),
			1,
			&dwRecvNumBytes,
			0,
			(LPWSAOVERLAPPED)sendOverlappedEx,
			NULL);

		//if (mSendPos <= 0 || mIsSending)
		//{
		//	return true;
		//}

		//std::lock_guard<std::mutex> guard(mSendLock);

		//mIsSending = true;

		//CopyMemory(mSendingBuf, &mSendBuf[0], mSendPos);

		////Overlapped I/O�� ���� �� ������ ������ �ش�.
		//mSendOverlappedEx.m_wsaBuf.len = mSendPos;
		//mSendOverlappedEx.m_wsaBuf.buf = &mSendingBuf[0];
		//mSendOverlappedEx.m_eOperation = IOOperation::SEND;

		//DWORD dwRecvNumBytes = 0;
		//int nRet = WSASend(mSocket,
		//	&(mSendOverlappedEx.m_wsaBuf),
		//	1,
		//	&dwRecvNumBytes,
		//	0,
		//	(LPWSAOVERLAPPED) & (mSendOverlappedEx),
		//	NULL);

		//socket_error�̸� client socket�� �������ɷ� ó���Ѵ�.
		if (nRet == SOCKET_ERROR && (WSAGetLastError() != ERROR_IO_PENDING))
		{
			printf("[����] WSASend()�Լ� ���� : %d\n", WSAGetLastError());
			return false;
		}

		//mSendPos = 0;
		return true;
	}

	void SendCompleted(const UINT32 dataSize_)
	{
		//mIsSending = false;
		printf("[�۽� �Ϸ�] bytes : %d\n", dataSize_);

		std::lock_guard<std::mutex> guard(mSendLock);

		delete[] mSendDataqueue.front()->m_wsaBuf.buf;
		delete mSendDataqueue.front();

		mSendDataqueue.pop();

		if (mSendDataqueue.empty() == false)
		{
			SendIO();
		}
	}

private:
	INT32 mIndex = 0;
	SOCKET			mSocket;			//Cliet�� ����Ǵ� ����
	stOverlappedEx	mRecvOverlappedEx;	//RECV Overlapped I/O�۾��� ���� ����
	//stOverlappedEx	mSendOverlappedEx;	//SEND Overlapped I/O�۾��� ���� ����

	char			mRecvBuf[MAX_SOCKBUF];	//������ ����

	std::mutex mSendLock;
	std::queue<stOverlappedEx*> mSendDataqueue;
	//bool mIsSending = false;
	//UINT64 mSendPos = 0;
	//char			mSendBuf[MAX_SOCK_SENDBUF]; //������ ����	
	//char			mSendingBuf[MAX_SOCK_SENDBUF]; //������ ����
};