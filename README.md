# Interpreter
编译原理 前端解释器
## 函数绘图语言的语法介绍
rot is 0; --旋转角度</br>
origin is (50,400);--原点坐标</br> 
scale is (2,1);--横纵坐标的大小比例</br>
</br>
for T from 0 to 300 step 1 draw(t,0); --横坐标轴</br>
for T from 0 to 300 step 1 draw(0,-t); --纵坐标轴</br>
</br>
for T from 0 to 300 step 1 draw(t,-t); --f(t)=t</br>
当执行rot is 0;这句代码时，得到Rot_Angle的值，执行origin is (50,400);函数时得到Origin_x,Origin_y的值。执行scale is (2,1);函数时得到Scale_x,Scale_y的值，最后在执行绘图函数时才会成功。不管前三条语句出现的顺序如何，总是比例-》旋转-》平移。最后再画出图形。</br>

## 函数绘图语言的文法变换
由于我们最终构造的是递归下降的语法分析器，要求文法是LL1(1)的，也即无二义性、无左递归、无公共左因子。</br>
最原始的文法G1：</br>
Program -> ε | Program Statement SEMICO</br>
Statement -> OriginStatement | ScaleStatement | RotStatement | ForStatement --分别对应上面例子中的前四条语句</br>
OriginStatement -> ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
ScaleStatement -> SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
RotStatement -> ROT IS Expression</br>
ForStatement -> FOR T FROM Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET</br>
-- 前面是句子的类型，下面是表达式的文法
Expression -> Expression PLUS Expression </br>
	    | Expression MINUS Expression</br>
	    | Expression MUL Expression</br>
	    | Expression DIV Expression</br>
	    | PLUS Expression</br>
	    | MINUS Expression</br>
	    | Expression POWER Expression</br>
	    | CONST_ID</br>
	    | T</br>
	    | FUNC L_BRACKET Expression COMMA Expression R_BRACKET</br>
	    | L_BRACKET Expression COMMA Expression R_BRACKET</br>

以上的文法G1给出了Expression所有的所有形式，但是没有区分表达式之间的优先级，即所有的候选项均具有同等的指导分析的权利，从而造成有些分析步骤的不确定性，因此G1是一个二义文法。为了改写文法，首先根据运算的优先级将Expression的所有候选项分组，具有相同优先级的候选项被分在同一个组中，并为每一个组引入一个非终结符。二元加减优先级最低，他们对应的非终结符沿用Expression；二元乘除运算应该高一级，为他们引入新的非终结符Term;依次类推。得到无二义的文法G2。
文法G2：
Program -> ε | Program Statement SEMICO</br>
Statement -> OriginStatement | ScaleStatement | RotStatement | ForStatement</br>
OriginStatement -> ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
ScaleStatement -> SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
RotStatement -> ROT IS Expression</br>
ForStatement -> FOR T FROM Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET</br>
Expression -> Expression PLUS Term </br>
	    | Expression MINUS Term</br>
	    | Term	 </br>
Term ->	Term MUL Factor	 </br>   
      | Term DIV Factor</br>
      | Factor</br>
Factor -> PLUS Factor</br>
	| MINUS Factor</br>
	| Component</br>
Component -> Atom POWER Component</br>
	   | Atom 	</br>
Atom ->	CONST_ID</br>
      | T</br>
      | FUNC L_BRACKET Expression COMMA Expression R_BRACKET</br>
      | L_BRACKET Expression COMMA Expression R_BRACK</br>

消除左递归和公共左因子G3：
Program -> Statement SEMICO Program | ε</br>
Statement -> OriginStatement | ScaleStatement | RotStatement | ForStatement</br>
OriginStatement -> ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
ScaleStatement -> SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
RotStatement -> ROT IS Expression</br>
ForStatement -> FOR T FROM Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET</br>
Expression -> Term Expression'</br>
Expression' -> PLUS Term Expression' | MINUS Term Expression' | ε</br>	
Term ->	Factor Term'</br>
Term' -> MUL Factor Term' | DIV Factor Term'| ε </br>	    
Factor -> PLUS Factor | MINUS Factor | Component</br>
Component -> Atom POWER Component | Atom </br>	
Atom ->	CONST_ID</br>
      | T</br>
      | FUNC L_BRACKET Expression COMMA Expression R_BRACKET</br>
      | L_BRACKET Expression COMMA Expression R_BRACK</br>

转换成适合写递归下降子程序的文法G4:
Program -> {Statement SEMICO} -- 一个程序由多条语句组成</br>
Statement -> OriginStatement | ScaleStatement | RotStatement | ForStatement</br>
OriginStatement -> ORIGIN IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
ScaleStatement -> SCALE IS L_BRACKET Expression COMMA Expression R_BRACKET</br>
RotStatement -> ROT IS Expression</br>
ForStatement -> FOR T FROM Expression TO Expression STEP Expression DRAW L_BRACKET Expression COMMA Expression R_BRACKET</br>
Expression -> Term {（PLUS|MINUS）Term }	</br>
Term ->	Factor {（MUL|DIV）Factor }    </br>
Factor -> PLUS Factor | MINUS Factor | Component</br>
Component -> Atom POWER Component | Atom 	</br>
Atom ->	CONST_ID</br>
      | T</br>
      | FUNC L_BRACKET Expression COMMA Expression R_BRACKET</br>
      | L_BRACKET Expression COMMA Expression R_BRACK</br>

按照此文法来写递归下降子程序，并构造抽象语法树。</br>

## 文件说明
scanner.h：定义了词法分析所需要的Token_Type、Token数据结构、符号表等</br>
scanner.cpp：定义了词法分析器与文件的交互方法，主要是通过**GetToken**方法(有穷自动机)来识别文件中的Token序列</br>
parser.h：定义了抽象语法树的节点类型和语法分析器对外的接口。抽象语法树的节点类型就已经大概描绘了程序的语法结构了</br>
parser.cpp：包含语法指导翻译的过程和一些全局变量，这些全局变量会在语法制导翻译过程中赋值，作为最后循环绘制图形时的参数。</br>
就以设置绘图原点坐标的语句为例</br>
static void OriginStatement (void) {

  	struct ExprNode *tmp;

	enter("<原点设置语句>");

	MatchToken (ORIGIN);		//匹配关键字 origin
	MatchToken (IS);			//匹配关键字 is
	MatchToken (L_BRACKET);		//匹配左括号

	tmp = Expression();			//分析横坐标表达式
	#ifndef PARSER_DEBUG
		**Origin_x** = GetExprValue(tmp); // 获取横坐标的平移距离
		DelExprTree(tmp);
	#endif

	MatchToken (COMMA);			//匹配逗号
	
	tmp = Expression();			//分析纵坐标表达式
	#ifndef PARSER_DEBUG
		**Origin_y** = GetExprValue(tmp); // 获取纵坐标的平移距离
		DelExprTree(tmp);
	#endif

	MatchToken (R_BRACKET);		//匹配右括号

	back("<原点设置语句>");
}
enter和back是宏定义，在控制台打印输出。

semantic.h：与图形绘制、获取抽象语法树的值 相关的函数定义</br>
semantic.cpp：与图形绘制、获取抽象语法树的值 相关的函数实现</br>
main.cpp：编译器的主程序
