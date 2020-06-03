﻿// project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#pragma once
#include "pch.h"
#include "project.h"
#include "CJumpPointSearch.h"
#include "CBresenham.h"

#define MAX_LOADSTRING 100

TILE g_Grid[WORLD_HEIGHT][WORLD_WIDTH] = { eBLANK, };

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HDC			g_hMemDC;
HBITMAP		g_hMemDC_Bitmap;
HBITMAP		g_hMemDC_OldBitmap;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 1200, 800, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECT));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PROJECT);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int startX;
    static int startY;
    static int endX;
    static int endY;

    static bool blockedFlag;

    static int mouseX;
    static int mouseY;

	static bool lineFlag = false;

    static CJumpPointSearch JPS;
    static CBresenham line;
    switch (message)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 50, NULL);
        break;
    case WM_TIMER:

        if(JPS.m_pEndNode == nullptr)
			JPS.pathFind(hWnd);
        break;
        //시작위치
    case WM_LBUTTONDBLCLK:
        g_Grid[startY][startX].grid_type = eBLANK;

        startX = LOWORD(lParam) / 20;
        startY = HIWORD(lParam) / 20;

        g_Grid[startY][startX].grid_type = eSTART;
        InvalidateRect(hWnd, NULL, false);
        break;

        //종료위치
    case WM_RBUTTONDBLCLK:
        g_Grid[endY][endX].grid_type = eBLANK;

        endX = LOWORD(lParam) / 20;
        endY = HIWORD(lParam) / 20;

        g_Grid[endY][endX].grid_type = eEND;
        InvalidateRect(hWnd, NULL, false);
        break;

        //길찾기 시작
    case WM_LBUTTONDOWN:
    {
        //openlist에 시작 노드 추가
        NODE* newNode = nullptr;

        for (int i = 0; i < 50; ++i)
            for (int j = 0; j < 30; ++j)
                if (g_Grid[j][i].grid_type == eSTART)
                    newNode = new NODE(i, j, nullptr);

        for (int i = 0; i < 50; ++i)
            for (int j = 0; j < 30; ++j)
                if (g_Grid[j][i].grid_type == eEND)
                {
                    JPS.setEndPos(i, j);
                    JPS.setG(newNode);
                    JPS.setH(newNode);
                    JPS.setF(newNode);
                    JPS.openList.push_back(newNode);
                }
        JPS.m_iStartX= startX;
        JPS.m_iStartY = startY;
        JPS.pathFind(hWnd);

        InvalidateRect(hWnd, NULL, false);
    }
    break;
	case WM_RBUTTONDOWN:
		lineFlag = true;
		InvalidateRect(hWnd, NULL, false);
		break;
	case WM_RBUTTONUP:
		lineFlag = false;
		InvalidateRect(hWnd, NULL, false);
		break;

    //장애물
    case WM_MBUTTONDOWN:
    {
        blockedFlag = true;
        InvalidateRect(hWnd, NULL, false);
    }
    break;
    case WM_MBUTTONUP:
    {
        blockedFlag = false;
        InvalidateRect(hWnd, NULL, false);
    }
    break;
    case WM_MOUSEMOVE:
        if (blockedFlag)
        {
            mouseX = LOWORD(lParam) / 20;
            mouseY = HIWORD(lParam) / 20;

            if (g_Grid[mouseY][mouseX].grid_type == eBLANK)
                g_Grid[mouseY][mouseX].grid_type = eBLOCKED;
            else if (g_Grid[mouseY][mouseX].grid_type == eBLOCKED)
              //  g_Grid[mouseY][mouseX] = eBLANK;
            InvalidateRect(hWnd, NULL, false);
        }

		//if (lineFlag)
		//{
		//	line.endPos.m_ix = LOWORD(lParam) / 20;
		//	line.endPos.m_iy = HIWORD(lParam) / 20;
		//
        //    std::list<POS>::iterator iter;
        //    while (line.dotList.empty() == false)
        //    {
        //        iter = line.dotList.begin();
        //        line.dotList.erase(iter);
        //    }
        //    for (int i = 0; i < 50; ++i)
        //    {
        //        for (int j = 0; j < 30; ++j)
        //        {
        //            g_Grid[j][i].grid_type = eBLANK;
        //        }
        //    }
        //
		//	line.checkDot();
		//	InvalidateRect(hWnd, NULL, true);
		//}
        //else
        //{
        //    line.startPos.m_ix = LOWORD(lParam) / 20;
        //    line.startPos.m_iy = HIWORD(lParam) / 20;
        //}
        break;
	case WM_KEYDOWN:
        //리셋
		if (GetAsyncKeyState(VK_SPACE))
		{
			std::list<NODE*>::iterator iter;
			while (JPS.openList.empty() == false)
			{
				iter = JPS.openList.begin();
				delete[](*iter);
				JPS.openList.erase(iter);
			}
			while (JPS.closeList.empty() == false)
			{
				iter = JPS.closeList.begin();
				delete[](*iter);
				JPS.closeList.erase(iter);
			}
			while (JPS.fastPathList.empty() == false)
			{
				iter = JPS.fastPathList.begin();
				JPS.fastPathList.erase(iter);
			}
			
			for (int i = 0; i < 50; ++i)
				for (int j = 0; j < 30; ++j)
					g_Grid[j][i].grid_type = eBLANK;
            
			JPS.setEndNodeNULL();

			InvalidateRect(hWnd, NULL, false);
		}
		break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        line.LineDraw(hWnd);

        for (int i = 0; i < 50; ++i)
        {
            for (int j = 0; j < 30; ++j)
            {
                HBRUSH Brush, oBrush;
                switch (g_Grid[j][i].grid_type)
                {
                case eBLANK:    Brush = CreateSolidBrush(RGB(255, 255, 255)); break;    //흰색
                case eSTART:    Brush = CreateSolidBrush(RGB(255, 0, 255)); break;      //보라
                case eEND:      Brush = CreateSolidBrush(RGB(255, 0, 0)); break;        //빨강
                case eBLOCKED:		Brush = CreateSolidBrush(RGB(200, 200, 200)); break;    //회색
                case eCLOSE:  Brush = CreateSolidBrush(RGB(255, 255, 0)); break;      //노랑
                case eOPEN:    Brush = CreateSolidBrush(RGB(0, 0, 255)); break;        //파랑
                case eLINE:    Brush = CreateSolidBrush(RGB(255, 100, 100)); break;        
                default:        Brush = CreateSolidBrush(RGB(0, 0, 0)); break;
                }
                oBrush = (HBRUSH)SelectObject(hdc, Brush);
                Rectangle(hdc, i * 20, j * 20, (i + 1) * 20, (j + 1) * 20);

                SelectObject(hdc, oBrush);
                DeleteObject(Brush);
            }
        }


		if (JPS.m_pEndNode != nullptr)
		{
			JPS.pathDraw(hWnd);
			JPS.fastPathDraw(hWnd);
		}

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}