%{ 
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
    //  printf("%d\n\n\n", QuadTable.len);
    for( i=0, ptr = QuadTable.base; i < QuadTable.len; i++,ptr++ ) {
        fprintf(fp, "%5d:  ", QuadTable.startaddr + i);
        op = ptr->op;
        // printf("OP = %d\n", op);
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
 




%}

%token BASIC
%token CONST
%token ID

%token ELSE
%token IF
%token WHILE
%token DO
%token BREAK

%token RELOP_LT
%token RELOP_LE
%token RELOP_GT
%token RELOP_GE

%token RELOP_EQ
%token RELOP_NEQ

%token OR
%token AND

%right '='
%left '+' '-'
%left '*' '/'
%right '!' UMINUS

%%
program : block                                 { printf("产生式：program->block\n"); } 
        ;

block   : '{' blockM1 decls stmts blockM2 '}'   { printf("产生式：block->{decls stmts}\n"); 
                                                    $$.block.nextlist = $4.stmts.nextlist;
} 
        ;

blockM1 :                     { TopSymbolList = CreateSymbolList( TopSymbolList, TopSymbolList->endaddr ); }
        ;

blockM2 :                     { SymbolList env;
                                 PrintSymbolList( TopSymbolList); 
                                 env = TopSymbolList->prev;
                                 DestroySymbolList( TopSymbolList ); 
                                 TopSymbolList = env;                 
                              }
		;
M       :      {$$.M.instr = QuadTable.len + QuadTable.startaddr;}
        ;
N       :       {Gen( OGoto, -1, -1, -1, "", "", "" );
                $$.N.nextlist = Makelist( QuadTable.len - 1 );}
        ;
decls   : decls decl          { printf("产生式：decls->decls decl\n"); }
        |                     { printf("产生式：decls->null\n"); }
        ;

decl    : type ID ';'         { int width;
                                
                                printf("产生式：decl->type ID; ID=%s\n",$2.id.name); 
                                
								switch( $1.basic.type ) {
                                    case CHAR  : width = CHAR_WIDTH;  break;
                                    case INT   : width = INT_WIDTH;   break;
                                    case FLOAT : width = FLOAT_WIDTH; break;
                                    case BOOL  : width = BOOL_WIDTH;  break;
                                    default    : width = -1; break;
                                }
                                AddToSymbolList( TopSymbolList, $2.id.name, $1.basic.type, width );

                              }
		;

type    : BASIC               { printf("产生式：type->BASIC\n"); $$.basic.type = $1.basic.type; }
        ;

stmts   : stmts M stmt    {printf("产生式：stmts->stmts stmt\n");
                            if ( $1.stmts.nextlist != NULL ) {
                                backpatch( $1.stmts.nextlist, $2.M.instr );
                            }
                            $$.stmts.nextlist = $3.stmts.nextlist;
}
        | /*empty*/     {printf("产生式：stmts->null\n");
                        $$.stmts.nextlist = NULL;
        }
        ;
stmt:   matched_stmt        {
            $$.stmt.nextlist = $1.matched_stmt.nextlist;
        }
        | unmatched_stmt    {
            $$.stmt.nextlist = $1.unmatched_stmt.nextlist;
        }
        ;
matched_stmt:   ID '=' expr ';'                  {  printf("产生式：stmt->id = expr;\n");  
                                                    struct SymbolElem *p;
                                                    p = LookUpAllSymbolList( TopSymbolList, $1.id.name );
                                                    if( p==NULL ) {
                                                        yyerror("语义错误：变量未定义！");
                                                        return 0;
                                                    }
                                                    if( p->type != $3.expr.type ) {
                                                        yyerror("语义错误：赋值号两边的类型不匹配！");
                                                        return 0;
                                                    }
                                                    Gen( p->type==INT ? OIntEvaluation : OFloatEvaluation, $3.expr.addr, -1, p->addr, $3.expr.str, "", p->name );
                                                    $$.matched_stmt.nextlist = NULL;
                                                }
        | IF '(' bool ')' M matched_stmt N ELSE M matched_stmt  { printf("产生式：stmt->if (bool) stmt else stmt\n"); 
                                                            backpatch( $3.bool.truelist, $5.M.instr );
                                                            backpatch( $3.bool.falselist, $9.M.instr );
                                                            $$.matched_stmt.nextlist = Merge( $6.matched_stmt.nextlist, $7.matched_stmt.nextlist );
                                                            $$.matched_stmt.nextlist = Merge( $$.matched_stmt.nextlist, $10.matched_stmt.nextlist );
                                                            }
        | WHILE M '(' bool ')' M matched_stmt          { printf("产生式：stmt->while (bool) stmt\n"); 
                                                    backpatch( $4.bool.truelist, $6.M.instr );
                                                    backpatch( $7.matched_stmt.nextlist, $2.M.instr );
                                                    $$.matched_stmt.nextlist = $4.bool.falselist;
                                                    Gen( OGoto, -1, -1, $2.M.instr, "", "", "" );
        }
        | DO M matched_stmt WHILE M '(' bool ')' ';'   { printf("产生式：stmt->do stmt while (bool)\n"); 
                                                    backpatch( $3.matched_stmt.nextlist, $5.M.instr );
                                                    backpatch( $7.bool.truelist, $2.M.instr );
                                                    $$.matched_stmt.nextlist = $7.bool.falselist;
        }
        | BREAK  ';'                       { printf("产生式：stmt->break ;\n"); }
        | block                            { printf("产生式：stmt->block\n"); 
                                            $$.matched_stmt.nextlist = $1.block.nextlist;
        }
        ;
unmatched_stmt: IF '(' bool ')' M matched_stmt N ELSE M unmatched_stmt { printf("产生式：stmt->if (bool) stmt else stmt\n");
                                                                    backpatch( $3.bool.truelist, $5.M.instr );
                                                                    backpatch( $3.bool.falselist, $9.M.instr );
                                                                    $$.unmatched_stmt.nextlist = Merge( $6.matched_stmt.nextlist, $7.unmatched_stmt.nextlist );
                                                                    $$.unmatched_stmt.nextlist = Merge( $$.unmatched_stmt.nextlist, $10.unmatched_stmt.nextlist );
}
        | IF '(' bool ')' M matched_stmt       {printf("产生式：stmt->if (bool) stmt\n"); 
                                            backpatch( $3.bool.truelist, $5.M.instr );
                                            $$.unmatched_stmt.nextlist = Merge( $3.bool.falselist, $6.stmt.nextlist );
        }
bool  :   bool OR M join                     { 
            printf("产生式：bool->bool || join\n");
            $$.bool.truelist = Merge( $1.bool.truelist, $4.join.truelist );
            $$.bool.falselist = $4.join.falselist;
            backpatch( $1.bool.falselist, $3.M.instr );
        }
        | join                             { printf("产生式：bool->join\n"); 
                                            $$.bool.truelist = $1.join.truelist;
                                            $$.bool.falselist = $1.join.falselist;
        }  
		;

join  :   join AND M equality                { 
            printf("产生式：join->join && equality\n"); 
            $$.join.truelist = $4.equality.truelist;
            $$.join.falselist = Merge( $1.join.falselist, $4.equality.falselist );
            backpatch( $1.join.truelist, $3.M.instr );
        } 
        | equality                         { printf("产生式：join->equality\n"); 
                                            $$.join.truelist = $1.equality.truelist;
                                            $$.join.falselist = $1.equality.falselist;
        } 
        ;

equality : equality RELOP_EQ  rel          { printf("产生式：equality->equality == rel\n"); 
                                            $$.equality.truelist = Makelist( QuadTable.len );
                                            $$.equality.falselist = Makelist( QuadTable.len + 1 );
                                            Gen( OEQGoto, $1.equality.addr, $3.rel.addr, -1, $1.equality.str, $3.rel.str, "" );
                                            Gen( OGoto, -1, -1, -1, "", "", "" );
}
        |  equality RELOP_NEQ rel          { printf("产生式：equality->equality != rel\n"); 
                                            $$.equality.truelist = Makelist( QuadTable.len );
                                            $$.equality.falselist = Makelist( QuadTable.len + 1 );
                                            Gen( ONEQGoto, $1.equality.addr, $3.rel.addr, -1, $1.equality.str, $3.rel.str, "" );
                                            Gen( OGoto, -1, -1, -1, "", "", "" );
        }
        |  rel                             { printf("产生式：equality->rel\n"); 
                                            $$.equality.truelist = $1.rel.truelist;
                                            $$.equality.falselist = $1.rel.falselist;
        }
		;
 
rel     :  expr RELOP_LT expr   { printf("产生式：rel -> expr < expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLTGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr RELOP_LE expr   { printf("产生式：rel -> expr <= expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLEGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr RELOP_GT expr   { printf("产生式：rel -> expr > expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OGTGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr RELOP_GE expr   { printf("产生式：rel -> expr >= expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLEGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr                 { printf("产生式：rel -> expr\n");
                                // ne 0
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( ONEQGoto, $1.expr.addr, 0, -1, $1.expr.str, "", "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        ;


expr  : expr  '+' term    { printf("产生式：expr->expr + term\n");  
                            Gen( $2.term.type==INT ? OIntAdd : OFloatAdd, $1.expr.addr, $3.term.addr, 
                            NewTemp( TopSymbolList, $$.expr.str, $$.expr.width ), 
                            $1.expr.str, $3.expr.str, $$.expr.str );
                            }
      | expr  '-' term    { printf("产生式：expr->expr - term\n");  
                            Gen( $2.term.type==INT ? OIntSub : OFloatSub, $1.expr.addr, $3.term.addr, 
                            NewTemp( TopSymbolList, $$.expr.str, $$.expr.width ), 
                            $1.expr.str, $3.expr.str, $$.expr.str );
                            } 
      | term              { printf("产生式：expr->term\n");
							strcpy( $$.expr.str, $1.term.str );
							$$.expr.type = $1.term.type;
							$$.expr.addr = $1.term.addr;
							$$.expr.width = $1.term.width;	
	                      }
      ;

term  : term '*' unary  { printf("产生式：term->term*factor\n"); 
                        Gen( $2.factor.type==INT ? OIntMultiply : OFloatMultiply, $1.term.addr, $3.factor.addr, NewTemp( TopSymbolList, $$.term.str, $$.term.width ), $1.term.str, $3.factor.str, $$.term.str );
}
      | term '/' unary  { printf("产生式：term->term/factor\n"); 
                        Gen( $2.factor.type==INT ? OIntDivide : OFloatDivide, $1.term.addr, $3.factor.addr, NewTemp( TopSymbolList, $$.term.str, $$.term.width ), $1.term.str, $3.factor.str, $$.term.str );
      }
      | unary            { printf("产生式：term->factor\n");
							strcpy( $$.term.str, $1.unary.str );
							$$.term.type = $1.unary.type;
							$$.term.addr = $1.unary.addr;
							$$.term.width = $1.unary.width;	
	                      }
      ;
unary : factor      {printf("unary->factor\n"); 
                        strcpy( $$.unary.str, $1.factor.str );
                        $$.unary.type = $1.factor.type;
                        $$.unary.addr = $1.factor.addr;
                        $$.unary.width = $1.factor.width;	
                    }
      ;

factor: '(' expr ')'      { printf("产生式：factor->(expr)\n" );
							strcpy( $$.factor.str, $2.expr.str );
							$$.factor.type  = $2.expr.type;
							$$.factor.addr  = $2.expr.addr;
							$$.factor.width = $2.expr.width;
                            }
      | ID                { 
	                        struct SymbolElem * p;
							printf("产生式：factor->id\n"); 
							p = LookUpAllSymbolList( TopSymbolList, $1.id.name );
							if( p != NULL ) {
								strcpy( $$.factor.str, p->name );
								$$.factor.type  = p->type;
								$$.factor.addr  = p->addr;
								$$.factor.width = p->width;
							}							    
							else {
							    yyerror( "变量名没有定义" );
								strcpy( $$.factor.str, "no_id_defined" ); /*容错处理*/
								$$.factor.type = INT;
								$$.factor.addr = -1;
								$$.factor.width = INT_WIDTH;							    
							}
	                      }

      | CONST              {                        
							struct ConstElem * p; 
							    printf("产生式：factor->CONST\n");

								p = LookUpConstList( $1.constval.type, $1.constval.value, $1.constval.width ) ;
								if( p== NULL )
                                    p = AddToConstList( $1.constval.str, $1.constval.type, $1.constval.value, $1.constval.width );

								strcpy( $$.factor.str, $1.constval.str );
								$$.factor.type  = $1.constval.type;
								$$.factor.addr  = p->addr;
								$$.factor.width = p->width;
                          }
      ; 

%%
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





   