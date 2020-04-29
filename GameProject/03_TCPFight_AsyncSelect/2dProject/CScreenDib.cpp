#pragma once
#include "framework.h"
#include "CScreenDib.h"

//#pragma comment (lib, "winmm")

CScreenDib::CScreenDib(int iWidth, int iHeight, int iColorBit)
{
	memset(&m_stDibInfo, 0, sizeof(BITMAPINFO));

	m_bypBuffer = NULL;

	m_iWidth = 0;
	m_iHeight = 0;
	m_iPitch = 0;
	m_iColorBit = 0;
	m_iBufferSize = 0;

	//�������� ���ڷ� ���۸� �޴´�
	CreateDibBuffer(iWidth, iHeight, iColorBit);
}

CScreenDib::~CScreenDib()
{
	ReleaseDibBuffer();
}

void CScreenDib::CreateDibBuffer(int iWidth, int iHeight, int iColorBit)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iColorBit = iColorBit;

	m_iPitch = ((m_iWidth * (m_iColorBit / 8)) + 3) & ~3; //�ø��ؼ� �ڿ� 2��Ʈ�� 00���� ������ ����. �׳� 00���� �ٲٸ� �����̴ϱ�
	m_iBufferSize = m_iPitch * m_iHeight;


	//DibInfo ��� ����
	//Dib ��½� ����� ����ϱ� ���� ���̰��� -�� �Է��ϰ� �ִ�.
	m_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_stDibInfo.bmiHeader.biWidth = m_iWidth;
	m_stDibInfo.bmiHeader.biHeight = -m_iHeight;
	m_stDibInfo.bmiHeader.biPlanes = 1;
	m_stDibInfo.bmiHeader.biBitCount = m_iColorBit;
	m_stDibInfo.bmiHeader.biCompression= 0;
	m_stDibInfo.bmiHeader.biSizeImage= m_iBufferSize;
	m_stDibInfo.bmiHeader.biXPelsPerMeter= 0;
	m_stDibInfo.bmiHeader.biYPelsPerMeter= 0;
	m_stDibInfo.bmiHeader.biClrUsed = 0;
	m_stDibInfo.bmiHeader.biClrImportant= 0;

	//���� ����
	m_bypBuffer = new BYTE[m_iBufferSize];
	memset(m_bypBuffer, 0xff, m_iBufferSize);
}

void CScreenDib::ReleaseDibBuffer(void)
{
	m_iWidth = 0;
	m_iHeight = 0;
	m_iPitch = 0;
	m_iBufferSize = 0;

	memset(&m_stDibInfo, 0x00, sizeof(BITMAPINFO));

	if (m_bypBuffer != NULL)
		delete[] m_bypBuffer;

	m_bypBuffer = NULL;
}

void CScreenDib::Flip(HWND hWnd, int iX, int iY)
{
	if(m_bypBuffer == NULL) return;

	RECT Rect;
	HDC hDC;


	//���� ����� DC�� ��´�
	GetWindowRect(hWnd, &Rect);
	hDC = GetDC(hWnd);

	//��Ʈ���� ���� ũ��� ���
	int i = SetDIBitsToDevice(hDC, 0, 0, m_iWidth, m_iHeight, 
		0, 0, 0, m_iHeight, m_bypBuffer, &m_stDibInfo, DIB_RGB_COLORS);


	//ȭ�鿡 ������ ǥ�� �κ�, �ʿ信 ���� ���
	{
		static wchar_t szFrame[15];
		static int iFrame = 0;
		static DWORD dwTick = timeGetTime();

		iFrame++;

		if (dwTick + 1000 < timeGetTime())
		{
			wsprintf(szFrame, L"TFrame : %d   ", iFrame);
			iFrame = 0;
			dwTick = timeGetTime();
		}

		TextOut(hDC, 700, 470, szFrame, wcslen(szFrame));
		TextOut(hDC, 0, 0, szFrame, wcslen(szFrame));
	}
	ReleaseDC(hWnd, hDC);
}

BYTE* CScreenDib::GetDibBuffer(void)
{
	return m_bypBuffer;
}

int CScreenDib::GetWidth(void)
{
	return m_iWidth;
}

int CScreenDib::GetHeight(void)
{
	return m_iHeight;
}

int CScreenDib::GetPitch(void)
{
	return m_iPitch;
}