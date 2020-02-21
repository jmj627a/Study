#include <Windows.h>
#include "CPlayer.h"
#include "CMessageQueue.h"

bool CPlayer::Action()
{
	if (true != flag) return false;

	//메세지큐 꺼내서 이동
	while (checkMessage());


	//충돌체크


	//키입력
	player_KeyInput();

	return true;
}

// 버퍼의 특정 위치에 원하는 문자를 출력.
bool CPlayer::Draw()
{
	if (true != flag) 
		return false;

	szScreenBuffer[Y][X] = 'A';
	return true;
}


//플레이어 체력이 0 이하인지 확인
bool CPlayer::isPlayerDeath()
{
	if (life < 0)
		return true;
	else
		return false;
}

void CPlayer::playerDeathCheck()
{
	if (isPlayerDeath())
	{
		message->Enq(GAME_OVER);
	}
}

//키보드 입력
void CPlayer::player_KeyInput()
{
	if (GetAsyncKeyState(VK_LEFT))
	{
		message->Enq(KEY_LEFT);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		message->Enq(KEY_RIGHT);
	}
	if (GetAsyncKeyState(VK_UP))
	{
		message->Enq(KEY_UP);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		message->Enq(KEY_DOWN);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		message->Enq(KEY_SPACE);
	}
	if (GetAsyncKeyState(VK_F12))
	{
		message->Enq(KEY_UP);
	}
	
}

bool CPlayer::checkMessage()
{
	int _message;
	int flag = message->Deq(&_message);

	if (flag == false)
		return false;

	//방향키 이동과, 총알 발사에 대한 메세지 분류
	switch (_message)
	{
	case KEY_UP:
		if(Y > 1)
			Y--;
		break;

	case KEY_DOWN:
		if(Y<dfSCREEN_HEIGHT-1)
			Y++;
		break;

	case KEY_RIGHT:
		if (X < dfSCREEN_WIDTH - 1)
			X++;
		break;

	case KEY_LEFT:
		if (X > 1)
			X--;
		break;

	case KEY_SPACE:
		//총알생성
		break;
	}

	return true;
}