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
#include "Sound.h"
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

//---구조체
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

Garlic gararray[100];//배열 구조체
Garlic garlics;//배열 ㄴㄴ인거
static bool garlicTime;//마늘이 내려올 시간...

struct macha {
	int xPos, yPos;
	int machacount;
	int machadir;
	int machaAttackedTime;//캐릭터에 의해 마차가 공격당한 횟수
	int dropbyMachaattack;//마차공격해서 떨궈지는 마늘 인덱스(80-100까지로함)
						  //즉 전체 마늘 (100)개 중 0-79까지는 하늘드랍&&캐릭터공격으로 획득가능한 마늘, 
						  //80-100까지는 마차에 의해 떨어지는 마늘 인덱스
	HBITMAP MachaAniBitmap[6];
	bool ismachamoving;//마차 비트맵 띄우는 거 bool
	bool machaani;//마차 맞을떄 위아래로 움직이는 애니
};

macha carriage;

struct gameset {
	int time;
	int musicpausetime;
	int keytoggle;
	bool start;
	bool pause;
	bool over;
	bool howto;
	bool setting;

};
gameset game;//전체 게임 컨트롤 구조체

			 //전역변수

float moonsunxPos = 10.f;
float moonsunyPos = 300.f;
float v_0 = 140.f;                  // 던지는 힘
float angle = 60.f;                  // 던지는 각도
float gravity = 30.f;               // 중력
float radian = (float)(angle * PI / 180);      // 각도를 호도법으로 변환
float Time = 0.f;
float z = 0.f;
float Cos;
int skyxPos;
bool isDay;
float fvolume;//사운드 볼륨 조절

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
	for (int i = 0; i < 2; i++)
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
	for (int i = 0; i < 2; i++)
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
		carriage.yPos = 435;
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

	for (int i = 80; i < carriage.dropbyMachaattack; i++) {//마차 공격해서 떨어진 마늘 표시 부분
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
	}

	for (int i = 0; i < 100; i++) {
		DeleteObject(gararray[i].garbit);
	}
	DeleteDC(memDC);
}

void gameReset(HWND hWnd) {
	KillTimer(hWnd, 0);
	KillTimer(hWnd, 1);
	KillTimer(hWnd, 2);

	moonsunxPos = 10.f;
	moonsunyPos = 300.f;
	//float diameter = 25.f;               // 원의 지름
	v_0 = 140.f;                  // 던지는 힘
	angle = 60.f;                  // 던지는 각도
	gravity = 30.f;               // 중력
	radian = (float)(angle * PI / 180);      // 각도를 호도법으로 변환
	Time = 0.f;
	z = 0.f;
	Cos;
	isDay = true;
	skyxPos = 0;
	//캐릭터
	bear.xPos = 85;
	bear.yPos = HEIGHT - 250 - 65;
	tiger.xPos = WIDTH - 85 - 125;
	tiger.yPos = HEIGHT - 250 - 70 - 5;
	bear.dir = 0;
	tiger.dir = 0;
	bear.takeGarlic = false;
	tiger.takeGarlic = false;
	bear.dropbyattack = 0;
	tiger.dropbyattack = 0;
	bear.addScore = 0;
	tiger.addScore = 0;
	//마늘,마차
	garlics.now = 0;
	garlicTime = false;
	garlics.GarlicOntheGround = 0;//땅에있는 마늘 개수 검사 
	carriage.ismachamoving = false;
	carriage.machaAttackedTime = 0;
	carriage.dropbyMachaattack = 80;
	carriage.machaani = false;
	for (int i = 0; i < 100; i++) {
		gararray[i].xPos = -100;
		gararray[i].yPos = -100;
	} //마늘 좌표 초기화
	srand((unsigned)time(NULL));
	SetTimer(hWnd, 0, 200, NULL);
	SetTimer(hWnd, 1, 5000, NULL);
	SetTimer(hWnd, 2, 10000, NULL);
	snd.pSound[0]->release();
	snd.Add_sound();
	snd.Play(0);
	game.pause = false;
	game.musicpausetime = 0;
	game.keytoggle = 0;
	fvolume = 1;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc, memDC, backmemDC;
	PAINTSTRUCT ps;
	HBITMAP hBit, oldBit, backBit, invenBit, endingBit, caveBit, skyBit, moonsunBit, titleBit, pauseBit;//inven : 마늘 쌓여있는 비트맵, ending: 엔딩비트맵
	CHAR str[] = "동물이라도 인간이하고싶어";

	static double timer;
	//static int keytoggle = 0;
	static int mousex, mousey;
	RECT rcTemp, bearRect, tigerRect, machaRect;
	bearRect = { bear.xPos, bear.yPos, bear.xPos + 85, bear.yPos + 65 };
	tigerRect = { tiger.xPos, tiger.yPos, tiger.xPos + 125, tiger.yPos + 70 };
	machaRect = { carriage.xPos,carriage.yPos,carriage.xPos + 200, carriage.yPos + 150 };

	//--------------------방향키 입력 시작------------//

	//곰공격
	if (bear.takeGarlic == false &&((GetAsyncKeyState('W') & 0x8000) != 0)) {
	
		bear.isAttack = true;
		snd.Add_sound();
		snd.Play(3);
		
		if (IntersectRect(&rcTemp, &bearRect, &tigerRect) != 0) {//호랑이랑 충돌하면(공격성공)
			if (tiger.takeGarlic == true) {//마늘 들고 있으면 마늘 떨굼
				if (bear.dir == 0) {//오른쪽보기:곰 뒤가 왼쪽
					gararray[tiger.dropbyattack].xPos = bear.xPos - 20;
					gararray[tiger.dropbyattack].yPos = 500;
					garlics.GarlicOntheGround++;
				}
				if (bear.dir == 1) {
					gararray[tiger.dropbyattack].xPos = bear.xPos + 100;
					gararray[tiger.dropbyattack].yPos = 500;
					garlics.GarlicOntheGround++;
				}
			}
			tiger.isDamaged = true;
			snd.Play(2);
		}
		if (IntersectRect(&rcTemp, &machaRect, &bearRect) != 0) {//곰이 마차를 떄렸다!
			snd.Play(2);
			carriage.machaAttackedTime++;
			carriage.machaani = true;
			carriage.yPos = 425;
			if (carriage.machaAttackedTime == 10) {
				gararray[carriage.dropbyMachaattack].xPos = carriage.xPos;
				gararray[carriage.dropbyMachaattack].yPos = 500;
				carriage.machaAttackedTime = 0;
				carriage.dropbyMachaattack++;
				garlics.GarlicOntheGround++;
			}
		}
	}
	//호랑이 공격
	if (tiger.takeGarlic == false && (GetAsyncKeyState(VK_UP) & 0x8000) != 0) {
		
		tiger.isAttack = true;
		snd.Add_sound();
		snd.Play(3);
		
		if (IntersectRect(&rcTemp, &bearRect, &tigerRect) != 0) {//공격성공
			if (bear.takeGarlic == true) {//비트맵 충돌이고 곰이 마늘 가지고 있을 때만 때리면 마늘 나옴
				if (tiger.dir == 0) {//왼쪽보기:호랑이 뒤가 오른쪽
					gararray[bear.dropbyattack].xPos = tiger.xPos + 130;
					gararray[bear.dropbyattack].yPos = 500;
					garlics.GarlicOntheGround++;
				}
				if (tiger.dir == 1) {
					gararray[bear.dropbyattack].xPos = tiger.xPos - 40;
					gararray[bear.dropbyattack].yPos = 500;
					garlics.GarlicOntheGround++;
				}
			}
			bear.isDamaged = true;//곰 피해액션
			snd.Play(2);
		}
		if ((IntersectRect(&rcTemp, &machaRect, &tigerRect) != 0) && (tiger.takeGarlic == false)) {
			//호랭이가 마늘이 없는 상태에서 마차를 때렸다!
			snd.Play(2);
			carriage.machaAttackedTime++;//마차맞은횟수 1 증가
			carriage.machaani = true;
			carriage.yPos = 425;
			if (carriage.machaAttackedTime == 10) {
				//마차가 한도까지 맞았다면
				gararray[carriage.dropbyMachaattack].xPos = carriage.xPos;
				gararray[carriage.dropbyMachaattack].yPos = 500;
				carriage.machaAttackedTime = 0;//공격받은횟수 다시초기화
				carriage.dropbyMachaattack++;
				garlics.GarlicOntheGround++;
			}
		}




	}
	
	//곰 움직임
	if (((GetAsyncKeyState('A') & 0x8000) != 0) || ((GetAsyncKeyState('A') & 1) == 1)) {
		if (bear.xPos > 25)
			bear.xPos -= 5;
		bear.dir = 1;
	}
	if (((GetAsyncKeyState('D') & 0x8000) != 0) || ((GetAsyncKeyState('D') & 1) == 1)) {
		if (bear.xPos < WIDTH - 25 - 85)
			bear.xPos += 5;
		bear.dir = 0;
	}
	//호랑이 움직임
	if (((GetAsyncKeyState(VK_LEFT) & 0x8000) != 0) || ((GetAsyncKeyState(VK_LEFT) & 1) == 1)) {//눌림 || 눌려져있었음
		if (tiger.xPos > 25)//동굴은 85, 입구는 35
			tiger.xPos -= 5;
		tiger.dir = 0;
	}
	if (((GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RIGHT) & 1) == 1)) {
		if (tiger.xPos < WIDTH - 25 - 125)
			tiger.xPos += 5;
		tiger.dir = 1;
	}

	//--------------------방향키 입력 끝------------//

	//메세지 큐
	switch (message)
	{
	case WM_CREATE:
		isDay = true; //처음은 낮으로 시작
		skyxPos = 0;
		game.start = false;
		game.pause = false;
		game.over = false;
		game.howto = false;
		game.keytoggle = 0;
		game.musicpausetime = 0;
		//캐릭터
		bear.xPos = 85;
		bear.yPos = HEIGHT - 250 - 65;
		tiger.xPos = WIDTH - 85 - 125;
		tiger.yPos = HEIGHT - 250 - 70 - 5;
		bear.dir = 0;
		tiger.dir = 0;
		bear.takeGarlic = false;
		tiger.takeGarlic = false;

		bear.dropbyattack = 0;
		tiger.dropbyattack = 0;
		//마늘,마차
		garlics.now = 0;
		garlicTime = false;
		garlics.GarlicOntheGround = 0;//땅에있는 마늘 개수 검사 
		carriage.ismachamoving = false;
		carriage.machaAttackedTime = 0;
		carriage.dropbyMachaattack = 80;
		carriage.machaani = false;
		for (int i = 0; i < 100; i++) {
			gararray[i].xPos = -100;
			gararray[i].yPos = -100;
		} //마늘 좌표 초기화해줫읍니다 null은 0,0이라 짱나서 마이너스로 바꿈.
		fvolume = 1;
		srand((unsigned)time(NULL));
		snd.Add_sound();
		snd.Play(1);
	
		break;

	case WM_PAINT://배경, 스프라이트, 동굴에 쌓이는거(키다운에서 카운트하고 invalidrect하면 count개수만큼 비트맵 그림)
		hdc = BeginPaint(hWnd, &ps);
		memDC = CreateCompatibleDC(hdc);
		backmemDC = CreateCompatibleDC(hdc);
		backBit = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);//도화지
		oldBit = (HBITMAP)SelectObject(backmemDC, backBit);

		//--------타이틀화면
		if (game.start == false) {
			titleBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP108));
			SelectObject(memDC, titleBit);
			TransparentBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, RGB(0, 64, 128));
			DeleteObject(titleBit);

			if (game.howto == true) {
				hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP109));
				SelectObject(memDC, hBit);
				BitBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, SRCCOPY);
				DeleteObject(hBit);
			}
		}
		//--------인게임
		else if (game.start == true) {
			//비트맵 순서: 배경(하늘 땅),마차,동굴,마늘이랑 캐릭터들
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
			TransparentBlt(backmemDC, (int)moonsunxPos, (int)(moonsunyPos - z), 101, 102, memDC, 0, 0, 101, 102, RGB(0, 64, 128));
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

			for (int i = 80; i < carriage.dropbyMachaattack; i++) {//마차 공격해서 떨어진 마늘 표시 부분
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

			//마늘인벤(마늘 쌓여있는것임)
			invenBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP98));
			oldBit = (HBITMAP)SelectObject(memDC, invenBit);
			//곰의 마늘인벤
			TransparentBlt(backmemDC, 50, HEIGHT - bear.addScore * 5 - 40, 300, 150, memDC, 0, 0, 547, 296, RGB(0, 64, 128));
			//호랑이의 마늘인벤
			TransparentBlt(backmemDC, WIDTH - 350, HEIGHT - tiger.addScore * 5 - 40, 300, 150, memDC, 0, 0, 547, 296, RGB(0, 64, 128));
			SelectObject(memDC, oldBit);
			DeleteObject(invenBit);

			if (game.pause == true) {//일시정지화면

				pauseBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP107));
				SelectObject(memDC, pauseBit);
				TransparentBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, RGB(0, 64, 128));
				DeleteObject(pauseBit);
			}
		}
		//엔딩
		if (game.over == true) {//게임한지 5분! 게임 끝!
			snd.pSound[0]->release();
			snd.pChannel[0]->stop();
			KillTimer(hWnd, 0);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			snd.Play(6);
			endingBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP104));
			oldBit = (HBITMAP)SelectObject(memDC, endingBit);
			StretchBlt(backmemDC, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 800, SRCCOPY);
			DeleteObject(endingBit);
			DeleteObject(oldBit);
		}

		else if (bear.win == true) {//승리조건 달성하면 모든 타이머 멈추고 엔딩 비트맵 띄움
			snd.pSound[0]->release();
			snd.pChannel[0]->stop();
			KillTimer(hWnd, 0);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			snd.Play(4);
			endingBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP99));
			oldBit = (HBITMAP)SelectObject(memDC, endingBit);
			TransparentBlt(backmemDC, 0, 0, 970, 800, memDC, 0, 0, 970, 800, RGB(0, 64, 128));
			DeleteObject(endingBit);
			DeleteObject(oldBit);

		}
		else if (tiger.win == true) {
			snd.pSound[0]->release();
			snd.pChannel[0]->stop();
			KillTimer(hWnd, 0);
			KillTimer(hWnd, 1);
			KillTimer(hWnd, 2);
			snd.Play(4);
			endingBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP100));
			oldBit = (HBITMAP)SelectObject(memDC, endingBit);
			TransparentBlt(backmemDC, 0, 0, 970, 800, memDC, 0, 0, 970, 800, RGB(0, 64, 128));
			DeleteObject(endingBit);
			DeleteObject(oldBit);

		}
		else;
		//hdc에 그리기
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
					if (carriage.xPos == 1020) {//끝에 닿으면'
						carriage.machaAttackedTime = 0;
						carriage.ismachamoving = false;
						SetTimer(hWnd, 2, 10000, NULL);
					}
				}
				else if (carriage.machadir == 2) {//오->왼
					carriage.xPos -= 10;
					if (carriage.xPos == -200) {//xpos 좌표 위치가 왼쪽상단이라 다 지나가기전에 먼저사라져서 -값줌
						carriage.machaAttackedTime = 0;
						carriage.ismachamoving = false;
						SetTimer(hWnd, 2, 10000, NULL);
					}
				}
			}
			//하늘
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
			if (moonsunxPos > WIDTH - 80) {//처음부터다시
				moonsunxPos = 0;
				Time = 0;
				v_0 = 140.f;
				z = 0;
				if (isDay == false) {//밤도 끝나면 게임오버
					game.over = true;
				}
				else if (isDay == true) {
					isDay = false;//밤되는거임
				}
			}
			//마늘과 캐릭터가 닿았는지 검사
			for (int i = 0; i < 100; i++) {
				
				if ((tiger.takeGarlic == false) && (gararray[i].yPos == 500)) {
					if ((1 >= gararray[i].xPos - (tiger.xPos + 125)) && (tiger.xPos - gararray[i].xPos <= 1)) {//VK_left
						tiger.takeGarlic = true; // FEED비트맵 출력하기위
						gararray[i].xPos = -100;
						gararray[i].yPos = -100;
						garlics.GarlicOntheGround--;//지면에 있는 갈릭 개수 줄어드니까 
						tiger.dropbyattack = i;
					}
					else if ((1 >= (tiger.xPos) - gararray[i].xPos) && (gararray[i].xPos - (tiger.xPos + 125) <= 1)) {//VK_right
						//호랑이의 (x+125)가 마늘 x와 가까워짐-->마늘x-호랑(x+125)<=1
						tiger.takeGarlic = true; //갈릭먹음
						gararray[i].xPos = -100;
						gararray[i].yPos = -100;
						garlics.GarlicOntheGround--;
						tiger.dropbyattack = i;
					}
					
				}

				if ((bear.takeGarlic == false) && (gararray[i].yPos == 500)) {
					if ((1 >= gararray[i].xPos - (bear.xPos + 85)) && (bear.xPos - gararray[i].xPos <= 1)) {//a
						bear.takeGarlic = true; // FEED비트맵 출력하기위해
						gararray[i].xPos = -100;
						gararray[i].yPos = -100;
						garlics.GarlicOntheGround--;//지면에 있는 갈릭 개수 줄어드니까 --
						bear.dropbyattack = i;
					}
					else if ((1 >= bear.xPos - gararray[i].xPos) && (gararray[i].xPos - (bear.xPos + 85) <= 1)) {//d
						bear.takeGarlic = true; //갈릭먹음
						gararray[i].xPos = -100;
						gararray[i].yPos = -100;
						garlics.GarlicOntheGround--;
						bear.dropbyattack = i;
					}
				}

				if (bear.isDamaged == true) {//날아가며 뒤로 물러나는 도중에 범위 내에 있으면

					if ((bear.xPos + 85) - gararray[i].xPos >= 1) {
						bear.takeGarlic = true;//마늘 없어지고 먹어야함
					}
					else if (bear.xPos - (gararray[i].xPos + 50) >= 1) {
						bear.takeGarlic = true;
					}
				}
				if (tiger.isDamaged == true) {

					if ((tiger.xPos+125)- gararray[i].xPos>=1) {
						tiger.takeGarlic = true;
					}
					else if (tiger.xPos-(gararray[i].xPos+50)>=1) {
					    tiger.takeGarlic = true;
						
					}
				}
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case 1://마늘쑥 좌표 잡는 타이머(5초 1회)
			   //gametime++;
			if (garlics.GarlicOntheGround == 5) {
				//gametime++;//마늘이 5개가 되면
				break;
			}
			else if (garlics.GarlicOntheGround < 5) {
				//gametime++;
				gararray[garlics.now].xPos = rand() % 700 + 135;
				gararray[garlics.now].yPos = 0;
				garlicTime = true;
			}
			break;

		case 2://마차 등장 타이머
			carriage.machadir = rand() % 2 + 1;
			if (carriage.machadir == 1) {//왼->오
				carriage.xPos = 0;//출발좌표설정
				carriage.yPos = 420;
				carriage.ismachamoving = true;
				KillTimer(hWnd, 2);
			}
			else if (carriage.machadir == 2) {//오->왼
				carriage.xPos = 800;
				carriage.yPos = 420;
				carriage.ismachamoving = true;
				KillTimer(hWnd, 2);
			}
			break;
		}
		break;

	case WM_KEYDOWN:
		//곰 저장
		if ((wParam == 's' || wParam == 'S')) {
			if (bear.takeGarlic == true && 0 < bear.xPos && bear.xPos < 50) {//마늘 문 채로 본인 동굴 앞으로 가면
				bear.takeGarlic = false;//마늘 사라졋
				bear.addScore++;
				snd.Play(5);
				if (bear.addScore == 20)
					bear.win = true;
			}
		}
		//if (GetAsyncKeyState('S') & 0x8001) {//마늘 저장
		//	if (bear.takeGarlic == true && 0 < bear.xPos && bear.xPos < 50) {//마늘 문 채로 본인 동굴 앞으로 가면
		//		bear.takeGarlic = false;//마늘 사라졋
		//		bear.addScore++;
		//		snd.Play(5);
		//		if (bear.addScore == 20)
		//			bear.win = true;
		//	}
		//}	
		//호랑이 저장
		if (wParam == VK_DOWN) {
			if (tiger.takeGarlic == true && WIDTH - 50 - 125 < tiger.xPos && tiger.xPos < WIDTH) {//마늘 문채로 동굴 앞으로 가면
				tiger.takeGarlic = false;//마늘 사라졋
				tiger.addScore++;
				snd.Play(5);
				if (tiger.addScore == 20)
					tiger.win = true;
			}
		}
		//if (GetAsyncKeyState(VK_DOWN) & 0x8001) {//마늘 저장
		//	if (tiger.takeGarlic == true && WIDTH - 50 - 125 < tiger.xPos && tiger.xPos < WIDTH) {//마늘 문채로 동굴 앞으로 가면
		//		tiger.takeGarlic = false;//마늘 사라졋
		//		tiger.addScore++;
		//		snd.Play(5);
		//		if (tiger.addScore == 20)
		//			tiger.win = true;
		//	}
		//}
		else if (wParam == VK_ESCAPE) {//esc
				if (game.start == true) {
					bool paused;
					snd.pChannel[0]->getPaused(&paused);
					snd.pChannel[0]->setPaused(!paused);

					if (game.keytoggle % 2 == 0) {
						game.pause = true;
						KillTimer(hWnd, 0);
						KillTimer(hWnd, 1);
						KillTimer(hWnd, 2);
						InvalidateRect(hWnd, NULL, FALSE);

					}
					else if (game.keytoggle % 2 == 1) {
						game.pause = false;
						SetTimer(hWnd, 0, 200, NULL);
						SetTimer(hWnd, 1, 5000, NULL);
						SetTimer(hWnd, 2, 10000, NULL);
					}
					game.keytoggle++;

				}

			}
		else if (wParam == VK_SPACE) {
			if (game.over == true || bear.win == true || tiger.win == true) {//게임 끝났을때 스페이스바 누르면 메인으로.
				game.pause = false;
				game.start = false;
				game.over = false;
				bear.win = false;
				tiger.win = false;
				snd.pSound[0]->release();
				snd.pSound[4]->release();
				snd.pSound[6]->release();
				snd.pChannel[0]->stop();
				snd.pChannel[4]->stop();
				snd.pChannel[6]->stop();
				snd.Play(1);
				//gameReset(hWnd);
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else;
		}
		//---------------메뉴선택 RECT------------------------------------------------

		break;

		case WM_KEYUP:
			bearRect = { bear.xPos, bear.yPos, bear.xPos + 85, bear.yPos + 65 };
			tigerRect = { tiger.xPos, tiger.yPos, tiger.xPos + 125, tiger.yPos + 70 };
			machaRect = { carriage.xPos,carriage.yPos,carriage.xPos + 200, carriage.yPos + 150 };
			
			break;
	case WM_CHAR:
		if ((wParam == 'q') || (wParam == 'Q')) {
			PostQuitMessage(0);
		}
		//엔딩 단축키
		if (game.start == true) {
			if (wParam == '1') {
				bear.win = true;
			}
			if (wParam == '2') {
				tiger.win = true;
			}
			if (wParam == '3') {
				game.over = true;
			}
		}
		break;

	case WM_LBUTTONDOWN:
		if (game.start == false) {

			mousex = LOWORD(lParam);
			mousey = HIWORD(lParam);
			//36-170, 25-110
			if ((mousex < 365) && (mousex > 114) && (mousey < 390) && (mousey > 319)) {  //스타트
				game.start = true;
				snd.pSound[1]->release();
				snd.pChannel[1]->stop();
				snd.Play(0);
				SetTimer(hWnd, 0, 200, NULL);
				SetTimer(hWnd, 1, 5000, NULL);
				SetTimer(hWnd, 2, 10000, NULL);
				gameReset(hWnd);
			}
			else if ((mousex < 365) && (mousex > 114) && (mousey < 495) && (mousey > 415)) {
				game.howto = true;
				
			}
			else if ((mousex < 365) && (mousex > 114) && (mousey < 586) && (mousey > 512)) {
				PostQuitMessage(0); // 나가기 
			}

			else if ((game.howto == true) && (mousex < 170) && (mousex > 36) && (mousey < 110) && (mousey > 25))
				game.howto = false;

		}
		//-------------------------ingame--------------------
		else if ((game.start == true) && (game.pause == true)) {//인게임에서 esc
			mousex = LOWORD(lParam);
			mousey = HIWORD(lParam);

			if ((mousex < 425) && (mousex > 261) && (mousey < 413) && (mousey > 249)) {
				//음악 on
				snd.pChannel[0]->setVolume(fvolume);
			}
			else if ((mousex < 710) && (mousex > 547) && (mousey < 413) && (mousey > 249)) {
				//음악오프
				snd.pChannel[0]->getVolume(&fvolume);
				snd.pChannel[0]->setVolume(fvolume - 1.0f);
			}
			else if ((mousex < 470) && (mousex > 190) && (mousey < 550) && (mousey > 460)) {
				gameReset(hWnd);
			}

			else if ((mousex < 775) && (mousex > 500) && (mousey < 550) && (mousey > 460)) {
				game.pause = false;
				game.start = false;
				snd.pSound[0]->release();
				snd.Play(1);
				gameReset(hWnd);
			}
		}
		else
			break;
		InvalidateRect(hWnd, NULL, FALSE);
		break;


	case WM_DESTROY:
		snd.pSystem->release();
		snd.pSystem->close();
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