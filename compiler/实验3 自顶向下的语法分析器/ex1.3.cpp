/**
* expr			-> expr + term
*				 | expr - term
*				 | term
* 
* term			->  term * factor
*				 |  term / factor
*				 |  factor
* factor		-> ( expr )
*				 | id
*				 | num
* 
* ------------------------------After translation:
* expr			-> term rest
* term			-> factor restfactor
* 
* rest			-> + term print{'+ '} rest
*				 | - term print{'- '} rest
*				 | NULL
* restfactor	-> * factor print{'* '} restfactor
*				 | / factor print{'/ '} restfactor
*				 | NULL
* factor		-> ( expr ) {print(' expr ')}
*				 |  id {print(idVal)}
*				 |  num {print(tokenVal)}
* 
* ----------example:
* 3 + 6*9
* 35 +3 / 6 - abc +(3+ 45) 
* 2+3+3+4 *7
*
*/

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define TKN_NUM 1000
#define TKN_ID 1500


int LookAhead;
int preLook;
int tokenVal;
char idVal[200] ;
/**
* 错误点1： LookAhead | t 为 int 
*/

int GetToken() {
	//char t;
	/**
	* 错误点2：没有ungetc()
	*/
	int t;
	int i;
	while (1) {
		t = getchar();
		if (t == ' ' || t == '\t') {
			;
		}
		else if (isdigit(t)) {
			tokenVal = 0;
			do {
				tokenVal = tokenVal * 10 + t - '0';
				t = getchar();
			} while (isdigit(t));
			ungetc(t, stdin);
			return TKN_NUM;
		}
		else if (isalpha(t)) {
			i = 0;
			do {
				idVal[i++] = t;
				t = getchar();
			} while (isalpha(t) || isdigit(t));
			idVal[i] = '\0';
			/**
			* 错误点3： 没有 = '\0'
			*/
			ungetc(t, stdin);
			return TKN_ID;
		}
		else {
			tokenVal = 0;
			return t;
		}
	}
}
void Expr();

void Match(int t) {
	if (LookAhead == t) {
		LookAhead = GetToken();
	}
	else {
		printf("\nError\n");
		exit(1);
	}
}
void Factor() {
	if (LookAhead == '(') {
		Match('(');
		Expr();
		Match(')');
	}
	else if (LookAhead==TKN_NUM )  {
		printf("%d ", tokenVal); Match(LookAhead);
	}
	else if (LookAhead==TKN_ID ) {
		printf("%s ", idVal); Match(LookAhead);
	}
	else {
		printf("Error: in term\n");
		exit(1);
	}
}
void RestFactor() {
	switch (LookAhead)
	{
	case '*':
		Match('*'); Factor(); printf("* "); RestFactor();
		break;
	case '/':
		Match('/'); Factor(); printf("/ "); RestFactor();
		break;
	default:
		break;
	}
}

void Term() {
	Factor();
	RestFactor();
}

void Rest() {
	switch (LookAhead)
	{
	case '+':
		Match('+'); Term(); printf("+ "); Rest();
		break;
	case '-':
		Match('-'); Term(); printf("- "); Rest();
		break;
	default:
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
	printf("->");
	if (LookAhead = GetToken () ) {
		Expr();
		
		if (LookAhead != '\n') {
			printf("\nError:in main\n");
			exit(1);
		}
	}
	else {
		printf("\nError: None\n");
	}
	printf("\nSuccess!\n");
	system("pause");

}