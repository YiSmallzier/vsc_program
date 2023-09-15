/*程序功能：将中缀表达式转换为后缀表达式的翻译器
  程序输入：常数、变量以及'+'和'-"构成的中缀表达式
            程序使用词法分析功能。
			例如: 在键盘上输入90-abc+23
  原始文法描述：
           expr --> expr + term
                  | expr - term
                  | term
           
		   term -->  term * factor
			      |  term / factor
				  |  factor
		   
		   factor --> num | ID
		   

  消除左递归后的语法制导的翻译方案：
            expr --> term  rest 

            rest --> factor term
			       + | -  -> mul | div
                   * | / -> 
			
			term --> * term {print('*')} rest
			       | / term {print('/')} rest
				   | factor

            factor --> ( expr ) 
			         | num  {print(tokenval)}
                     | ID   {print(lexeme)}
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define TKN_NUM  500
#define TKN_ID   600

int lineno = 1;
int LookAhead; //存放当前的词法单元的类型
int tokenval = 0; char lexeme[1024];

void Rest();
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
				tokenval = tokenval * 10 + t -'0';
				t = getchar();
			} while (isdigit(t));
			ungetc(t, stdin);
			return TKN_NUM;
		}
        else if( isalpha(t) ) {
            i=0; 
            do {
                lexeme[i++]=t; t = getchar(); 
            }while( isalpha(t) || isdigit(t) );
            lexeme[i]='\0'; ungetc(t, stdin);
            return TKN_ID;
        }
		else {
			tokenval = 0;
			return t;//例如t=’+’，则把’+’的ASCII作为’+’的TokenName。
		}
	}
}

void Match(int t)
{
	if( LookAhead==t ) 
		LookAhead = GetToken(); //继续往前看后一个词法单元
    else {
		printf("\n表达式错误：\n"); 
		exit(1); //结束程序
	}

}

void TermFactor()
{
	if( LookAhead==TKN_NUM) {
		printf("%d ",tokenval); Match(LookAhead); 
	}
	else if( LookAhead==TKN_ID) {
		printf("%s ",lexeme); Match(LookAhead);
	}
    else {
		printf("\n表达式错误：Term函数中需要输入整数或变量\n" );
		exit(1); //结束程序
	}
}

void Term()
{
	switch (LookAhead) {
		case '*':
			Match('*'); TermFactor(); printf("* "); Term(); // rest --> + term {print('+')} rest
			break;
		case '/':
			Match('/'); TermFactor(); printf("/ "); Term(); // rest --> - term {print('-')} rest
			break;
		default:
			TermFactor();
			break;
	}
}

void Rest()
{
	//Term();
	//TermFactor(); //Factor
	switch( LookAhead ) {
	    case '+':
			Match('+'); Term(); printf("+ "); Rest(); // rest --> + term {print('+')} rest
			break;
		case '-':
			Match('-'); Term(); printf("- "); Rest(); // rest --> - term {print('-')} rest
			break;
		case '*':
			//获取前一个Lookahead()
			;
			Match('*'); Term(); printf("* "); Rest();
		case '/':
			//same in *
			; Match('/'); Term(); printf("/ "); Rest();

		default:   // rest --> 空
			break;
	}
}

void Expr()
{
    Term();
	Rest();
}

void main()
{
	printf("请输入中缀表达式：");
	LookAhead = GetToken();

	printf("其后缀表达式为：");
	Expr();
    
	if( LookAhead !='\n' ) {
		printf("\n输入的表达式错误.\n");
		exit(1);
	}

	printf("\n表达式分析成功！\n");
	system("pause");

}