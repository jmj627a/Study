#include "Network.h"

//��ü ����� sessionList
unordered_map<DWORD, SESSION*> sessionList;

//���� ������ ��� <accountNo , NickName >
unordered_map<UINT64, WCHAR*> clientInfoMap;

// �̹� ������� ģ����� <From, ģ�� ����ü>
unordered_multimap<UINT64, FRIEND *> friendMap;

// ģ����û <From, ģ�� ��û ����ü>
unordered_multimap<UINT64, FRIEND *> requestFriendMap;

// From ģ����û �ѻ��(Key), To ģ����û �������(Data)
unordered_multimap<UINT64, UINT64> requestFrom;

// To ģ����û ������� (Key), From ģ����û �ѻ��(Data)
unordered_multimap<UINT64, UINT64> requestTo;


SOCKET g_listenSocket;

UINT64 g_userNoCount = 1;
//UINT64 g_totalFriendNum = 0;

UINT64 g_recvCount = 0;
UINT64 g_sendCount = 0;

int CNetwork::networkInit()
{
	WSADATA wsaData;
	SOCKADDR_IN servAddr;

	int ret;
	// ���� ���̺귯�� �ʱ�ȭ
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		wprintf(L"WSAStartup failed with error: %ld\n", ret);
		return -1;
	}

	// ���� ����
	g_listenSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (g_listenSocket == INVALID_SOCKET)
		return -1;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(dfNETWORK_PORT);

	// IP �ּ� PORT��ȣ �Ҵ�
	ret = bind(g_listenSocket, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (ret == SOCKET_ERROR)
		return -1;

	// ������ ������ ���� �������� �ϼ�
	ret = listen(g_listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;


	u_long on = 1;
	ioctlsocket(g_listenSocket, FIONBIO, &on);

	return 1;
}

void CNetwork::networkAccept(SOCKET* listenSocket)
{
	SOCKADDR_IN addr;
	int addrLen = sizeof(addr);

	SOCKET clientSocket = accept(*listenSocket, (SOCKADDR*)&addr, &addrLen);

	//char Ip[20];
	//inet_ntop(AF_INET, &(addr->sin_addr), Ip, INET_ADDRSTRLEN);

	SESSION* newSession = new SESSION;
	newSession->socket = clientSocket;
	newSession->userNo = -1;
	//memcpy(&newSession->ip, &addr.sin_addr, sizeof(addr.sin_addr));
	//newSession->port = addr.sin_port;

	//list�� ����
	sessionList.insert({ newSession->socket,newSession });

//	wprintf(L"[%d] accpet / total : %d \n", newSession->socket, sessionList.size());

}

void CNetwork::networkFunc()
{
	FD_SET ReadSet;
	FD_SET WriteSet;

	DWORD UserTable_No[FD_SETSIZE];
	SOCKET UserTable_SOCKET[FD_SETSIZE];

	//������ ��ü�� ��ȸ�� iter
	std::unordered_multimap<DWORD, SESSION*>::iterator iter = sessionList.begin();

	FD_ZERO(&ReadSet);
	FD_ZERO(&WriteSet);
	memset(UserTable_No, -1, sizeof(DWORD) * FD_SETSIZE);
	memset(UserTable_SOCKET, INVALID_SOCKET, sizeof(SOCKET) *FD_SETSIZE);
	
	int count = 0;
	
	FD_SET(g_listenSocket, &ReadSet);
	UserTable_No[count] = 0;
	UserTable_SOCKET[count] = g_listenSocket;
	count++;

	for (iter; iter != sessionList.end();)
	{
	//	startIter = iter;
		SESSION* session = iter->second;
		++iter;

		UserTable_No[count] = session->userNo;
		UserTable_SOCKET[count] = session->socket;

		FD_SET(session->socket, &ReadSet);

		if (session->sendPacket.GetDataSize() > 0)
			FD_SET(session->socket, &WriteSet);

		count++;
		

		if(count >= FD_SETSIZE-1)
		{
			SocketSelect(UserTable_No, UserTable_SOCKET, &ReadSet, &WriteSet);

			count = 1;
			FD_ZERO(&ReadSet);
			FD_ZERO(&WriteSet);
			FD_SET(g_listenSocket, &ReadSet);
		}
		
	}
	
	if (count > 0)
	{
		SocketSelect(UserTable_No, UserTable_SOCKET, &ReadSet, &WriteSet);
	}
}

void CNetwork::SocketSelect(DWORD* pTableNo, SOCKET* pTableSocket, FD_SET * readSet, FD_SET * writeSet)
{
	timeval time;
	time.tv_usec = 0;
	time.tv_sec = 0;

	int ret = select(0, readSet, writeSet, 0, &time); //������ timeout�� 0���� �־���Ѵ�. ���� ���⼭ ���� �� �ڿ��ִ� select�� ��� ��ٸ��Ե�. ��� ������ ��û ���Եȴ�.

	if (ret == SOCKET_ERROR)
		return;

	//���� ����Ʈ ���鼭 FD_ISSET���� �ϰ� ������ �ֳİ� �����, �³� ������� recv �õ�.
	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		if (pTableSocket[i] == INVALID_SOCKET)
			continue;

		SESSION* session = nullptr;

		if (pTableSocket[i] == g_listenSocket)
		{
			if (FD_ISSET(pTableSocket[i], readSet))
				networkAccept(&pTableSocket[i]);
		}
		else
		{
			auto findIter = sessionList.find(pTableSocket[i]);
			if (findIter != sessionList.end()) 
				session = findIter->second;
		}
		
		if (session == nullptr)
			continue;
		//if (session->recvFlag < 5)
		{
			//recv üũ
			if (FD_ISSET(pTableSocket[i], readSet))
			{
				recvCheck(session);
//				session->recvFlag = true;
				g_recvCount++;

				if (session->recvPacket.GetDataSize() > 0)
					int a = 0;
			}
		}

		//send üũ
//		if (session->sendFlag < 5)
		{
			if (FD_ISSET(pTableSocket[i], writeSet))
			{
				if (session == nullptr)
					continue;

				sendCheck(session);
//				session->sendFlag = true;
				g_sendCount++;
			}
		}

	}
}

bool CNetwork::recvCheck(SESSION *session)
{
	//Ŭ�� ����ִ� ť Ȯ��
	CPacket* packet = &session->recvPacket;

	//recv 
	int recvSize = recv(session->socket, packet->GetWritePtr(), packet->GetBufferSize() - packet->GetDataSize(), 0);
	packet->MoveWritePos(recvSize);
	wprintf(L"[%d] rcvd : %d byte \n", session->socket, recvSize);

	if (recvSize <=0)
	{
		//disconnect(session);
		if (packet->GetDataSize() > 0)
			int a = 0;
		return false;
	}

	if (recvSize > 0)
	{
		//���ۿ��� ������ ������ ���� �� ����������� ó��.
		while (true)
		{
			if (packet->GetDataSize() < sizeof(st_PACKET_HEADER))
			{
				if (packet->GetDataSize() > 0)
					int a = 0;
				return true;
			}

			st_PACKET_HEADER Header;
			if (packet->PickData((char*)&Header, sizeof(st_PACKET_HEADER)) < sizeof(st_PACKET_HEADER))
			{
				return true;
			}

			if (packet->GetDataSize() < (Header.wPayloadSize + 5))
			{
				return true;
			}

			BYTE byCode;
			WORD wMsgType;
			WORD wPayloadSize;

			*packet >> byCode  >> wMsgType >> wPayloadSize;

			if (byCode != dfPACKET_CODE)
			{
				if (packet->GetDataSize() > 0)
					int a = 0;
				return true;
			}
			switch (wMsgType)
			{
				//ȸ������ ��û
			case df_REQ_ACCOUNT_ADD:
				recv_AccountAdd_Require(session, wPayloadSize);
				break;

				//ȸ�� �α���
			case df_REQ_LOGIN:
				recv_Login_Require(session, wPayloadSize);
				break;

				//ȸ�� ����Ʈ ��û
			case df_REQ_ACCOUNT_LIST:
				recv_AccountList_Require(session, wPayloadSize);
				break;

				//ģ�� ��� ��û
			case df_REQ_FRIEND_LIST:
				recv_FriendList_Require(session, wPayloadSize);
				break;

				//ģ����û ���� ��� ��û
			case df_REQ_FRIEND_REQUEST_LIST:
				recv_FriendRequestList_Require(session, wPayloadSize);
				break;

				//ģ�� ��û ������ ��� ��û
			case df_REQ_FRIEND_REPLY_LIST:
				recv_FriendReplyList_Require(session, wPayloadSize);
				break;

				//ģ�� ���� ���� 
			case df_REQ_FRIEND_REMOVE:
				recv_FriendRemove_Require(session, wPayloadSize);
				break;

				//ģ�� ��û
			case df_REQ_FRIEND_REQUEST:
				recv_FriendRequset_Require(session, wPayloadSize);
				break;

				//ģ�� ��û ���
			case df_REQ_FRIEND_CANCEL:
				recv_FriendCancel_Require(session, wPayloadSize);
				break;

				//ģ�� ��û �ź�
			case df_REQ_FRIEND_DENY:
				recv_FriendRequestDeny_Require(session, wPayloadSize);
				break;

				//ģ�� ��û ����
			case df_REQ_FRIEND_AGREE:
				recv_FriendRequestAgree_Require(session, wPayloadSize);
				break;

				//��Ʈ���� �׽�Ʈ�� ����
			case df_REQ_STRESS_ECHO:
				recv_StressTest_Require(session, wPayloadSize);
				break;
			}

			return true;
		}
	}

	if (packet->GetDataSize() > 0)
		int a = 0;

	return true;
}

void CNetwork::sendCheck(SESSION *session)
{
	//Ŭ�� ����ִ� ť Ȯ��
	CPacket* packet = &session->sendPacket;

	//send
	int sendSize = send(session->socket, packet->GetReadPtr(), packet->GetDataSize(), 0);
	session->sendPacket.MoveReadPos(sendSize);

	if (sendSize > 0)
	{
//		wprintf(L"[%d] send : %d byte \n", session->socket, sendSize);
	}

	if (sendSize <=0)
		return;
}

void CNetwork::testFunc_flagFalse()
{
	std::unordered_multimap<DWORD, SESSION*>::iterator iter = sessionList.begin();

	for (iter; iter != sessionList.end() ;++iter)
	{
		iter->second->recvFlag = false;

		if(iter->second->sendFlag == true)
			iter->second->sendFlag = false;
		else
			iter->second->sendFlag = 100;
	}
}

void CNetwork::make_packet(SESSION* session, CPacket* temp, int packet_type, int multimapCount)
{
	CPacket* packet = &session->sendPacket;

	BYTE	byCode = dfPACKET_CODE;
	WORD	wMsgType = packet_type;
	WORD	wPayloadSize;
	if (packet_type == df_RES_FRIEND_LIST ||
		packet_type == df_RES_FRIEND_REQUEST_LIST ||
		packet_type == df_RES_FRIEND_REPLY_LIST)
		wPayloadSize = temp->GetDataSize() + sizeof(UINT);
	else
		wPayloadSize = temp->GetDataSize();

	*packet << byCode << wMsgType << wPayloadSize;
	if (packet_type == df_RES_FRIEND_LIST ||
		packet_type == df_RES_FRIEND_REQUEST_LIST ||
		packet_type == df_RES_FRIEND_REPLY_LIST)
	{
		*packet << (UINT)multimapCount;
		packet->PutData(temp->GetReadPtr(), wPayloadSize - sizeof(UINT));
	}
	else
		packet->PutData(temp->GetReadPtr(), wPayloadSize);
}

SESSION* CNetwork::findAccountNo(UINT64 accountNo, int type)
{
	unordered_map<DWORD,SESSION*>::iterator startIter = sessionList.begin();
	unordered_map<DWORD, SESSION*>::iterator endIter = sessionList.end();

	for (startIter; startIter != endIter; ++startIter)
	{
		if ((startIter)->second->userNo == accountNo)
			return (startIter->second);
	}

	return nullptr;
}

WCHAR* CNetwork::findNickname(UINT64 accountNo, int type)
{
	switch (type)
	{
	case MAP:
		{
			auto findIter = clientInfoMap.find(accountNo);
			if (findIter == clientInfoMap.end())
				return nullptr;
			else
				return findIter->second;
		}
		break;
	}
}

bool CNetwork::findRelation(UINT64 from, UINT64 to, int type)
{
	switch (type)
	{
		//���� �̹� ģ���ΰ�?
		case dfFRIEND:
		{
			auto iter = friendMap.find(from);
			if (iter != friendMap.end())
				return true;
			else
				return false;
		}
		break;

		//�̹� ģ����û�� �ߴ°�?
		case dfFRIEND_REQUEST:
		{
			auto iter = requestFrom.begin();
			pair<unordered_multimap<UINT64, UINT64>::iterator,
				unordered_multimap<UINT64, UINT64>::iterator> range;

			range = requestFrom.equal_range(from);

			for (iter = range.first; iter != range.second;)
			{
				if ((iter->first == from && iter->second == to))
				{
					return true;
				}
				else
					iter++;
			}
			
			return false;
		}
		break;
		//�̹� ģ����û��	 �޾Ҵ°�?
		case dfFRIEND_REPLY:
		{
			auto iter = requestTo.begin();
			pair<unordered_multimap<UINT64, UINT64>::iterator,
				unordered_multimap<UINT64, UINT64>::iterator> range;
			range = requestTo.equal_range(to);

			for (iter = range.first; iter != range.second;)
			{
				if ((iter->first == to && iter->second == from))
				{
					return true;
				}
				else
					iter++;
			}

			return false;
		}
		break;
	}

	return false;
}

bool CNetwork::deleteRelation(UINT64 from, UINT64 to, int type)
{
	if (!findRelation(from, to, type))
		return false;

	switch (type)
	{
		//���� �̹� ģ���ΰ�?
	case dfFRIEND:
	{
		auto iter = friendMap.begin();
		pair<unordered_multimap<UINT64, FRIEND*>::iterator,
			unordered_multimap<UINT64, FRIEND*>::iterator> range;

		range = friendMap.equal_range(from);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->second->FromAccountNo == from && iter->second->ToAccountNo == to))
			{
				iter = friendMap.erase(iter);
				break;
			}
			else
				iter++;
		}

		iter = friendMap.begin();
		range = friendMap.equal_range(to);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->second->FromAccountNo == to && iter->second->ToAccountNo == from))
			{
				iter = friendMap.erase(iter);
				return true;
			}
			else
				iter++;
		}
	}
	break;

	//�̹� ģ����û�� �ߴ°�?
	case dfFRIEND_REQUEST:
	{
		auto iter = requestFrom.begin();
		pair<unordered_multimap<UINT64, UINT64>::iterator,
			unordered_multimap<UINT64, UINT64>::iterator> range;

		range = requestFrom.equal_range(from);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->first == from && iter->second == to))
			{
				iter = requestFrom.erase(iter);
				break;
			}
			else
				iter++;
		}

		iter = requestTo.begin();
		range = requestTo.equal_range(to);

		for (iter = range.first; iter != range.second;)
		{
			if ((iter->first == to && iter->second == from))
			{
				iter = requestTo.erase(iter);
				return true;
			}
			else
				iter++;
		}
	}
	return false;
	}
}
bool CNetwork::recv_AccountAdd_Require(SESSION* session, WORD wPayloadSize)
{
	CPacket* packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	WCHAR* nickname = new WCHAR[dfNICK_MAX_LEN];
	packet->GetData((char*)nickname, dfNICK_MAX_LEN * 2);
	
	clientInfoMap.insert({ g_userNoCount++ , nickname });

	send_AccountAdd_Response(session);
	
	wprintf(L"[%d] login succ : df_REQ_ACCOUNT_ADD \n", session->socket);
	return true;
}

bool CNetwork::send_AccountAdd_Response(SESSION* session)
{
	CPacket temp;
	temp << g_userNoCount - 1;

	make_packet(session, &temp, df_RES_ACCOUNT_ADD);
	return false;
}


bool CNetwork::recv_Login_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;
	
	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 accountNo = 0;
	*packet >> accountNo;

	WCHAR* findname = findNickname(accountNo , MAP);

	if (findname != nullptr)
	{
		//�ִ� ������ȣ�� �� session�� ������ȣ�� �г����� �ֱ�
		session->userNo = accountNo;
		wcscpy(session->nickName, findname);
	}

	send_Login_Response(session, findname);
	wprintf(L"[%d] login succ : df_REQ_LOGIN \n", session->socket);

	return true;
}

bool CNetwork::send_Login_Response(SESSION * session, WCHAR* findName)
{
	CPacket temp;
	if (findName != nullptr)
	{
		temp << (UINT64)session->userNo;
		temp.PutData((char*)findName, dfNICK_MAX_LEN * 2);
	}
	else
	{
		temp << (UINT64)0;
	}
	make_packet(session, &temp, df_RES_LOGIN);

	return true;
}

bool CNetwork::recv_AccountList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_AccoiuntList_Response(session);
	wprintf(L"[%d] ACCOUNT_LIST succ : df_REQ_ACCOUNT_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_AccoiuntList_Response(SESSION * session)
{
	CPacket temp;
	
	temp << (UINT)clientInfoMap.size();

	unordered_map<UINT64, WCHAR*>::iterator iter; 
	for (iter = clientInfoMap.begin(); iter != clientInfoMap.end(); ++iter) {
		temp << (UINT64)iter->first;
		temp.PutData((char*)iter->second, dfNICK_MAX_LEN * 2);
	}

	make_packet(session, &temp, df_RES_ACCOUNT_LIST);

	return true;
}

bool CNetwork::recv_FriendList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_FriendList_Response(session);
	wprintf(L"[%d] FRIEND_LIST succ : df_REQ_FRIEND_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendList_Response(SESSION * session)
{
	CPacket temp;

	UINT friendNum = 0;
	
	if (session->userNo != 0)
	{
		pair<unordered_multimap<UINT64, FRIEND*>::iterator,
			unordered_multimap<UINT64, FRIEND*>::iterator> range;

		range = friendMap.equal_range(session->userNo);

		//session->userNo�� from���� ������ ģ�� ���, ���� ��ȸ
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			friendNum++;

			//ģ�� ���� ��ȣ
			temp << iter->second->ToAccountNo;

			//ģ�� �г���
			WCHAR *nickname = findNickname(iter->second->ToAccountNo, MAP);
			temp.PutData((char*)nickname, dfNICK_MAX_LEN * 2);
			
		}
	}

	make_packet(session, &temp, df_RES_FRIEND_LIST, friendNum);
	return true;
}

bool CNetwork::recv_FriendRequestList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_FriendRequestList_Response(session);
	wprintf(L"[%d] FRIEND_REQUEST_LIST succ : df_REQ_FRIEND_REQUEST_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRequestList_Response(SESSION * session)
{
	CPacket temp;

	UINT friendNum = 0;

	if (session->userNo != 0)
	{
		pair<unordered_multimap<UINT64, UINT64>::iterator,
			unordered_multimap<UINT64, UINT64>::iterator> range;

		range = requestFrom.equal_range(session->userNo);

		//session->userNo�� from���� ������ ģ�� ���, ���� ��ȸ
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			friendNum++;

			//ģ�� ���� ��ȣ
			temp << iter->second;

			//ģ�� �г���
			WCHAR *nickname = findNickname(iter->second, MAP);
			temp.PutData((char*)nickname, dfNICK_MAX_LEN * 2);
			
		}
	}

	make_packet(session, &temp, df_RES_FRIEND_REQUEST_LIST, friendNum);
	return true;
}

bool CNetwork::recv_FriendReplyList_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	send_FriendReplyList_Response(session);
	wprintf(L"[%d] FRIEND_REPLY_LIST succ : df_REQ_FRIEND_REPLY_LIST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendReplyList_Response(SESSION * session)
{
	CPacket temp;

	UINT friendNum = 0;

	if (session->userNo != 0)
	{
		pair<unordered_multimap<UINT64, UINT64>::iterator,
			unordered_multimap<UINT64, UINT64>::iterator> range;

		range = requestTo.equal_range(session->userNo);

		//session->userNo�� from���� ������ ģ�� ���, ���� ��ȸ
		for (auto iter = range.first; iter != range.second; ++iter)
		{
			friendNum++;

			//ģ�� ���� ��ȣ
			temp << iter->second;

			//ģ�� �г���
			WCHAR *nickname = findNickname(iter->second, MAP);
			temp.PutData((char*)nickname, dfNICK_MAX_LEN * 2);
		}
	}

	make_packet(session, &temp, df_RES_FRIEND_REPLY_LIST, friendNum);
	return true;
}

bool CNetwork::recv_FriendRemove_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendRemove_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_CANCEL succ : df_REQ_FRIEND_CANCEL \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRemove_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//�̹� ģ�� ��Ͽ� �ִ°�?
		if (findRelation(session->userNo, requestAccountNo, dfFRIEND))
		{
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_OK;
			deleteRelation(requestAccountNo, session->userNo, dfFRIEND);
		}
		else
		{
			//��� ������ ���� ����
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_FAIL;
			//�� ������ ��Ͽ��� �����Ϸ��� ��
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_FAIL;
			//ģ���� �ƴ�
			else
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REMOVE_NOTFRIEND;
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_REMOVE);
	return true;
}

bool CNetwork::recv_FriendRequset_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendRequset_Response(session , requestAccountNo);
	wprintf(L"[%d] FRIEND_REQUEST succ : df_REQ_FRIEND_REQUEST \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRequset_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//�̹� ģ�� ��Ͽ� �ִ°�?
		if (findRelation(session->userNo, requestAccountNo, dfFRIEND))
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_AREADY;
		else if (findRelation(session->userNo, requestAccountNo, dfFRIEND_REQUEST))
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_AREADY;
		else
		{
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_NOTFOUND;
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_NOTFOUND;
			else
			{
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_OK;
				requestFrom.insert({ session->userNo, requestAccountNo });
				requestTo.insert({ requestAccountNo, session->userNo });
			}
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_REQUEST_NOTFOUND;

	make_packet(session, &temp, df_RES_FRIEND_REQUEST);
	return true;
}

bool CNetwork::recv_FriendCancel_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendCancel_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_CANCEL succ : df_REQ_FRIEND_CANCEL \n", session->socket);

	return true;
}

bool CNetwork::send_FriendCancel_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//�̹� ģ�� ��û ��Ͽ� �ִ°�?
		if (findRelation(session->userNo, requestAccountNo, dfFRIEND_REQUEST))
		{
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_OK;
			deleteRelation(session->userNo, requestAccountNo, dfFRIEND_REQUEST);
		}
		else
		{
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_NOTFRIEND;
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_NOTFRIEND;
			else
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_FAIL;
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_CANCEL_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_CANCEL);
	return true;
}

bool CNetwork::recv_FriendRequestDeny_Require(SESSION* session, WORD wPayloadSize)
{
	CPacket* packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;
	*packet >> requestAccountNo;
	send_FriendRequestDeny_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_DENY succ : df_RES_FRIEND_DENY \n", session->socket);
	return true;
}

bool CNetwork::send_FriendRequestDeny_Response(SESSION* session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//�̹� ģ�� ��û ��Ͽ� �ִ°�?
		if (findRelation(requestAccountNo, session->userNo, dfFRIEND_REQUEST))
		{
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_OK;
			deleteRelation(requestAccountNo, session->userNo, dfFRIEND_REQUEST);
		}
		else
		{
			if (findNickname(requestAccountNo, MAP) == nullptr)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_FAIL;
			else if (requestAccountNo == session->userNo)
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_FAIL;
			else
				temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_NOTFRIEND;
		}
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_DENY_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_DENY);
	return true;
}

bool CNetwork::recv_FriendRequestAgree_Require(SESSION * session, WORD wPayloadSize)
{
	CPacket *packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
		return false;

	UINT64 requestAccountNo;

	*packet >> requestAccountNo;

	send_FriendRequestAgree_Response(session, requestAccountNo);
	wprintf(L"[%d] FRIEND_AGREE succ : df_REQ_FRIEND_AGREE \n", session->socket);

	return true;
}

bool CNetwork::send_FriendRequestAgree_Response(SESSION * session, UINT64 requestAccountNo)
{
	CPacket temp;

	if (session->userNo != 0)
	{
		//�̹� ģ�� ��Ͽ� �ִ°�?
		if(findRelation(requestAccountNo, session->userNo, dfFRIEND_REPLY))
		{
			deleteRelation(requestAccountNo, session->userNo, dfFRIEND_REQUEST);
			FRIEND* newFriend = new FRIEND(requestAccountNo, session->userNo);
			friendMap.insert({ requestAccountNo, newFriend });
			newFriend = new FRIEND(session->userNo, requestAccountNo);
			friendMap.insert({ session->userNo, newFriend});
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_AGREE_OK;
		}
		else
			temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_AGREE_NOTFRIEND;
	}
	else
		temp << requestAccountNo << (BYTE)df_RESULT_FRIEND_AGREE_FAIL;

	make_packet(session, &temp, df_RES_FRIEND_AGREE);
	return true;
}


bool CNetwork::recv_StressTest_Require(SESSION* session, WORD wPayloadSize)
{
	CPacket* packet = &session->recvPacket;

	if (packet->GetDataSize() < wPayloadSize)
	{
		return false;
		wprintf(L"Payload Size Error : %d != $d\n", packet->GetDataSize(), wPayloadSize);
	}

	WORD size;
	*packet >> size;
	char* str = new char[size + 1];
	packet->GetData(str, size);
	packet->Clear();
	str[size] = '\0';

	if (packet->GetDataSize() > 0)
		int a = 0;

	session->recvFlag += 1;

	send_StressTest_Response(session, size, str);
//	wprintf(L"[%d] STRESS_ECHO succ : df_REQ_STRESS_ECHO \n", session->socket);

	return true;
}


bool CNetwork::send_StressTest_Response(SESSION* session, WORD size, char* str)
{
	CPacket temp;
	
	temp << (WORD)size;
	temp.PutData(str, size);

	make_packet(session, &temp, df_RES_STRESS_ECHO);

	session->sendFlag += 1;

	return true;
}

void CNetwork::printText()
{
	wprintf(L"recv Count : %d \t // send Count : %d \n", g_recvCount, g_sendCount);
}


/*
void  CNetwork::disconnect(SESSION *session)
{
	// �濡 �� �ִٸ�
	if (session->state == eROOM)
	{
		std::list<ROOM*>::iterator iter = roomList.begin();

		for (iter; iter != roomList.end(); ++iter)
		{
			ROOM *room = (*iter);

			if (room->m_iRoom_No == session->roomNO)
			{
				//sendRoomExit(session);
				room->m_iEnter_player_num--;

				if (room->m_iEnter_player_num == 0)
				{
					// �� ����
					//sendRoomDelete(room);
					roomList.remove(room);
					delete room;
					break;
				}
			}
		}
	}

	sessionList.remove(session);
	closesocket(session->socket);
	delete session;
	session = nullptr;

}

//�� MsgType, Payload �� �� ����Ʈ ���ϱ� % 256
//char CNetwork::CheckSumCheck(WORD _type, char* _bufPtr, int _payLoadSize)
//{
//	char checkSum;
//
//	checkSum = char(((_type) >> 8) & 0xff) + char((_type) & 0xff);
//
//	for (int i = 0; i < _payLoadSize; i++)
//		checkSum += _bufPtr[i];
//
//	checkSum %= 256;
//
//	return checkSum;
//}

*/