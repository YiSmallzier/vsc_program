/**
 * @file parser.cpp
 * @author Deng Dingxuan
 * @brief 
 * @version 0.1
 * @date 2023-04-07
 * 
 * @copyright Copyright (c) 2023
 * program -> block
 * block   -> {stmts}
 * stmts   -> stmt stmts | NULL
 * stmt    -> matched_stmt 
 *          | unmatched_stmt
 * matched_stmt -> if (bool) matched_stmt else matched_stmt
 *               | while (bool) stmt 
 *               | id = expr
 *               | break
 *               | block
 * unmatched_stmt -> if (bool) stmt 
 *                 | if (bool) matched_stmt else unmatched_stmt
 * 
 * stmt    -> id = expr
 *          | if (bool) stmt
 *          | if (bool) stmt else stmt
 *          | while (bool) stmt
 *          | do stmt while (bool)
 *          | break
 *          | block
 */


#include <cstdio>
// #include <iostream>
#include "lex.yy.h"
#include "lex.yy.c"
// using namespace std;

const int MAXN = 1000;

// extern "C" void BeginCompileOneFile( const char * filename );
// extern "C" void EndCompileOneFile(void);
// extern "C" int yylex();
// extern "C" int GetLineNo(void);

int lookahead;
void match(int TokenID);

void Block();
void error()
{
    printf("Error: in %d line  \n",GetLineNo());
	EndCompileOneFile();
	printf("Press Enter to exit. \n"); getchar();
    exit(1);
}

void match(int TokenID)
{

    if( lookahead == TokenID )
        lookahead = yylex();
    else 
        error();
}

void Bool() {
    while(lookahead != ')') 
        match(lookahead);
}
void matched_stmt();
void unmatched_stmt();
void stmt()
{
	if(lookahead==BREAK_TKN || lookahead==DO_TKN ||
        lookahead==WHILE_TKN || lookahead==ID_TKN ||
        lookahead==IF_TKN) {
		printf("### stmt ->	matched_stmt ; \n");
        // match(BREAK_TKN);

        matched_stmt();
		// match(';');
        // printf("Debug: stmt : lookahead = %c\n", lookahead);
        return;
	}
    //?????
    if( lookahead == IF_TKN) {
        printf("### stmt -> unmatched_stmt ; \n");
        unmatched_stmt();
    }
    else if(lookahead == '{') {
        printf("### stmt -> block \n");
	    Block();
        //block
    }
    else return ;
	// error();
}
void matched_stmt() {
    if(lookahead == IF_TKN) {
        printf("### matched_stmt -> if (bool) matched_stmt else matched_stmt\n");
        match(IF_TKN);
        match('('); Bool();  match (')');
        matched_stmt();

        // printf("Debug: matched : lookahead = %c\n", lookahead);
        if(lookahead == ELSE_TKN) {
            match(ELSE_TKN);
            matched_stmt();
        }
        return;
    }
    else if(lookahead == WHILE_TKN) {
        printf("### matched_stmt -> while (bool) matched_stmt\n");
        match(WHILE_TKN);
        match('('); Bool(); match (')');
        matched_stmt();
    }
    else if(lookahead == ID_TKN) {
        printf("### matched_stmt -> id = expr\n");
        match(ID_TKN);
        match('=');
        // expr();
        while(lookahead != ';') match(lookahead);
        match(';');
    }
    else if(lookahead == BREAK_TKN) {
        printf("### matched_stmt -> break\n");
        match(BREAK_TKN);
        match(';');
        return;
    }
    else if(lookahead == '{') {
        printf("### matched_stmt -> block\n");
        Block();
    }
    else {
        return ;
    } 
    //error();
}

void unmatched_stmt() {
    match(IF_TKN);
    match('('); Bool(); match(')');
    stmt();

    if(lookahead == ELSE_TKN) {
        match(ELSE_TKN);
        unmatched_stmt();
    }
    // if(lookahead == IF_TKN) {
    //     printf("unmatched_stmt -> if (bool) stmt\n");
    //     match(IF_TKN);
    //     match('('); Bool();  match (')');
    //     stmt();
    // }

    // if(lookahead == IF_TKN) {
    //     printf("unmatched_stmt -> if (bool) matched_stmt else unmatched_stmt\n");
    //     match(IF_TKN);
    //     match('('); Bool();  match (')');
    //     matched_stmt();
    //     match(ELSE_TKN);
    //     unmatched_stmt();
    // }
    // else error();
}
void stmts()
{   
    /*???lookahead??First(stmt stmts)?��?????????????stmts??stmt stmts????*/
    if( lookahead==ID_TKN ||lookahead==IF_TKN ||
		lookahead==WHILE_TKN ||lookahead==DO_TKN ||
		lookahead==BREAK_TKN ||lookahead=='{' ) {
	    printf("### stmts -> stmt stmts\n");
	    stmt();
	    stmts();
	    return;
    }
    if( lookahead=='}' ) { 
        /*???lookahead??stmts?????follow,???stmts ????empty????*/
        printf("### stmts -> empty \n");
        // match('}');
	    return;
    } 
    else return;
	/*????*/
    error();
}

void Block()
{
    if( lookahead=='{') {
	    printf("### block ->  { stmts }\n");
	    match( '{' );
        stmts();
        match( '}' );
        // printf("Debug: Block lookahead is %c !\n", lookahead);
        return;
    }
    else error();
}

void Program()
{
    if( lookahead == '{' ) {
	    printf("### program -> block\n");
	    Block();
        return;
    }
    else
        error();
}

int main()
{ 
char filename[MAXN];

    printf("Filename: "); 
    // std::cin >> filename;
    gets(filename);
    BeginCompileOneFile( filename );

    lookahead = yylex();
    Program();

	if( lookahead == 0 ) 
	    printf("\n Analyse successed! \n");
	else
		error();

    EndCompileOneFile();
        printf("Please press 'Enter' to return !\n");getchar();

    return 0;
}
