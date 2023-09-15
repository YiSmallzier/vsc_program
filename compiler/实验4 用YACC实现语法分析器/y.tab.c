#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/02/91\n\
 Modified 11/4/96 by Wensong Zhang to make getenv() function right\n\
 and to remove the warning of unreferenced yyerrlab and yynewerror labels";
#endif
#define YYBYACC 1
#line 1 "ex.y"
 

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

#line 25 "y.tab.c"
#define NUM_TKN 257
#define ID_TKN 258
#define IF_TKN 259
#define WHILE_TKN 260
#define DO_TKN 261
#define BREAK_TKN 262
#define ELSE_TKN 263
#define RELOP_TKN 264
#define TRUE_TKN 265
#define FALSE_TKN 266
#define BASIC_TKN 267
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    2,    2,    3,    3,    4,    4,    4,    4,
    4,    4,    5,    5,    7,    7,    6,    6,    6,    8,
    8,    8,    9,    9,    9,    9,
};
short yylen[] = {                                         2,
    1,    3,    2,    0,    1,    1,    2,    1,    4,    7,
    5,    7,    5,    7,    3,    1,    3,    3,    1,    3,
    3,    1,    0,    3,    1,    1,
};
short yydefred[] = {                                      0,
    0,    0,    1,    0,    0,    0,    0,    0,    8,    0,
    0,    5,    6,    0,    0,    0,    0,    0,    7,    2,
    3,   26,   25,    0,    0,    0,   22,    0,    0,    0,
    0,    0,    0,    0,    0,    9,    0,    0,    0,    0,
    0,    0,    0,   24,    0,    0,   20,   21,    0,   13,
    0,   11,    0,    0,    0,    0,    0,   12,   14,    0,
   10,
};
short yydgoto[] = {                                       2,
    9,   10,   11,   12,   13,   28,   29,   26,   27,
};
short yysindex[] = {                                    -87,
 -107,    0,    0,  -18,   26,   30,  -93,  -19,    0,  -57,
 -107,    0,    0,  -17,  -17,  -17,   31, -188,    0,    0,
    0,    0,    0,  -17,  -14,    2,    0,  -40,   32,   34,
  -17,   36,   15,  -17,  -17,    0,  -17,  -17,  -17, -107,
  -93,   37,  -17,    0,    2,    2,    0,    0,   16,    0,
 -186,    0,  -93,   38, -107, -183,   22,    0,    0,  -93,
    0,
};
short yyrindex[] = {                                      0,
  -43,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -43,    0,    0,   -8,  -34,  -34,    0,    0,    0,    0,
    0,    0,    0,    5,    0,  -31,    0,   42,    0,    0,
  -34,    0,    0,  -41,  -41,    0,  -41,  -41,    5,    0,
    0,    0,  -34,    0,  -26,  -21,    0,    0,   43,    0,
  -98,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,
};
short yygindex[] = {                                      0,
   85,   75,   47,   14,   33,   18,   10,   28,   27,
};
#define YYTABLESIZE 243
short yytable[] = {                                      23,
   23,   23,   34,   23,   35,   23,   23,   23,   23,   19,
   23,   19,   23,   19,   17,    1,   17,   23,   17,   18,
   18,   18,   24,   18,    5,   30,    5,   19,   34,    1,
   35,   25,   17,   23,   23,    1,   23,   18,   23,   19,
   42,   33,   14,   37,   36,   23,   23,   23,   38,   23,
   23,   23,   54,   51,   52,   44,   49,   34,   34,   35,
   35,   45,   46,   47,   48,   15,   56,   20,   58,   16,
   31,   32,   40,   58,   41,   43,   55,   53,   57,   60,
   61,    4,   16,   15,    3,   21,   50,   59,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    4,    5,    6,    7,    8,    0,    0,    0,    0,    5,
    5,    5,    5,    5,    4,   17,    6,    7,    8,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,   23,   39,    0,    0,    0,    0,    0,   23,
    0,    0,   19,    0,    0,    0,    0,   17,    0,   22,
   23,    0,   18,
};
short yycheck[] = {                                      41,
   42,   43,   43,   45,   45,   47,   41,   42,   43,   41,
   45,   43,   47,   45,   41,  123,   43,   59,   45,   41,
    7,   43,   40,   45,  123,   16,  125,   59,   43,  123,
   45,   14,   59,   42,   43,  123,   45,   59,   47,   59,
   31,   24,   61,   42,   59,   41,   42,   43,   47,   45,
   59,   47,   43,   40,   41,   41,   39,   43,   43,   45,
   45,   34,   35,   37,   38,   40,   53,  125,   55,   40,
   40,  260,   41,   60,   41,   40,  263,   41,   41,  263,
   59,  125,   41,   41,    0,   11,   40,   55,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
  258,  259,  260,  261,  262,   -1,   -1,   -1,   -1,  258,
  259,  260,  261,  262,  258,  259,  260,  261,  262,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,  264,  264,   -1,   -1,   -1,   -1,   -1,  264,
   -1,   -1,  264,   -1,   -1,   -1,   -1,  264,   -1,  257,
  258,   -1,  264,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 267
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,"';'",
"'<'","'='","'>'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,"NUM_TKN","ID_TKN","IF_TKN","WHILE_TKN","DO_TKN","BREAK_TKN",
"ELSE_TKN","RELOP_TKN","TRUE_TKN","FALSE_TKN","BASIC_TKN",
};
char *yyrule[] = {
"$accept : program",
"program : block",
"block : '{' stmts '}'",
"stmts : stmt stmts",
"stmts :",
"stmt : matched_stmt",
"stmt : unmatched_stmt",
"matched_stmt : BREAK_TKN ';'",
"matched_stmt : block",
"matched_stmt : ID_TKN '=' expr ';'",
"matched_stmt : DO_TKN matched_stmt WHILE_TKN '(' bool ')' ';'",
"matched_stmt : WHILE_TKN '(' bool ')' matched_stmt",
"matched_stmt : IF_TKN '(' bool ')' matched_stmt ELSE_TKN matched_stmt",
"unmatched_stmt : IF_TKN '(' bool ')' stmt",
"unmatched_stmt : IF_TKN '(' bool ')' matched_stmt ELSE_TKN unmatched_stmt",
"bool : expr RELOP_TKN expr",
"bool : expr",
"expr : expr '+' term",
"expr : expr '-' term",
"expr : term",
"term : term '*' factor",
"term : term '/' factor",
"term : factor",
"factor :",
"factor : '(' expr ')'",
"factor : ID_TKN",
"factor : NUM_TKN",
};
#endif
#ifndef YYSTYPE
typedef int YYSTYPE;
#endif
#define yyclearin (yychar=(-1))
#define yyerrok (yyerrflag=0)
#ifdef YYSTACKSIZE
#ifndef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#endif
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 600
#define YYMAXDEPTH 600
#endif
#endif
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short yyss[YYSTACKSIZE];
YYSTYPE yyvs[YYSTACKSIZE];
#define yystacksize YYSTACKSIZE
#line 102 "ex.y"

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





   
#line 244 "y.tab.c"
#define YYABORT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab
int
yyparse()
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register char *yys;

    if (yys = getenv("YYDEBUG"))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if (yyn = yydefred[yystate]) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, reading %d (%s)\n", yystate,
                    yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: state %d, shifting to state %d\n",
                    yystate, yytable[yyn]);
#endif
        if (yyssp >= yyss + yystacksize - 1)
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#ifdef lint
    goto yynewerror;
yynewerror:
#endif
    yyerror("syntax error");
#ifdef lint
    goto yyerrlab;
yyerrlab:
#endif
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: state %d, error recovery shifting\
 to state %d\n", *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yyss + yystacksize - 1)
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("yydebug: error recovery discarding state %d\n",
                            *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("yydebug: state %d, error recovery discards token %d (%s)\n",
                    yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("yydebug: state %d, reducing by rule %d (%s)\n",
                yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 1:
#line 39 "ex.y"
{
            printf("program->block\n");}
break;
case 2:
#line 42 "ex.y"
{
            printf("block->{stmts}\n");}
break;
case 3:
#line 45 "ex.y"
{
            printf("stmts->stmt stmts\n");}
break;
case 4:
#line 47 "ex.y"
{printf("stmt-> null\n");}
break;
case 5:
#line 49 "ex.y"
{
            printf("stmt-> matched_stmt\n");    }
break;
case 6:
#line 51 "ex.y"
{
            printf("stmt-> unmatched_stmt\n");  }
break;
case 7:
#line 55 "ex.y"
{
            printf("stmt->break\n");    }
break;
case 8:
#line 57 "ex.y"
{
            printf("stmt->block\n");    }
break;
case 9:
#line 59 "ex.y"
{
            printf("stmt->id = expr\n");   }
break;
case 10:
#line 61 "ex.y"
{
            printf("stmt->do (bool) while stmt\n");   }
break;
case 11:
#line 63 "ex.y"
{
            printf("stmt->while (bool) stmt\n");   }
break;
case 12:
#line 65 "ex.y"
{
            printf("match -> if (bool) match else match\n");    }
break;
case 13:
#line 69 "ex.y"
{
            printf("stmt -> if (bool) stmt\n");   }
break;
case 14:
#line 71 "ex.y"
{
            printf("unmatch -> if (bool) match else unmatch\n" );    }
break;
case 15:
#line 74 "ex.y"
{
            printf("bool->expr relop expr\n");  }
break;
case 16:
#line 76 "ex.y"
{
            printf("bool->expr\n");  }
break;
case 17:
#line 79 "ex.y"
{
            printf("expr->expr + term\n"); }
break;
case 18:
#line 81 "ex.y"
{
            printf("expr->expr - term\n"); }
break;
case 19:
#line 83 "ex.y"
{
            printf("expr->term\n");}
break;
case 20:
#line 86 "ex.y"
{ 
            printf("term->term * factor\n"); }
break;
case 21:
#line 88 "ex.y"
{
            printf("term->term / factor\n"); }
break;
case 22:
#line 90 "ex.y"
{
            printf("term->factor\n");  }
break;
case 24:
#line 92 "ex.y"
{ 
            printf("factor->(expr)\n");}
break;
case 25:
#line 94 "ex.y"
{
            printf("factor -> id\n");   }
break;
case 26:
#line 96 "ex.y"
{
            printf("factor->num\n"); 
            /* "%f"和"%g"都是输出浮点数，"%g"不会输出多余的0, */ }
break;
#line 508 "y.tab.c"
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("yydebug: after reduction, shifting from state 0 to\
 state %d\n", YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("yydebug: state %d, reading %d (%s)\n",
                        YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("yydebug: after reduction, shifting from state %d \
to state %d\n", *yyssp, yystate);
#endif
    if (yyssp >= yyss + yystacksize - 1)
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
