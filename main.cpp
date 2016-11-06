#include <windows.h>
#include "ffplay.h"
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char szClassName[] = "CodeBlocksWindowsApp";

int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
	HWND hwnd;               /* This is the handle for our window */
	MSG messages;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

	/* The Window structure */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof (WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	/* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND;

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx(&wincl))
		return 0;

	/* The class is registered, let's create the program*/
	hwnd = CreateWindowEx(
		0,                   /* Extended possibilites for variation */
		szClassName,         /* Classname */
		"ffplay",       /* Title Text */
		WS_OVERLAPPEDWINDOW, /* default window */
		CW_USEDEFAULT,       /* Windows decides the position */
		CW_USEDEFAULT,       /* where the window ends up on the screen */
		600,                 /* The programs width */
		400,                 /* and height in pixels */
		HWND_DESKTOP,        /* The window is a child-window to desktop */
		NULL,                /* No menu */
		hThisInstance,       /* Program Instance handler */
		NULL                 /* No Window Creation data */
		);

	/* Make the window visible on the screen */
	ShowWindow(hwnd, nCmdShow);

	/* Run the message loop. It will run until GetMessage() returns 0 */
	while (GetMessage(&messages, NULL, 0, 0))
	{
		/* Translate virtual-key messages into character messages */
		TranslateMessage(&messages);
		/* Send message to WindowProcedure */
		DispatchMessage(&messages);
	}

	/* The program return-value is 0 - The value that PostQuitMessage() gave */
	return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	double srcRatio,screenRatio;
	double width, height;
	double pos,incr;
	switch (message)                  /* handle the messages */
	{
	case WM_LBUTTONUP:		
		if (!is){
			Init(hwnd);
			play("d:/1.rmvb");
		}			
		break;
	case WM_SIZE:
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
		break;
	case WM_KEYUP:
		if (!is)
			break;
		switch (wParam){
		case VK_RIGHT:
			incr = 60.0;
			goto do_seek;
			break;
		case VK_LEFT:
			incr = -60.0;
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
	
		break;
	case WM_DESTROY:
		PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
		break;
	default:                      /* for messages that we don't deal with */
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}
