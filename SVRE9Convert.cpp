#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#pragma comment(lib, "winmm.lib")

INPUT input[1];

//KeyMap
#define XR 'A'
#define XL 'S'
#define YR 'L'
#define YL VK_OEM_PLUS

// Offset
#define DTIMER 2000
#define WAITTIMER 0

typedef struct {
	int status;
	int oldStatus;
	int counter;
}VOL;

POINT pt;
RECT rec;
HWND hDeskWnd = GetDesktopWindow();
VOL left = { 0 };
VOL right = { 0 };

bool sw = false;
int cnt = 0;
int centerX = 0;
int centerY = 0;
int OffTimer = DTIMER;
int frameCounter = 0;
bool dispMsg = false;


void KeyDown(unsigned char key) {
	input[0].ki.wVk = key;
	input[0].ki.wScan = MapVirtualKey(input[0].ki.wVk, 0);
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.dwFlags = 0x0000;
	input[0].ki.dwExtraInfo = GetMessageExtraInfo();
	SendInput(1, input, sizeof(INPUT));
}

void KeyUp(unsigned char key) {
	input[0].ki.wVk = key;
	input[0].ki.wScan = MapVirtualKey(input[0].ki.wVk, 0);
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.dwFlags = KEYEVENTF_KEYUP;
	input[0].ki.dwExtraInfo = GetMessageExtraInfo();
	SendInput(1, input, sizeof(INPUT));
}

void displayHeader() {
#ifdef _DEBUG
	printf("(Debug)SVRE9 アナログコンバーター\n\n");
#else
	printf("SVRE9 アナログコンバーター\n\n");
#endif
}

void initialize() {
	HANDLE hOut;
	CONSOLE_CURSOR_INFO cci;
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleCursorInfo(hOut, &cci);
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hOut, &cci);

	displayHeader(); // ヘッダー表示
}

int _tmain() {
	initialize();
	GetWindowRect(hDeskWnd, &rec);

	while (!GetAsyncKeyState(VK_ESCAPE)) {
		bool TriggerKey = GetAsyncKeyState(VK_HOME);
		if (TriggerKey & 1 && (cnt == 0)) {
			sw = !sw; cnt = 1;
			GetCursorPos(&pt);
			centerX = pt.x;
			centerY = pt.y;
			system("cls");
			displayHeader();
			dispMsg = false;
		}
		else if (TriggerKey == 0) cnt = 0;

		if (sw) {
			GetCursorPos(&pt);
			// X座標中心から変化したら状態変化
			if (pt.x > centerX) {
				left.status = 1;
				left.counter = 0;
			}
			else if (pt.x < centerX) {
				left.status = -1;
				left.counter = 0;
			}
			else left.counter < OffTimer ? left.counter++ : left.status = 0;

			// つまみ状態が変化したら
			if (left.status != left.oldStatus) {
				if (left.status == 1)KeyDown(XL);			// 時計回り
				else if (left.status == -1) KeyDown(XR);	// 反時計回り

				// つまみ状態が変化する前が「回転状態」だったらキーを放す
				if (left.oldStatus == 1)KeyUp(XL);
				else if (left.oldStatus == -1)KeyUp(XR);
				left.oldStatus = left.status;
			}

			if (pt.y > centerY) {
				right.status = 1;
				right.counter = 0;
			}
			else if (pt.y < centerY) {
				right.status = -1;
				right.counter = 0;
			}
			else right.counter < OffTimer ? right.counter++ : right.status = 0;

			if (right.status != right.oldStatus) {
				if (right.status == 1) KeyDown(YL);			// 時計回り
				else if (right.status == -1) KeyDown(YR);	// 反時計回り

				if (right.oldStatus == 1) KeyUp(YL);		// 時計回り
				else if (right.oldStatus == -1) KeyUp(YR);	// 反時計回り

				right.oldStatus = right.status;
			}

			#ifdef _DEBUG
				printf("\r[debug] Mouse Center PosX:%d  PosY:%d  LeftVolStatus:%d  RightVolStatus:%d  cnt=%d", centerX, centerY, left.status, right.status, frameCounter);
			#else
				if (!dispMsg) {
					printf("動作中... HOMEキーを押すと動作を停止します\n");
					dispMsg = true;
				}
			#endif
			SetCursorPos(centerX, centerY);
			frameCounter++;
		}
		else {
			if (!dispMsg) {
				printf("HOMEキーを押すと動作開始します");
				dispMsg = true;
			}
			frameCounter = 0;
			left.status = 0;
			left.oldStatus = 0;
			right.status = 0;
			right.oldStatus = 0;
		}
		timeBeginPeriod(1);
		Sleep(WAITTIMER);
		timeEndPeriod(1);
	}
	return 0;
}