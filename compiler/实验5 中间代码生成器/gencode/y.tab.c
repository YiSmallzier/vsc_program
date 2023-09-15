#ifndef lint
static char yysccsid[] = "@(#)yaccpar	1.8 (Berkeley) 01/02/91\n\
 Modified 11/4/96 by Wensong Zhang to make getenv() function right\n\
 and to remove the warning of unreferenced yyerrlab and yynewerror labels";
#endif
#define YYBYACC 1
#line 1 ".\compile.y"
 
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int LineNo = 1; /*当前lookahead所指向的字符所在的行号，即当前编译到的行号*/

int CompileFailed = 0;

void yyerror( char * ErrStr )
{
    CompileFailed = 1; /*编译失败标志*/
    printf("错误信息:%s, 行号:%d\n", ErrStr, LineNo);
}



/*变量和常量的基本类型BASIC*/
#define CHAR     1
#define INT      2
#define FLOAT    3
#define BOOL     4

#define CHAR_WIDTH  1
#define INT_WIDTH   4
#define FLOAT_WIDTH 8  
#define BOOL_WIDTH  1

/*****************************下面：符号表的定义和相关函数*******************************/

/*变量名长度不超过ID_MAX_LEN 个字符*/
#define ID_MAX_LEN   64

/*存放一个标识符*/
struct SymbolElem {
    char name[ ID_MAX_LEN + 1 ]; /*符号名(例如变量名)长度不超过ID_MAX_LEN 个字符*/
    int type; /*用来存放类型名，例如int, 这个程序只处理简单类型，在实际的编译器中，这里要建立树结构*/
    int  addr;      /*为该变量分配的空间的首地址*/
	int  width;     /*该变量的宽度，即占用多少个字节*/
    struct SymbolElem * next;  /*指向下一个标识符*/
};

/*标识符表*/
typedef struct SymbolList{
    struct SymbolElem * head;  /*指向符号表（用链表实现）的第一个结点，没有头结点,初始化为NULL*/
    struct SymbolList * prev; /*上一层的符号表*/
    int beginaddr; /*该符号表中分配给变量和临时变量空间的开始地址*/
    int endaddr;    /*该符号表中分配给变量和临时变量空间的结束地址*/
                   /*beginaddr~endaddr的空间存放该符号表的所有变量和临时变量*/
} * SymbolList;  /*符号表*/

SymbolList TopSymbolList=NULL; /*全局变量，存放当前的符号表（即当前处于最顶层的符号表）,对应为书上的top*/

/*创建并返回一个新的符号表（SymbolList就是书上的Env），PrevList是其的上一层符号表*/
SymbolList CreateSymbolList( SymbolList PrevList, int StartAddr )
{ SymbolList list;
    list = (SymbolList) malloc( sizeof(struct SymbolList) );
    memset( list, 0, sizeof( struct SymbolList ) );
    list->prev = PrevList;
	list->endaddr = list->beginaddr = StartAddr;

    return list;
}

void DestroySymbolList( SymbolList List )
{struct SymbolElem * p, *q;
    
    if( List == NULL) return;
    p = List->head;
    while( p!=NULL ) {
        q = p->next; free(p); p=q;
    }
    free(List);    
}

/*在符号表List中查找是否存在标识符IdName，如果存在，则返回该结点指针，否则返回空*/
struct SymbolElem * LookUpSymbolList( SymbolList List, char * IdName )
{struct SymbolElem * p;
    if( List==NULL ) return NULL;
    for( p = List->head; p!=NULL; p = p->next ) 
        if( strcmp( p->name, IdName ) == 0 ) break;
    return p;
}

/*从符号表List开始并不断地往上一层符号表中查找是否存在标识符IdName，如果存在，则返回该结点指针，否则返回空*/
struct SymbolElem * LookUpAllSymbolList( SymbolList List, char * IdName )
{ SymbolList env;
struct SymbolElem * p;
    env = List;
    while( env!=NULL ) {
        p = LookUpSymbolList( env, IdName );
        if(  p != NULL ) return p; /*找到该符号*/
        env = env->prev;
    }
    return NULL;
}


/*创建一个新的符号结点,并添加到符号表中，而后返回该结点指针*/
struct SymbolElem * AddToSymbolList( SymbolList List, char * IdName,int IdType, int Width )
{struct SymbolElem * p;

    p = (struct SymbolElem *) malloc( sizeof(struct SymbolElem) );

    strcpy( p->name, IdName );
    p->type = IdType;
	p->width = Width;
	p->addr = List->endaddr;
	List->endaddr += Width;

    p->next = List->head;  /*将该标识符添加到符号表表头*/
    List->head = p;

    return p;    
}

void PrintSymbolList( SymbolList List )
{struct SymbolElem * p;
    printf("***********************变量列表*************************\n");
    if( List ==NULL ) return ;
    for( p=List->head; p!=NULL; p=p->next ) {
        printf("变量名:%s, 类型:", p->name);
		switch( p->type ) {
            case CHAR : printf("char");  break;
            case INT  : printf("int");   break;
            case FLOAT: printf("float"); break;
            case BOOL : printf("bool");  break;
		}
        printf(", 地址:%d, 宽度:%d\n", p->addr, p->width );
	}
    printf("*************该变量列表共占用%d个字节空间***************\n", List->endaddr - List->beginaddr);
}

/*分配一个临时变量,返回临时变量的地址、临时变量的名称*/
int NewTemp( SymbolList List, char Name[], int Width )
{ static int TempID = 1;
  int addr;
    sprintf( Name, "T%d", TempID++ ); /*例如T1，T2等*/
	addr = List->endaddr;
    List->endaddr += Width;
  
    return addr;
}

/*****************************上面：符号表的定义和相关函数*****************************/




/*****************************下面：常数表的定义和相关函数*******************************/

union ConstVal {
        char    ch;    /*存放字符常量*/
        int     n;     /*存放整型常量，或true=1，false=0 */
        double  f;     /*存放浮点数常量*/
};	

/*存放一个常数*/
struct ConstElem {
    char str[ID_MAX_LEN + 1 ]; /*该变量用于存储常数的文本形式，演示的时候用的,实际的编译系统不需要*/	   
    int type; /*用来存放类型名，例如int*/
    union ConstVal value;
    int  addr;      /*为该常量分配的空间的首地址*/
	int  width;     /*该变量的宽度，即占用多少个字节*/
    struct ConstElem * next;  /*指向下一个常量*/
};

/*常量表*/
struct ConstList{
    struct ConstElem * head;  /*指向常量表（用链表实现）的第一个结点，没有头结点,初始化为NULL*/
    int beginaddr;  /*该符号表中分配给常量空间的开始地址*/
    int endaddr;    /*该符号表中分配给常量空间的结束地址*/
                   /*beginaddr~endaddr的空间存放该常量表的所有常量*/
} ConstList ;  /*常量表，全局变量，注意整个程序只需要一个常量表**/


/*创建并返回常量表*/
void CreateConstList( int StartAddr )
{ 
	ConstList.head = NULL;
	ConstList.endaddr = ConstList.beginaddr = StartAddr;
}

void DestroyConstList( void )
{struct ConstElem * p, *q;
    
    p = ConstList.head;
    while( p!=NULL ) {
        q = p->next; free(p); p=q;
    }
	memset( &ConstList, 0, sizeof(struct ConstList) );
}

/*在常量表ConstList中查找是否存在常量，如果存在，则返回该结点指针，否则返回空*/
struct ConstElem * LookUpConstList( int ConstType, union ConstVal ConstValue, int Width )
{struct ConstElem * p;
    for( p = ConstList.head; p!=NULL; p = p->next ) 
        if( p->type == ConstType && memcmp( &p->value,&ConstValue, Width) == 0 )  break;
	
    return p;
}


/*创建一个新的常数结点,并添加到常数表中，而后返回该结点指针*/
struct ConstElem * AddToConstList( char * Str, int ConstType, union ConstVal ConstValue, int Width )
{struct ConstElem * p;

    p = (struct ConstElem *) malloc( sizeof(struct ConstElem) );

    strcpy( p->str, Str );
    p->type = ConstType;
    p->value = ConstValue;
	p->width = Width;

	p->addr = ConstList.endaddr;
	ConstList.endaddr += Width;

    p->next = ConstList.head;  /*将该常量添加到常量表表头*/
    ConstList.head = p;

    return p;    
}

void PrintConstList(void)
{struct ConstElem * p;
    printf("***********************常量列表*************************\n");
    for( p=ConstList.head; p!=NULL; p=p->next ) {
	    printf("常量:%s, 类型:", p->str);
		switch( p->type ) {
            case CHAR : printf("char");  break;
            case INT  : printf("int");   break;
            case FLOAT: printf("float"); break;
            case BOOL : printf("bool");  break;
		}
        printf(", 地址:%d, 宽度:%d\n", p->addr, p->width );
	}
    printf("**************该常量列表共占用%d个字节空间***************\n", ConstList.endaddr - ConstList.beginaddr);
}

/*****************************上面：常数表的定义和相关函数*****************************/





/********************************下面:四元式的定义和函数****************************/

/* 整型加减乘除 */
#define OIntAdd          1001
#define OIntSub          1002
#define OIntMultiply     1003
#define OIntDivide       1004

/* 浮点数加减乘除 */
#define OFloatAdd        1011
#define OFloatSub        1012
#define OFloatMultiply   1013
#define OFloatDivide     1014

/*赋值a=b*/
#define OIntEvaluation   1021
#define OFloatEvaluation 1022
#define OCharEvaluation  1023
#define OBoolEvaluation  1024

/* 无条件goto语句 */
#define OGoto            1031

/* if a op b goto 语句 */
#define OGTGoto          1041
#define OGEGoto          1042
#define OLTGoto          1043
#define OLEGoto          1044
#define OEQGoto          1045
#define ONEQGoto         1046

/*类型转换运算符*/
#define OCharToInt       1051
#define OCharToFloat     1052
#define OIntToFloat      1053
#define OIntToChar       1054
#define OFloatToChar     1055
#define OFloatToInt      1056
#define OCharToBool      1057
#define OIntToBool       1058
#define OFloatToBool     1059
#define OBoolToChar      1060
#define OBoolToInt       1061
#define OBoolToFloat     1062 

/*四元式数据结构*/
struct Quadruple {
    int op; /*运算符*/
    int arg1; /*存放第一个参数的地址，可能是变量、临时变量的地址*/
    int arg2;
    int arg3;/*存放第三个参数的地址，可能是变量、临时变量的地址，还可能是四元式的地址(Goto 的地址参数)*/
    char arg1name[ID_MAX_LEN + 1]; /*本不需要，用于演示时能显示arg1对应的变量或临时变量的名称(若有的话）*/
    char arg2name[ID_MAX_LEN + 1]; /*本不需要，用于演示时能显示arg2对应的变量或临时变量的名称(若有的话）*/
    char arg3name[ID_MAX_LEN + 1]; /*本不需要，用于演示时能显示arg3对应的变量或临时变量的名称(若有的话）*/
};

/*四元式表*/
struct QuadTable {
    int startaddr; /*四元式开始存放的地址,比如100*/
    struct Quadruple * base; /*指向一块内存，用来存放多个四元式，从base[0]开始存放*/
    int size; /*base中可以存放的四元式的个数*/
    int len; /*base[len]是下一个四元式要存放的空间*/

};

struct QuadTable QuadTable; /*只需要一个四元式表*/

void CreateQuadTable(int StartAddr)
{
    QuadTable.startaddr = StartAddr; 
    QuadTable.size = 1000; /* 一开始假设可以存放1000个四元式*/
    QuadTable.base = ( struct Quadruple *)malloc( QuadTable.size * sizeof(struct Quadruple) );
    QuadTable.len = 0;
}

void DestroyQuadTable( void )
{
    QuadTable.startaddr = 0; 
    QuadTable.size = 0;
    if( QuadTable.base != NULL) free(QuadTable.base); 
    QuadTable.len = 0;   
}

/*当Arg1是变量或临时变量时，Arg1Name是该变量的名称,用于演示时使用，其余参数类同 */
int Gen( int Op, int Arg1, int Arg2, int Arg3, char *Arg1Name, char *Arg2Name, char *Arg3Name )
{ struct Quadruple * ptr; 
  int incr = 100;
    if( QuadTable.len >= QuadTable.size ) {
        ptr = realloc( QuadTable.base, QuadTable.size+incr );
        if( ptr==NULL ) return -1;
        QuadTable.base = ptr;
        QuadTable.size += incr;
    }
    ptr = & QuadTable.base[QuadTable.len];
    ptr->op = Op;
    ptr->arg1 = Arg1;
    ptr->arg2 = Arg2;
    ptr->arg3 = Arg3;
    strcpy( ptr->arg1name, Arg1Name);
    strcpy( ptr->arg2name, Arg2Name);
    strcpy( ptr->arg3name, Arg3Name);
    QuadTable.len++;

    return QuadTable.len - 1;
}

/*把四元式所对应的三地址代码写入到文件中*/
void WriteQuadTableToFile( const char * FileName )
{
   
    FILE * fp;
struct Quadruple * ptr;
int i,op;
char str[1000],ch;
    fp = fopen( FileName, "w" );
    if( fp==NULL ) {
        printf("ERROR: write failed\n");
        return;
    }
    /*  printf("%d\n\n\n", QuadTable.len);*/
    for( i=0, ptr = QuadTable.base; i < QuadTable.len; i++,ptr++ ) {
        fprintf(fp, "%5d:  ", QuadTable.startaddr + i);
        op = ptr->op;
        /* printf("OP = %d\n", op);*/
        switch( op ) {
            case OIntAdd        :
            case OIntSub        :
            case OIntMultiply   :
            case OIntDivide     :
            case OFloatAdd      :
            case OFloatSub      :
            case OFloatMultiply :
            case OFloatDivide   : if( op==OIntAdd || op==OFloatAdd) ch = '+';
                                  if( op==OIntSub || op==OFloatSub) ch = '-';
                                  if( op==OIntMultiply || op==OFloatMultiply) ch = '*';
                                  if( op==OIntDivide || op==OFloatDivide) ch = '/';
                                  sprintf(str,"[%d] = [%d] %c [%d]", ptr->arg3, ptr->arg1, ch, ptr->arg2);
                                  break;
            case OIntEvaluation   :
            case OFloatEvaluation :
            case OCharEvaluation  :
            case OBoolEvaluation  : sprintf(str,"[%d] = [%d]", ptr->arg3, ptr->arg1);
                                    break;
            case OGoto            : sprintf(str,"Goto %d", ptr->arg3);
                                    break;
            case OGTGoto  : sprintf(str,"if [%d]>[%d] Goto %d", ptr->arg1, ptr->arg2, ptr->arg3 );  break;
            case OGEGoto  : sprintf(str,"if [%d]>=[%d] Goto %d", ptr->arg1, ptr->arg2, ptr->arg3 ); break;
            case OLTGoto  : sprintf(str,"if [%d]<[%d] Goto %d", ptr->arg1, ptr->arg2, ptr->arg3 );  break;
            case OLEGoto  : sprintf(str,"if [%d]<=[%d] Goto %d", ptr->arg1, ptr->arg2, ptr->arg3 ); break;
            case OEQGoto  : sprintf(str,"if [%d]==[%d] Goto %d", ptr->arg1, ptr->arg2, ptr->arg3 ); break;
            case ONEQGoto : sprintf(str,"if [%d]<>[%d] Goto %d", ptr->arg1, ptr->arg2, ptr->arg3 ); break;

            case OCharToInt   : sprintf( str,"[%d] = (int) [%d]",   ptr->arg3, ptr->arg1 );  break;
            case OCharToFloat : sprintf( str,"[%d] = (float) [%d]", ptr->arg3, ptr->arg1 );  break;
            case OIntToFloat  : sprintf( str,"[%d] = (float) [%d]", ptr->arg3, ptr->arg1 );  break;
            case OIntToChar   : sprintf( str,"[%d] = (char) [%d]",  ptr->arg3, ptr->arg1 );  break;
            case OFloatToChar : sprintf( str,"[%d] = (char) [%d]",  ptr->arg3, ptr->arg1 );  break;
            case OFloatToInt  : sprintf( str,"[%d] = (int) [%d]",   ptr->arg3, ptr->arg1 );  break;

            case OCharToBool   : sprintf( str,"[%d] = (bool) [%d]",  ptr->arg3, ptr->arg1 );  break;
            case OIntToBool    : sprintf( str,"[%d] = (bool) [%d]",  ptr->arg3, ptr->arg1 );  break;
            case OFloatToBool  : sprintf( str,"[%d] = (bool) [%d]",  ptr->arg3, ptr->arg1 );  break;
            case OBoolToChar   : sprintf( str,"[%d] = (char) [%d]",  ptr->arg3, ptr->arg1 );  break;
            case OBoolToInt    : sprintf( str,"[%d] = (int) [%d]",   ptr->arg3, ptr->arg1 );  break;
            case OBoolToFloat  : sprintf( str,"[%d] = (float) [%d]", ptr->arg3, ptr->arg1 );  break;

            default: yyerror("程序错误：出现不认识的运算符！"); strcpy(str, "error: Unknown operator");break;
        }
        fprintf(fp,"%s\n",str);
    }
    fclose(fp);
}

/********************************上面:四元式的定义和函数****************************/



/**************下面:定义句法分析栈中元素的信息，即终结符和非终结符的综合属性****************/
struct node {
    int val;
    struct node* next;
};
typedef struct node node;
typedef struct {
    int len;
    node* begin;
    node* end;
} List;

 union ParseStackNodeInfo{
    struct {
	    /*符号名(例如变量名)长度不超过ID_MAX_LEN 个字符*/
        /*如果建立hash表来存储所有的不同名字的标识符的名字，这里就可以使用一个指针指向该标识符的名字，
		  好处是减少分析栈中元素的空间大小，从而节省空间且提高编译效率，*/
        char name[ID_MAX_LEN + 1 ]; 
    }id;  /*标识符:终结符ID的综合属性*/

    struct {
	   char str[ID_MAX_LEN + 1 ]; /*该变量用于存储常数的文本形式，演示的时候用的,实际的编译系统不需要*/	   
       int type; /*用来存放类型名，例如INT*/
	   union ConstVal value; /*常量：终结符CONST的信息*/
	   int width;
	} constval; /*终结符const的综合属性*/

    struct {
        int type; /*用来存放类型名，例如INT*/
    }basic; /*基本数据类型：终结符BASIC的综合属性*/

	struct {
	   char str[ID_MAX_LEN + 1 ]; /*该变量用于存储变量名、临时变量名或常数的文本形式，演示的时候用的,实际的编译系统不需要*/
	   int type;
	   int addr;
	   int width;
	} factor, term, expr, unary;/*非终结符factor, term, expr的综合属性*/
        /*其它文法符号的属性记录可以在下面继续添加*/
    struct {
        int instr;
        List *nextlist;
    } stmts, stmt, matched_stmt, unmatched_stmt, N;
    struct {
        int instr;
    } M;
    struct {
        List *nextlist;
    } block, decls, decl;
    struct {
        List *truelist, *falselist;
    } bool, join;
    struct {
        char str[ID_MAX_LEN + 1 ]; /*该变量用于存储变量名、临时变量名或常数的文本形式，演示的时候用的,实际的编译系统不需要*/
        int type;
        int addr;
        int width;
        List *truelist, *falselist;
    } rel, equality;
} ;

List* Makelist( int instr )
{
    List* list = (List*)malloc( sizeof(List) );
    list->len = 1;
    list->begin = (node*)malloc( sizeof(node) );
    list->begin->val = instr;
    list->begin->next = NULL;
    list->end = list->begin;
    return list;
}
List* Merge (List* a, List* b) {
    if ( a == NULL ) return b;
    if ( b == NULL ) return a;
    List* list = (List*)malloc( sizeof(List) );
    list->len = a->len + b->len;
    list->begin = a->begin;
    list->end = b->end;
    a->end->next = b->begin;
    return list;
}
void backpatch( List* list, int instr ) {
    node* p = list->begin;
    while ( p != NULL ) {
        QuadTable.base[p->val].arg3 = instr;
        p = p->next;
    }
}

#define YYSTYPE union ParseStackNodeInfo 
       

/**************上面:定义句法分析栈中元素的信息，即终结符和非终结符的综合属性****************/
 




#line 529 "y.tab.c"
#define BASIC 257
#define CONST 258
#define ID 259
#define ELSE 260
#define IF 261
#define WHILE 262
#define DO 263
#define BREAK 264
#define RELOP_LT 265
#define RELOP_LE 266
#define RELOP_GT 267
#define RELOP_GE 268
#define RELOP_EQ 269
#define RELOP_NEQ 270
#define OR 271
#define AND 272
#define UMINUS 273
#define YYERRCODE 256
short yylhs[] = {                                        -1,
    0,    1,    2,    5,    6,    7,    3,    3,    8,    9,
    4,    4,   10,   10,   11,   11,   11,   11,   11,   11,
   12,   12,   14,   14,   15,   15,   16,   16,   16,   17,
   17,   17,   17,   17,   13,   13,   13,   18,   18,   18,
   19,   20,   20,   20,
};
short yylen[] = {                                         2,
    1,    6,    0,    0,    0,    0,    2,    0,    3,    1,
    3,    0,    1,    1,    4,   10,    7,    9,    2,    1,
   10,    6,    4,    1,    4,    1,    3,    3,    1,    3,
    3,    3,    3,    1,    3,    3,    1,    3,    3,    1,
    1,    3,    1,    1,
};
short yydefred[] = {                                      0,
    3,    0,    1,    8,    0,   10,    0,    7,    0,    0,
    0,    0,    2,    0,    0,    5,    5,    0,   20,   11,
   13,   14,    9,    0,    0,    0,    0,   19,   44,   43,
    0,    0,    0,   40,   41,    0,    0,    0,    0,   29,
    0,    0,    0,    0,    0,    0,   15,    0,    0,    0,
    0,    0,    0,    5,    5,    5,    0,    0,    0,    0,
    5,   42,    0,    0,   38,   39,    0,    0,    0,    0,
    0,    0,    0,   27,   28,    5,    0,    0,    0,    0,
    0,    0,    5,    0,    0,   17,    0,    0,    5,    6,
    0,    0,    0,   18,   16,   21,    5,    0,
};
short yydgoto[] = {                                       2,
   19,    4,    5,    7,   10,   11,   85,    8,    9,   20,
   95,   22,   36,   37,   38,   39,   40,   33,   34,   35,
};
short yysindex[] = {                                   -103,
    0,    0,    0,    0, -229,    0,    0,    0, -225,  -86,
 -108,   -7,    0,  -16,   22,    0,    0,   20,    0,    0,
    0,    0,    0,  -39,  -39,   38,  -94,    0,    0,    0,
  -39,  -29,   12,    0,    0,   15,  -17, -204, -263,    0,
  -39,   40, -181,   10,  -39,  -39,    0,  -39,  -39,  -39,
  -39,  -39,  -39,    0,    0,    0,  -39,  -39,   -8,  -39,
    0,    0,   12,   12,    0,    0,   18,   18,   18,   18,
  -39,  -94,  -39,    0,    0,    0,    5,   43, -204,    0,
 -263,  -94,    0,  -39, -176,    0,  -94,    6,    0,    0,
   26, -108, -174,    0,    0,    0,    0,  -94,
};
short yyrindex[] = {                                      0,
    0,    0,    0,    0, -114,    0, -100,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -41,    0,    0,  -19,    0,    7,    1,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  -33,  -24,    0,    0,  -14,  -10,   -5,   -1,
    0,    0,    0,    0,    0,    0,    0,    0,    8, -120,
    3,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,
};
short yygindex[] = {                                      0,
   87,    0,    0,    0,    0,   21,   -2,    0,    0,    0,
   30,   -3,   19,  -28,   23,   17,    9,   28,   16,    0,
};
#define YYTABLESIZE 283
short yytable[] = {                                      37,
   31,   37,   22,   37,   22,   57,   58,   35,   12,   35,
   12,   35,   59,   45,    1,   46,   36,   37,   36,    1,
   36,   34,    5,   55,    4,   35,   30,    6,    1,   47,
   31,   77,   76,   12,   36,   32,   26,   27,   13,   33,
   21,   26,   32,   25,   24,   83,   91,   24,   23,   44,
   62,   23,   45,   48,   46,   88,   43,   45,   49,   46,
   45,   25,   46,   65,   66,   74,   75,   56,   67,   68,
   69,   70,   63,   64,   71,   72,   73,   41,   28,   60,
   61,   78,   84,   89,   94,   97,    3,   93,   96,   81,
    0,    0,    0,   79,    0,    0,   82,    0,    0,    0,
    0,   80,    0,   87,    0,    0,    0,    0,    0,   92,
    0,   86,    0,    0,    0,    0,   90,   98,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   22,    6,
   22,   22,   22,   22,   12,    0,   12,   12,   12,   12,
   14,    0,   15,   16,   17,   18,    0,    0,    5,    0,
    5,    5,    5,    5,   14,    0,   42,   16,   17,   18,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   29,   30,
    0,    0,    0,   37,   37,   37,   37,   37,   37,   37,
   37,   35,   35,   35,   35,   35,   35,   35,   35,    0,
   36,   36,   36,   36,   36,   36,   36,   36,    0,   34,
   34,   34,   34,   54,   30,   30,   30,   30,   31,   31,
   31,   31,   54,   32,   32,   32,   32,   33,   33,   33,
   33,   26,   26,   25,   25,   54,   54,   24,   23,   50,
   51,   52,   53,
};
short yycheck[] = {                                      41,
   40,   43,  123,   45,  125,  269,  270,   41,  123,   43,
  125,   45,   41,   43,  123,   45,   41,   59,   43,  123,
   45,   41,  123,   41,  125,   59,   41,  257,  123,   59,
   41,   60,   41,  259,   59,   41,   16,   17,  125,   41,
   11,   41,   24,   41,   61,   41,   41,   41,   41,   31,
   41,   59,   43,   42,   45,   84,   27,   43,   47,   45,
   43,   40,   45,   48,   49,   57,   58,  272,   50,   51,
   52,   53,   45,   46,   54,   55,   56,   40,   59,   40,
  262,   61,   40,  260,   59,  260,    0,   90,   92,   73,
   -1,   -1,   -1,   71,   -1,   -1,   76,   -1,   -1,   -1,
   -1,   72,   -1,   83,   -1,   -1,   -1,   -1,   -1,   89,
   -1,   82,   -1,   -1,   -1,   -1,   87,   97,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  259,  260,
  261,  262,  263,  264,  259,   -1,  261,  262,  263,  264,
  259,   -1,  261,  262,  263,  264,   -1,   -1,  259,   -1,
  261,  262,  263,  264,  259,   -1,  261,  262,  263,  264,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,  258,  259,
   -1,   -1,   -1,  265,  266,  267,  268,  269,  270,  271,
  272,  265,  266,  267,  268,  269,  270,  271,  272,   -1,
  265,  266,  267,  268,  269,  270,  271,  272,   -1,  269,
  270,  271,  272,  271,  269,  270,  271,  272,  269,  270,
  271,  272,  271,  269,  270,  271,  272,  269,  270,  271,
  272,  271,  272,  271,  272,  271,  271,  271,  271,  265,
  266,  267,  268,
};
#define YYFINAL 2
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 273
#if YYDEBUG
char *yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
"'!'",0,0,0,0,0,0,"'('","')'","'*'","'+'",0,"'-'",0,"'/'",0,0,0,0,0,0,0,0,0,0,0,
"';'",0,"'='",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'{'",0,"'}'",0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"BASIC","CONST","ID","ELSE","IF","WHILE","DO","BREAK","RELOP_LT",
"RELOP_LE","RELOP_GT","RELOP_GE","RELOP_EQ","RELOP_NEQ","OR","AND","UMINUS",
};
char *yyrule[] = {
"$accept : program",
"program : block",
"block : '{' blockM1 decls stmts blockM2 '}'",
"blockM1 :",
"blockM2 :",
"M :",
"N :",
"decls : decls decl",
"decls :",
"decl : type ID ';'",
"type : BASIC",
"stmts : stmts M stmt",
"stmts :",
"stmt : matched_stmt",
"stmt : unmatched_stmt",
"matched_stmt : ID '=' expr ';'",
"matched_stmt : IF '(' bool ')' M matched_stmt N ELSE M matched_stmt",
"matched_stmt : WHILE M '(' bool ')' M matched_stmt",
"matched_stmt : DO M matched_stmt WHILE M '(' bool ')' ';'",
"matched_stmt : BREAK ';'",
"matched_stmt : block",
"unmatched_stmt : IF '(' bool ')' M matched_stmt N ELSE M unmatched_stmt",
"unmatched_stmt : IF '(' bool ')' M matched_stmt",
"bool : bool OR M join",
"bool : join",
"join : join AND M equality",
"join : equality",
"equality : equality RELOP_EQ rel",
"equality : equality RELOP_NEQ rel",
"equality : rel",
"rel : expr RELOP_LT expr",
"rel : expr RELOP_LE expr",
"rel : expr RELOP_GT expr",
"rel : expr RELOP_GE expr",
"rel : expr",
"expr : expr '+' term",
"expr : expr '-' term",
"expr : term",
"term : term '*' unary",
"term : term '/' unary",
"term : unary",
"unary : factor",
"factor : '(' expr ')'",
"factor : ID",
"factor : CONST",
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
#line 815 ".\compile.y"
#include "lex.yy.c"

int yyparse();  /*main函数要调用yyparse()函数，但该函数的定义在后面，所以要先声明(才能引用)*/

int main()
{
	char sourcefile[1000],destfile[1000];

	printf("请输入要编译的源程序文件名(回车默认为d:.\\code.txt)："); gets(sourcefile);
        if( strcmp( sourcefile,"")== 0 ) 
            strcpy( sourcefile, ".\\code.txt");
	printf("请输入存放中间代码的文件名(回车默认为d:.\\gencode.txt)："); gets(destfile);
        if( strcmp( destfile,"")== 0 ) 
            strcpy( destfile, ".\\gencode.txt");

	BeginCompileOneFile( sourcefile );

    CreateConstList(3000);/*创建常量表,假设从地址3000开始分配空间给常量*/
    /*在C语言编译器中，该符号表用于存放外部变量，函数名等。
      在我们的语法中并不支持外部变量和函数，所以该表没有被用到*/
    TopSymbolList = CreateSymbolList( NULL, 2000 ); /*假设从地址2000开始分配空间给变量*/
    CreateQuadTable(100); /*创建四元式表，假设四元式从地址空间100开始存放*/

    yyparse();

    PrintConstList();
    WriteQuadTableToFile( destfile ); /*把四元式写入到文件destfile中*/

    DestroyQuadTable();
    DestroySymbolList(TopSymbolList);
	DestroyConstList();

    if( CompileFailed == 0 ) 
	    printf("编译成功，生成的四元式在文件%s中。\n", destfile );
	else
	    printf("源文件%s有错误，编译失败。\n", sourcefile );

	EndCompileOneFile();
        
	getchar();
    return 0;
}





   
#line 811 "y.tab.c"
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
#line 550 ".\compile.y"
{ printf("产生式：program->block\n"); }
break;
case 2:
#line 553 ".\compile.y"
{ printf("产生式：block->{decls stmts}\n"); 
                                                    yyval.block.nextlist = yyvsp[-2].stmts.nextlist;
}
break;
case 3:
#line 558 ".\compile.y"
{ TopSymbolList = CreateSymbolList( TopSymbolList, TopSymbolList->endaddr ); }
break;
case 4:
#line 561 ".\compile.y"
{ SymbolList env;
                                 PrintSymbolList( TopSymbolList); 
                                 env = TopSymbolList->prev;
                                 DestroySymbolList( TopSymbolList ); 
                                 TopSymbolList = env;                 
                              }
break;
case 5:
#line 568 ".\compile.y"
{yyval.M.instr = QuadTable.len + QuadTable.startaddr;}
break;
case 6:
#line 570 ".\compile.y"
{Gen( OGoto, -1, -1, -1, "", "", "" );
                yyval.N.nextlist = Makelist( QuadTable.len - 1 );}
break;
case 7:
#line 573 ".\compile.y"
{ printf("产生式：decls->decls decl\n"); }
break;
case 8:
#line 574 ".\compile.y"
{ printf("产生式：decls->null\n"); }
break;
case 9:
#line 577 ".\compile.y"
{ int width;
                                
                                printf("产生式：decl->type ID; ID=%s\n",yyvsp[-1].id.name); 
                                
								switch( yyvsp[-2].basic.type ) {
                                    case CHAR  : width = CHAR_WIDTH;  break;
                                    case INT   : width = INT_WIDTH;   break;
                                    case FLOAT : width = FLOAT_WIDTH; break;
                                    case BOOL  : width = BOOL_WIDTH;  break;
                                    default    : width = -1; break;
                                }
                                AddToSymbolList( TopSymbolList, yyvsp[-1].id.name, yyvsp[-2].basic.type, width );

                              }
break;
case 10:
#line 593 ".\compile.y"
{ printf("产生式：type->BASIC\n"); yyval.basic.type = yyvsp[0].basic.type; }
break;
case 11:
#line 596 ".\compile.y"
{printf("产生式：stmts->stmts stmt\n");
                            if ( yyvsp[-2].stmts.nextlist != NULL ) {
                                backpatch( yyvsp[-2].stmts.nextlist, yyvsp[-1].M.instr );
                            }
                            yyval.stmts.nextlist = yyvsp[0].stmts.nextlist;
}
break;
case 12:
#line 602 ".\compile.y"
{printf("产生式：stmts->null\n");
                        yyval.stmts.nextlist = NULL;
        }
break;
case 13:
#line 606 ".\compile.y"
{
            yyval.stmt.nextlist = yyvsp[0].matched_stmt.nextlist;
        }
break;
case 14:
#line 609 ".\compile.y"
{
            yyval.stmt.nextlist = yyvsp[0].unmatched_stmt.nextlist;
        }
break;
case 15:
#line 613 ".\compile.y"
{  printf("产生式：stmt->id = expr;\n");  
                                                    struct SymbolElem *p;
                                                    p = LookUpAllSymbolList( TopSymbolList, yyvsp[-3].id.name );
                                                    if( p==NULL ) {
                                                        yyerror("语义错误：变量未定义！");
                                                        return 0;
                                                    }
                                                    if( p->type != yyvsp[-1].expr.type ) {
                                                        yyerror("语义错误：赋值号两边的类型不匹配！");
                                                        return 0;
                                                    }
                                                    Gen( p->type==INT ? OIntEvaluation : OFloatEvaluation, yyvsp[-1].expr.addr, -1, p->addr, yyvsp[-1].expr.str, "", p->name );
                                                    yyval.matched_stmt.nextlist = NULL;
                                                }
break;
case 16:
#line 627 ".\compile.y"
{ printf("产生式：stmt->if (bool) stmt else stmt\n"); 
                                                            backpatch( yyvsp[-7].bool.truelist, yyvsp[-5].M.instr );
                                                            backpatch( yyvsp[-7].bool.falselist, yyvsp[-1].M.instr );
                                                            yyval.matched_stmt.nextlist = Merge( yyvsp[-4].matched_stmt.nextlist, yyvsp[-3].matched_stmt.nextlist );
                                                            yyval.matched_stmt.nextlist = Merge( yyval.matched_stmt.nextlist, yyvsp[0].matched_stmt.nextlist );
                                                            }
break;
case 17:
#line 633 ".\compile.y"
{ printf("产生式：stmt->while (bool) stmt\n"); 
                                                    backpatch( yyvsp[-3].bool.truelist, yyvsp[-1].M.instr );
                                                    backpatch( yyvsp[0].matched_stmt.nextlist, yyvsp[-5].M.instr );
                                                    yyval.matched_stmt.nextlist = yyvsp[-3].bool.falselist;
                                                    Gen( OGoto, -1, -1, yyvsp[-5].M.instr, "", "", "" );
        }
break;
case 18:
#line 639 ".\compile.y"
{ printf("产生式：stmt->do stmt while (bool)\n"); 
                                                    backpatch( yyvsp[-6].matched_stmt.nextlist, yyvsp[-4].M.instr );
                                                    backpatch( yyvsp[-2].bool.truelist, yyvsp[-7].M.instr );
                                                    yyval.matched_stmt.nextlist = yyvsp[-2].bool.falselist;
        }
break;
case 19:
#line 644 ".\compile.y"
{ printf("产生式：stmt->break ;\n"); }
break;
case 20:
#line 645 ".\compile.y"
{ printf("产生式：stmt->block\n"); 
                                            yyval.matched_stmt.nextlist = yyvsp[0].block.nextlist;
        }
break;
case 21:
#line 649 ".\compile.y"
{ printf("产生式：stmt->if (bool) stmt else stmt\n");
                                                                    backpatch( yyvsp[-7].bool.truelist, yyvsp[-5].M.instr );
                                                                    backpatch( yyvsp[-7].bool.falselist, yyvsp[-1].M.instr );
                                                                    yyval.unmatched_stmt.nextlist = Merge( yyvsp[-4].matched_stmt.nextlist, yyvsp[-3].unmatched_stmt.nextlist );
                                                                    yyval.unmatched_stmt.nextlist = Merge( yyval.unmatched_stmt.nextlist, yyvsp[0].unmatched_stmt.nextlist );
}
break;
case 22:
#line 655 ".\compile.y"
{printf("产生式：stmt->if (bool) stmt\n"); 
                                            backpatch( yyvsp[-3].bool.truelist, yyvsp[-1].M.instr );
                                            yyval.unmatched_stmt.nextlist = Merge( yyvsp[-3].bool.falselist, yyvsp[0].stmt.nextlist );
        }
break;
case 23:
#line 659 ".\compile.y"
{ 
            printf("产生式：bool->bool || join\n");
            yyval.bool.truelist = Merge( yyvsp[-3].bool.truelist, yyvsp[0].join.truelist );
            yyval.bool.falselist = yyvsp[0].join.falselist;
            backpatch( yyvsp[-3].bool.falselist, yyvsp[-1].M.instr );
        }
break;
case 24:
#line 665 ".\compile.y"
{ printf("产生式：bool->join\n"); 
                                            yyval.bool.truelist = yyvsp[0].join.truelist;
                                            yyval.bool.falselist = yyvsp[0].join.falselist;
        }
break;
case 25:
#line 671 ".\compile.y"
{ 
            printf("产生式：join->join && equality\n"); 
            yyval.join.truelist = yyvsp[0].equality.truelist;
            yyval.join.falselist = Merge( yyvsp[-3].join.falselist, yyvsp[0].equality.falselist );
            backpatch( yyvsp[-3].join.truelist, yyvsp[-1].M.instr );
        }
break;
case 26:
#line 677 ".\compile.y"
{ printf("产生式：join->equality\n"); 
                                            yyval.join.truelist = yyvsp[0].equality.truelist;
                                            yyval.join.falselist = yyvsp[0].equality.falselist;
        }
break;
case 27:
#line 683 ".\compile.y"
{ printf("产生式：equality->equality == rel\n"); 
                                            yyval.equality.truelist = Makelist( QuadTable.len );
                                            yyval.equality.falselist = Makelist( QuadTable.len + 1 );
                                            Gen( OEQGoto, yyvsp[-2].equality.addr, yyvsp[0].rel.addr, -1, yyvsp[-2].equality.str, yyvsp[0].rel.str, "" );
                                            Gen( OGoto, -1, -1, -1, "", "", "" );
}
break;
case 28:
#line 689 ".\compile.y"
{ printf("产生式：equality->equality != rel\n"); 
                                            yyval.equality.truelist = Makelist( QuadTable.len );
                                            yyval.equality.falselist = Makelist( QuadTable.len + 1 );
                                            Gen( ONEQGoto, yyvsp[-2].equality.addr, yyvsp[0].rel.addr, -1, yyvsp[-2].equality.str, yyvsp[0].rel.str, "" );
                                            Gen( OGoto, -1, -1, -1, "", "", "" );
        }
break;
case 29:
#line 695 ".\compile.y"
{ printf("产生式：equality->rel\n"); 
                                            yyval.equality.truelist = yyvsp[0].rel.truelist;
                                            yyval.equality.falselist = yyvsp[0].rel.falselist;
        }
break;
case 30:
#line 701 ".\compile.y"
{ printf("产生式：rel -> expr < expr\n"); 
                                yyval.rel.truelist = Makelist( QuadTable.len );
                                yyval.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLTGoto, yyvsp[-2].expr.addr, yyvsp[0].expr.addr, -1, yyvsp[-2].expr.str, yyvsp[0].expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
break;
case 31:
#line 707 ".\compile.y"
{ printf("产生式：rel -> expr <= expr\n"); 
                                yyval.rel.truelist = Makelist( QuadTable.len );
                                yyval.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLEGoto, yyvsp[-2].expr.addr, yyvsp[0].expr.addr, -1, yyvsp[-2].expr.str, yyvsp[0].expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
break;
case 32:
#line 713 ".\compile.y"
{ printf("产生式：rel -> expr > expr\n"); 
                                yyval.rel.truelist = Makelist( QuadTable.len );
                                yyval.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OGTGoto, yyvsp[-2].expr.addr, yyvsp[0].expr.addr, -1, yyvsp[-2].expr.str, yyvsp[0].expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
break;
case 33:
#line 719 ".\compile.y"
{ printf("产生式：rel -> expr >= expr\n"); 
                                yyval.rel.truelist = Makelist( QuadTable.len );
                                yyval.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLEGoto, yyvsp[-2].expr.addr, yyvsp[0].expr.addr, -1, yyvsp[-2].expr.str, yyvsp[0].expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
break;
case 34:
#line 725 ".\compile.y"
{ printf("产生式：rel -> expr\n");
                                /* ne 0*/
                                yyval.rel.truelist = Makelist( QuadTable.len );
                                yyval.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( ONEQGoto, yyvsp[0].expr.addr, 0, -1, yyvsp[0].expr.str, "", "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
break;
case 35:
#line 735 ".\compile.y"
{ printf("产生式：expr->expr + term\n");  
                            Gen( yyvsp[-1].term.type==INT ? OIntAdd : OFloatAdd, yyvsp[-2].expr.addr, yyvsp[0].term.addr, 
                            NewTemp( TopSymbolList, yyval.expr.str, yyval.expr.width ), 
                            yyvsp[-2].expr.str, yyvsp[0].expr.str, yyval.expr.str );
                            }
break;
case 36:
#line 740 ".\compile.y"
{ printf("产生式：expr->expr - term\n");  
                            Gen( yyvsp[-1].term.type==INT ? OIntSub : OFloatSub, yyvsp[-2].expr.addr, yyvsp[0].term.addr, 
                            NewTemp( TopSymbolList, yyval.expr.str, yyval.expr.width ), 
                            yyvsp[-2].expr.str, yyvsp[0].expr.str, yyval.expr.str );
                            }
break;
case 37:
#line 745 ".\compile.y"
{ printf("产生式：expr->term\n");
							strcpy( yyval.expr.str, yyvsp[0].term.str );
							yyval.expr.type = yyvsp[0].term.type;
							yyval.expr.addr = yyvsp[0].term.addr;
							yyval.expr.width = yyvsp[0].term.width;	
	                      }
break;
case 38:
#line 753 ".\compile.y"
{ printf("产生式：term->term*factor\n"); 
                        Gen( yyvsp[-1].factor.type==INT ? OIntMultiply : OFloatMultiply, yyvsp[-2].term.addr, yyvsp[0].factor.addr, NewTemp( TopSymbolList, yyval.term.str, yyval.term.width ), yyvsp[-2].term.str, yyvsp[0].factor.str, yyval.term.str );
}
break;
case 39:
#line 756 ".\compile.y"
{ printf("产生式：term->term/factor\n"); 
                        Gen( yyvsp[-1].factor.type==INT ? OIntDivide : OFloatDivide, yyvsp[-2].term.addr, yyvsp[0].factor.addr, NewTemp( TopSymbolList, yyval.term.str, yyval.term.width ), yyvsp[-2].term.str, yyvsp[0].factor.str, yyval.term.str );
      }
break;
case 40:
#line 759 ".\compile.y"
{ printf("产生式：term->factor\n");
							strcpy( yyval.term.str, yyvsp[0].unary.str );
							yyval.term.type = yyvsp[0].unary.type;
							yyval.term.addr = yyvsp[0].unary.addr;
							yyval.term.width = yyvsp[0].unary.width;	
	                      }
break;
case 41:
#line 766 ".\compile.y"
{printf("unary->factor\n"); 
                        strcpy( yyval.unary.str, yyvsp[0].factor.str );
                        yyval.unary.type = yyvsp[0].factor.type;
                        yyval.unary.addr = yyvsp[0].factor.addr;
                        yyval.unary.width = yyvsp[0].factor.width;	
                    }
break;
case 42:
#line 774 ".\compile.y"
{ printf("产生式：factor->(expr)\n" );
							strcpy( yyval.factor.str, yyvsp[-1].expr.str );
							yyval.factor.type  = yyvsp[-1].expr.type;
							yyval.factor.addr  = yyvsp[-1].expr.addr;
							yyval.factor.width = yyvsp[-1].expr.width;
                            }
break;
case 43:
#line 780 ".\compile.y"
{ 
	                        struct SymbolElem * p;
							printf("产生式：factor->id\n"); 
							p = LookUpAllSymbolList( TopSymbolList, yyvsp[0].id.name );
							if( p != NULL ) {
								strcpy( yyval.factor.str, p->name );
								yyval.factor.type  = p->type;
								yyval.factor.addr  = p->addr;
								yyval.factor.width = p->width;
							}							    
							else {
							    yyerror( "变量名没有定义" );
								strcpy( yyval.factor.str, "no_id_defined" ); /*容错处理*/
								yyval.factor.type = INT;
								yyval.factor.addr = -1;
								yyval.factor.width = INT_WIDTH;							    
							}
	                      }
break;
case 44:
#line 799 ".\compile.y"
{                        
							struct ConstElem * p; 
							    printf("产生式：factor->CONST\n");

								p = LookUpConstList( yyvsp[0].constval.type, yyvsp[0].constval.value, yyvsp[0].constval.width ) ;
								if( p== NULL )
                                    p = AddToConstList( yyvsp[0].constval.str, yyvsp[0].constval.type, yyvsp[0].constval.value, yyvsp[0].constval.width );

								strcpy( yyval.factor.str, yyvsp[0].constval.str );
								yyval.factor.type  = yyvsp[0].constval.type;
								yyval.factor.addr  = p->addr;
								yyval.factor.width = p->width;
                          }
break;
#line 1311 "y.tab.c"
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
