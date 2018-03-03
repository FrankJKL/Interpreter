
// ----------------------------- scanner.cpp --------------------------------

#include "scanner.h"

#define TOKEN_LEN 100 			// �Ǻ���󳤶�

static char TokenBuffer[TOKEN_LEN];	// �Ǻ��ַ�����

static FILE *InFile; // �����ļ���
unsigned int LineNo; // ����Դ�ļ��к�


// ----------------- ��ʼ���ʷ�������
extern int InitScanner (const char *FileName) { 

	LineNo = 1;

	InFile = fopen (FileName, "r");

	if (InFile != NULL) 
		return 1;
	else 
		return 0;
}

// ----------------- �رմʷ�������
extern void CloseScanner (void) { 

	if (InFile != NULL) fclose (InFile);

}

// ----------------- ������Դ�����ļ����ж���һ���ַ�
static char GetChar(void) { 

int Char;

	Char = getc (InFile);
	return toupper(Char);	// ת��Ϊ��д
}

// ----------------- ��Ԥ�����ַ��˻ص�����Դ������ļ�����
static void BackChar(char Char) { 

	if (Char != EOF) ungetc (Char, InFile);

}

// ----------------- �����ַ����ǺŻ�����
static void AddCharTokenString (char Char) { 

int TokenLength;

	TokenLength = strlen (TokenBuffer);

	if (TokenLength + 1 >= sizeof (TokenBuffer)) return;

	TokenBuffer[TokenLength ] = Char;
	TokenBuffer[TokenLength+ 1] = '\0';
}

// ----------------- ��ռǺŻ�����
static void EmptyTokenString () { 

	memset(TokenBuffer, 0, TOKEN_LEN); 

}

// -----------------�ж��ַ����Ƿ��ڷ��ű���
static Token JudgeKeyToken(const char * IDString) { 

int loop;
Token errortoken;

	for (loop = 0; loop < sizeof (TokenTab) / sizeof(TokenTab[0]); loop ++) {
		if (strcmp (TokenTab[loop].lexeme, IDString) == 0) return TokenTab[loop];
	}

	memset(&errortoken, 0, sizeof(Token));
	errortoken.type = ERRTOKEN;

	return errortoken;
}

// ----------------- ��ȡһ���Ǻ�
extern Token GetToken (void) {

Token token;
int Char;

	memset(&token, 0, sizeof(Token));

	EmptyTokenString();

	token.lexeme = TokenBuffer;

	for (;;) { 							// ����Դ�����еĿո�TAB���س��ȣ������ļ����������ؿռǺ�

		Char = GetChar();

		if (Char == EOF) {
			token.type = NONTOKEN;
			return token;
		}

		if (Char == '\n') LineNo ++;

		if (!isspace(Char)) break;
	} // end of for

	AddCharTokenString (Char);	// �����ǿո�TAB���ļ��������ȣ����ȼ��뵽�Ǻŵ��ַ���������

	if(isalpha(Char)) { 		// �� char��A-Za-z, ��һ���Ǻ������ؼ��֡�����PI��E��

		for (;;) {

			Char = GetChar ();
			
			if (isalnum(Char) ) 
				AddCharTokenString (Char);
			else
				break;
		}

		BackChar (Char);
		
		token = JudgeKeyToken (TokenBuffer);
		token.lexeme = TokenBuffer;

		return token;

	} else if(isdigit(Char)) {	// ����һ�����֣���һ���ǳ���

		for (;;) {
			
			Char = GetChar ();			

			if (isdigit(Char)) 
				AddCharTokenString (Char);
			else 
				break;
		}

		if (Char == '.') {

			AddCharTokenString (Char);

			for (;;) {
				
				Char = GetChar();
				if (isdigit(Char)) 
					AddCharTokenString (Char);
				else 
					break;
			}
		} // end of if (Char == '.')

		BackChar (Char);

		token.type = CONST_ID;
		token.value = atof(TokenBuffer);

		return token;

	} else {	// ������ĸ�����֣���һ����������Ż�ָ���

		switch (Char) {
			case ';':
				token.type = SEMICO; break;
			case '(':
				token.type = L_BRACKET; break;
			case ')':
				token.type = R_BRACKET; break;
			case ',':
				token.type = COMMA; break;
			case '+':
				token.type = PLUS; break;
			case '-':

				Char = GetChar();

				if (Char =='-') {

					while (Char != '\n' && Char != EOF) Char = GetChar();

					BackChar(Char);

					return GetToken();

				} else {

					BackChar(Char);
		
					token.type = MINUS;

					break; 
				}
			case '/':
				Char = GetChar();

				if (Char =='/') {
		
					while (Char != '\n' && Char != EOF) Char = GetChar();

					BackChar(Char);

					return GetToken();
				} else {

					BackChar(Char);

					token.type = DIV;

					break;
				}
			case '*':
				Char = GetChar();
	
				if (Char == '*') {
	
					token.type = POWER;
	
					break;
		
				} else {
		
					BackChar (Char);
	
					token.type = MUL;
	
					break;
				}

			default:
				token.type = ERRTOKEN;
				break;
		} // end of switch

	} // end of else(������ĸ�����֣���һ���Ƿ���)
	
	return token;

} // end of GetTOken
