
%{

#include "lex.yy.h"

/*����ʹ�øñ���yylval�����ñ�������flex�Դ��ģ���������Ҫ�Լ������������?*/
int yylval;
int lineNo=1;

/*��ο�������ҳ�е�˵��?*/
/* http://www.ibm.com/developerworks/cn/linux/sdk/lex/index.html */

%}

delim    [" "\t]
ws       {delim}+
letter   [A-Za-z]
digit    [0-9]
id       {letter}({letter}|{digit})*
number   {digit}+(\.{digit}+)?(E[+-]?{digit}+)?

%%

{ws}       {/*no action and no return */ }
"\n"       { lineNo++; }
if          { return (IF_TKN); }
else        { return (ELSE_TKN); }
while       { return (WHILE_TKN); }
do          { return (DO_TKN); }
break       { return (BREAK_TKN); }
true        { return (TRUE_TKN); }
false       { return (FALSE_TKN); }

{id}       { return (ID_TKN); }
{number}   { return (NUM_TKN); } 

"<="       {  return(RELOP_TKN); }
"=="       {  return(RELOP_TKN); }
"!="       {  return(RELOP_TKN); }
">="       {  return(RELOP_TKN); }

[\/][\*]([^\*])*[\*]([^\*\/](([^\*])*)[\*]|[\*])*(\/)  { printf("Remark: %s\n",yytext); }

.          { printf("TKN:%s\n",yytext); return yytext[0]; }

%%

/*�ú�������yyin������fflex��yyin��������Ӧ���ļ����дʷ�����*/
void BeginCompileOneFile( const char * filename )
{  
   yyin = fopen( filename, "r");
   fseek( yyin, 0, SEEK_SET );

}

void EndCompileOneFile(void) 
{
   fclose(yyin);
   yyin = 0;
}

int yywrap()
{
	return 1;
}

/*���أ��ʷ�������ǰ���������кţ�һ���ھ䷨��������ʱʹ��*/
int GetLineNo(void)
{
    return lineNo;
}










