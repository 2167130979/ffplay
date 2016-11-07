#include <windows.h>
#include "ffplay.h"

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void OnSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnKeyUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnClick(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL  OpenFileDialog(HWND hWnd);
char szFile[2048];

int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = "ffplayapp";
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof (WNDCLASSEX);
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0; 
	wincl.cbWndExtra = 0; 
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	RegisterClassEx(&wincl);

	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		"ffplayapp",         /* Classname */
		"ffplay",       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		800,                 /* The programs width */
		452,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
		);

	ShowWindow(hwnd, nCmdShow);
	SetTimer(hwnd, 1, 1000, NULL);
	Init(hwnd);
	while (GetMessage(&messages, NULL, 0, 0))
	{
		TranslateMessage(&messages);
		DispatchMessage(&messages);
	}

	return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
	case WM_LBUTTONDOWN:		
		OnClick(hwnd, message, wParam, lParam);
		break;
	case WM_SIZE:
		OnSize(hwnd,message,wParam,lParam);
		break;
	case WM_KEYUP:		
		OnKeyUp(hwnd, message, wParam, lParam);
		break;
	case WM_TIMER:
		OnTimer(hwnd, message, wParam, lParam);		
		break;
	case WM_DESTROY:
		exit(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

BOOL  OpenFileDialog(HWND hWnd)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFile[0] = '\0';
	return GetOpenFileName((LPOPENFILENAME)&ofn);
}

void OnSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	double srcRatio, screenRatio;
	double width, height;
	if (wParam != SIZE_MINIMIZED){
		GetClientRect(hwnd, &rect);
		srcRatio = (double)src_width / (double)src_height;
		screenRatio = (double)(rect.right - rect.left) / (double)(rect.bottom - rect.top);
		if (screenRatio > srcRatio){
			height = rect.bottom - rect.top;
			width = (rect.bottom - rect.top)*srcRatio;
		}
		else{
			width = (rect.right - rect.left);
			height = width / srcRatio;
		}
		glViewport(((rect.right - rect.left) - width) / 2, ((rect.bottom - rect.top) - height) / 2, width, height);
	}
}

void OnKeyUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	double pos, incr;
	if (!is)
		return;
	switch (wParam){
	case VK_RIGHT:
		incr = 60.0;
		goto do_seek;
		break;
	case VK_LEFT:
		incr = -60.0;
		goto do_seek;
		break;
	case VK_DOWN:
		incr = 600.0;
		goto do_seek;
		break;
	case VK_UP:
		incr = -600.0;
		goto do_seek;
		break;
	do_seek:
		pos = get_master_clock(is);
		if (isnan(pos))
			pos = (double)is->seek_pos / AV_TIME_BASE;
		pos += incr;
		if (is->ic->start_time != AV_NOPTS_VALUE && pos < is->ic->start_time / (double)AV_TIME_BASE)
			pos = is->ic->start_time / (double)AV_TIME_BASE;
		stream_seek(is, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
	}
}

void OnClick(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!is){
		if (OpenFileDialog(hwnd) != 0){
			play(szFile);
		}
	}
	else
		stream_toggle_pause(is);
}

void OnTimer(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char title[64];
	LONGLONG time;
	if (is)
	{
		memset(title, 0, 64);
		time = get_master_clock(is);
		sprintf(title, "%02lld:%02lld:%02lld/%02lld:%02lld:%02lld", time / 3600, time / 60 % 60, time % 60, totaltime / 3600, totaltime / 60 % 60, totaltime % 60);
		SetWindowText(hwnd, title);
	}
}