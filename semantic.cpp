
// ----------------------------- semantic.cpp --------------------------------
#include "semantic.h"

extern double Parameter, // ���� T
			Origin_x, Origin_y, // �ᡢ��ƽ�ƾ���
			Scale_x, Scale_y, // �ᡢ�ݱ�������
			Rot_angle; // ��ת�Ƕ�

double GetExprValue(struct ExprNode *root);

void DrawPixel(unsigned long x, unsigned long y);

void DrawLoop(double Start, double End,double Step,struct ExprNode *HorPtr,struct ExprNode *VerPtr);

void DelExprTree(struct ExprNode *root);

static void Errmsg (char *string); 

// ���������
static void CalcCoord(struct ExprNode *Hor_Exp,struct ExprNode *Ver_Exp,double &Hor_X,double &Ver_y);


// ------------------------ ������Ϣ��ʾ
void Errmsg (char *string) { exit(1); }

// ------------------------ ������Ƶ�����
static void CalcCoord(struct ExprNode * Hor_Exp,struct ExprNode *Ver_Exp,double &Hor_x,double &Ver_y) {

double HorCord, VerCord, Hor_tmp;

	// ������ʽֵ���õ����ԭʼ����
	HorCord = GetExprValue (Hor_Exp);//���ڲ���Parameter�����˱仯 ����ÿ�μ���Ľ��������ͬ
	VerCord = GetExprValue (Ver_Exp);
	
	// ���б����任
	HorCord *= Scale_x;
	VerCord *= Scale_y;

	// ������ת�任
	Hor_tmp = HorCord * cos(Rot_angle) + VerCord * sin(Rot_angle);
	VerCord = VerCord * cos(Rot_angle) - HorCord * sin(Rot_angle);
	HorCord = Hor_tmp;

	// ����ƽ�Ʊ任
	HorCord += Origin_x;
	VerCord += Origin_y;

	// ���ر任��ĵ�����
	Hor_x = HorCord;
	Ver_y = VerCord;
}

// ------------------------ ѭ�����Ƶ�����
void DrawLoop(double Start,double End,double Step,struct ExprNode *HorPtr,struct ExprNode * VerPtr) {

extern double Parameter;

double x, y;

	for(Parameter = Start; Parameter <= End; Parameter += Step) {

		CalcCoord(HorPtr, VerPtr, x, y);
		DrawPixel((unsigned long)x, (unsigned long)y);

	}
}

// ------------------------ ������ʽ
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

// ------------------------ (�ݹ�)ɾ���﷨��
void DelExprTree(struct ExprNode * root) {
	
	if (root == NULL) return;

	switch (root -> OpCode) {

		// �������ڲ��ڵ�
		case PLUS: 
		case MINUS:
		case MUL:
		case DIV:
		case POWER:
			DelExprTree (root->Content.CaseOperator.Left);
			DelExprTree (root->Content.CaseOperator.Right);
			break;

		// �������ڲ��ڵ�
		case FUNC: 
			DelExprTree (root->Content.CaseFunc.Child);
			break;
		
		default: // Ҷ�ڵ�
			break;
	}

	delete(root); // ɾ���ڵ�
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

// ------------------------ ���ƽڵ�
void DrawPixel(unsigned long x, unsigned long y) {
#ifdef _VC_COMPILER
	SetPixel(hDC, x, y, black);
#endif

#ifdef _BC_COMPILER
	putpixel(x, y, white);
#endif
}
