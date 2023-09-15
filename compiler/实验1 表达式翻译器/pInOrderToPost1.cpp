/*程序功能：将中缀表达式转换为后缀表达式的翻译器
  程序输入：单个数字(0~9)以及'+'和'-"构成的中缀表达式
            为了简化程序而忽略词法分析功能，所以要求表达式中不能有空格等分隔符。
			例如: 在键盘上输入9-5+2
  原始文法描述：
           expr --> expr + term
                  | expr - term
                  | term
           term --> 0
                  | 1
                  | 2
	              ......
                  | 9

  消除左递归后的语法制导的翻译方案：
            expr --> term  rest 

            rest --> + term {print('+')} rest
                   | - term {print('-')} rest
   		           | 空

            term --> 0  {print('0')}
                   | 1  {print('1')}
                   | 2  {print('2')}
		              ......
                   | 9  {print('9')} 
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define TKN_NUM  500
#define TKN_ID   600

int lineno = 1;
int LookAhead; //存放当前的词法单元的类型
int tokenval = 0; char lexeme[1024];

int GetToken()
{
	int t, i;
	while (1) {
		t = getchar();
		if (t == ' ' || t == '\t')
			;
		//else if (t == '\n')  //因为在该程序中，回车符作为表达式的结束，所以不能把回车符当成空白符
			//lineno++;
		else if (isdigit(t)) {
			tokenval = 0;
			do {
				tokenval = tokenval * 10 + t - '0';
				t = getchar();
			} while (isdigit(t));
			ungetc(t, stdin);
			return TKN_NUM;
		}
		else if (isalpha(t)) {
			i = 0;
			do {
				lexeme[i++] = t; t = getchar();
			} while (isalpha(t) || isdigit(t));
			lexeme[i] = '\0'; ungetc(t, stdin);
			return TKN_ID;
		}
		else {
			tokenval = 0;
			return t;//例如t=’+’，则把’+’的ASCII作为’+’的TokenName。
		}
	}
}

void Match(char t)
{
	if( LookAhead==t ) 
		LookAhead = GetToken(); //继续往前看后一个字符
    else {
		printf("\n表达式错误：\n");
		exit(1); //结束程序
	}

}

void Term()
{
	if (LookAhead == TKN_NUM) {
		printf("%d ", tokenval); Match(LookAhead);
	}
	else if (LookAhead == TKN_ID) {
		printf("%s ", lexeme); Match(LookAhead);
	}
	else {
		printf("\n表达式错误：Term函数中需要输入整数或变量\n");
		exit(1); //结束程序
	}

}

void Rest()
{
	switch( LookAhead ) {
	    case '+':
			Match('+'); Term(); putchar('+'); Rest(); // rest --> + term {print('+')} rest
			break;
		case '-':
			Match('-'); Term(); putchar('-'); Rest(); // rest --> - term {print('-')} rest
			break;
		default:   // rest --> 空
			break;
	}
}

void Expr()
{
    Term();
	Rest();
}

int main()
{
	printf("请输入中缀表达式：");
	LookAhead = getchar();

	printf("其后缀表达式为：");
	Expr();
    
	if( LookAhead !='\n' ) {
	    //例如：3+45
		printf("\n输入的表达式错误，错误的字符：%c\n", LookAhead);
		exit(1);
	}

	printf("\n表达式分析成功！\n");

}