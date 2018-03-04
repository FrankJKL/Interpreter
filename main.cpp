
// ----------------------------- main.cpp --------------------------------
#ifdef _VC_COMPILER

#include "semantic.h"

HDC hDC; // ���ھ����ȫ�ֱ���

#define MAX_CHARS 200
char SrcFilePath[MAX_CHARS]; // Դ�����ļ�·����

static char Name[] = "������"; // ������

// ------------------------- ��ʼ�����ں���
static bool PrepareWindow (HINSTANCE, HINSTANCE, int);

// ------------------------- Դ�����ļ��Ϸ��Լ��
static bool CheckSrcFile(LPSTR);

// ------------------------- ������Ϣ������
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// ------------------------- window ����������
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {

	// ����Դ�����ļ�·����
	strcpy(SrcFilePath, lpCmdLine);

	// ��ʼ������
	if (PrepareWindow (hInstance, hPrevInstance, nCmdShow ) != true ) {

		MessageBox(NULL, "���ڳ�ʼ��ʧ��!","����", MB_OK);
		return 1;
	}

	// ���Դ�����ļ�
	if (!CheckSrcFile(lpCmdLine)) return 1;

	// ���û�ͼ���Խ�����
	Parser(SrcFilePath);

	// ����window ��Ϣѭ��
	MSG Msg;
	while(GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}


// ------------------------- ���ڳ�ʼ��
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

// ------------------------- ���Դ�����ļ��Ϸ���
bool CheckSrcFile(LPSTR lpszCmdParam) {

FILE *file = NULL;

	if(strlen(lpszCmdParam) == 0) {
		MessageBox(NULL, "δָ��Դ�����ļ�!", "����", MB_OK);
		return false;
	}

	if((file = fopen(lpszCmdParam, "r")) == NULL) {
		MessageBox(NULL, "��Դ�����ļ�������!", "����", MB_OK);
		MessageBox(NULL, lpszCmdParam, "�ļ���", MB_OK);
		return false;
	} else 
		fclose(file);

	return true;
}

// ------------------------- ���ڴ�����
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
