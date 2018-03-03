// ----------------------------- parser.h --------------------------------

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"

typedef double (*FuncPtr)(double);

struct ExprNode {	// 语法树节点类型
	
	enum Token_Type OpCode;	// 记号类别号 PLUS, MINUS, MUL, DIV POWER, FUNC, CONSTID 等
	
	//记号内容
	union {
		struct { ExprNode *Left, *Right; } CaseOperator;		//运算符
		struct { ExprNode *Child; FuncPtr MathFuncPtr; } CaseFunc;	//数学函数
		double CaseConst;		//常量
		double *CaseParmPtr;	//循环变量
	} Content;
};

extern void Parser(char *SrcFilePtr); // 语法分析器对外的接口

#endif
