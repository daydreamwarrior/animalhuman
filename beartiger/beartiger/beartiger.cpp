// beartiger.cpp: 응용 프로그램의 진입점을 정의합니다.
//
#pragma comment(lib, "msimg32.lib")
#include "stdafx.h"
#include "beartiger.h"
#include "Resource.h"
#include <atlimage.h>
#include <tchar.h>
#include <time.h>
//----------------fmod---------------
//#include <fmod.h>
#include "Sound.h"

//FMOD_SYSTEM* soundSystem;
//FMOD_SOUND *soundFile;  // 사운드 선언
//FMOD_CHANNEL *channel;
//FMOD_RESULT result;
CSound snd;

#define MAX_LOADSTRING 100
#define WIDTH 1024
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

struct Animal {
	int xPos, yPos;//좌표
	int movingCount, attackCount, damageCount;//비트맵애니메이션 카운트
	HBITMAP Walk[6], Attack[6], Feed[6], Damage[2];//비트맵애니메이션 _ 비트맵배열
	bool isAttack;//공격키가 눌렸는지 검사
	bool takeGarlic;//마늘 먹은 상태인지 검사
	int dir;//방향(원래방향=0, 반대방향=1)
};
Animal bear;
Animal tiger;

struct Garlic {
	int xPos, yPos;//좌표
	int now;//갈릭쑥 드랍 인덱스(0-49) 총 50개
	HBITMAP garbit, oldgarbit;//갈릭넣는비트맵
	bool show = TRUE;
};

Garlic gararray[50];//배열 구조체
Garlic garlics;//배열 ㄴㄴ인거
static bool garlicTime;//마늘이 내려올 시간...

struct macha {
	int xPos, yPos;
	int machacount;
	int machadir;
	HBITMAP MachaAniBitmap[6];
	bool ismachamoving;//마차 비트맵 띄우는 거 bool
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
	HBITMAP oldBit;

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
	HBITMAP oldBit;

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
	HBITMAP oldBit;

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
	HBITMAP oldBit;

	
	if (bear.dir == 0) {
		bear.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP90));
		bear.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP91));
		if (bear.xPos > 85)
			bear.xPos -= 20;
		
	}
	else if (bear.dir == 1) {
		bear.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP92));
		bear.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP93));
		if (bear.xPos < WIDTH-85-70)
			bear.xPos += 20;
	}
	memDC = CreateCompatibleDC(hdc);

	(HBITMAP)SelectObject(memDC, bear.Damage[bear.damageCount]);
	TransparentBlt(hdc, xPos, yPos, 65, 60, memDC, 0, 0, 65, 60, RGB(0, 64, 128));

	bear.damageCount++;
	if (bear.damageCount == 2) {
		bear.damageCount = 0;
		bear.isAttack = false;
	}
	for (int i = 0; i < 1; i++)
		DeleteObject(bear.Damage[i]);
	DeleteDC(memDC);
}

void tigerAnimation(int xPos, int yPos, HDC hdc) {

	HDC memDC;
	HBITMAP oldBit;

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
	HBITMAP oldBit;

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
	HBITMAP oldBit;

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
	HBITMAP oldBit;

	if (tiger.dir == 0) {
		tiger.Damage[0] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP96));
		tiger.Damage[1] = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP97));
		if (tiger.xPos < WIDTH-85-96)
			tiger.xPos += 20;

	}
	else if (tiger.dir == 1) {
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
		tiger.isAttack = false;
	}
	for (int i = 0; i < 1; i++)
		DeleteObject(tiger.Damage[i]);
	DeleteDC(memDC);
}

void machaAnimation(int xPos, int yPos, HDC hdc) {
	HDC memDC;
	HBITMAP oldBit;

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

	carriage.machacount++;
	carriage.machacount = carriage.machacount % 6;
	for (int i = 0; i < 6; i++)
		DeleteObject(carriage.MachaAniBitmap[i]);
	DeleteDC(memDC);
}

//-----------------------------fmod 함수-----------------------
//FMOD_RESULT FMOD_System_Create(FMOD_SYSTEM **system);
//FMOD_RESULT FMOD_System_Init(FMOD_SYSTEM *system, int MaxChannels, FMOD_INITFLAGS flags, void *ExtraDriverdata);
//FMOD_RESULT FMOD_System_createSound(FMOD_SYSTEM *system, const char *name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exinfo, FMOD_SOUND **sound);
//FMOD_RESULT FMOD_System_PlaySound(FMOD_SYSTEM *system, FMOD_SOUND *SoundFile, FMOD_CHANNELGROUP *ChannelGroup, FMOD_BOOL paused, FMOD_CHANNEL **Channel);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc, memDC;
	PAINTSTRUCT ps;
	HBITMAP hBit, oldBit;
	CHAR str[] = "동물이라도 인간이하고싶어";
	LOGFONT lf;
	HFONT font, oldfont;
	
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
		bear.xPos = 85;
		bear.yPos = HEIGHT - 250 - 65;
		tiger.xPos = WIDTH - 85 - 125;
		tiger.yPos = HEIGHT - 250 - 70 - 5;
		bear.dir = 0;
		tiger.dir = 0;
		bear.takeGarlic = false;
		tiger.takeGarlic = false;
		garlics.now = 0;
		garlicTime = false;
		carriage.ismachamoving = false;
		srand((unsigned)time(NULL));
		SetTimer(hWnd, 0, 200, NULL);
		SetTimer(hWnd, 1, 5000, NULL);
	    SetTimer(hWnd, 2, 7000, NULL);

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
		//배경
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP7));
		oldBit = (HBITMAP)SelectObject(memDC, hBit);
		TransparentBlt(hdc, 0, 0, WIDTH, HEIGHT, memDC, 0, 0, 970, 970, RGB(39, 87, 207));

		if (carriage.ismachamoving == true) {
			machaAnimation(carriage.xPos, carriage.yPos, hdc);
		}
		if (garlics.show) {//원래 쌓여있는 마늘 출력
			for (int i = 0; i < garlics.now; i++) {
				gararray[i].garbit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP23));
				garlics.oldgarbit = (HBITMAP)SelectObject(memDC, gararray[i].garbit);
				TransparentBlt(hdc, gararray[i].xPos, gararray[i].yPos, 50, 50, memDC, 0, 0, 50, 50, RGB(0, 64, 128));
				SelectObject(memDC, garlics.oldgarbit);
			}
		}
		if (garlicTime == true) {//새 마늘 출력
			gararray[garlics.now].garbit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP23));
			garlics.oldgarbit = (HBITMAP)SelectObject(memDC, gararray[garlics.now].garbit);
			TransparentBlt(hdc, gararray[garlics.now].xPos, gararray[garlics.now].yPos, 50, 50, memDC, 0, 0, 50, 50, RGB(0, 64, 128));
			SelectObject(memDC, garlics.oldgarbit);
			DeleteObject(gararray[garlics.now].garbit);
		}

		//비트맵 출력 순서 고정(마차와 마늘쑥 호랑이 곰보다 먼저)
		 
		//곰 움직임
		if (bear.isAttack == true) {
			//bearDamage(bear.xPos, bear.yPos, hdc);//맞는애니메이션 확인할라구
			bearAttack(bear.xPos, bear.yPos, hdc);
		}
		else if (bear.takeGarlic == true)//마늘 먹은상태 
			bearFeed(bear.xPos, bear.yPos + 15, hdc);
		else if (bear.takeGarlic == false)//아무것도 아닌 상태 - 기본애니메이션
			bearAnimation(bear.xPos, bear.yPos + 15, hdc);

		//호랑이 움직임
		if (tiger.isAttack == true) {
			//tigerDamage(tiger.xPos, tiger.yPos, hdc);
			tigerAttack(tiger.xPos - 10, tiger.yPos + 15, hdc);
		}
		else if (tiger.takeGarlic == true)//마늘 먹은상태 
			tigerFeed(tiger.xPos, tiger.yPos + 25, hdc);
		else if (tiger.takeGarlic == false)
			tigerAnimation(tiger.xPos, tiger.yPos + 25, hdc);
		
		////폰트~앞으로빼면 뭔진몰라도 안나오니 뒤로 빼자
		//font = CreateFont(80, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("궁서"));
		////폰트 이룸 중 띄어쓰기가 된 것은 적용안되는듯 멍청한 기계문명.... 인수가 14개이니까 주의하긔
		//oldfont = (HFONT)SelectObject(hdc, font);
		//SetBkMode(hdc, TRANSPARENT);//폰트배경 투명화
		//TextOut(hdc, 100, 100, str, strlen(str));
		//SelectObject(hdc, oldfont);
		//DeleteObject(font);

		for (int i = 0; i < garlics.now; i++) {
			DeleteObject(gararray[i].garbit);
		}
		DeleteDC(memDC);
		DeleteObject(hBit);
		EndPaint(hWnd, &ps);
		break;

	case WM_TIMER://0번-움직임 1번-마늘, 마차 생성 텀
		switch (wParam) {
		case 0://마늘과 마차 이동 타이머
			//마늘 부분----------------------------
			if (garlicTime == true) {
				gararray[garlics.now].yPos += 20;
				if (gararray[garlics.now].yPos == 500) {//발 맞는 부분(체크 ㅇㅋ)
					garlics.now++; //인덱스 증가
					garlicTime = false;
					SetTimer(hWnd, 1, 5000, NULL);
				}
			}
			////마차 부분-----------------------------
			if (carriage.ismachamoving == true) {
				if (carriage.machadir == 1) {//왼->오
					carriage.xPos += 10;
					if (carriage.xPos == 1024) {
						carriage.ismachamoving = false;
						SetTimer(hWnd, 2, 7000, NULL);
					}
				}
				else if (carriage.machadir == 2) {//오->왼
					carriage.xPos -= 10;
					if (carriage.xPos == -200) {//xpos 좌표 위치가 왼쪽상단이라 다 지나가기전에 먼저사라져서 -값줌
						carriage.ismachamoving = false;
						SetTimer(hWnd, 2, 7000, NULL);
					}
				}

			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case 1://마늘쑥 좌표 잡는 타이머(5초 1회)
			gararray[garlics.now].xPos = rand() % 924 + 100 ; //x좌표 85~715(동굴 빼고)
			gararray[garlics.now].yPos = 0;
			garlicTime = true;
			break;

		case 2://마차 등장 타이머
			carriage.machadir = rand() % 2 + 1;

			if (carriage.machadir == 1) {//왼->오
				carriage.xPos = 0;//출발좌표설정
				carriage.yPos = 500;
				KillTimer(hWnd, 2);
			}
			else if (carriage.machadir == 2) {//오->왼
				carriage.xPos = 800;
				carriage.yPos = 500;
				KillTimer(hWnd, 2);
			}
			carriage.ismachamoving = true;
			break;
		}
		break;

	case WM_KEYDOWN:
		//호랑이 움직임
		if (GetAsyncKeyState(VK_UP) & 0x8000) {//공격
			tiger.isAttack = true;
			snd.Add_sound();
			snd.Play(2);//n번쨰 채널의 음악을 플레이한다. 
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {//마늘 저장

		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
			if (tiger.xPos > 25)//동굴은 85, 입구는 35
				tiger.xPos -= 10;
			tiger.dir = 0;
		}
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
			if (tiger.xPos < WIDTH - 25 - 125)
				tiger.xPos += 10;
			tiger.dir = 1;
		}
		//곰 움직임
		if (GetAsyncKeyState('W') & 0x8000) {//공격
			bear.isAttack = true;
			snd.Add_sound();
			snd.Play(2);
		}
		if (GetAsyncKeyState('W') & 0x8000) {//마늘 저장

		}
		if (GetAsyncKeyState('A') & 0x8000) {
			if (bear.xPos > 25)
				bear.xPos -= 10;
			bear.dir = 1;
		}
		if (GetAsyncKeyState('D') & 0x8000) {
			if (bear.xPos < WIDTH - 25 - 85)
				bear.xPos += 10;
			bear.dir = 0;
		}
		break;

	case WM_KEYUP:
		break;

	case WM_CHAR:
		if((wParam == 'q')||(wParam=='Q')) {
			PostQuitMessage(0);
		}
		break;

	case WM_DESTROY:
		
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