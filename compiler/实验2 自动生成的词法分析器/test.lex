
%{
#include <stdio.h>
#include <stdlib.h>


#define ID_TKN      500
#define NUM_TKN     510

#define RELOP_TKN   520
#define RELOP_LT    521
#define RELOP_LE    522
#define RELOP_EQ    523
#define RELOP_NE    524
#define RELOP_GT    525
#define RELOP_GE    526
#define RELOP_ADD   528
#define RELOP_SUB   530
#define RELOP_MUL   532
#define RELOP_DIV   534
#define RELOP_JE    536
#define RELOP_JNE   538
#define RELOP_SP    540
#define RELOP_CM    542
#define RELOP_LP    544
#define RELOP_RP    546
#define RELOP_LBR   560
#define RELOP_RBR   562
#define RELOP_LB    548
#define RELOP_RB    550

#define IF_TKN      610
#define ELSE_TKN    612

#define WHILE_TKN   614
#define DO_TKN      616    
#define BREAK_TKN   618    
#define FLOAT_TKN   620  
#define TRUE_TKN    622
#define FALSE_TKN   624
#define INT_TKN     626
#define CHAR_TKN    630
#define BOOL_TKN    632

#define BASIC_TKN   700 
/*书上使用该变量yylval，但该变量不是flex自带的，所以我们要自己定义这个变量*/
int yylval;

/*该函数可以将变量添加到符号表中，在这里我们没有实现该功能*/
int installID() ;

/*该函数可以将数值添加到数值表中，在这里我们没有实现该功能*/
int installNum() ;

/*请参考下面网页中的说明*/
/* http://www.ibm.com/developerworks/cn/linux/sdk/lex/index.html */

%}

delim    [" "\t\n]
ws       {delim}+
letter   [A-Za-z]
digit    [0-9]
basic    "int"|"float"|"bool"|"char"
id       {letter}({letter}|{digit})*
number   {digit}+(\.{digit}+)?(E[+-]?{digit}+)?



%%

{ws}        {/*no action and no return */ }
if          { printf("IF:%s\n",yytext);   return (IF_TKN); }
else        { printf("ELSE:%s\n",yytext); return (ELSE_TKN); }
while       { printf("WHILE:%s\n",yytext); return (WHILE_TKN); }
do          { printf("DO:%s\n",yytext); return (DO_TKN); }
break       { printf("BREAK:%s\n",yytext); return (BREAK_TKN); }
true        { printf("TRUE:%s\n",yytext); return (TRUE_TKN); }
false       { printf("FALSE:%s\n",yytext); return (FALSE_TKN); }
{basic}     { yylval =  printf("BASIC:%s\n", yytext); return (BASIC_TKN);}
{id}        { yylval = (int) installID(); printf("ID:%s\n",yytext); return (ID_TKN); }
{number}    { yylval = (int) installNum();printf("Num:%s\n",yytext); return (NUM_TKN); } 
"+"         { yylval = RELOP_ADD; printf("ADD:%s\n",yytext);  return(RELOP_TKN); }
"-"         { yylval = RELOP_SUB; printf("SUB:%s\n",yytext);  return(RELOP_TKN); }
"*"         { yylval = RELOP_MUL; printf("MUL:%s\n",yytext);  return(RELOP_TKN); }
"/"         { yylval = RELOP_DIV; printf("DIV:%s\n",yytext);  return(RELOP_TKN); }
";"         { yylval = RELOP_SP; printf(";:%s\n",yytext);  return(RELOP_TKN); }
","         { yylval = RELOP_CM; printf(",:%s\n",yytext);  return(RELOP_TKN); }
"("         { yylval = RELOP_LP; printf("(:%s\n",yytext);  return(RELOP_TKN); }
")"         { yylval = RELOP_RP; printf("):%s\n",yytext);  return(RELOP_TKN); }
"["         { yylval = RELOP_LBR; printf("[:%s\n",yytext);  return(RELOP_TKN); }
"]"         { yylval = RELOP_RBR; printf("]:%s\n",yytext);  return(RELOP_TKN); }
"{"         { yylval = RELOP_LB; printf("{:%s\n",yytext);  return(RELOP_TKN); }
"}"         { yylval = RELOP_RB; printf("}:%s\n",yytext);  return(RELOP_TKN); }
"<"         { yylval = RELOP_LT; printf("<:%s\n",yytext);  return(RELOP_TKN); }
">"         { yylval = RELOP_GT; printf(">:%s\n",yytext);  return(RELOP_TKN); }
"="         { yylval = RELOP_EQ; printf("=:%s\n",yytext);  return(RELOP_TKN); }
"<="        { yylval = RELOP_LE; printf("<=:%s\n",yytext);  return(RELOP_TKN); }
"=="        { yylval = RELOP_JE; printf("==:%s\n",yytext);  return(RELOP_TKN); }
"!="        { yylval = RELOP_JNE; printf("!=:%s\n",yytext);  return(RELOP_TKN); }
"<>"        { yylval = RELOP_NE; printf("<>:%s\n",yytext);  return(RELOP_TKN); }
">="        { yylval = RELOP_GE; printf(">=:%s\n",yytext);  return(RELOP_TKN); }

[\/][\*]([^\*])*[\*]([^\*\/](([^\*])*)[\*]|[\*])*(\/)  { printf("Remark:%s\n",yytext); }
[\/][\/]([^\n])*(\n)      {printf("Remark:%s", yytext);}   
.          { printf("%c:%c\n",yytext[0],yytext[0]); return yytext[0]; }

%%
/*该函数设置yyin变量，fflex对yyin变量所对应的文件进行词法分析*/
void BeginCompileOneFile( const char * filename )
{  
   yyin = fopen( filename, "r");
   fseek( yyin, 0, SEEK_SET );

/* 缺省情况下，yyin 和 yyout 都指向标准输入和输出。
   yyout = fopen( outputfilename, "w");
   fseek( yyout, 0, SEEK_SET );
*/
}

void EndCompileOneFile(void) 
{
   fclose(yyin);
   yyin = 0;
}

/*如果函数的返回值是1，就停止解析。 因此它可以用来解析多个文件。*/
int yywrap()
{
	return 1;
}

int installID() 
{
   return 1;
}

int installNum() 
{
   return 1;
}


/*因为lex.yy.c是C语言写的，下面这些函数是以C语言的函数调用方式编译的
#ifdef __cplusplus
    extern "C" int yylex(void);
    extern "C" void BeginCompileOneFile( const char * filename );
    extern "C" void EndCompileOneFile( void);
#endif
*/


int main()
{ 
   int token;
   char filename[1000];

	printf("请输入要编译的源程序文件名："); gets(filename);
	BeginCompileOneFile( filename );

	//当flex扫描到文件末尾，yylex函数返回0
    while( ( token = yylex() ) > 0 ) ;

	EndCompileOneFile();

	getchar();
   return 0;
}









