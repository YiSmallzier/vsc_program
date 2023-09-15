%{ 

#include <ctype.h>
#include <stdio.h>

/*YYSTYPE是属性栈的元素的类型，该类型由我们定义,yyparse会使用该类型创建属性栈等等*/
#define YYSTYPE double

int successFlag = 1;

/*yyerror函数是yacc规定的一个函数，在yyparse函数中会调用该函数，但该函数的定义是我们提供的。该函数如下*/
void yyerror( char * ErrStr )
{
    printf("ERROR:%s\n", ErrStr);
}

%}

%start  program

%token NUM_TKN
%token ID_TKN
%token IF_TKN
%token WHILE_TKN
%token DO_TKN
%token BREAK_TKN
%token ELSE_TKN
%token RELOP_TKN
%token TRUE_TKN
%token FALSE_TKN
%token BASIC_TKN

%left   '+' '-'
%left   '*' '/'
%left   '>' '<'


%%
program : block {
            printf("program->block\n");}
        ;
block   : '{' stmts '}' {
            printf("block->{stmts}\n");}
        ;
stmts   : stmt stmts {
            printf("stmts->stmt stmts\n");}
        | {printf("stmt-> null\n");}
        ;
stmt    : matched_stmt  {
            printf("stmt-> matched_stmt\n");    }
        | unmatched_stmt {
            printf("stmt-> unmatched_stmt\n");  }
        ;
matched_stmt: 
        BREAK_TKN ';'{
            printf("stmt->break\n");    }
        |   block {
            printf("stmt->block\n");    }
        |   ID_TKN '=' expr ';' {
            printf("stmt->id = expr\n");   }
        |   DO_TKN matched_stmt WHILE_TKN '(' bool ')' ';' {
            printf("stmt->do (bool) while stmt\n");   }
        |   WHILE_TKN '(' bool ')' matched_stmt {
            printf("stmt->while (bool) stmt\n");   }
        |   IF_TKN '(' bool ')' matched_stmt ELSE_TKN matched_stmt {
            printf("match -> if (bool) match else match\n");    }
        ;
unmatched_stmt:  
        IF_TKN '(' bool ')' stmt; {
            printf("stmt -> if (bool) stmt\n");   }
        |   IF_TKN '(' bool ')' matched_stmt ELSE_TKN unmatched_stmt {
            printf("unmatch -> if (bool) match else unmatch\n" );    }
        ;
bool    : expr RELOP_TKN expr  {
            printf("bool->expr relop expr\n");  }    
        | expr {
            printf("bool->expr\n");  }
        ;
expr    : expr '+' term {
            printf("expr->expr + term\n"); }
        | expr '-' term {
            printf("expr->expr - term\n"); }
        | term {
            printf("expr->term\n");}
        ;
term    : term '*' factor { 
            printf("term->term * factor\n"); }
        | term '/' factor {
            printf("term->term / factor\n"); }
        | factor {
            printf("term->factor\n");  }
factor: | '(' expr ')'  { 
            printf("factor->(expr)\n");}
        | ID_TKN {
            printf("factor -> id\n");   }
        | NUM_TKN    {
            printf("factor->num\n"); 
            /* "%f"和"%g"都是输出浮点数，"%g"不会输出多余的0, */ }
      ;
      
%%

/*如果把lex.yy.c包含在y.tab.c中，在工程中就只需要y.tab.c，不要再有lex.yy.c，否则yylex等函数会重复定义*/
#include "lex.yy.c"


int yyparse();  /*main函数要调用yyparse()函数，但该函数的定义在后面，所以要先声明(才能引用)*/

int main()
{ 
      char filename[1000];

	printf("Filename:"); gets(filename);
      /*因为lex.yy.c被包含在y.tab.c中，所以可以直接使用BeginCompileOneFile函数。
        否则，就要在main函数前面写声明: void BeginCompileOneFile(const char *); */
	BeginCompileOneFile( filename );

        if( yyparse()==0 && successFlag==1 ) 
            printf("Successful!\n");
        else
            printf("Failed!\n");

	EndCompileOneFile();

	getchar();
      return 0;
}





   