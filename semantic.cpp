
// ----------------------------- semantic.cpp --------------------------------
#include "semantic.h"

extern double Parameter, // 参数 T
			Origin_x, Origin_y, // 横、纵平移距离
			Scale_x, Scale_y, // 横、纵比例因子
			Rot_angle; // 旋转角度

double GetExprValue(struct ExprNode *root);

void DrawPixel(unsigned long x, unsigned long y);

void DrawLoop(double Start, double End,double Step,struct ExprNode *HorPtr,struct ExprNode *VerPtr);

void DelExprTree(struct ExprNode *root);

static void Errmsg (char *string); 

// 计算点坐标
static void CalcCoord(struct ExprNode *Hor_Exp,struct ExprNode *Ver_Exp,double &Hor_X,double &Ver_y);


// ------------------------ 出错信息显示
void Errmsg (char *string) { exit(1); }

// ------------------------ 计算绘制点坐标
static void CalcCoord(struct ExprNode * Hor_Exp,struct ExprNode *Ver_Exp,double &Hor_x,double &Ver_y) {

double HorCord, VerCord, Hor_tmp;

	// 计算表达式值，得到点的原始坐标
	HorCord = GetExprValue (Hor_Exp);//由于参数Parameter发生了变化 所以每次计算的结果都不相同
	VerCord = GetExprValue (Ver_Exp);
	
	// 进行比例变换
	HorCord *= Scale_x;
	VerCord *= Scale_y;

	// 进行旋转变换
	Hor_tmp = HorCord * cos(Rot_angle) + VerCord * sin(Rot_angle);
	VerCord = VerCord * cos(Rot_angle) - HorCord * sin(Rot_angle);
	HorCord = Hor_tmp;

	// 进行平移变换
	HorCord += Origin_x;
	VerCord += Origin_y;

	// 返回变换后的点坐标
	Hor_x = HorCord;
	Ver_y = VerCord;
}

// ------------------------ 循环绘制点坐标
void DrawLoop(double Start,double End,double Step,struct ExprNode *HorPtr,struct ExprNode * VerPtr) {

extern double Parameter;

double x, y;

	for(Parameter = Start; Parameter <= End; Parameter += Step) {

		CalcCoord(HorPtr, VerPtr, x, y);
		DrawPixel((unsigned long)x, (unsigned long)y);

	}
}

// ------------------------ 计算表达式
double GetExprValue(struct ExprNode *root) {

	if (root == NULL) return 0.0;

	switch (root->OpCode) {
		case PLUS:
			return GetExprValue(root->Content.CaseOperator.Left)+GetExprValue(root->Content.CaseOperator.Right);
		case MINUS:
			return GetExprValue(root->Content.CaseOperator.Left)-GetExprValue(root->Content.CaseOperator.Right);
		case MUL:
			return GetExprValue(root->Content.CaseOperator.Left)*GetExprValue(root->Content.CaseOperator.Right);
		case DIV:
			return GetExprValue(root->Content.CaseOperator.Left)/GetExprValue(root->Content.CaseOperator.Right);
		case POWER:
			return pow(GetExprValue(root->Content.CaseOperator.Left),GetExprValue(root->Content.CaseOperator.Right));
		case FUNC:
			return (*root->Content.CaseFunc.MathFuncPtr) (GetExprValue(root->Content.CaseFunc.Child));
		case CONST_ID:
			return root->Content.CaseConst;
		case T:
			return *(root->Content.CaseParmPtr);
		default:
			return 0.0;
	}
}

// ------------------------ (递归)删除语法树
void DelExprTree(struct ExprNode * root) {
	
	if (root == NULL) return;

	switch (root -> OpCode) {

		// 两孩子内部节点
		case PLUS: 
		case MINUS:
		case MUL:
		case DIV:
		case POWER:
			DelExprTree (root->Content.CaseOperator.Left);
			DelExprTree (root->Content.CaseOperator.Right);
			break;

		// 单孩子内部节点
		case FUNC: 
			DelExprTree (root->Content.CaseFunc.Child);
			break;
		
		default: // 叶节点
			break;
	}

	delete(root); // 删除节点
}

#ifdef _BC_COMPILER

int InGraphMode = 0;

int InitGraph (void) {

int gd = DETECT, gm;

	if (InGraphMode) return 0;

	registerbgidriver (EGAVGA_driver);

	initgraph (&gd, &gm, "");
	
	setcolor (-1);
	
	InGraPhMode = 1;
	
	return 1;
}

void CloseGraph (void) {

	if (!InGraphMode) return;

	getch ();
	
	closegraph ();
	
	InGraphMode = 0;
}

#endif

// ------------------------ 绘制节点
void DrawPixel(unsigned long x, unsigned long y) {
#ifdef _VC_COMPILER
	SetPixel(hDC, x, y, black);
#endif

#ifdef _BC_COMPILER
	putpixel(x, y, white);
#endif
}
