/*�����ܣ�����׺����ʽת��Ϊ��׺����ʽ�ķ�����
  �������룺�����������Լ�'+'��'-"���ɵ���׺����ʽ
            ����ʹ�ôʷ��������ܡ�
			����: �ڼ���������90-abc+23
  ԭʼ�ķ�������
           expr --> expr + term
                  | expr - term
                  | term
           term --> num | ID

  ������ݹ����﷨�Ƶ����壺
            ����ʽ                       �������
            expr --> term  rest       expr.syn = term.syn || rest.syn

            rest --> + term rest1     rest.syn = term.syn || '+' || rest1.syn
                   | - term rest1     rest.syn = term.syn || '-' || rest1.syn
   		           | ��               rest.syn = ""

            term --> num              term.syn =  tokenval;
                   | ID               term.syn = lexeme; 

  ˵�����ۺ����Ե�Ӣ�ķ���Ϊ: synthesized attribute
         expr.syn��ʾexpr��һ���ۺ�����Ϊsyn�������Դ洢expr�ĺ�׺����ʽ��
*/
#include <stdio.h>
#include <stdlib.h>
#include <string>
using std::string;

#include <ctype.h>

#define TKN_NUM  500
#define TKN_ID   600

int lineno = 1;
int LookAhead; //��ŵ�ǰ�Ĵʷ���Ԫ������
int tokenval = 0; char lexeme[1024];

int GetToken()
{
	int t, i;
	while (1) {
		t = getchar();
		if (t == ' ' || t == '\t') 
			;
		//else if (t == '\n')  //��Ϊ�ڸó����У��س�����Ϊ����ʽ�Ľ��������Բ��ܰѻس������ɿհ׷�
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
			return t;//����t=��+������ѡ�+����ASCII��Ϊ��+����TokenName��
		}
	}
}

void Match(int t)
{
	if( LookAhead==t ) 
		LookAhead = GetToken(); //������ǰ����һ���ʷ���Ԫ
    else {
		printf("\n����ʽ����:Matchʧ�ܡ�\n"); 
		system("pause");
		exit(1); //��������
	}

}



void Term(string & TermSyn)  
{char buf[100];
	if( LookAhead==TKN_NUM ) {
		sprintf(buf,"%d ", tokenval); //�ú�����tokenvalת����10���Ƶ��ַ���������buf�С�
		TermSyn = buf; Match(LookAhead);
	}
	else if( LookAhead==TKN_ID) {
		sprintf(buf,"%s ",lexeme);
		TermSyn = buf; Match(LookAhead);
	}
    else {
		printf("\n����ʽ����:������Ҫһ�������������\n" );
		system("pause");
		exit(1); //��������
	}
}

void Rest(string & RestSyn)  
{string term_syn,rest_syn;
	switch( LookAhead ) {
	    case '+':
			Match('+'); Term(term_syn); Rest(rest_syn); // rest.syn = term.syn || '+' || rest1.syn
			RestSyn = term_syn + "+ " + rest_syn;
			break;
		case '-':
			Match('-'); Term(term_syn); Rest(rest_syn); // rest.syn = term.syn || '-' || rest1.syn
			RestSyn = term_syn + "- " + rest_syn;
			break;
		default:   // rest --> ��   rest.syn = ""
			RestSyn = "";
			break;
	}
}

void Expr(string & ExprSyn) 
{string term_syn, rest_syn;
    Term(term_syn);
	Rest(rest_syn);
	ExprSyn = term_syn + rest_syn;
}

int main()
{string expr_syn;
	printf("��������׺����ʽ��");
	LookAhead = GetToken();

	printf("���׺����ʽΪ��");
	Expr(expr_syn);
    printf( "%s", expr_syn.c_str() );

	if( LookAhead !='\n' ) {
		printf("\n����ı���ʽ����\n");
		exit(1);
	}

	printf("\n����ʽ�����ɹ���\n");
	system("pause");
}