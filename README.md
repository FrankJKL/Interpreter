# Interpreter
编译原理 前端解释器
## 文件说明
scanner.h：定义了词法分析所需要的Token_Type、Token数据结构、符号表等</br>
scanner.cpp：定义了词法分析器与文件的交互方法，主要是通过**GetToken**方法(有穷自动机)来识别文件中的Token序列</br>


