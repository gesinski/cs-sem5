%{
#include <stdio.h> /* printf() */
#include <string.h> /* strcpy() */
#include "common.h" /* MAX_STR_LEN */
int yylex(void);
void yyerror(const char *txt);
 
void found( const char *nonterminal, const char *value );
%}

%union 
{
        char s[ MAX_STR_LEN + 1 ]; /* text field for idents etc. */
        int i; /* integer field */
        double d; /* floating point field */
}

%token<i> KW_ACTIVATE KW_AFTER KW_AND KW_ARRAY KW_AT KW_BEFORE KW_BEGIN
%token<i> KW_BOOLEAN KW_CHARACTER KW_CLASS KW_COMMENT KW_DELAY KW_DO
%token<i> KW_ELSE KW_END KW_EQ KW_EQV KW_EXTERNAL KW_FALSE KW_FOR KW_GE
%token<i> KW_GO KW_GOTO KW_GT KW_HIDDEN KW_IF KW_IMP KW_IN KW_INNER
%token<i> KW_INSPECT KW_INTEGER KW_IS KW_LABEL KW_LONG KW_LT KW_NAME KW_NE
%token<i> KW_NEW KW_NONE KW_NOT KW_NOTEXT KW_OR KW_OTHERWISE KW_PRIOR
%token<i> KW_PROCEDURE KW_PROTECTED KW_QUA KW_REACTIVATE KW_REAL KW_REF
%token<i> KW_SHORT KW_STEP KW_SWITCH KW_TEXT KW_THEN KW_THIS KW_TO KW_TRUE
%token<i> KW_UNTIL KW_VALUE KW_VIRTUAL KW_WHEN KW_WHILE
%token<i> INTEGER_CONST
%token<s> TEXT_CONST CHARACTER_CONST IDENT
%token<i> ASSIGN REF_ASSIGN INT_DIV EXPO REF_EQ REF_NE
%token<d> REAL_CONST

 /* Precedence of operators */
%left '+' '-'
%left '*' '/' INT_DIV
%left EXPO
%nonassoc '<' KW_LE '=' KW_GE '>' KW_NE KW_IS KW_IN
%right KW_NOT

%define parse.error detailed
%%


Grammar: %empty
	| TOKENS
;
	
TOKENS: TOKEN
	| TOKENS TOKEN
;

TOKEN: KW_ACTIVATE | KW_AFTER | KW_AND | KW_ARRAY | KW_AT | KW_BEFORE
	| KW_BEGIN | KW_BOOLEAN | KW_CHARACTER | KW_CLASS | KW_COMMENT
	| KW_DELAY | KW_DO | KW_ELSE | KW_END | KW_EQ | KW_EQV | KW_EXTERNAL
	| KW_FALSE | KW_FOR | KW_GE KW_GO | KW_GOTO | KW_GT | KW_HIDDEN
	| KW_IF | KW_IMP | KW_IN | KW_INNER | KW_INSPECT | KW_INTEGER | KW_IS
	| KW_LABEL | KW_LONG | KW_LT | KW_NAME | KW_NE | KW_NEW | KW_NONE
	| KW_NOT | KW_NOTEXT | KW_OR | KW_OTHERWISE | KW_PRIOR | KW_PROCEDURE
	| KW_PROTECTED | KW_QUA | KW_REACTIVATE | KW_REAL | KW_REF | KW_SHORT
	| KW_STEP | KW_SWITCH | KW_TEXT | KW_THEN | KW_THIS | KW_TO | KW_TRUE
	| KW_UNTIL | KW_VALUE | KW_VIRTUAL | KW_WHEN | KW_WHILE
	| INTEGER_CONST | REAL_CONST | IDENT | TEXT_CONST | CHARACTER_CONST
	| ASSIGN | REF_ASSIGN | '+' | '-' | '*' | '/' | ':' | '\''
	| ';' | ',' | '=' | '(' | ')' | '{' | '}' | '.' | '[' | ']' | '<'
	| error
;	

%%

/***************************************************************************/
/*                            programs section                             */
/***************************************************************************/

int
main(void)
{
	int ret;
	printf("Author: Pawel Gesinski\n");
	printf("yytext              Token type       Token value as string\n\n");
	ret = yyparse();
	return ret;
}

void
yyerror(const char *txt)
{
	printf("Syntax error %s\n", txt);
}
