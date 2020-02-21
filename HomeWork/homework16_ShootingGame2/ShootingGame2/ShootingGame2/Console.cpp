#include <windows.h>
#include <stdio.h>
#include "Console.h"

HANDLE  hConsole;

// ȭ�� �������� ���ֱ� ���� ȭ�� ����.
char szScreenBuffer[dfSCREEN_HEIGHT][dfSCREEN_WIDTH];

//Ÿ��Ʋ ����
char szTitleBuffer[20][20] = { 0, };

//-------------------------------------------------------------
// �ܼ� ��� ���� �غ� �۾�.
//
//-------------------------------------------------------------
void cs_Initial(void)
{
	CONSOLE_CURSOR_INFO stConsoleCursor;

	//-------------------------------------------------------------
	// ȭ���� Ŀ���� �Ⱥ��̰Բ� �����Ѵ�.
	//-------------------------------------------------------------
	stConsoleCursor.bVisible = FALSE;
	stConsoleCursor.dwSize   = 1;			// Ŀ�� ũ��.
											// �̻��ϰԵ� 0 �̸� ���´�. 1���ϸ� �ȳ��´�.

	//-------------------------------------------------------------
	// �ܼ�ȭ�� (���ٴٵ� �ƿ�ǲ) �ڵ��� ���Ѵ�.
	//-------------------------------------------------------------
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(hConsole, &stConsoleCursor);
}

//-------------------------------------------------------------
// �ܼ� ȭ���� Ŀ���� X, Y ��ǥ�� �̵���Ų��.
//
//-------------------------------------------------------------
//void cs_MoveCursor(int iPosX, int iPosY)
//{
//	COORD stCoord;
//	stCoord.X = iPosX;
//	stCoord.Y = iPosY;
//	//-------------------------------------------------------------
//	// ���ϴ� ��ġ�� Ŀ���� �̵���Ų��.
//	//-------------------------------------------------------------
//	SetConsoleCursorPosition(hConsole, stCoord);
//}

void gotoxy(int iPosX, int iPosY)
{
	COORD stCoord;
	stCoord.X = iPosX;
	stCoord.Y = iPosY;
	//-------------------------------------------------------------
	// ���ϴ� ��ġ�� Ŀ���� �̵���Ų��.
	//-------------------------------------------------------------
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), stCoord);
}

//-------------------------------------------------------------
// �ܼ� ȭ���� ����ȭ �Ѵ�.
//
//-------------------------------------------------------------
void cs_ClearScreen(void)
{
	int iCountX, iCountY;
	DWORD dw;

	FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', 100*100, { 0, 0 }, &dw);

/*
	//-------------------------------------------------------------
	// ȭ�� ũ�⸸ŭ ����, ���� ���� for ���� ����Ͽ�
	// ������ ��ǥ���� printf(" ");  ������ ���� ��� ���ش�.
	//-------------------------------------------------------------
	for ( iCountY = 0 ; iCountY < dfSCREEN_HEIGHT; iCountY++ )
	{
		for ( iCountX = 0; iCountX < dfSCREEN_WIDTH; iCountX++ )
		{
			cs_MoveCursor(iCountX, iCountY);
			printf(" ");
		}
	}
*/
}

// ������ ������ ȭ������ ����ִ� �Լ�.
void Buffer_Flip(void)
{
	//cs_ClearScreen();

	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		gotoxy(0, i);
		printf("%s\n", szScreenBuffer[i]);
	}
}

// ȭ�� ���۸� �����ִ� �Լ�
void Buffer_Clear(void)
{
	for (int i = 0; i < dfSCREEN_HEIGHT; ++i)
	{
		for (int j = 0; j < dfSCREEN_WIDTH - 1; ++j)
		{
			szScreenBuffer[i][j] = ' ';
		}
		szScreenBuffer[i][dfSCREEN_WIDTH - 1] = '\0';
	}
}