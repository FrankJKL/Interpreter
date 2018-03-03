// ----------------------------- parser.h --------------------------------

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

typedef double (*FuncPtr)(double);

struct ExprNode {	// �﷨���ڵ�����
	
	enum Token_Type OpCode;	// �Ǻ����� PLUS, MINUS, MUL, DIV POWER, FUNC, CONSTID ��
	
	//�Ǻ�����
	union {
		struct { ExprNode *Left, *Right; } CaseOperator;		//�����
		struct { ExprNode *Child; FuncPtr MathFuncPtr; } CaseFunc;	//��ѧ����
		double CaseConst;		//����
		double *CaseParmPtr;	//ѭ������
	} Content;
};

extern void Parser(char *SrcFilePtr); // �﷨����������Ľӿ�

#endif
