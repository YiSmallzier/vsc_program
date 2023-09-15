%{ 
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int LineNo = 1; /*��ǰlookahead��ָ����ַ����ڵ��кţ�����ǰ���뵽���к�*/

int CompileFailed = 0;

void yyerror( char * ErrStr )
{
    CompileFailed = 1; /*����ʧ�ܱ�־*/
    printf("������Ϣ:%s, �к�:%d\n", ErrStr, LineNo);
}



/*�����ͳ����Ļ�������BASIC*/
#define CHAR     1
#define INT      2
#define FLOAT    3
#define BOOL     4

#define CHAR_WIDTH  1
#define INT_WIDTH   4
#define FLOAT_WIDTH 8  
#define BOOL_WIDTH  1

/*****************************���棺���ű��Ķ������غ���*******************************/

/*���������Ȳ�����ID_MAX_LEN ���ַ�*/
#define ID_MAX_LEN   64

/*���һ����ʶ��*/
struct SymbolElem {
    char name[ ID_MAX_LEN + 1 ]; /*������(���������)���Ȳ�����ID_MAX_LEN ���ַ�*/
    int type; /*�������������������int, �������ֻ���������ͣ���ʵ�ʵı������У�����Ҫ�������ṹ*/
    int  addr;      /*Ϊ�ñ�������Ŀռ���׵�ַ*/
	int  width;     /*�ñ����Ŀ��ȣ���ռ�ö��ٸ��ֽ�*/
    struct SymbolElem * next;  /*ָ����һ����ʶ��*/
};

/*��ʶ����*/
typedef struct SymbolList{
    struct SymbolElem * head;  /*ָ����ű���������ʵ�֣��ĵ�һ����㣬û��ͷ���,��ʼ��ΪNULL*/
    struct SymbolList * prev; /*��һ��ķ��ű�*/
    int beginaddr; /*�÷��ű��з������������ʱ�����ռ�Ŀ�ʼ��ַ*/
    int endaddr;    /*�÷��ű��з������������ʱ�����ռ�Ľ�����ַ*/
                   /*beginaddr~endaddr�Ŀռ��Ÿ÷��ű������б�������ʱ����*/
} * SymbolList;  /*���ű�*/

SymbolList TopSymbolList=NULL; /*ȫ�ֱ�������ŵ�ǰ�ķ��ű�������ǰ�������ķ��ű���,��ӦΪ���ϵ�top*/

/*����������һ���µķ��ű���SymbolList�������ϵ�Env����PrevList�������һ����ű�*/
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

/*�ڷ��ű�List�в����Ƿ���ڱ�ʶ��IdName��������ڣ��򷵻ظý��ָ�룬���򷵻ؿ�*/
struct SymbolElem * LookUpSymbolList( SymbolList List, char * IdName )
{struct SymbolElem * p;
    if( List==NULL ) return NULL;
    for( p = List->head; p!=NULL; p = p->next ) 
        if( strcmp( p->name, IdName ) == 0 ) break;
    return p;
}

/*�ӷ��ű�List��ʼ�����ϵ�����һ����ű��в����Ƿ���ڱ�ʶ��IdName��������ڣ��򷵻ظý��ָ�룬���򷵻ؿ�*/
struct SymbolElem * LookUpAllSymbolList( SymbolList List, char * IdName )
{ SymbolList env;
struct SymbolElem * p;
    env = List;
    while( env!=NULL ) {
        p = LookUpSymbolList( env, IdName );
        if(  p != NULL ) return p; /*�ҵ��÷���*/
        env = env->prev;
    }
    return NULL;
}


/*����һ���µķ��Ž��,�����ӵ����ű��У����󷵻ظý��ָ��*/
struct SymbolElem * AddToSymbolList( SymbolList List, char * IdName,int IdType, int Width )
{struct SymbolElem * p;

    p = (struct SymbolElem *) malloc( sizeof(struct SymbolElem) );

    strcpy( p->name, IdName );
    p->type = IdType;
	p->width = Width;
	p->addr = List->endaddr;
	List->endaddr += Width;

    p->next = List->head;  /*���ñ�ʶ�����ӵ����ű���ͷ*/
    List->head = p;

    return p;    
}

void PrintSymbolList( SymbolList List )
{struct SymbolElem * p;
    printf("***********************�����б�*************************\n");
    if( List ==NULL ) return ;
    for( p=List->head; p!=NULL; p=p->next ) {
        printf("������:%s, ����:", p->name);
		switch( p->type ) {
            case CHAR : printf("char");  break;
            case INT  : printf("int");   break;
            case FLOAT: printf("float"); break;
            case BOOL : printf("bool");  break;
		}
        printf(", ��ַ:%d, ����:%d\n", p->addr, p->width );
	}
    printf("*************�ñ����б���ռ��%d���ֽڿռ�***************\n", List->endaddr - List->beginaddr);
}

/*����һ����ʱ����,������ʱ�����ĵ�ַ����ʱ����������*/
int NewTemp( SymbolList List, char Name[], int Width )
{ static int TempID = 1;
  int addr;
    sprintf( Name, "T%d", TempID++ ); /*����T1��T2��*/
	addr = List->endaddr;
    List->endaddr += Width;
  
    return addr;
}

/*****************************���棺���ű��Ķ������غ���*****************************/




/*****************************���棺�������Ķ������غ���*******************************/

union ConstVal {
        char    ch;    /*����ַ�����*/
        int     n;     /*������ͳ�������true=1��false=0 */
        double  f;     /*��Ÿ���������*/
};	

/*���һ������*/
struct ConstElem {
    char str[ID_MAX_LEN + 1 ]; /*�ñ������ڴ洢�������ı���ʽ����ʾ��ʱ���õ�,ʵ�ʵı���ϵͳ����Ҫ*/	   
    int type; /*�������������������int*/
    union ConstVal value;
    int  addr;      /*Ϊ�ó�������Ŀռ���׵�ַ*/
	int  width;     /*�ñ����Ŀ��ȣ���ռ�ö��ٸ��ֽ�*/
    struct ConstElem * next;  /*ָ����һ������*/
};

/*������*/
struct ConstList{
    struct ConstElem * head;  /*ָ��������������ʵ�֣��ĵ�һ����㣬û��ͷ���,��ʼ��ΪNULL*/
    int beginaddr;  /*�÷��ű��з���������ռ�Ŀ�ʼ��ַ*/
    int endaddr;    /*�÷��ű��з���������ռ�Ľ�����ַ*/
                   /*beginaddr~endaddr�Ŀռ��Ÿó����������г���*/
} ConstList ;  /*��������ȫ�ֱ�����ע����������ֻ��Ҫһ��������**/


/*���������س�����*/
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

/*�ڳ�����ConstList�в����Ƿ���ڳ�����������ڣ��򷵻ظý��ָ�룬���򷵻ؿ�*/
struct ConstElem * LookUpConstList( int ConstType, union ConstVal ConstValue, int Width )
{struct ConstElem * p;
    for( p = ConstList.head; p!=NULL; p = p->next ) 
        if( p->type == ConstType && memcmp( &p->value,&ConstValue, Width) == 0 )  break;
	
    return p;
}


/*����һ���µĳ������,�����ӵ��������У����󷵻ظý��ָ��*/
struct ConstElem * AddToConstList( char * Str, int ConstType, union ConstVal ConstValue, int Width )
{struct ConstElem * p;

    p = (struct ConstElem *) malloc( sizeof(struct ConstElem) );

    strcpy( p->str, Str );
    p->type = ConstType;
    p->value = ConstValue;
	p->width = Width;

	p->addr = ConstList.endaddr;
	ConstList.endaddr += Width;

    p->next = ConstList.head;  /*���ó������ӵ���������ͷ*/
    ConstList.head = p;

    return p;    
}

void PrintConstList(void)
{struct ConstElem * p;
    printf("***********************�����б�*************************\n");
    for( p=ConstList.head; p!=NULL; p=p->next ) {
	    printf("����:%s, ����:", p->str);
		switch( p->type ) {
            case CHAR : printf("char");  break;
            case INT  : printf("int");   break;
            case FLOAT: printf("float"); break;
            case BOOL : printf("bool");  break;
		}
        printf(", ��ַ:%d, ����:%d\n", p->addr, p->width );
	}
    printf("**************�ó����б���ռ��%d���ֽڿռ�***************\n", ConstList.endaddr - ConstList.beginaddr);
}

/*****************************���棺�������Ķ������غ���*****************************/





/********************************����:��Ԫʽ�Ķ���ͺ���****************************/

/* ���ͼӼ��˳� */
#define OIntAdd          1001
#define OIntSub          1002
#define OIntMultiply     1003
#define OIntDivide       1004

/* �������Ӽ��˳� */
#define OFloatAdd        1011
#define OFloatSub        1012
#define OFloatMultiply   1013
#define OFloatDivide     1014

/*��ֵa=b*/
#define OIntEvaluation   1021
#define OFloatEvaluation 1022
#define OCharEvaluation  1023
#define OBoolEvaluation  1024

/* ������goto��� */
#define OGoto            1031

/* if a op b goto ��� */
#define OGTGoto          1041
#define OGEGoto          1042
#define OLTGoto          1043
#define OLEGoto          1044
#define OEQGoto          1045
#define ONEQGoto         1046

/*����ת�������*/
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

/*��Ԫʽ���ݽṹ*/
struct Quadruple {
    int op; /*�����*/
    int arg1; /*��ŵ�һ�������ĵ�ַ�������Ǳ�������ʱ�����ĵ�ַ*/
    int arg2;
    int arg3;/*��ŵ����������ĵ�ַ�������Ǳ�������ʱ�����ĵ�ַ������������Ԫʽ�ĵ�ַ(Goto �ĵ�ַ����)*/
    char arg1name[ID_MAX_LEN + 1]; /*������Ҫ��������ʾʱ����ʾarg1��Ӧ�ı�������ʱ����������(���еĻ���*/
    char arg2name[ID_MAX_LEN + 1]; /*������Ҫ��������ʾʱ����ʾarg2��Ӧ�ı�������ʱ����������(���еĻ���*/
    char arg3name[ID_MAX_LEN + 1]; /*������Ҫ��������ʾʱ����ʾarg3��Ӧ�ı�������ʱ����������(���еĻ���*/
};

/*��Ԫʽ��*/
struct QuadTable {
    int startaddr; /*��Ԫʽ��ʼ��ŵĵ�ַ,����100*/
    struct Quadruple * base; /*ָ��һ���ڴ棬������Ŷ����Ԫʽ����base[0]��ʼ���*/
    int size; /*base�п��Դ�ŵ���Ԫʽ�ĸ���*/
    int len; /*base[len]����һ����ԪʽҪ��ŵĿռ�*/

};

struct QuadTable QuadTable; /*ֻ��Ҫһ����Ԫʽ��*/

void CreateQuadTable(int StartAddr)
{
    QuadTable.startaddr = StartAddr; 
    QuadTable.size = 1000; /* һ��ʼ������Դ��1000����Ԫʽ*/
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

/*��Arg1�Ǳ�������ʱ����ʱ��Arg1Name�Ǹñ���������,������ʾʱʹ�ã����������ͬ */
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
    printf("%d\n", QuadTable.len);

    return QuadTable.len - 1;
}

/*����Ԫʽ����Ӧ������ַ����д�뵽�ļ���*/
void WriteQuadTableToFile( const char * FileName )
{FILE * fp;
struct Quadruple * ptr;
int i,op;
char str[1000],ch;
    fp = fopen( FileName, "w" );
    if( fp==NULL ) return;
    for( i=0, ptr = QuadTable.base; i < QuadTable.len; i++,ptr++ ) {
        fprintf(fp, "%5d:  ", QuadTable.startaddr + i);
        op = ptr->op;
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

            default: yyerror("������󣺳��ֲ���ʶ���������"); strcpy(str, "error: Unknown operator");break;
        }
        fprintf(fp,"%s\n",str);
    }

    fclose(fp);
}

/********************************����:��Ԫʽ�Ķ���ͺ���****************************/



/**************����:����䷨����ջ��Ԫ�ص���Ϣ�����ս���ͷ��ս�����ۺ�����****************/
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
	    /*������(���������)���Ȳ�����ID_MAX_LEN ���ַ�*/
        /*�������hash�����洢���еĲ�ͬ���ֵı�ʶ�������֣�����Ϳ���ʹ��һ��ָ��ָ��ñ�ʶ�������֣�
		  �ô��Ǽ��ٷ���ջ��Ԫ�صĿռ��С���Ӷ���ʡ�ռ�����߱���Ч�ʣ�*/
        char name[ID_MAX_LEN + 1 ]; 
    }id;  /*��ʶ��:�ս��ID���ۺ�����*/

    struct {
	   char str[ID_MAX_LEN + 1 ]; /*�ñ������ڴ洢�������ı���ʽ����ʾ��ʱ���õ�,ʵ�ʵı���ϵͳ����Ҫ*/	   
       int type; /*�������������������INT*/
	   union ConstVal value; /*�������ս��CONST����Ϣ*/
	   int width;
	} constval; /*�ս��const���ۺ�����*/

    struct {
        int type; /*�������������������INT*/
    }basic; /*�����������ͣ��ս��BASIC���ۺ�����*/

	struct {
	   char str[ID_MAX_LEN + 1 ]; /*�ñ������ڴ洢����������ʱ�������������ı���ʽ����ʾ��ʱ���õ�,ʵ�ʵı���ϵͳ����Ҫ*/
	   int type;
	   int addr;
	   int width;
	} factor, term, expr;/*���ս��factor, term, expr���ۺ�����*/
        /*�����ķ����ŵ����Լ�¼�����������������*/
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
        char str[ID_MAX_LEN + 1 ]; /*�ñ������ڴ洢����������ʱ�������������ı���ʽ����ʾ��ʱ���õ�,ʵ�ʵı���ϵͳ����Ҫ*/
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
       

/**************����:����䷨����ջ��Ԫ�ص���Ϣ�����ս���ͷ��ս�����ۺ�����****************/
 




%}

%token BASIC
%token CONST
%token ID

%token IF
%token ELSE
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
program : block                                 { printf("����ʽ��program->block\n"); } 
        ;

block   : '{' blockM1 decls stmts blockM2 '}'   { printf("����ʽ��block->{decls stmts}\n"); 
                                                    $$.block.nextlist = $4.stmts.nextlist;
                                                    }
        ;

blockM1 : /* empty*/          { TopSymbolList = CreateSymbolList( TopSymbolList, TopSymbolList->endaddr ); }
        ;

blockM2 : /* empty*/          { SymbolList env;
                                 PrintSymbolList( TopSymbolList); 
                                 env = TopSymbolList->prev;
                                 DestroySymbolList( TopSymbolList ); 
                                 TopSymbolList = env;                 
                              }
		;
M       :       {$$.M.instr = QuadTable.len + QuadTable.startaddr;} 
        ;
N       :       {
                    Gen( OGoto, -1, -1, -1, "", "", "" );
                    $$.N.nextlist = Makelist( QuadTable.len - 1 );
                }
        ;

decls   : decls decl          { printf("����ʽ��decls->decls decl\n"); }
        |                     { printf("����ʽ��decls->null\n"); }
        ;

decl    : type ID ';'         { int width;
                                
                                printf("����ʽ��decl->type ID; ID=%s\n",$2.id.name); 
                                
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

type    : BASIC               { printf("����ʽ��type->BASIC\n"); $$.basic.type = $1.basic.type; }
        ;

stmts   : stmts M stmt    {printf("����ʽ��stmts->stmts stmt\n");
                            if ( $1.stmts.nextlist != NULL ) {
                                backpatch( $1.stmts.nextlist, $2.M.instr );
                            }
                            $$.stmts.nextlist = $3.stmts.nextlist;
                            }
        | /*empty*/     {printf("����ʽ��stmts->null\n");
                        $$.stmts.nextlist = NULL;
                        }
        ;

/* stmt    : ID '=' expr ';'                  { printf("����ʽ��stmt->id = expr;\n"); }
        | IF '(' bool ')' stmt             { printf("����ʽ��stmt->if (bool) stmt\n");}
        | IF '(' bool ')' stmt ELSE stmt   { printf("����ʽ��stmt->if (bool) stmt esle stmt\n"); }
        | WHILE '(' bool ')' stmt          { printf("����ʽ��stmt->while (bool) stmt\n"); }
        | DO stmt WHILE '(' bool ')' ';'   { printf("����ʽ��stmt->do stmt while (bool)\n"); }
        | BREAK  ';'                       { printf("����ʽ��stmt->break ;\n"); }
        | block                            { printf("����ʽ��stmt->block\n"); }
        ; */
stmt            : matched_stmt { printf("����ʽ��stmt -> matched_stmt\n"); 
                                    $$.stmt.nextlist = $1.matched_stmt.nextlist;

                                    }
                | unmatched_stmt { printf("����ʽ��stmt -> unmatched_stmt\n"); 
                                    $$.stmt.nextlist = $1.unmatched_stmt.nextlist;}
                ;
+    : IF '(' bool ')' M matched_stmt N ELSE M matched_stmt      { printf("����ʽ��matched_stmt -> if (bool) matched_stmt else matched_stmt\n"); 
                                                                            backpatch( $3.bool.truelist, $5.M.instr );
                                                                            backpatch( $3.bool.falselist, $9.M.instr );
                                                                            $$.matched_stmt.nextlist = Merge( $6.matched_stmt.nextlist, $7.matched_stmt.nextlist );
                                                                            $$.matched_stmt.nextlist = Merge( $$.matched_stmt.nextlist, $10.matched_stmt.nextlist );
                                                                            }      
                | WHILE M '(' bool ')' M matched_stmt               { printf("����ʽ��matched_stmt -> while (bool) matched_stmt\n"); 
                                                                    backpatch( $4.bool.truelist, $6.M.instr );
                                                                    backpatch( $7.matched_stmt.nextlist, $2.M.instr );
                                                                    $$.matched_stmt.nextlist = Merge( $4.bool.falselist, $7.matched_stmt.nextlist );
                                                                    Gen( OGoto, -1, -1, $2.M.instr, "", "", "" );
                                                                    }
                | DO M matched_stmt WHILE M '(' bool ')' ';'        { printf("����ʽ��matched_stmt -> do matched_stmt while (bool)\n"); 
                                                                    backpatch( $3.matched_stmt.nextlist, $5.M.instr );
                                                                    backpatch( $7.bool.truelist, $2.M.instr );
                                                                    $$.matched_stmt.nextlist = $7.bool.falselist;
                                                                    }
                | BREAK ';'                                         { printf("����ʽ��matched_stmt -> break;\n"); }
                | block                                             { printf("����ʽ��matched_stmt -> block\n"); 
                                                                    // goto
                                                                    $$.matched_stmt.nextlist = $1.block.nextlist;
                                                                    }
                | ID '=' expr ';'                                   { printf("����ʽ��matched_stmt -> id = expr\n"); 
                                                                    // id = expr
                                                                    struct SymbolElem *p;
                                                                    p = LookUpAllSymbolList( TopSymbolList, $1.id.name );
                                                                    if( p==NULL ) {
                                                                        yyerror("������󣺱���δ���壡");
                                                                        return 0;
                                                                    }
                                                                    if( p->type != $3.expr.type ) {
                                                                        int op = (p->type == INT)
                                                                                ? (($3.expr.type == CHAR) ? OCharToInt
                                                                                : ($3.expr.type == FLOAT) ? OFloatToInt
                                                                                : ($3.expr.type == BOOL) ? OBoolToInt
                                                                                : op)
                                                                                : (p->type == CHAR)
                                                                                ? (($3.expr.type == INT) ? OIntToChar
                                                                                    : ($3.expr.type == FLOAT) ? OFloatToChar
                                                                                    : ($3.expr.type == BOOL) ? OBoolToChar
                                                                                    : op)
                                                                                : (p->type == FLOAT)
                                                                                ? (($3.expr.type == INT) ? OIntToFloat
                                                                                    : ($3.expr.type == CHAR) ? OCharToFloat
                                                                                    : ($3.expr.type == BOOL) ? OBoolToFloat
                                                                                    : op)
                                                                                : (p->type == BOOL)
                                                                                ? (($3.expr.type == INT) ? OIntToBool
                                                                                    : ($3.expr.type == CHAR) ? OCharToBool
                                                                                    : ($3.expr.type == FLOAT) ? OFloatToBool
                                                                                    : op)
                                                                                : op;
                                                                        Gen( op, $3.expr.addr, -1, p->addr, $3.expr.str, "", p->name );
                                                                    }
                                                                    else {
                                                                        Gen( p->type==INT ? OIntEvaluation : OFloatEvaluation, $3.expr.addr, -1, p->addr, $3.expr.str, "", p->name );
                                                                    }
                                                                    Gen( p->type==INT ? OIntEvaluation : OFloatEvaluation, $3.expr.addr, -1, p->addr, $3.expr.str, "", p->name );
                                                                    $$.matched_stmt.nextlist = NULL;
                                                                    }
                                
                ;
unmatched_stmt  : IF '(' bool ')' M stmt                                    { printf("����ʽ��unmatched_stmt -> if (bool) stmt\n"); 
                                                                            backpatch( $3.bool.truelist, $5.M.instr );
                                                                            $$.unmatched_stmt.nextlist = Merge( $3.bool.falselist, $6.stmt.nextlist );
                                                                            }
                | IF '(' bool ')' M matched_stmt N ELSE M unmatched_stmt    { printf("����ʽ��unmatched_stmt -> if (bool) matched_stmt else unmatched_stmt\n"); 
                                                                            backpatch( $3.bool.truelist, $5.M.instr );
                                                                            backpatch( $3.bool.falselist, $9.M.instr );
                                                                            $$.unmatched_stmt.nextlist = Merge( $6.matched_stmt.nextlist, $7.unmatched_stmt.nextlist );
                                                                            $$.unmatched_stmt.nextlist = Merge( $$.unmatched_stmt.nextlist, $10.unmatched_stmt.nextlist );
                                                                            }
                ;
bool  :   bool OR M join                     { printf("����ʽ��bool->bool || join\n");
                                            $$.bool.truelist = Merge( $1.bool.truelist, $4.join.truelist );
                                            $$.bool.falselist = $4.join.falselist;
                                            backpatch( $1.bool.falselist, $3.M.instr );
                                            }
        | join                             { printf("����ʽ��bool->join\n"); 
                                            $$.bool.truelist = $1.join.truelist;
                                            $$.bool.falselist = $1.join.falselist;
                                            }
		;

join  :   join AND M equality                { printf("����ʽ��join->join && equality\n"); 
                                            $$.join.truelist = $4.equality.truelist;
                                            $$.join.falselist = Merge( $1.join.falselist, $4.equality.falselist );
                                            backpatch( $1.join.truelist, $3.M.instr );
                                            } 
        | equality                         { printf("����ʽ��join->equality\n"); 
                                            $$.join.truelist = $1.equality.truelist;
                                            $$.join.falselist = $1.equality.falselist;
                                            } 
        ;

equality : equality RELOP_EQ  rel          { printf("����ʽ��equality->equality == rel\n"); 
                                            $$.equality.truelist = Makelist( QuadTable.len );
                                            $$.equality.falselist = Makelist( QuadTable.len + 1 );
                                            Gen( OEQGoto, $1.equality.addr, $3.rel.addr, -1, $1.equality.str, $3.rel.str, "" );
                                            Gen( OGoto, -1, -1, -1, "", "", "" );
                                            }
        |  equality RELOP_NEQ rel           { printf("����ʽ��equality->equality != rel\n"); 
                                            $$.equality.truelist = Makelist( QuadTable.len );
                                            $$.equality.falselist = Makelist( QuadTable.len + 1 );
                                            Gen( ONEQGoto, $1.equality.addr, $3.rel.addr, -1, $1.equality.str, $3.rel.str, "" );
                                            Gen( OGoto, -1, -1, -1, "", "", "" );
                                            }
        |  rel                              { printf("����ʽ��equality->rel\n"); 
                                            $$.equality.truelist = $1.rel.truelist;
                                            $$.equality.falselist = $1.rel.falselist;
                                            }
		;
 
rel     :  expr RELOP_LT expr   { printf("����ʽ��rel -> expr < expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLTGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr RELOP_LE expr   { printf("����ʽ��rel -> expr <= expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLEGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr RELOP_GT expr   { printf("����ʽ��rel -> expr > expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OGTGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr RELOP_GE expr   { printf("����ʽ��rel -> expr >= expr\n"); 
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( OLEGoto, $1.expr.addr, $3.expr.addr, -1, $1.expr.str, $3.expr.str, "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        |  expr                 { printf("����ʽ��rel -> expr\n");
                                // ne 0
                                $$.rel.truelist = Makelist( QuadTable.len );
                                $$.rel.falselist = Makelist( QuadTable.len + 1 );
                                Gen( ONEQGoto, $1.expr.addr, 0, -1, $1.expr.str, "", "" );
                                Gen( OGoto, -1, -1, -1, "", "", "" );
                                }
        ;



expr    : expr  '+' term    { printf("����ʽ��expr->expr + term\n"); 
                            Gen( $2.term.type==INT ? OIntAdd : OFloatAdd, $1.expr.addr, $3.term.addr, NewTemp( TopSymbolList, $$.expr.str, $$.expr.width ), $1.expr.str, $3.expr.str, $$.expr.str );
                            }

        | expr  '-' term    { printf("����ʽ��expr->expr - term\n"); 
                            Gen( $2.term.type==INT ? OIntSub : OFloatSub, $1.expr.addr, $3.term.addr, NewTemp( TopSymbolList, $$.expr.str, $$.expr.width ), $1.expr.str, $3.expr.str, $$.expr.str );
                            }
 
        | term              { printf("����ʽ��expr->term\n");
							strcpy( $$.expr.str, $1.term.str );
							$$.expr.type = $1.term.type;
							$$.expr.addr = $1.term.addr;
							$$.expr.width = $1.term.width;	
	                        }
        ;

term    : term  '*' factor  { printf("����ʽ��term->term*factor\n"); 
                            Gen( $2.factor.type==INT ? OIntMultiply : OFloatMultiply, $1.term.addr, $3.factor.addr, NewTemp( TopSymbolList, $$.term.str, $$.term.width ), $1.term.str, $3.factor.str, $$.term.str );
                            }

        | term  '/' factor  { printf("����ʽ��term->term/factor\n"); 
                            Gen( $2.factor.type==INT ? OIntDivide : OFloatDivide, $1.term.addr, $3.factor.addr, NewTemp( TopSymbolList, $$.term.str, $$.term.width ), $1.term.str, $3.factor.str, $$.term.str );
                            }

        | factor            { printf("����ʽ��term->factor\n");
							strcpy( $$.term.str, $1.factor.str );
							$$.term.type = $1.factor.type;
							$$.term.addr = $1.factor.addr;
							$$.term.width = $1.factor.width;	
                            }
        ;

factor  : '(' expr ')'      { printf("����ʽ��factor->(expr)\n" );
							strcpy( $$.factor.str, $2.expr.str );
							$$.factor.type  = $2.expr.type;
							$$.factor.addr  = $2.expr.addr;
							$$.factor.width = $2.expr.width;
                            }

        | ID                {   struct SymbolElem * p;
                                printf("����ʽ��factor->id\n"); 
                                p = LookUpAllSymbolList( TopSymbolList, $1.id.name );
                                if( p != NULL ) {
                                    strcpy( $$.factor.str, p->name );
                                    $$.factor.type  = p->type;
                                    $$.factor.addr  = p->addr;
                                    $$.factor.width = p->width;
                                }							    
                                else {
                                    yyerror( "������û�ж���" );
                                    strcpy( $$.factor.str, "no_id_defined" ); /*�ݴ�����*/
                                    $$.factor.type = INT;
                                    $$.factor.addr = -1;
                                    $$.factor.width = INT_WIDTH;							    
                                }
	                        }

        | CONST             {   struct ConstElem * p; 
							    printf("����ʽ��factor->CONST\n");

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

int yyparse();  /*main����Ҫ����yyparse()���������ú����Ķ����ں��棬����Ҫ������(��������)*/

int main()
{
	char sourcefile[1000],destfile[1000];

	printf("������Ҫ�����Դ�����ļ���(�س�Ĭ��Ϊd:\\gencode\\code.txt)��"); gets(sourcefile);
        if( strcmp( sourcefile,"")== 0 ) 
            strcpy( sourcefile, "code.txt");
	printf("���������м������ļ���(�س�Ĭ��Ϊd:\\gencode\\gencode.txt)��"); gets(destfile);
        if( strcmp( destfile,"")== 0 ) 
            strcpy( destfile, "gencode.txt");

	BeginCompileOneFile( sourcefile );

    CreateConstList(3000);/*����������,����ӵ�ַ3000��ʼ����ռ������*/
    /*��C���Ա������У��÷��ű����ڴ���ⲿ�������������ȡ�
      �����ǵ��﷨�в���֧���ⲿ�����ͺ��������Ըñ�û�б��õ�*/
    TopSymbolList = CreateSymbolList( NULL, 2000 ); /*����ӵ�ַ2000��ʼ����ռ������*/
    CreateQuadTable(100); /*������Ԫʽ����������Ԫʽ�ӵ�ַ�ռ�100��ʼ���*/

    yyparse();

    PrintConstList();
    WriteQuadTableToFile( destfile ); /*����Ԫʽд�뵽�ļ�destfile��*/

    DestroyQuadTable();
    DestroySymbolList(TopSymbolList);
	DestroyConstList();

    if( CompileFailed == 0 ) 
	    printf("����ɹ������ɵ���Ԫʽ���ļ�%s�С�\n", destfile );
	else
	    printf("Դ�ļ�%s�д��󣬱���ʧ�ܡ�\n", sourcefile );

	EndCompileOneFile();
        
	getchar();
    return 0;
}





   