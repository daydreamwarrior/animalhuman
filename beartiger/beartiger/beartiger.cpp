// beartiger.cpp: 응용 프로그램의 진입점을 정의합니다.
//
#pragma comment(lib, "msimg32.lib")
#pragma warning (disable:4996)

#include "stdafx.h"
#include "beartiger.h"
#include "Resource.h"
#include <atlimage.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
//----------------fmod---------------
//#include <fmod.h>
#include "Sound.h"

//FMOD_SYSTEM* soundSystem;
//FMOD_SOUND *soundFile;  // 사운드 선언
//FMOD_CHANNEL *channel;
//FMOD_RESULT result;
CSound snd;
//--------------TIMER-----------------
#define MAX_LOADSTRING 100
#define PI 3.141592
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
	LoadStringW(hInstance, IDC_BEARTIGER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BEARTIGER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;// MAKEINTRESOURCEW(IDC_BEARTIGER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED,
		350, 10, WIDTH, HEIGHT, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

struct Animal {
	int xPos, yPos;//좌표
	int movingCount, attackCount, damageCount;//비트맵애니메이션 카운트
	HBITMAP Walk[6], Attack[6], Feed[6], Damage[2];//비트맵애니메이션 _ 비트맵배열
	bool isAttack;//공격키가 눌렸는지 검사
	bool isDamaged;//맞았는지 검사
	bool takeGarlic;//마늘 먹은 상태인지 검사
	int dir;//방향(원래방향=0, 반대방향=1)
	int dropbyattack; //공격당했을 때 마늘이 다시 드랍되니까-feed가 된 인덱스 i를 저장함.
	int addScore;//점수 카운트 20되면 승리!
	bool win;//이겼는지 검사
};
Animal bear;
Animal tiger;

struct Garlic {
	int xPos, yPos;//좌표
	int now;//갈릭쑥 드랍 인덱스(0-49) 총 50개
	int GarlicOntheGround;
	HBITMAP garbit, oldgarbit;//갈릭넣는비트맵
	bool show = TRUE;
	//bool holdingGarilc;
};

Garlic gararray[200];//배열 구조체
Garlic garlics;//배열 ㄴㄴ인거
static bool garlicTime;//마늘이 내려올 시간...

struct macha {
	int xPos, yPos;
	int machacount;
	int machadir;
	int machaAttackedTime;//캐릭터에 의해 마차가 공격당한 횟수
	int dropbyMachaattack;//마차공격해서 떨궈지는 마늘 인덱스(90-100까지로함)
						  //즉 전체 마늘 (100)개 중 0-89까지는 하늘드랍&&캐릭터공격으로 획득가능한 마늘, 
						  //90-100까지는 마차에 의해 떨어지는 마늘 인덱스
	HBITMAP MachaAniBitmap[6];
	bool ismachamoving;//마차 비트맵 띄우는 거 bool
	bool machaani;//마차 맞을떄 위아래로 움직이는 애니
};

macha carriage;

//--------------------------------------------------------------------------
//투명화 구조체 BLENDFUNCTION
//struct BLENDFUNCTION {
//   BYTE BlendOp; // 그냥 AC_SRC_OVER
//   BYTE BlendFlags; // 그냥 0
//   BYTE SourceConstantAlpha; // 0 ~ 255 값이며 0이면 아에 안보이고, 255면 투명도를 적용 안한거랑 같다.
//   BYTE AlphaFormat; // 그냥 0
//};

void bearFeed(int xPos, int yPos, HDC hdc) {
	HDC memDC;
	if (bear.dir == 0) {
		bear.Feed[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP66));
		bear.Feed[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP67));
		bear.Feed[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP68));
		bear.Feed[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP69));
		bear.Feed[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP70));
		bear.Feed[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP71));
	}
	else if (bear.dir == 1) {
		bear.Feed[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP72));
		bear.Feed[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP73));
		bear.Feed[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP74));
		bear.Feed[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP75));
		bear.Feed[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP76));
		bear.Feed[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP77));
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, bear.Feed[bear.movingCount]);
	TransparentBlt(hdc, xPos, yPos, 85, 65, memDC, 0, 0, 85, 65, RGB(0, 64, 128));

	bear.movingCount++;
	bear.movingCount = bear.movingCount % 6;

	for (int i = 0; i < 6; i++)
		DeleteObject(bear.Feed[i]);
	DeleteDC(memDC);
}

void bearAnimation(int xPos, int yPos, HDC hdc) {
	HDC memDC;

	if (bear.dir == 0) {
		bear.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		bear.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		bear.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3));
		bear.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
		bear.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5));
		bear.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
	}
	else if (bear.dir == 1) {
		bear.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP30));
		bear.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP31));
		bear.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP32));
		bear.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP33));
		bear.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP34));
		bear.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP35));
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, bear.Walk[bear.movingCount]);
	TransparentBlt(hdc, xPos, yPos, 85, 65, memDC, 0, 0, 85, 65, RGB(0, 64, 128));

	bear.movingCount++;
	bear.movingCount = bear.movingCount % 6;

	for (int i = 0; i < 6; i++)
		DeleteObject(bear.Walk[i]);
	DeleteDC(memDC);
}

void bearAttack(int xPos, int yPos, HDC hdc) {
	HDC memDC;

	if (bear.dir == 0) {
		bear.Attack[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP17));
		bear.Attack[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP18));
		bear.Attack[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP19));
		bear.Attack[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP20));
		bear.Attack[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP21));
		bear.Attack[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP22));
	}
	else if (bear.dir == 1) {
		bear.Attack[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP36));
		bear.Attack[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP37));
		bear.Attack[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP38));
		bear.Attack[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP39));
		bear.Attack[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP40));
		bear.Attack[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP41));
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, bear.Attack[bear.attackCount]);
	TransparentBlt(hdc, xPos, yPos, 90, 80, memDC, 0, 0, 90, 80, RGB(0, 64, 128));

	bear.attackCount++;
	if (bear.attackCount == 6) {
		bear.attackCount = 0;
		bear.isAttack = false;
	}
	for (int i = 0; i < 6; i++)
		DeleteObject(bear.Attack[i]);
	DeleteDC(memDC);
}

void bearDamage(int xPos, int yPos, HDC hdc) {
	HDC memDC;

	if (tiger.dir == 0) {//호랑이가 때리면 그 반대방향으로 튕겨나감
		bear.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP90));
		bear.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP91));
		if (bear.xPos > 85)
			bear.xPos -= 20;

	}
	else if (tiger.dir == 1) {
		bear.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP92));
		bear.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP93));
		if (bear.xPos < WIDTH - 85 - 70)
			bear.xPos += 20;
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, bear.Damage[bear.damageCount]);
	TransparentBlt(hdc, xPos, yPos, 65, 60, memDC, 0, 0, 65, 60, RGB(0, 64, 128));

	bear.damageCount++;
	if (bear.damageCount == 2) {
		bear.damageCount = 0;
		bear.isDamaged = false;
	}
	for (int i = 0; i < 1; i++)
		DeleteObject(bear.Damage[i]);
	DeleteDC(memDC);
}

void tigerAnimation(int xPos, int yPos, HDC hdc) {

	HDC memDC;

	if (tiger.dir == 0) {
		tiger.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP8));
		tiger.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
		tiger.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP10));
		tiger.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP11));
		tiger.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP12));
		tiger.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP13));
	}
	else if (tiger.dir == 1) {
		tiger.Walk[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP42));
		tiger.Walk[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP43));
		tiger.Walk[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP44));
		tiger.Walk[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP45));
		tiger.Walk[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP46));
		tiger.Walk[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP47));
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, tiger.Walk[tiger.movingCount]);
	TransparentBlt(hdc, xPos, yPos, 125, 70, memDC, 0, 0, 125, 70, RGB(39, 87, 207));

	tiger.movingCount++;
	tiger.movingCount = tiger.movingCount % 6;

	//삭제
	for (int i = 0; i < 6; i++)
		DeleteObject(tiger.Walk[i]);
	DeleteDC(memDC);
}

void tigerFeed(int xPos, int yPos, HDC hdc) {

	HDC memDC;

	if (tiger.dir == 0) {
		tiger.Feed[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP78));
		tiger.Feed[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP79));
		tiger.Feed[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP80));
		tiger.Feed[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP81));
		tiger.Feed[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP82));
		tiger.Feed[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP83));
	}
	else if (tiger.dir == 1) {
		tiger.Feed[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP84));
		tiger.Feed[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP85));
		tiger.Feed[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP86));
		tiger.Feed[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP87));
		tiger.Feed[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP88));
		tiger.Feed[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP89));
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, tiger.Feed[tiger.movingCount]);
	TransparentBlt(hdc, xPos, yPos, 125, 70, memDC, 0, 0, 125, 70, RGB(39, 87, 207));

	tiger.movingCount++;
	tiger.movingCount = tiger.movingCount % 6;

	//삭제
	for (int i = 0; i < 6; i++)
		DeleteObject(tiger.Feed[i]);
	DeleteDC(memDC);
}

void tigerAttack(int xPos, int yPos, HDC hdc) {
	HDC memDC;

	if (tiger.dir == 0) {
		tiger.Attack[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP24));
		tiger.Attack[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP25));
		tiger.Attack[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP26));
		tiger.Attack[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP27));
		tiger.Attack[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP28));
		tiger.Attack[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP29));
	}
	else if (tiger.dir == 1) {
		tiger.Attack[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP48));
		tiger.Attack[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP49));
		tiger.Attack[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP50));
		tiger.Attack[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP51));
		tiger.Attack[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP52));
		tiger.Attack[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP53));
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, tiger.Attack[tiger.attackCount]);
	TransparentBlt(hdc, xPos, yPos, 150, 75, memDC, 0, 0, 150, 75, RGB(39, 87, 207));

	tiger.attackCount++;
	if (tiger.attackCount == 6) {
		tiger.attackCount = 0;
		tiger.isAttack = false;
	}
	for (int i = 0; i < 6; i++)
		DeleteObject(tiger.Attack[i]);
	DeleteDC(memDC);
}

void tigerDamage(int xPos, int yPos, HDC hdc) {
	HDC memDC;

	if (bear.dir == 0) {//곰이 때리면 그 반대방향으로 튕겨나감
		tiger.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP96));
		tiger.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP97));
		if (tiger.xPos < WIDTH - 85 - 96)
			tiger.xPos += 20;

	}
	else if (bear.dir == 1) {
		tiger.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP94));
		tiger.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP95));
		if (tiger.xPos > 85)
			tiger.xPos -= 20;
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, tiger.Damage[tiger.damageCount]);
	TransparentBlt(hdc, xPos, yPos, 96, 76, memDC, 0, 0, 96, 76, RGB(39, 87, 207));

	tiger.damageCount++;
	if (tiger.damageCount == 2) {
		tiger.damageCount = 0;
		tiger.isDamaged = false;
	}
	for (int i = 0; i < 1; i++)
		DeleteObject(tiger.Damage[i]);
	DeleteDC(memDC);
}

void machaAnimation(int xPos, int yPos, HDC hdc) {
	HDC memDC;

	if (carriage.machadir == 1) {
		carriage.MachaAniBitmap[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP54));
		carriage.MachaAniBitmap[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP55));
		carriage.MachaAniBitmap[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP56));
		carriage.MachaAniBitmap[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP57));
		carriage.MachaAniBitmap[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP58));
		carriage.MachaAniBitmap[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP59));

	}
	if (carriage.machadir == 2) {
		carriage.MachaAniBitmap[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP60));
		carriage.MachaAniBitmap[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP61));
		carriage.MachaAniBitmap[2] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP62));
		carriage.MachaAniBitmap[3] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP63));
		carriage.MachaAniBitmap[4] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP64));
		carriage.MachaAniBitmap[5] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP65));
	}

	memDC = CreateCompatibleDC(hdc);
	(HBITMAP)SelectObject(memDC, carriage.machacount[carriage.MachaAniBitmap]);
	TransparentBlt(hdc, xPos, yPos, 200, 100, memDC, 0, 0, 200, 100, RGB(0, 64, 128));

	if (carriage.machaani == true) {//공격받을떄 마차 흔들거리는 애니
		carriage.yPos = 445;
		(HBITMAP)SelectObject(memDC, carriage.machacount[carriage.MachaAniBitmap]);
		TransparentBlt(hdc, xPos, yPos, 200, 100, memDC, 0, 0, 200, 100, RGB(0, 64, 128));
		carriage.machaani = false;
	}

	carriage.machacount++;
	carriage.machacount = carriage.machacount % 6;
	for (int i = 0; i < 6; i++)
		DeleteObject(carriage.MachaAniBitmap[i]);
	DeleteDC(memDC);
}

void garlicprint(int xpos, int ypos, HDC hdc) {
	HDC memDC;
	memDC = CreateCompatibleDC(hdc);
	for (int i = 0; i < garlics.now; i++) {
		gararray[i].garbit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP23));
		garlics.oldgarbit = (HBITMAP)SelectObject(memDC, gararray[i].garbit);
		TransparentBlt(hdc, gararray[i].xPos, gararray[i].yPos, 50, 50, memDC, 0, 0, 50, 50, RGB(0, 64, 128));
		SelectObject(memDC, garlics.oldgarbit);
	}

	for (int i = 180; i < carriage.dropbyMachaattack; i++) {//마차 공격해서 떨어진 마늘 표시 부분
		gararray[i].garbit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP23));
		garlics.oldgarbit = (HBITMAP)SelectObject(memDC, gararray[i].garbit);
		TransparentBlt(hdc, gararray[i].xPos, gararray[i].yPos, 50, 50, memDC, 0, 0, 50, 50, RGB(0, 64, 128));
		SelectObject(memDC, garlics.oldgarbit);
	}

	if (garlicTime == true) {//새 마늘 출력
		gararray[garlics.now].garbit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP23));
		garlics.oldgarbit = (HBITMAP)SelectObject(memDC, gararray[garlics.now].garbit);
		TransparentBlt(hdc, gararray[garlics.now].xPos, gararray[garlics.now].yPos, 50, 50, memDC, 0, 0, 50, 50, RGB(0, 64, 128));
		SelectObject(memDC, garlics.oldgarbit);
		//DeleteObject(gararray[garlics.now].garbit);
	}

	for (int i = 0; i < 200; i++) {
		DeleteObject(gararray[i].garbit);
	}
	DeleteDC(memDC);
	//DeleteDC(backmemDC);
}
//
//BOOL isGarlicInCave(int ){
//	return TRUE;
//}

//-----------------------------fmod 함수-----------------------
//FMOD_RESULT FMOD_System_Create(FMOD_SYSTEM **system);
//FMOD_RESULT FMOD_System_Init(FMOD_SYSTEM *system, int MaxChannels, FMOD_INITFLAGS flags, void *ExtraDriverdata);
//FMOD_RESULT FMOD_System_createSound(FMOD_SYSTEM *system, const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, FMOD_SOUND **sound);
//FMOD_RESULT FMOD_System_PlaySound(FMOD_SYSTEM *system, FMOD_SOUND *SoundFile, FMOD_CHANNELGROUP *ChannelGroup, FMOD_BOOL paused, FMOD_CHANNEL **Channel);
float moonsunxPos = 10.f;
float moonsunyPos = 300.f;

//float diameter = 25.f;               // 원의 지름
float v_0 = 140.f;                  // 던지는 힘
float angle = 60.f;                  // 던지는 각도
float gravity = 30.f;               // 중력
float radian = angle * PI / 180;      // 각도를 호도법으로 변환
float vectorX = 0.f;               // 움직이는 x 량
float vectorY = 0.f;
float Time = 0.f;
float z = 0.f;
float Cos;
//[출처] [WINDOW_API] 포물선 운동 | 작성자 손별명

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc, memDC, backmemDC;
	PAINTSTRUCT ps;
	HBITMAP hBit, oldBit, backBit, invenBit, endingBit, caveBit, skyBit, moonsunBit;//inven : 마늘 쌓여있는 비트맵, ending: 엔딩비트맵
	CHAR str[] = "동물이라도 인간이하고싶어";
	LOGFONT lf;
	HFONT font, oldfont;
	clock_t start, end;
	static int skyxPos;
	static double timer;
	static char clockstr[1000];
	static bool isDay;
	static int gametime;// 인게임 타임 대략 잼
	static bool gameover = false;//게임오버

	//FONT {//폰트 함수 인수 설명~

	//CreateFont(height, width, escapement, orientation, weight,italic, underline, strikeout,
	//charset, outprecision, clipprecison, quality, pitchandgroup, name);

	//Height  폰트의 높이 지정. 폰트크기라고생각하면됨
	//width  폰트의 폭, 0이면 nheight를 따라 자동 지정
	//eescapement  x측과 문자열 각도(가로쓰기 세로쓰기 설정할수있겟쬬~~
	//orientation  폰트의 무게(두께),기본이 400
	//weight  폰트의 두께
	//Italic  기울임(FALSE)
	//underline  언더라인(FALSE)
	//strikeout  가운데선(FALSE)
	//charset  문자셋 설정(ANSI_CHARSET,HANGEUL_CHARSET)
	//outprecision  출력정확도, 같은 이름의 폰트가 여러개 일때 쓰는거니까 상관ㄴㄴ
	//clipprecicon  클리핑 정확도, 영역 벗어난 친구를 어떻게 할래?라는 것. 
	//quality 논리적 폰트를 물리적 폰트에 얼마나 근접시킬 것이냐(말이 개어려운데 논리 폰트에 벗어나면 글자의 품질을 더 중요시한다 뭐 이런거라함
	//pitchandfamily 폰트의 피치와 그룹(???)보통 가변 피치와 로만 그룹 씀(VARIABLE_PITCH|FF_ROMAN)안바꿔도될듯
	//name 글꼴 이름 나타내는 문자열

	switch (message)
	{
	case WM_CREATE:
		//처음 서있는 위치
		/*start = clock();
		end = clock();
		timer= ((float)(end - start) / CLOCKS_PER_SEC);*/
		isDay = true; //처음은 낮으로 시작
		skyxPos = 0;
		bear.xPos = 85;
		bear.yPos = HEIGHT - 250 - 65;
		tiger.xPos = WIDTH - 85 - 125;
		tiger.yPos = HEIGHT - 250 - 70 - 5;
		//처음 방향(곰 오른쪽, 호랑이 왼쪽 바라봄)
		bear.dir = 0;
		tiger.dir = 0;
		bear.takeGarlic = false;
		tiger.takeGarlic = false;
		bear.dropbyattack = 0;
		tiger.dropbyattack = 0;
		garlics.now = 0;
		garlicTime = false;
		garlics.GarlicOntheGround = 0;//땅에있는 마늘 개수 검사 
		carriage.ismachamoving = false;
		carriage.machaAttackedTime = 0;
		carriage.dropbyMachaattack = 180;
		carriage.machaani = false;
		for (int i = 0; i < 200; i++) {
			gararray[i].xPos = -100;
			gararray[i].yPos = -100;
		} //마늘 좌표 초기화해줫읍니다 null은 0,0이라 짱나서 마이너스로 바꿈.
		gametime = 0;
		srand((unsigned)time(NULL));
		SetTimer(hWnd, 0, 200, NULL);
		SetTimer(hWnd, 1, 5000, NULL);
		SetTimer(hWnd, 2, 10000, NULL);

		//FMOD_System_Create(&soundSystem);
		//FMOD_System_Init(soundSystem, 8, FMOD_INIT_NORMAL, NULL);
		//FMOD_System_CreateSound(soundSystem, "Electrodoodle.mp3", FMOD_LOOP_NORMAL, 0, &soundFile);
		//FMOD_System_PlaySound(soundSystem, soundFile, NULL, 0, &channel);

		snd.Add_sound();
		snd.Play(0);//n번쨰 채널의 음악을 플레이한다. 
		break;

	case WM_PAINT://배경, 스프라이트, 동굴에 쌓이는거(키다운에서 카운트하고 invalidrect하면 count개수만큼 비트맵 그림)
		hdc = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hdc);
		backmemDC = CreateCompatibleDC(hdc);

		/*_stprintf_s(clockstr, TEXT("PLAYTIME: %.2f sec"), timer);
		TextOut(hdc, 255, 300, clockstr, strlen(clockstr));
		*/
		//비트맵 순서: 배경(하늘 땅),마차,동굴,마늘이랑 캐릭터들
		backBit = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);//도화지
		oldBit = (HBITMAP)SelectObject(backmemDC, backBit);

		//하늘
		if (isDay == true) {
			skyBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP101));
		}
		else {
			skyBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP102));
		}
		SelectObject(memDC, skyBit);
		//앞부분
		StretchBlt(backmemDC, skyxPos, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, SRCCOPY);
		//잘린부분
		StretchBlt(backmemDC, 0, 0, skyxPos, HEIGHT, memDC, WIDTH - skyxPos, 0, skyxPos, 800, SRCCOPY);
		DeleteObject(skyBit);

		//달 or 해
	
		if (isDay == true) {//낮이면 해
			moonsunBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP106));
		}
		else {//달
			moonsunBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP105));
		}
		SelectObject(memDC, moonsunBit);
		TransparentBlt(backmemDC, moonsunxPos, moonsunyPos-z, 101, 102, memDC, 0, 0, 101, 102, RGB(0, 64, 128));
		DeleteObject(moonsunBit);

		//배경
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
		SelectObject(memDC, hBit);
		TransparentBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, RGB(0, 64, 128));
		DeleteObject(hBit);

		//마차
		if (carriage.ismachamoving == true) {
			machaAnimation(carriage.xPos, carriage.yPos, backmemDC);
		}

		for (int i = 0; i < garlics.now; i++) {
			garlicprint(gararray[i].xPos, gararray[i].yPos, backmemDC);
		}

		for (int i = 180; i < carriage.dropbyMachaattack; i++) {//마차 공격해서 떨어진 마늘 표시 부분
			garlicprint(gararray[i].xPos, gararray[i].yPos, backmemDC);
		}

		if (garlicTime == true) {//새 마늘 출력
			garlicprint(gararray[garlics.now].xPos, gararray[garlics.now].yPos, backmemDC);
		}

		//비트맵 출력 순서 고정(마차와 마늘쑥 호랑이 곰보다 먼저)

		//동굴
		caveBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP103));
		SelectObject(memDC, caveBit);
		TransparentBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 10, 0, 960, 800, RGB(0, 64, 128));
		DeleteObject(caveBit);

		//곰 움직임
		if (bear.isAttack == true) {
			bearAttack(bear.xPos, bear.yPos, backmemDC);
		}
		else if (bear.takeGarlic == true) {//마늘 먹은상태 
			if (bear.isDamaged == true) {//마늘먹고있는 상태에서 맞으면
				bearDamage(bear.xPos, bear.yPos, backmemDC);
				//마늘 뱉어야댐
				bear.takeGarlic = false;
			}
			else bearFeed(bear.xPos, bear.yPos + 15, backmemDC);//안맞으면 그냥 물고있는거 출력
		}
		else if (bear.isDamaged == true)//마늘 안먹은 상태에서는 그냥 맞기만해
			bearDamage(bear.xPos, bear.yPos, backmemDC);
		else if (bear.isDamaged == false)//아무것도 아닌 상태 - 기본애니메이션
			bearAnimation(bear.xPos, bear.yPos + 15, backmemDC);

		//호랑이 움직임
		if (tiger.isAttack == true) {//공격키 눌렀을때
			tigerAttack(tiger.xPos - 10, tiger.yPos + 15, backmemDC);
		}
		else if (tiger.takeGarlic == true) {//마늘 먹은상태 
			if (tiger.isDamaged == true) {//마늘먹고있는 상태에서 맞으면
				tigerDamage(tiger.xPos, tiger.yPos, backmemDC);
				//마늘 뱉어야댐
				tiger.takeGarlic = false;
			}
			else tigerFeed(tiger.xPos, tiger.yPos + 25, backmemDC);
		}
		else if (tiger.isDamaged == true)
			tigerDamage(tiger.xPos, tiger.yPos, backmemDC);
		else if (tiger.isDamaged == false)
			tigerAnimation(tiger.xPos, tiger.yPos + 25, backmemDC);

		////폰트~앞으로빼면 뭔진몰라도 안나오니 뒤로 빼자
		//font = CreateFont(80, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("궁서"));
		////폰트 이룸 중 띄어쓰기가 된 것은 적용안되는듯 멍청한 기계문명.... 인수가 14개이니까 주의하긔
		//oldfont = (HFONT)SelectObject(hdc, font);
		//SetBkMode(hdc, TRANSPARENT);//폰트배경 투명화
		//TextOut(hdc, 100, 100, str, strlen(str));
		//SelectObject(hdc, oldfont);
		//DeleteObject(font);

		//마늘인벤(마늘 쌓여있는것임)
		invenBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP98));
		oldBit = (HBITMAP)SelectObject(memDC, invenBit);
		//곰의 마늘인벤
		TransparentBlt(backmemDC, 0, HEIGHT - bear.addScore * 5 - 40, 400, 150, memDC, 0, 0, 547, 296, RGB(0, 64, 128));
		//호랑이의 마늘인벤
		TransparentBlt(backmemDC, WIDTH - 400, HEIGHT - tiger.addScore * 5 - 40, 300, 150, memDC, 0, 0, 547, 296, RGB(0, 64, 128));
		SelectObject(memDC, oldBit);
		DeleteObject(invenBit);

		//엔딩
		if (gameover == true) {//게임한지 5분! 게임 끝!
			KillTimer(hWnd, 0);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			endingBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP104));
			oldBit = (HBITMAP)SelectObject(memDC, endingBit);
			StretchBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, SRCCOPY);
			DeleteObject(endingBit);
		}
		else if (bear.win == true) {//승리조건 달성하면 모든 타이머 멈추고 엔딩 비트맵 띄움
			KillTimer(hWnd, 0);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			endingBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP99));
			oldBit = (HBITMAP)SelectObject(memDC, endingBit);
			StretchBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, SRCCOPY);
			DeleteObject(endingBit);
		}

		else if (tiger.win == true) {
			KillTimer(hWnd, 0);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			endingBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP100));
			oldBit = (HBITMAP)SelectObject(memDC, endingBit);
			StretchBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, SRCCOPY);
			DeleteObject(endingBit);
		}
		else;

		TransparentBlt(hdc, 0, 0, WIDTH, HEIGHT, backmemDC, 0, 0, 970, 800, RGB(0, 64, 128));
		DeleteObject(SelectObject(backmemDC, oldBit));

		//삭제
		DeleteDC(backmemDC);
		DeleteDC(memDC);
		EndPaint(hWnd, &ps);
		break;

	case WM_TIMER://0번-움직임 1번-마늘, 마차 생성 텀
		switch (wParam) {

		case 0://마늘과 마차 이동 타이머
			   //마늘 부분----------------------------
			if (garlicTime == true) {
				gararray[garlics.now].yPos += 50;
				if (gararray[garlics.now].yPos == 500) {//발 맞는 부분(체크 ㅇㅋ)
					garlics.now++; //인덱스 증가
					garlicTime = false;
					garlics.GarlicOntheGround++;//땅에있는 마늘개수에 1 추가
					SetTimer(hWnd, 1, 5000, NULL);
				}
			}
			////마차 부분-----------------------------
			if (carriage.ismachamoving == true) {
				if (carriage.machadir == 1) {//왼->오
					carriage.xPos += 10;
					if (carriage.xPos == 1020) {//끝에 닿으면
						carriage.ismachamoving = false;
						SetTimer(hWnd, 2, 10000, NULL);
					}
				}
				else if (carriage.machadir == 2) {//오->왼
					carriage.xPos -= 10;
					if (carriage.xPos == -200) {//xpos 좌표 위치가 왼쪽상단이라 다 지나가기전에 먼저사라져서 -값줌
						carriage.ismachamoving = false;
						SetTimer(hWnd, 2, 10000, NULL);
					}
				}
			}
			//킬타이머는 ㄴㄴ 마늘이랑 같이 쓰기때문에 죽이면 마늘도 안 뜸
			skyxPos++;
			if (skyxPos == WIDTH) {
				skyxPos = 0;//다시 처음부터 반복반복
			}
			//달해
			Cos = cosf(radian);
			z = tanf(radian) * Time - gravity / (2 * v_0 * v_0 * Cos * Cos) * Time * Time;
			Time += 0.8f;
			moonsunxPos += 1.25f;//커질수록 넓어짐

			if (moonsunyPos - z > 300) {
				if (v_0 <= 0) {
					v_0 = 0;
					z = 0;
				}
				moonsunyPos = 300;
				Time = 0.f;
				z = 0;
			}
			if (moonsunxPos > WIDTH-80) {//처음부터다시
				moonsunxPos = 0;
				Time = 0;
				v_0 = 140.f;
				z = 0;
				if (isDay == false) {//밤도 끝나면 게임오버
					gameover = true;
				}
				else if( isDay == true){
					isDay = false;//밤되는거임
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case 1://마늘쑥 좌표 잡는 타이머(5초 1회)
			gametime++;
			if (garlics.GarlicOntheGround == 5) {
				//gametime++;//마늘이 5개가 되면
				break;
			}
			else if (garlics.GarlicOntheGround < 5) {
				//gametime++;
				gararray[garlics.now].xPos = rand() % 765 + 135;
				gararray[garlics.now].yPos = 0;
				garlicTime = true;
			}
			if (gametime == 58)
				gameover = true;
			break;

		case 2://마차 등장 타이머
			
			carriage.machadir = rand() % 2 + 1;
			if (carriage.machadir == 1) {//왼->오
				carriage.xPos = 0;//출발좌표설정
				carriage.yPos = 450;
				carriage.ismachamoving = true;
				KillTimer(hWnd, 2);
			}
			else if (carriage.machadir == 2) {//오->왼
				carriage.xPos = 800;
				carriage.yPos = 450;
				carriage.ismachamoving = true;
				KillTimer(hWnd, 2);
			}
			
			break;
		}
		break;

	case WM_KEYDOWN:
		RECT rcTemp, bearRect, tigerRect, machaRect;
		bearRect = { bear.xPos, bear.yPos, bear.xPos + 85, bear.yPos + 65 };
		tigerRect = { tiger.xPos, tiger.yPos, tiger.xPos + 125, tiger.yPos + 70 };
		machaRect = { carriage.xPos,carriage.yPos,carriage.xPos + 200, carriage.yPos + 100 };
		//호랑이 움직임

		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {//마늘 저장
			if (tiger.takeGarlic == true && WIDTH - 50 - 125 < tiger.xPos && tiger.xPos < WIDTH) {//마늘 문채로 동굴 앞으로 가면
				tiger.takeGarlic = false;//마늘 사라졋
				tiger.addScore++;
				snd.Add_sound();
				snd.Play(5);
				if (tiger.addScore == 20)
					tiger.win = true;
			}
		}

		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			if (tiger.xPos > 25)//동굴은 85, 입구는 35
				tiger.xPos -= 10;

			for (int i = 0; i < 200; i++) {
				if ((1 >= gararray[i].xPos - (tiger.xPos + 125)) && (tiger.xPos - gararray[i].xPos <= 1)
					&& (tiger.takeGarlic == false) && (gararray[i].yPos == 500)) {//곰 x좌표와 n번째 마늘 x좌표가같다면
																				  //호랑이가 왼쪽으로 다가오면서 마늘과 가까워짐-> 호랑이 x좌-마늘 x좌<=1
					tiger.takeGarlic = true; // FEED비트맵 출력하기위
					gararray[i].xPos = -100;
					gararray[i].yPos = -100;
					garlics.GarlicOntheGround--;//지면에 있는 갈릭 개수 줄어드니까 
					tiger.dropbyattack = i;
				}
			}
			tiger.dir = 0;
		}

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			if (tiger.xPos < WIDTH - 25 - 125)
				tiger.xPos += 10;

			for (int i = 0; i < 200; i++) {
				if ((1 >= (tiger.xPos) - gararray[i].xPos) && (gararray[i].xPos - (tiger.xPos + 125) <= 1)
					&& (tiger.takeGarlic == false) && (gararray[i].yPos == 500)) {
					//호랑이의 (x+125)가 마늘 x와 가까워짐-->마늘x-호랑(x+125)<=1
					tiger.takeGarlic = true; //갈릭먹음
					gararray[i].xPos = -100;
					gararray[i].yPos = -100;
					garlics.GarlicOntheGround--;
					tiger.dropbyattack = i;
				}
			}
			tiger.dir = 1;
		}
		//곰 움직임

		if (GetAsyncKeyState('S') & 0x8000) {//마늘 저장
			if (bear.takeGarlic == true && 0 < bear.xPos && bear.xPos < 50) {//마늘 문 채로 본인 동굴 앞으로 가면
				bear.takeGarlic = false;//마늘 사라졋
				bear.addScore++;
				snd.Add_sound();
				snd.Play(5);
				if (bear.addScore == 20)
					bear.win = true;
			}
		}
		if (GetAsyncKeyState('A') & 0x8000) {

			if (bear.xPos > 25)
				bear.xPos -= 10;

			for (int i = 0; i < 200; i++) {
				if ((1 >= gararray[i].xPos - (bear.xPos + 85)) && (bear.xPos - gararray[i].xPos <= 1)
					&& (bear.takeGarlic == false) && (gararray[i].yPos == 500)) {//곰 x좌표와 n번째 마늘 x좌표가같다면
					bear.takeGarlic = true; // FEED비트맵 출력하기위해
					gararray[i].xPos = -100;
					gararray[i].yPos = -100;
					garlics.GarlicOntheGround--;//지면에 있는 갈릭 개수 줄어드니까 --
					bear.dropbyattack = i;
				}
			}
			bear.dir = 1;
		}
		if (GetAsyncKeyState('D') & 0x8000) {

			if (bear.xPos < WIDTH - 25 - 85)
				bear.xPos += 10;

			for (int i = 0; i < 200; i++) {
				if ((1 >= bear.xPos - gararray[i].xPos) && (gararray[i].xPos - (bear.xPos + 85) <= 1)
					&& (bear.takeGarlic == false) && (gararray[i].yPos == 500)) {

					//마늘 xpos는 랜덤인데도 불구하고 지면에 닿자마자 곰과 만나는 이유는??
					bear.takeGarlic = true; //갈릭먹음

					gararray[i].xPos = -100;
					gararray[i].yPos = -100;
					garlics.GarlicOntheGround--;
					bear.dropbyattack = i;
				}
			}
			bear.dir = 0;
		}
		break;

	case WM_KEYUP:
		//RECT rcTemp, bearRect, tigerRect, machaRect;
		bearRect = { bear.xPos, bear.yPos, bear.xPos + 85, bear.yPos + 65 };
		tigerRect = { tiger.xPos, tiger.yPos, tiger.xPos + 125, tiger.yPos + 70 };
		machaRect = { carriage.xPos,carriage.yPos,carriage.xPos + 200, carriage.yPos + 100 };

		//호랑이
		if (GetAsyncKeyState(VK_UP) & 0x0001) {
			if (tiger.takeGarlic == false) {//공격
				tiger.isAttack = true;
				snd.Add_sound();
				snd.Play(3);
			}

			if (IntersectRect(&rcTemp, &bearRect, &tigerRect) != 0) {
				if (tiger.takeGarlic == false) {//충돌하면
					bear.isDamaged = true;//곰 피해액션
					snd.Add_sound();
					snd.Play(2);
				}
				if (bear.takeGarlic == true) {//비트맵 충돌이고 곰이 마늘 가지고 있을 때만 때리면 마늘 나옴
					if (tiger.dir == 0) {//왼쪽보기:호랑이 뒤가 오른쪽
						gararray[bear.dropbyattack].xPos = tiger.xPos + 20;
						gararray[bear.dropbyattack].yPos = 500;
						garlics.GarlicOntheGround++;
					}
					if (tiger.dir == 1) {
						gararray[bear.dropbyattack].xPos = tiger.xPos - 20;
						gararray[bear.dropbyattack].yPos = 500;
						garlics.GarlicOntheGround++;
					}

				}
			}

			if ((IntersectRect(&rcTemp, &machaRect, &tigerRect) != 0) && (tiger.takeGarlic == false)) {
				//호랭이가 마늘이 없는 상태에서 마차를 때렸다!
				carriage.machaAttackedTime++;//마차맞은횟수 1 증가
											 //carriage.yPos = 440;
				carriage.machaani = true;
				carriage.yPos = 455;
				if (carriage.machaAttackedTime == 15) {
					//마차가 한도까지 맞았다면
					gararray[carriage.dropbyMachaattack].xPos = carriage.xPos;
					gararray[carriage.dropbyMachaattack].yPos = 500;
					carriage.machaAttackedTime = 0;//공격받은횟수 다시초기화
					carriage.dropbyMachaattack++;
					garlics.GarlicOntheGround++;
				}
			}

		}
		//곰
		if (GetAsyncKeyState('W') & 0x0001) {
			if (bear.takeGarlic == false) {//공격
				bear.isAttack = true;
				snd.Add_sound();
				snd.Play(3);
			}
			if (IntersectRect(&rcTemp, &bearRect, &tigerRect) != 0) {//충돌하면
				if (bear.takeGarlic == false) {
					tiger.isDamaged = true;
					snd.Add_sound();
					snd.Play(2);
				}
				if (tiger.takeGarlic == true) {
					if (bear.dir == 0) {//오른쪽보기:곰 뒤가 왼쪽
						gararray[tiger.dropbyattack].xPos = bear.xPos - 20;
						gararray[tiger.dropbyattack].yPos = 500;
						garlics.GarlicOntheGround++;
					}
					if (bear.dir == 1) {
						gararray[tiger.dropbyattack].xPos = bear.xPos + 20;
						gararray[tiger.dropbyattack].yPos = 500;
						garlics.GarlicOntheGround++;
					}
				}
			}

			if ((IntersectRect(&rcTemp, &machaRect, &bearRect) != 0) && (bear.takeGarlic == false)) {//곰이 마차를 떄렸다!
				carriage.machaAttackedTime++;
				carriage.machaani = true;
				carriage.yPos = 455;
				if (carriage.machaAttackedTime == 15) {
					gararray[carriage.dropbyMachaattack].xPos = carriage.xPos;
					gararray[carriage.dropbyMachaattack].yPos = 500;
					carriage.machaAttackedTime = 0;
					carriage.dropbyMachaattack++;
					garlics.GarlicOntheGround++;
				}

			}

		}
		break;

	case WM_CHAR:

		if ((wParam == 'q') || (wParam == 'Q')) {
			PostQuitMessage(0);
		}
		if ((wParam == 'p') || (wParam == 'P')) {
			snd.pSound[0]->release();
		}
		break;

	case WM_DESTROY:
		/*end = clock();*/
		KillTimer(hWnd, 0);
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
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