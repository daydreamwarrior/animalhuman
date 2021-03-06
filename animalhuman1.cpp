// animalhuman1.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "animalhuman1.h"

#define MAX_LOADSTRING 100
#define WIDTH 970
#define HEIGHT 800

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
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
    LoadStringW(hInstance, IDC_ANIMALHUMAN1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ANIMALHUMAN1));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANIMALHUMAN1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = nullptr;// MAKEINTRESOURCEW(IDC_ANIMALHUMAN1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      200, 10, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

struct Animal {
	int xPos, yPos;//좌표
	int count;//비트맵애니메이션 카운트
	HBITMAP Walk[6];//비트맵애니메이션 비트맵배열
};

Animal bear;
Animal tiger;

struct Garlic {
	int xPos, yPos;
};
Garlic garlics[50];

void bearAnimation(int xPos, int yPos, HDC hdc) {
	HDC memDC;
	HBITMAP oldBit;

	bear.count++;
	bear.count = bear.count % 6;
	bear.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
	bear.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
	bear.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3));
	bear.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
	bear.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5));
	bear.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));

	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, bear.Walk[bear.count]);
	TransparentBlt(hdc, xPos, yPos, 85, 65, memDC, 0, 0, 85, 65, RGB(0, 64, 128));
	//SelectObject(memDC, oldBit);

	for (int i = 0; i < 10; i++)
		DeleteObject(bear.Walk[i]);
	DeleteDC(memDC);
	//DeleteObject(hBit);
}
void tigerAnimation(int xPos, int yPos, HDC hdc) {

	HDC memDC;
	HBITMAP oldBit;

	tiger.count++;
	tiger.count = tiger.count % 6;
	tiger.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
	tiger.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP8));
	tiger.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
	tiger.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP10));
	tiger.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP11));
	tiger.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP12));

	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, tiger.Walk[tiger.count]);
	TransparentBlt(hdc, xPos, yPos, 125, 70, memDC, 0, 0, 125, 70, RGB(39, 87, 207));

	//AlphaBlend(hdc, xPos, yPos, 125, 70, memDC, 0, 0, 125, 70, bf);
	//	SelectObject(memDC, oldBit);
	//삭제
	for (int i = 0; i < 10; i++)
		DeleteObject(tiger.Walk[i]);
	DeleteDC(memDC);
	//DeleteObject(hBit);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memDC;
	PAINTSTRUCT ps;
	HBITMAP hBit, oldBit;
	
    switch (message)
    {
	case WM_CREATE:
		bear.xPos = 85;
		tiger.xPos = WIDTH - 85 - 125;	
		bear.yPos = tiger.xPos = 600;//땅
		break;

    case WM_PAINT://배경, 스프라이트, 동굴에 쌓이는거(키다운에서 카운트하고 invalidrect하면 count개수만큼 비트맵 그림)
        break;

	case WM_TIMER://0번-움직임 1번-마늘, 마차 생성 텀
		break;

	case WM_KEYDOWN://스프라이트 움직이는거, 동굴 앞 좌표까지 (마늘을 든상태로) 도달하면 count됨
		//곰 움직임
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			bear.yPos -= 10;
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			bear.yPos += 10;
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			bear.xPos -= 10;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			bear.xPos += 10;
		}
		//호랑이 움직임
		if (GetAsyncKeyState('W') & 0x8000) {
			bear.yPos -= 10;
		}
		if (GetAsyncKeyState('S') & 0x8000) {
			bear.yPos += 10;
		}
		if (GetAsyncKeyState('A') & 0x8000) {
			bear.xPos -= 10;
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			bear.xPos += 10;
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

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
