
// ----------------------------- parser.cpp --------------------------------
#include "parser.h"

#ifndef PARSER_DEBUG
#include "semantic.h"
#endif

#ifdef PARSER_DEBUG
	#define enter(x) printf("����"); printf(x); printf("\n")
#else
	#define enter(x)
#endif

#ifdef PARSER_DEBUG
	#define back(x) printf("�˳�"); printf(x); printf("\n")
#else
	#define back(x)
#endif

#ifdef PARSER_DEBUG
	#define call_match(x) printf("ƥ��Ǻţ�"); printf(x), printf("\n")
#else
	#define call_match(x)
#endif

#ifdef PARSER_DEBUG
	#define Tree_trace(x) PrintSyntaxTree(x, 1);
#else
	#define Tree_trace
#endif

#ifdef PARSER_DEBUG
	double  Parameter=0; // ���� T
#else
	double 	Parameter=0, 			// ���� T �Ĵ洢�ռ�
			Origin_x=0, Origin_y=0, // �ᡢ��ƽ�ƾ���
			Scale_x=1, Scale_y=1,	// �ᡢ�ݱ�������
			Rot_angle=0; 			// ��ת�Ƕ�
#endif

static Token token; // �Ǻ�

// ----------------- ��������
static void FetchToken ();											//ȡ�Ǻ�
static void MatchToken (enum Token_Type AToken);					//ƥ��Ǻ�
static void SyntaxError (int case_of);								//�����﷨����
static void ErrMsg(unsigned LineNo, char *descrip, char *string );	//��ʾ������Ϣ
static void PrintSyntaxTree(struct ExprNode * root, int indent);	//��ӡ�﷨��

// ----------------- ���ս�����﷨��λ���ĵݹ��ӳ���
static void Program ();					//����
static void Statement ();				//���
static void OriginStatement ();			//ԭ���������
static void RotStatement ();			//��ת���
static void ScaleStatement ();			//�����任���
static void ForStatement ();			//ѭ���������
static struct ExprNode *Expression ();	//���ʽ
static struct ExprNode *Term ();		//�Ӽ���
static struct ExprNode *Factor ();		//�˻���
static struct ExprNode *Component ();	//
static struct ExprNode *Atom ();		//����������

// ----------------- �ⲿ�ӿ����﷨�����캯��
extern void Parser(char *SrcFilePtr);
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

// ------------------------ ���ôʷ������� GetToken ��ȡһ���Ǻ�
static void FetchToken () {

	token = GetToken ();
	
	if (token.type == ERRTOKEN) SyntaxError(1);
}

// ------------------------ ƥ��Ǻ�
static void MatchToken (enum Token_Type The_Token) {

	if (token.type != The_Token) SyntaxError (2);

	FetchToken();

}

// ------------------------ �﷨������
static void SyntaxError (int case_of) {

	switch(case_of) {
		case 1: 
			ErrMsg (LineNo, "����Ǻ�", token.lexeme);
			break;
		case 2:
			ErrMsg (LineNo, "����Ԥ�ڼǺ�", token.lexeme);
			break;
	}
}

// ------------------------ ��ӡ������Ϣ
void ErrMsg(unsigned LineNo, char *descrip, char *string ) {

	#ifdef PARSER_DEBUG
		printf("�к� %5d:%s %s!\n", LineNo, descrip, string);
	#else
		char msg[256];
		memset(msg, 0, 256);
		sprintf(msg, "�к� %5d:%s %s!", LineNo, descrip, string);
	#endif

	#ifdef _VC_COMPILER
		MessageBox(NULL, msg, "����!", MB_OK);
	#endif

	#ifdef _BC_COMPILER
		printf("%s\n", msg);
	#endif

	CloseScanner();

	exit(1);
}

// ------------------------ �����������ӡ���ʽ���﷨��
void PrintSyntaxTree(struct ExprNode* root, int indent) {

int temp;

	for (temp=1; temp<=indent; temp++) printf("\t"); // ����

	switch(root->OpCode) { // ��ӡ���ڵ�
		case PLUS: 
			printf("%s\n", "+"); break;
		case MINUS:
			printf("%s\n", "-"); break;
		case MUL:
			printf("%s\n", "*"); break;
		case DIV:
			printf("%s\n", "/"); break;
		case POWER:
			printf("%s\n", "**"); break;
		case FUNC:
			printf("%x\n", root->Content.CaseFunc.MathFuncPtr); break;
		case CONST_ID:
			printf("%f\n", root->Content.CaseConst); break;
		case T:
			printf("%s\n", "T" ); break;
		default:
			printf("��������ڵ�!\n"); 
			exit(0);
	}

	if(root->OpCode == CONST_ID || root->OpCode == T) return; // Ҷ�ӽڵ㷵��
	
	if(root->OpCode == FUNC) 
		// �ݹ��ӡһ�����ӵĽڵ�
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent+1);
	else {
		// �ݹ��ӡ�������ӵĽڵ�
		PrintSyntaxTree(root->Content.CaseOperator.Left, indent+1);
		PrintSyntaxTree(root->Content.CaseOperator.Right, indent+1);
	}

}

// ------------------------ ��ͼ���Խ��������(�����������߽ӿ�)
void Parser(char *SrcFilePtr) {

	enter("�﷨����");
	
	// ��ʼ���ʷ�������
	if(!InitScanner(SrcFilePtr)) {
		printf("Դ�����ļ��򿪴���!\n");
		return;
	}

	FetchToken(); // ��ȡ��һ���Ǻ�

	Program(); // �ݹ��½�����

	CloseScanner(); // �رմʷ�������

	back("�﷨����");

	return;
}

// ------------------------ Program �ݹ��ӳ���
static void Program ()


{
	enter("<����>");

	while (token.type != NONTOKEN) {
		Statement ();
		MatchToken (SEMICO);
	}

	back("<����>");
}

// ------------------------ Statement �ĵݹ��ӳ���
static void Statement() {

	enter("<���>");

	switch (token.type) {
		case ORIGIN: 
			OriginStatement(); break;
		case SCALE: 
			ScaleStatement(); break;
		case ROT: 
			RotStatement(); break;
		case FOR: 
			ForStatement(); break;
		default: 
			SyntaxError(2);
	}

	back("<���>");
}

// ------------------------ OriginStatement �ĵݹ��ӳ���
static void OriginStatement (void) {

struct ExprNode *tmp;

	enter("<ԭ���������>");

	MatchToken (ORIGIN);		//ƥ��ؼ��� origin
	MatchToken (IS);			//ƥ��ؼ��� is
	MatchToken (L_BRACKET);		//ƥ��������

	tmp = Expression();			//������������ʽ
	#ifndef PARSER_DEBUG
		Origin_x = GetExprValue(tmp); // ��ȡ�������ƽ�ƾ���
		DelExprTree(tmp);
	#endif

	MatchToken (COMMA);			//ƥ�䶺��
	
	tmp = Expression();			//������������ʽ
	#ifndef PARSER_DEBUG
		Origin_y = GetExprValue(tmp); // ��ȡ�������ƽ�ƾ���
		DelExprTree(tmp);
	#endif

	MatchToken (R_BRACKET);		//ƥ��������

	back("<ԭ���������>");
}

// ------------------------ ScaleStatement �ĵݹ��ӳ���
static void ScaleStatement (void) {

struct ExprNode *tmp;
	
	enter("<�����������>");

	MatchToken (SCALE);			//ƥ��ؼ��� scale
	MatchToken (IS);			//ƥ��ؼ��� is
	MatchToken (L_BRACKET);		//ƥ��������

	tmp = Expression();			//����������������ӱ��ʽ
	#ifndef PARSER_DEBUG
		Scale_x = GetExprValue(tmp); // ��ȡ������ı�������
		DelExprTree(tmp);
	#endif

	MatchToken (COMMA);			//ƥ�䶺��

	tmp = Expression();			//����������������ӱ��ʽ
	#ifndef PARSER_DEBUG
		Scale_y = GetExprValue(tmp); // ��ȡ������ı�������
		DelExprTree(tmp);
	#endif

	MatchToken (R_BRACKET);		//ƥ��������

	back("<�����������>");
}

// ------------------------ RotStatement �ĵݹ��ֳ���
static void RotStatement (void) {
	struct ExprNode *tmp;

	enter("<��ת�Ƕ��������>");

	MatchToken (ROT);		//ƥ��ؼ��� rot
	MatchToken (IS);		//ƥ��ؼ��� is

	tmp = Expression();		//ƥ��Ƕȱ��ʽ

	#ifndef PARSER_DEBUG
		Rot_angle = GetExprValue(tmp); // ��ȡ��ת�Ƕ�
		DelExprTree(tmp);
	#endif
	
	back("<��ת�Ƕ��������>");
}

// ------------------------ For �ݹ��ӳ���
static void ForStatement (void) {

#ifndef PARSER_DEBUG
double Start, End, Step; // ��ͼ��㡢�յ㡢����
#endif

struct ExprNode *start_ptr, *end_ptr, *step_ptr,*x_ptr,*y_ptr; // �����ʽ�﷨�����ڵ�ָ��
	
	enter("<ѭ����ͼ���>");

	MatchToken (FOR);	
	call_match("FOR");
	
	MatchToken(T); 
	call_match("T");
	
	MatchToken (FROM); 
	call_match("FROM");

	start_ptr = Expression(); //���������ʽ
	#ifndef PARSER_DEBUG
		Start = GetExprValue(start_ptr);	// ���������ʽֵ
		DelExprTree(start_ptr);				// �ͷ������ʽ�﷨��
	#endif

	MatchToken (TO);
	call_match("TO");

	end_ptr = Expression(); //�����յ���ʽ

	#ifndef PARSER_DEBUG
		End = GetExprValue(end_ptr); //�����յ���ʽֵ
		DelExprTree(end_ptr);		// �ͷ��յ���ʽ�﷨��
	#endif

	MatchToken (STEP);
	call_match("STEP");

	step_ptr = Expression(); // �����������ʽ

	#ifndef PARSER_DEBUG
		Step = GetExprValue(step_ptr);	//���㲽�����ʽֵ
		DelExprTree(step_ptr);			//�ͷŲ������ʽ�﷨��
	#endif

	MatchToken (DRAW);
	call_match("DRAW");

	MatchToken (L_BRACKET);
	call_match("(");

	x_ptr = Expression(); //������������ʽ

	MatchToken(COMMA);
	call_match(",");

	y_ptr = Expression(); //������������ʽ

	MatchToken (R_BRACKET);
	call_match(")");

	#ifndef PARSER_DEBUG
		DrawLoop (Start, End, Step, x_ptr, y_ptr); // ����ͼ��		
		DelExprTree(x_ptr); // �ͷź������﷨��
		DelExprTree(y_ptr); // �ͷ��������﷨��
	#endif

	back("<ѭ����ͼ���>");
}

// ------------------------- Expression �ĵݹ��ӳ���
static struct ExprNode* Expression() {

struct ExprNode *left, *right; // ���������ڵ��ָ��
Token_Type token_tmp; // ��ǰ�Ǻ�

	enter("<���ʽ>");

	left = Term(); // �����������

	while (token.type == PLUS || token.type == MINUS) {

		token_tmp = token.type;

		MatchToken(token_tmp);	//

		right = Term();			//�����Ҳ�����

		left = MakeExprNode(token_tmp, left, right);

	}

	Tree_trace(left); // ��ӡ���ʽ�﷨��

	back("<���ʽ>");

	return left; // ���ر��ʽ�﷨��ָ��
}

// ------------------------ Term �ݹ��ӳ���
static struct ExprNode *Term(){

struct ExprNode *left, *right;
Token_Type token_tmp;

	left = Factor();
	
	while (token.type==MUL || token.type==DIV) {

		token_tmp = token.type;
		MatchToken(token_tmp);

		right = Factor();

		left = MakeExprNode(token_tmp, left, right);

	}
	
	return left;
}

// ------------------------ Factor �ݹ��ӳ���
static struct ExprNode * Factor () {
struct ExprNode *left, *right;

	if(token.type == PLUS) {	// ƥ��һԪ��
		MatchToken(PLUS);
		right = Factor(); // ���ʽ�˻�Ϊ�����в������ı��ʽ

	} else if(token.type == MINUS) {	// ƥ��һԪ��
	
		MatchToken(MINUS); // ���ʽת��Ϊ��Ԫ������ı��ʽ
		right = Factor();
		
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		
		right = MakeExprNode(MINUS, left, right);
	} else 
		right = Component(); // ƥ����ս�� Component

	return right;
}

// ------------------------ Component �ݹ��ӳ���
static struct ExprNode* Component() {

struct ExprNode	*left, *right;

	left = Atom();

	if(token.type == POWER) {

		MatchToken(POWER);
		right = Component(); // �ݹ���� ComPonent ��ʵ�� POWER ���ҽ��
		
		left =MakeExprNode(POWER, left, right);
	}

	return left;
}

// ------------------------ Atom �ݹ��ӳ���
static struct ExprNode* Atom() {

struct Token t = token;
struct ExprNode *address, *tmp;

	switch (token.type) {
		case CONST_ID:
			MatchToken (CONST_ID);
			address = MakeExprNode(CONST_ID, t.value);
			break;

		case T:
			MatchToken(T);
			address = MakeExprNode(T);
			break;

		case FUNC:

			MatchToken (FUNC);
			
			MatchToken (L_BRACKET);
			
			tmp = Expression ();
			
			address = MakeExprNode(FUNC, t.FuncPtr, tmp);
			
			MatchToken (R_BRACKET);
			break;

		case L_BRACKET:
			
			MatchToken (L_BRACKET);
			
			address = Expression ();
			
			MatchToken (R_BRACKET);
			
			break;

		default:
			
			SyntaxError (2);
	}

	return address;
}

// ----------------- �����﷨���ڵ�
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...) {

struct ExprNode *ExprPtr = new (struct ExprNode);
va_list ArgPtr;

	ExprPtr->OpCode = opcode; // ���ܼǺŵ����

	va_start(ArgPtr, opcode);

	switch(opcode) {	// ���ݼǺŵ�����첻ͬ�Ľڵ�
		case CONST_ID: // ����
			ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);
			break;

		case T:
			ExprPtr->Content.CaseParmPtr = &Parameter;
			break;

		case FUNC:
			ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);
			ExprPtr->Content.CaseFunc.Child = (struct ExprNode *) va_arg(ArgPtr, struct ExprNode *);
			break;

		default:
			ExprPtr->Content.CaseOperator.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
			ExprPtr->Content.CaseOperator.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);
			break;
	}

	va_end(ArgPtr);

	return ExprPtr;
}
