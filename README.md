# Interpreter
编译原理 前端解释器
## 函数绘图语言的语法介绍
rot is 0; --旋转角度
origin is (50,400);--原点坐标 
scale is (2,1);--横纵坐标的大小比例

for T from 0 to 300 step 1 draw(t,0); --横坐标轴
for T from 0 to 300 step 1 draw(0,-t); --纵坐标轴

for T from 0 to 300 step 1 draw(t,-t); --f(t)=t
当执行rot is 0;这句代码时，得到Rot_Angle的值，执行origin is (50,400);函数时得到Origin_x,Origin_y的值。执行scale is (2,1);函数时得到Scale_x,Scale_y的值，最后在执行绘图函数时才会成功。不管前三条语句出现的顺序如何，总是比例-》旋转-》平移。最后再画出图形。

## 函数绘图语言的文法变换
由于我们最终构造的是递归下降的语法分析器，要求文法是LL1(1)的，也即无二义性、无左递归、无公共左因子。
最原始的文法G1：
Program -> ε | Program Statement SEMICO
Statement -> OriginStatement | ScaleStatement RotStatement | ForStatement --分别对应上面例子中的前四条语句
## 文件说明
scanner.h：定义了词法分析所需要的Token_Type、Token数据结构、符号表等</br>
scanner.cpp：定义了词法分析器与文件的交互方法，主要是通过**GetToken**方法(有穷自动机)来识别文件中的Token序列</br>
parser.h：定义了抽象语法树的节点类型和语法分析器对外的接口。抽象语法树的节点类型就已经大概描绘了程序的语法结构了</br>
parser.cpp：定义了一些全局变量，这些全局变量会在语法制导翻译过程中赋值，就以设置绘图原点坐标的语句为例</br>
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
enter和back是宏定义，在控制台打印输出。
