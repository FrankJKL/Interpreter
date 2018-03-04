
// ----------------------------- main.cpp --------------------------------
#ifdef _VC_COMPILER

#include "semantic.h"

HDC hDC; // 窗口句柄，全局变量

#define MAX_CHARS 200
char SrcFilePath[MAX_CHARS]; // 源程序文件路径名

static char Name[] = "编译器"; // 窗口名

// ------------------------- 初始化窗口函数
static bool PrepareWindow (HINSTANCE, HINSTANCE, int);

// ------------------------- 源程序文件合法性检查
static bool CheckSrcFile(LPSTR);

// ------------------------- 窗口消息处理函数
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ------------------------- window 程序主函数
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {

	// 保存源程序文件路径名
	strcpy(SrcFilePath, lpCmdLine);

	// 初始化窗口
	if (PrepareWindow (hInstance, hPrevInstance, nCmdShow ) != true ) {

		MessageBox(NULL, "窗口初始化失败!","错误", MB_OK);
		return 1;
	}

	// 检查源程序文件
	if (!CheckSrcFile(lpCmdLine)) return 1;

	// 调用绘图语言解释器
	Parser(SrcFilePath);

	// 进入window 消息循环
	MSG Msg;
	while(GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}


// ------------------------- 窗口初始化
bool PrepareWindow(HINSTANCE hInst, HINSTANCE hPrevInstance, int nCmdShow) {

HWND hWnd;
WNDCLASS W;

	memset(&W,0,sizeof(WNDCLASS));

	W.style = CS_HREDRAW | CS_VREDRAW;
	W.lpfnWndProc = WndProc;
	W.hInstance = hInst;
	W.hCursor = LoadCursor(NULL, IDC_ARROW);
	W.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	W.lpszClassName = Name;
	RegisterClass(&W);

	hWnd = CreateWindow(Name, Name,WS_OVERLAPPEDWINDOW,10,10,740,490,NULL,NULL,hInst,NULL);
	if(hWnd == NULL) return false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetCursor(LoadCursor(hInst, IDC_ARROW));

	hDC = GetDC(hWnd);

	return true;
}

// ------------------------- 检查源程序文件合法性
bool CheckSrcFile(LPSTR lpszCmdParam) {

FILE *file = NULL;

	if(strlen(lpszCmdParam) == 0) {
		MessageBox(NULL, "未指定源程序文件!", "错误", MB_OK);
		return false;
	}

	if((file = fopen(lpszCmdParam, "r")) == NULL) {
		MessageBox(NULL, "打开源程序文件出错误!", "错误", MB_OK);
		MessageBox(NULL, lpszCmdParam, "文件名", MB_OK);
		return false;
	} else 
		fclose(file);

	return true;
}

// ------------------------- 窗口处理函数
LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam,LPARAM lParam) {

	switch(Message) {
		case WM_DESTROY:
			ReleaseDC(hWnd, hDC);

			PostQuitMessage(0);
			return 0;
		case WM_PAINT:

			PAINTSTRUCT pt;
			
			BeginPaint(hWnd, &pt);
			
			Parser(SrcFilePath);
			
			EndPaint(hWnd, &pt);

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);
	}
}

#endif

#ifdef _BC_COMPILER

#include "semantic.h"
#include "parser.h"

int main(int argc, char **argv) {

int Result;

	if (!InitGraph ()) return -1;

	if (argc != 2) return -1;

	Parser(argv[l]);

	CloseGraPh ();
	
	return 0;
}
#endif
