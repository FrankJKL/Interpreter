
// ----------------------------- parser.cpp --------------------------------
#include "parser.h"

#ifndef PARSER_DEBUG
#include "semantic.h"
#endif

#ifdef PARSER_DEBUG
	#define enter(x) printf("进入"); printf(x); printf("\n")
#else
	#define enter(x)
#endif

#ifdef PARSER_DEBUG
	#define back(x) printf("退出"); printf(x); printf("\n")
#else
	#define back(x)
#endif

#ifdef PARSER_DEBUG
	#define call_match(x) printf("匹配记号："); printf(x), printf("\n")
#else
	#define call_match(x)
#endif

#ifdef PARSER_DEBUG
	#define Tree_trace(x) PrintSyntaxTree(x, 1);
#else
	#define Tree_trace
#endif

#ifdef PARSER_DEBUG
	double  Parameter=0; // 参数 T
#else
	double 	Parameter=0, 			// 参数 T 的存储空间
			Origin_x=0, Origin_y=0, // 横、纵平移距离
			Scale_x=1, Scale_y=1,	// 横、纵比例因子
			Rot_angle=0; 			// 旋转角度
#endif

static Token token; // 记号

// ----------------- 辅助函数
static void FetchToken ();											//取记号
static void MatchToken (enum Token_Type AToken);					//匹配记号
static void SyntaxError (int case_of);								//处理语法错误
static void ErrMsg(unsigned LineNo, char *descrip, char *string );	//显示错误信息
static void PrintSyntaxTree(struct ExprNode * root, int indent);	//打印语法树

// ----------------- 非终结符（语法单位）的递归子程序
static void Program ();					//程序
static void Statement ();				//语句
static void OriginStatement ();			//原点设置语句
static void RotStatement ();			//旋转语句
static void ScaleStatement ();			//比例变换语句
static void ForStatement ();			//循环画点语句
static struct ExprNode *Expression ();	//表达式
static struct ExprNode *Term ();		//加减项
static struct ExprNode *Factor ();		//乘积项
static struct ExprNode *Component ();	//
static struct ExprNode *Atom ();		//变量及常量

// ----------------- 外部接口与语法树构造函数
extern void Parser(char *SrcFilePtr);
static struct ExprNode *MakeExprNode(enum Token_Type opcode, ...);

// ------------------------ 调用词法分析器 GetToken 获取一个记号
static void FetchToken () {

	token = GetToken ();
	
	if (token.type == ERRTOKEN) SyntaxError(1);
}

// ------------------------ 匹配记号
static void MatchToken (enum Token_Type The_Token) {

	if (token.type != The_Token) SyntaxError (2);

	FetchToken();

}

// ------------------------ 语法错误处理
static void SyntaxError (int case_of) {

	switch(case_of) {
		case 1: 
			ErrMsg (LineNo, "错误记号", token.lexeme);
			break;
		case 2:
			ErrMsg (LineNo, "不是预期记号", token.lexeme);
			break;
	}
}

// ------------------------ 打印错误信息
void ErrMsg(unsigned LineNo, char *descrip, char *string ) {

	#ifdef PARSER_DEBUG
		printf("行号 %5d:%s %s!\n", LineNo, descrip, string);
	#else
		char msg[256];
		memset(msg, 0, 256);
		sprintf(msg, "行号 %5d:%s %s!", LineNo, descrip, string);
	#endif

	#ifdef _VC_COMPILER
		MessageBox(NULL, msg, "错误!", MB_OK);
	#endif

	#ifdef _BC_COMPILER
		printf("%s\n", msg);
	#endif

	CloseScanner();

	exit(1);
}

// ------------------------ 先序遍历并打印表达式的语法树
void PrintSyntaxTree(struct ExprNode* root, int indent) {

int temp;

	for (temp=1; temp<=indent; temp++) printf("\t"); // 缩进

	switch(root->OpCode) { // 打印根节点
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
			printf("错误的树节点!\n"); 
			exit(0);
	}

	if(root->OpCode == CONST_ID || root->OpCode == T) return; // 叶子节点返回
	
	if(root->OpCode == FUNC) 
		// 递归打印一个孩子的节点
		PrintSyntaxTree(root->Content.CaseFunc.Child, indent+1);
	else {
		// 递归打印两个孩子的节点
		PrintSyntaxTree(root->Content.CaseOperator.Left, indent+1);
		PrintSyntaxTree(root->Content.CaseOperator.Right, indent+1);
	}

}

// ------------------------ 绘图语言解释器入口(与主程序的外边接口)
void Parser(char *SrcFilePtr) {

	enter("语法分析");
	
	// 初始化词法分析器
	if(!InitScanner(SrcFilePtr)) {
		printf("源程序文件打开错误!\n");
		return;
	}

	FetchToken(); // 获取第一个记号

	Program(); // 递归下降分析

	CloseScanner(); // 关闭词法分析器

	back("语法分析");

	return;
}

// ------------------------ Program 递归子程序
static void Program ()


{
	enter("<程序>");

	while (token.type != NONTOKEN) {
		Statement ();
		MatchToken (SEMICO);
	}

	back("<程序>");
}

// ------------------------ Statement 的递归子程序
static void Statement() {

	enter("<语句>");

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

	back("<语句>");
}

// ------------------------ OriginStatement 的递归子程序
static void OriginStatement (void) {

struct ExprNode *tmp;

	enter("<原点设置语句>");

	MatchToken (ORIGIN);		//匹配关键字 origin
	MatchToken (IS);			//匹配关键字 is
	MatchToken (L_BRACKET);		//匹配左括号

	tmp = Expression();			//分析横坐标表达式
	#ifndef PARSER_DEBUG
		Origin_x = GetExprValue(tmp); // 获取横坐标的平移距离
		DelExprTree(tmp);
	#endif

	MatchToken (COMMA);			//匹配逗号
	
	tmp = Expression();			//分析纵坐标表达式
	#ifndef PARSER_DEBUG
		Origin_y = GetExprValue(tmp); // 获取纵坐标的平移距离
		DelExprTree(tmp);
	#endif

	MatchToken (R_BRACKET);		//匹配右括号

	back("<原点设置语句>");
}

// ------------------------ ScaleStatement 的递归子程序
static void ScaleStatement (void) {

struct ExprNode *tmp;
	
	enter("<比例设置语句>");

	MatchToken (SCALE);			//匹配关键字 scale
	MatchToken (IS);			//匹配关键字 is
	MatchToken (L_BRACKET);		//匹配左括号

	tmp = Expression();			//分析横坐标比例因子表达式
	#ifndef PARSER_DEBUG
		Scale_x = GetExprValue(tmp); // 获取横坐标的比例因子
		DelExprTree(tmp);
	#endif

	MatchToken (COMMA);			//匹配逗号

	tmp = Expression();			//分析纵坐标比例因子表达式
	#ifndef PARSER_DEBUG
		Scale_y = GetExprValue(tmp); // 获取纵坐标的比例因子
		DelExprTree(tmp);
	#endif

	MatchToken (R_BRACKET);		//匹配右括号

	back("<比例设置语句>");
}

// ------------------------ RotStatement 的递归字程序
static void RotStatement (void) {
	struct ExprNode *tmp;

	enter("<旋转角度设置语句>");

	MatchToken (ROT);		//匹配关键字 rot
	MatchToken (IS);		//匹配关键字 is

	tmp = Expression();		//匹配角度表达式

	#ifndef PARSER_DEBUG
		Rot_angle = GetExprValue(tmp); // 获取旋转角度
		DelExprTree(tmp);
	#endif
	
	back("<旋转角度设置语句>");
}

// ------------------------ For 递归子程序
static void ForStatement (void) {

#ifndef PARSER_DEBUG
double Start, End, Step; // 绘图起点、终点、步长
#endif

struct ExprNode *start_ptr, *end_ptr, *step_ptr,*x_ptr,*y_ptr; // 各表达式语法树根节点指针
	
	enter("<循环绘图语句>");

	MatchToken (FOR);	
	call_match("FOR");
	
	MatchToken(T); 
	call_match("T");
	
	MatchToken (FROM); 
	call_match("FROM");

	start_ptr = Expression(); //分析起点表达式
	#ifndef PARSER_DEBUG
		Start = GetExprValue(start_ptr);	// 计算起点表达式值
		DelExprTree(start_ptr);				// 释放起点表达式语法树
	#endif

	MatchToken (TO);
	call_match("TO");

	end_ptr = Expression(); //分析终点表达式

	#ifndef PARSER_DEBUG
		End = GetExprValue(end_ptr); //计算终点表达式值
		DelExprTree(end_ptr);		// 释放终点表达式语法树
	#endif

	MatchToken (STEP);
	call_match("STEP");

	step_ptr = Expression(); // 分析步长表达式

	#ifndef PARSER_DEBUG
		Step = GetExprValue(step_ptr);	//计算步长表达式值
		DelExprTree(step_ptr);			//释放步长表达式语法树
	#endif

	MatchToken (DRAW);
	call_match("DRAW");

	MatchToken (L_BRACKET);
	call_match("(");

	x_ptr = Expression(); //分析横坐标表达式

	MatchToken(COMMA);
	call_match(",");

	y_ptr = Expression(); //分析纵坐标表达式

	MatchToken (R_BRACKET);
	call_match(")");

	#ifndef PARSER_DEBUG
		DrawLoop (Start, End, Step, x_ptr, y_ptr); // 绘制图形		
		DelExprTree(x_ptr); // 释放横坐标语法树
		DelExprTree(y_ptr); // 释放纵坐标语法树
	#endif

	back("<循环绘图语句>");
}

// ------------------------- Expression 的递归子程序
static struct ExprNode* Expression() {

struct ExprNode *left, *right; // 左右子树节点的指针
Token_Type token_tmp; // 当前记号

	enter("<表达式>");

	left = Term(); // 分析左操作数

	while (token.type == PLUS || token.type == MINUS) {

		token_tmp = token.type;

		MatchToken(token_tmp);	//

		right = Term();			//分析右操作数

		left = MakeExprNode(token_tmp, left, right);

	}

	Tree_trace(left); // 打印表达式语法树

	back("<表达式>");

	return left; // 返回表达式语法树指针
}

// ------------------------ Term 递归子程序
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

// ------------------------ Factor 递归子程序
static struct ExprNode * Factor () {
struct ExprNode *left, *right;

	if(token.type == PLUS) {	// 匹配一元加
		MatchToken(PLUS);
		right = Factor(); // 表达式退化为仅有有操作数的表达式

	} else if(token.type == MINUS) {	// 匹配一元减
	
		MatchToken(MINUS); // 表达式转化为二元减运算的表达式
		right = Factor();
		
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		
		right = MakeExprNode(MINUS, left, right);
	} else 
		right = Component(); // 匹配非终结符 Component

	return right;
}

// ------------------------ Component 递归子程序
static struct ExprNode* Component() {

struct ExprNode	*left, *right;

	left = Atom();

	if(token.type == POWER) {

		MatchToken(POWER);
		right = Component(); // 递归调用 ComPonent 以实现 POWER 的右结合
		
		left =MakeExprNode(POWER, left, right);
	}

	return left;
}

// ------------------------ Atom 递归子程序
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

// ----------------- 生成语法树节点
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...) {

struct ExprNode *ExprPtr = new (struct ExprNode);
va_list ArgPtr;

	ExprPtr->OpCode = opcode; // 接受记号的类别

	va_start(ArgPtr, opcode);

	switch(opcode) {	// 根据记号的类别构造不同的节点
		case CONST_ID: // 常数
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
