
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

// ------------------------ 外部函数
extern void DrawPixel(unsigned long x, unsigned long y); // 绘制点
extern double GetExprValue(struct ExprNode *root); // 获取表达式值

// 绘制图形
extern void DrawLoop(double Start, double End,double Step,struct ExprNode *HorPtr,struct ExprNode * VerPtr);

extern void DelExprTree(struct ExprNode *root); // 删除树

#ifdef _BC_COMPILER
	extern int InGraphMode;
	extern int InitGraph (void);
	extern void CloseGraph (void);
#endif
