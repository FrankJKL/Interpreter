
//---------------semantic.h---------------------
#ifdef _VC_COMPILER
	#include <windows.h>
	
	#include <wingdi.h>
	
	extern HDC hDC;

#endif;

#ifdef _BC_COMPILER
	#include <graphics.h>
	#include <conio.h>
#endif

#include "parser.h"

#ifdef _VC_COMPILER
	#define red RGB(255, 0, 0)
	#define black RGB(0, 0, 0)
#endif;

#ifdef _BC_COMPILER
	#define white 255 
#endif

// ------------------------ �ⲿ����
extern void DrawPixel(unsigned long x, unsigned long y); // ���Ƶ�
extern double GetExprValue(struct ExprNode *root); // ��ȡ���ʽֵ

// ����ͼ��
extern void DrawLoop(double Start, double End,double Step,struct ExprNode *HorPtr,struct ExprNode * VerPtr);

extern void DelExprTree(struct ExprNode *root); // ɾ����

#ifdef _BC_COMPILER
	extern int InGraphMode;
	extern int InitGraph (void);
	extern void CloseGraph (void);
#endif
