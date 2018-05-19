// beartiger.cpp: 응용 프로그램의 진입점을 정의합니다.
//
#pragma comment(lib, "msimg32.lib")
#include "stdafx.h"
#include "beartiger.h"
#include "resource.h"
#include <atlimage.h>


#define MAX_LOADSTRING 100
#define WIDTH 800
#define HEIGHT 800

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
BOOL keybuffer[256];

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
    LoadStringW(hInstance, IDC_BEARTIGER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BEARTIGER));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BEARTIGER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = nullptr;// MAKEINTRESOURCEW(IDC_BEARTIGER);
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
      350, 10, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

CImage g_cimg;
CImage g_cimgBackBuff;
UINT g_SpriteX;
UINT g_SpriteY;
UINT g_SpriteCount;
UINT g_SpriteCurrent;

//void DrawSprite(HDC hdc, POINT ptStart = POINT{ 0,0 }) {
//	UINT SprtieWidth = g_cimg
//}

//void OnDraw(HWND hWnd) {
//
//	RECT Clientrc;
//	GetClientRect(hWnd, &Clientrc);
//	PAINTSTRUCT ps;
//	HDC hdc, memDC;
//
//	hdc = BeginPaint(hWnd, &ps);
//	//memDC = g_cimgBackBuff.GetDC();
//
//	//FillRect(memDC, &Clientrc, (HBRUSH)(GetStockObject(WHITE_BRUSH)));
//
//	////곰 그리기(memDC에)
//	//g_cimg.Draw(memDC, 0, 0, 621, 380);
//
//	//g_cimgBackBuff.Draw(hdc, 0, 0);
//	//g_cimgBackBuff.ReleaseDC();
//	//
//	{
//		HDC memDC = CreateCompatibleDC(hdc);
//		HBITMAP memBit = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
//		SelectObject(memDC, memBit);
//		g_cimg.Draw(memDC, 15, 75, 85, 65);
//
//		TransparentBlt(hdc,0,0,200,200,memDC,15,75,85,65,RGB(0,64,128));
//		DeleteObject(memBit);
//		DeleteDC(memDC);
//	}
//	EndPaint(hWnd, &ps);
//	g_cimg.Destroy();
//}

struct Animal {
	int xPos, yPos;
	int count;
	HBITMAP Walk[6],die;
	int damage;
};

Animal bear;
Animal tiger;

struct Garlic {
	int xPos, yPos;
};

Garlic garlics[50];
//void DrawSprite(HDC hdc, int xPos, int yPos) {
//	HDC memDC;
//	HBITMAP garlic;
//	//마늘 비트맵 불러오기
//	garlic = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP14));
//	
//	memDC = CreateCompatibleDC(hdc);
//
//
//}

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

//struct BLENDFUNCTION {
//	BYTE BlendOp; // 그냥 AC_SRC_OVER
//	BYTE BlendFlags; // 그냥 0
//	BYTE SourceConstantAlpha; // 0 ~ 255 값이며 0이면 아에 안보이고, 255면 투명도를 적용 안한거랑 같다.
//	BYTE AlphaFormat; // 그냥 0
//};
void tigerDead(int xPos, int yPos, HDC hdc) {
	HDC memDC, hAlpha;
	HBITMAP bAlpha;

	//호랑이
	memDC = CreateCompatibleDC(hdc);
	tiger.die = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP14));
	SelectObject(memDC, tiger.die);

	//배경
	hAlpha = CreateCompatibleDC(hdc);
	bAlpha = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
	SelectObject(hAlpha, bAlpha);

	StretchBlt(hdc, xPos, yPos, 125, 70, memDC, xPos, yPos, 125, 70, SRCCOPY);//배경복사
	TransparentBlt(hAlpha, xPos, yPos, 125, 70, memDC, 0, 0, 125, 70, RGB(39, 87, 207));//트랜스

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = 0;
	bf.SourceConstantAlpha = 50;

	AlphaBlend(hdc, xPos, yPos, 125, 70, hAlpha, 0, 0, 125, 70, bf);
}
void tigerAnimation(int xPos, int yPos, HDC hdc) {

	HDC memDC;
	HBITMAP oldBit;

	tiger.count++;
	tiger.count = tiger.count % 6;
	tiger.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP8));
	tiger.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
	tiger.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP10));
	tiger.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP11));
	tiger.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP12));
	tiger.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP13));

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
	HDC hdc;
	PAINTSTRUCT ps;
	HDC memDC;
	RECT rc1,rc2,rc3;
	RECT rctemp;
	HBITMAP garlic, hBit, oldBit;
	static int xPos;//마늘
	static int yPos;
	static int garlicCount;
	
	RECT rcbear = { bear.xPos,bear.yPos }; 

    switch (message)
    {
	case WM_CREATE:
		bear.xPos = 400;//중앙에서 시작
		bear.yPos = 400;
		tiger.xPos = 700-125;
		tiger.yPos = 300;
		garlicCount = 0;
		tiger.damage = 0;
		SetTimer(hWnd, 1, 100, NULL);
		/*if (g_cimg.IsNull()) {
			MessageBox(hWnd, TEXT("NULL"), TEXT("NULL"), MB_OK);
		}
*/
		//backbuffer 생성
		//g_cimgBackBuff.Create(WIDTH, HEIGHT, 24, 0);
		//SetTimer(hWnd, 0, 60, NULL);
		break;

	case WM_KEYDOWN:
		switch (wParam){
		
		case VK_UP:
			keybuffer[wParam] = TRUE;
			bear.yPos -= 10;
		if (IntersectRect(&rctemp, &rcbear, &rc1) == true || IntersectRect(&rctemp, &rcbear, &rc2) == true || IntersectRect(&rctemp, &rcbear, &rc3) == true)
			if (bear.yPos == 600||(bear.yPos==300 && bear.xPos>300 && bear.xPos<400))
				bear.yPos += 10;
			/*if (bear.yPos < 0)
				bear.yPos = HEIGHT;*/
			break;

		case VK_DOWN:
			keybuffer[wParam] = TRUE;
			bear.yPos += 10;
			if (IntersectRect(&rctemp, &rcbear, &rc1) == true || IntersectRect(&rctemp, &rcbear, &rc2) == true || IntersectRect(&rctemp, &rcbear, &rc3) == true)
			if ((bear.yPos == 500 && bear.xPos>100&bear.xPos<200)||bear.yPos==200)
				bear.yPos -= 10;
			/*if (bear.yPos > HEIGHT)
				bear.yPos = 0;*/
			break;

		case VK_LEFT:
			keybuffer[wParam] = TRUE;
			bear.xPos -= 10;
			if (IntersectRect(&rctemp, &rcbear, &rc1) == true || IntersectRect(&rctemp, &rcbear, &rc2) == true || IntersectRect(&rctemp, &rcbear, &rc3) == true)
				bear.xPos += 10;
			/*if (bear.xPos < 0)
				bear.xPos = WIDTH;*/
			break;

		case VK_RIGHT:
			keybuffer[wParam] = TRUE;
			bear.xPos += 10;
			if (IntersectRect(&rctemp, &rcbear, &rc1) == true || IntersectRect(&rctemp, &rcbear, &rc2) == true || IntersectRect(&rctemp, &rcbear, &rc3) == true)
				bear.xPos -= 10;
			/*if (bear.xPos > WIDTH)
				bear.xPos = 0;*/
			break;

		case VK_SPACE:
			if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
				MessageBox(hWnd, TEXT("스페이스 바를 눌렀습니다."), TEXT("GetAsyncKeyState"), MB_OK);
			}
			/*garlicCount++;
			if (garlicCount > 50)
				garlicCount = 0;
			InvalidateRect(hWnd, NULL, false);*/

			break;

		default:
			break;
		}
		break;


		
	case WM_CHAR:
		switch (wParam) {

		case 'q':
				PostQuitMessage(0);
				break;

		}
		break;

	case WM_TIMER:

		//마늘발사
		garlics[garlicCount].xPos = bear.xPos + 90;
		garlics[garlicCount].yPos = bear.yPos + 25;
		for (int i = 0; i < garlicCount; i++) {
			if (garlics[i].xPos < WIDTH)
				garlics[i].xPos += 10;
			else;
			//if (garlics[i].xPos + 20 == tiger.xPos) {//충돌
			//	garlics[i].xPos = NULL;
			//	garlics[i].yPos = NULL;
			//}
		}
		//호랑이 움직임
		tiger.xPos -= 2;
		if (tiger.xPos < 0)
			tiger.xPos = 700-125;

		////호랑이마늘 맞음
		//if (tiger.damage > 5) {
		//	if (bf.SourceConstantAlpha > 50) {
		//		bf.SourceConstantAlpha -= 10;
		//	}
		//}
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
		/*(++g_SpriteCurrent) %= g_SpriteCount;
		InvalidateRect(hWnd, NULL, false);*/

    case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hdc);

		//배경
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
		oldBit = (HBITMAP)SelectObject(memDC, hBit);
		TransparentBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 1024, 1024, RGB(0, 64, 128));
		
		//장애물(충돌체크 해야함)
		HBRUSH hbrush, oldbrush;
		hbrush = CreateSolidBrush(RGB(0, 210, 190));
		oldbrush = (HBRUSH)SelectObject(hdc,hbrush);
		rc1 = { 700,0,800,100 };
		FillRect(hdc, &rc1, hbrush);
		rc2 = { 100,500,200,600 };
		FillRect(hdc, &rc2, hbrush);
		rc3 = { 300,200,400,300 };
		FillRect(hdc, &rc3, hbrush);
		SelectObject(hdc, oldbrush);
		DeleteObject(hbrush);
	
		if (tiger.damage >= 10 && tiger.damage < 15) {
			tigerDead(tiger.xPos, tiger.yPos, hdc);
		}
		else if (tiger.damage >= 15) {
			tiger.xPos = -200;
			tiger.yPos = -200;
		}
		else tigerAnimation(tiger.xPos, tiger.yPos, hdc);
		
		bearAnimation(bear.xPos,bear.yPos, hdc);
		
		//마늘(호랑이랑 충돌체크해야함)
		//if (garlicGo) {
			garlic = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP14));
			(HBITMAP)SelectObject(memDC, garlic);
			
			for (int i = 0; i < garlicCount; i++) {
				if (garlics[i].xPos > tiger.xPos && tiger.damage < 15) {//충돌(호랑이 살아있을때 넘어가지 않게)
					garlics[i].xPos = -20;
					garlics[i].yPos = -20;
					tiger.damage++;
				}
				else {
					TransparentBlt(hdc, garlics[i].xPos, garlics[i].yPos, 20, 20, memDC, 0, 0, 180, 180, RGB(0, 0, 255));
				}
			}
		//}	
		DeleteDC(memDC);
		DeleteObject(hBit);

		EndPaint(hWnd,&ps);
        break;

    case WM_DESTROY:
		//g_cimgBackBuff.Destroy();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

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
