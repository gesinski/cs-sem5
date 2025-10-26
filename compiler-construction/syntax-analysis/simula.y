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
%token<i> ASSIGN REF_ASSIGN INT_DIV EXPO REF_EQ REF_NE POWER
%token<d> REAL_CONST

 /* Precedence of operators */
%left '+' '-'
%left '*' '/' INT_DIV
%left EXPO
%nonassoc '<' KW_LE '=' KW_GE '>' KW_NE KW_IS KW_IN
%right KW_NOT

%type<s> MAIN_PART PROCEDURE_HEADING IDENTIFIER_1 PROCEDURE_STATEMENT
%type<s> REMOTE_PREFIX

%%

 /* Structure of a Simula program */

 /* program can be empty (semantic error),
    it may contain a syntax error,
    or it may consist of statements (STATEMENTS) */
GRAMMAR: %empty { yyerror("File is ampty"); YYERROR; }
	| error
        /* !!! Start here !!! */

;

/* BLOCK */
/* Block head (BLOCK_HEAD) followed by compound tail (COMPOUND_TAIL) */

/* BLOCK_HEAD */
/* Keyword begin followed by a sequence of:
   declarations (DECLARATION) followed by semicolon */

/* DECLARATION */
/* Either: procedure declaration (PROCEDURE_DECLARATION),
   or: class declaration (CLASS_DECLARATION),
   or: simple variable declaration (SIMPLE_VARIABLE_DECLARATION),
   or: array declaration (ARRAY_DECLARATION) */

/* CLASS_DECLARATION */
/* Optional identifier (OPT_IDENT) followed by main part (MAIN_PART) */

/* OPT_IDENT */
/* Either empty or identifier */

/* MAIN_PART */
/* Keyword class followed by identifier, formal-value-specification part
   (FVS_PART), semicolon, optional virtual part (OPT_VIRTUAL_PART),
   and class body (CLASS_BODY). */

/* FVS_PART */
/* Either empty or a sequence of formal parameter part (FORMAL_PARAMETER_PART),
   semicolon, optional value part (OPT_VALUE_PART), and specification part
   (SPECIFICATION_PART) */

/* FORMAL_PARAMETER_PART */
/* List of identifiers (IDENTIFIER_LIST) in parentheses */

/* IDENTIFIER_LIST */
/* Comma-separated list of identifiers */

/* OPT_VIRTUAL_PART */
/* Either empty or virtual part followed by semicolon */

/* VIRTUAL_PART */
/* Keyword virtual followed by a colon, and a non-empty sequence
   of virtual specifications (VIRTUAL_SPEC), each followed by a semicolon */

/* VIRTUAL_SPEC */
/* Either a specifier (SPECIFIER) followed by a list of identifiers
   (IDENTIFIER_LIST),
   or keyword procedure, followed by identifier and procedure specification
   (PROCEDURE_SPECIFICATION)
*/

/* PROCEDURE_SPECIFICATION */
/* Keyword is, followed by procedure declaration (PROCEDURE_DECLARATION) */

/* SPECIFICATION_PART */
/* A non-empty sequence of pairs: specifier (SPECIFIER) and list of identifiers
   (IDENTIFIER_LIST) */

/* SPECIFIER */
/* Type followed by optional array or procedure parts (OPT_ARRAY_PROC) */

/* OPT_ARRAY_PROC */
/* Either empty, or keyword array, or keyword procedure */

/* TYPE */
/* Either value type (VALUE_TYPE) or reference type (REFERENCE_TYPE) */

/* VALUE_TYPE */
/* Either arithmetic type (ARITHMETIC_TYPE), or keyword boolean, or keyword
   character */

/* ARITHMETIC_TYPE */
/* Either integer type (INTEGER_TYPE) or real type (REAL_TYPE) */

/* INTEGER_TYPE */
/* Optional keyword short (OPT_KW_SHORT) followed by keyword integer */

/* OPT_KW_SHORT */
/* Either empty or keyword short */

/* REAL_TYPE */
/* optional keyword long (OPT_KW_SHORT) followed by keyword real */

/* OPT_KW_LONG */
/* Either empty of keyword long */

/* OPT_VALUE_PART */
/* empty */

/* CLASS_BODY */
/* statement (STATEMENT) */

/* COMPOUND_STATEMENT */
/* Keyword begin and compound tail (COMPOUND_TAIL) */

/* COMPOUND_TAIL */
/* Statements (STATEMENTS) followed by keyword end */

/* STATEMENTS */
/* Semicolon-separated list of statements (STATEMENT) */

/* STATEMENT */
/* Either unconditional statement (UNCONDITIONAL_STATEMENT)
   or for statement (FOR_STATEMENT) */

/* UNCONDITIONAL_STATEMENT */
/* Either: assignment statement (ASSIGNMENT_STATEMENT),
   or: possibly remote procedure statement (PROCEDURE_STATEMENT_1),
   or: possibly remote identifier (IDENTIFIER_1),
   or: object generator (OBJECT_GENERATOR),
   or: compound statement (COMPOUND_STATEMENT),
   or: block (BLOCK),
   or: a dummy statement (DUMMY_STATEMENT) */

/* ASSIGNMENT_STATEMENT */
/* Either value assignment (VALUE_ASSIGNMENT) 
   or reference assignment (REFERENCE_ASSIGNEMENT) */

/* VALUE_ASSIGNMENT */
/* Left part of a value assignment (VALUE_LEFT_PART), followed by
   assignment operator, and the right part of a value assignment
   (VALUE_RIGHT_PART) */

/* VALUE_LEFT_PART */
/* Either a possibly remote procedure statement (PROCEDURE_STATEMENT_1)
   or a possibly remote identifier (IDENTIFIER_1) */

/* VALUE_RIGHT_PART */
/* Either a value expression (VALUE_EXPRESSION)
   or text expression (TEXT_EXPRESSION) or value assignment (VALUE_ASSIGNMENT)*/

/* DESTINATION */
/* A variable (VARIABLE) */

/* SIMPLE_TEXT_EXPRESSION */
/* Ampersand-separated list of primary text expressions (TEXT_PRIMARY) */

/* TEXT_PRIMARY */
/* Either: keyword notext, or text constant (TEXT_CONST),
   or variable (VARIABLE), or function designator (FUNCTION_DESIGNATOR),
   or a text expression (TEXT_EXPRESSION) in parentheses */

/* VARIABLE */
/* Either indentifier or a subscripted variable (SUBSCRIPTED_VARIABLE) */

/* SUBSCRIPTED_VARIABLE */
/* Identifier followed by a list of subscripts (SUBSCRIPT_LIST)
   in parentheses */

/* SUBSCRIPT_LIST */
/* Comma-separated list of subscript expressions (SUBSCRIPT_EXPRESSION) */

/* SUBSCRIPT_EXPRESSION */
/* Arithmetic expression (ARITHMETIC_EXPRESSION) */

/* ARITHMETIC_EXPRESSION */
/* Either:
   unsigned number (UNSIGNED_NUMBER),
   or: function designator (FUNCTION_DESIGNATOR),
   or: sum of arithmetic expressions,
   or: difference of arithmetic expressions,
   or: multiplication of arithmetic expressions,
   or: division of arithmetic expressions,
   or: integer division of arithmetic expressions,
   or: an arithmetic expression in parentheses. */

/* UNSIGNED_NUMBER */
/* Either an integer constant or real constant */

/* FUNCTION_DESIGNATOR */
/* Identifier followed by actual parameter part (ACTUAL_PARAMETER_PART) */

/* TEXT_EXPRESSION */
/* A simple text expression (SIMPLE_TEXT_EXPRESSION) */

/* VALUE_EXPRESSION */
/* An arithmetic expression (ARITHMETIC_EXPRESSION) */

/* REFERENCE_ASSIGNEMENT */
/* Left part of reference assignment (REFERENCE_LEFT_PART),
   reference assignment operator,
   and right part or reference assignment (REFERENCE_RIGHT_PART) */

/* REFERENCE_LEFT_PART */
/* Destination (DESTINATION) */

/*  REFERENCE_RIGHT_PART */
/* Either a reference expression (REFERENCE_EXPRESSION),
   or a reference assignment (REFERENCE_ASSIGNEMENT) */

/* REFERENCE_EXPRESSION */
/* Either an object expression (OBJECT_EXPRESSION), or a text expression
   (TEXT_EXPRESSION) */

/* OBJECT_EXPRESSION */
/* A simple object expression (SIMPLE_OBJECT_EXPRESSION) */

/* SIMPLE_OBJECT_EXPRESSION */
/* Either: keyword none,
   or: object generator (OBJECT_GENERATOR),
   or: an object expression in parentheses
 */

/* REFERENCE_TYPE */
/* Either object reference type (OBJECT_REFERENCE_TYPE) or keword text */

/* OBJECT_REFERENCE_TYPE */
/* Keyword ref, followed by qualification (QUALIFICATION) in parentheses */

/* QUALIFICATION */
/* Identifier */

/* PROCEDURE_STATEMENT_1 */
/* Either a procedure statement, or a remote prefix followed by a dot,
   and a procedure statement*/

/* REMOTE_PREFIX */
/* A dot-separated list of mixed identifiers or procedure statements
   (PROCEDURE_STATEMENT) */

/* PROCEDURE_STATEMENT */
/* Identifier followed by actual parameter part (ACTUAL_PARAMETER_PART) */

/* OPT_ACTUAL_PARAMETER_PART */
/* Either empty or actual parameter part (ACTUAL_PARAMETER_PART) */

/* ACTUAL_PARAMETER_PART */
/* Actual parameters (ACTUAL_PARAMETERS) in parentheses */

/* ACTUAL_PARAMETERS */
/* A comma-separated list of actual parameters (ACTUAL_PARAMETER) */

/* ACTUAL_PARAMETER */
/* Expression (EXPRESSION) */

/* EXPRESSION */
/* Either: integer constant (INTEGER_CONST), or identifier, or text constant
   (TEXT_CONST), or character constant (CHARACTER_CONST) */

/* IDENTIFIER_1 */
/* Either an identifier, or a remote prefix (REMOTE_PREFIX) followed by dot,
   and by an identifier*/

/* PROCEDURE_DECLARATION */
/* Optional type (OPT_TYPE), keyword procedure, procedure heading
   (PROCEDURE_HEADING), and procedure body (PROCEDURE_BODY) */

/* OPT_TYPE */
/* Either empty or type (TYPE) */

/* PROCEDURE_HEADING */
/* Identifier followed by procedure parts (PROCEDURE_PARTS) */
PROCEDURE_HEADING:
         IDENT PROCEDURE_PARTS
         {
            found("PROCEDURE_HEADING", $1);
            strcpy($$, $1);
         }
;

/* PROCEDURE_PARTS */
/* Formal parameter part (FORMAL_PARAMETER_PART), followed by optional
   mode part (OPT_MODE_PART), and by specification part (SPECIFICATION_PART) */

/* PROCEDURE_BODY */
/* Statement (STATEMENT) */

/* OBJECT_GENERATOR */
/* Keyword new, identifier, amd optional actual parameter part
   (OPT_ACTUAL_PARAMETER_PART) */

/* OPT_MODE_PART */
/* Empty */

/* DUMMY_STATEMENT */
/* Empty */

/* SIMPLE_VARIABLE_DECLARATION */
/* Type (TYPE), followed by a type list (TYPE_LIST) */

/* TYPE_LIST */
/* A comma-separated list of type list elements (TYPE_LIST_ELEMENT) */

/* TYPE_LIST_ELEMENT */
/* Either identifier or a constant element (CONSTANT_ELEMENT) */

/* CONSTANT_ELEMENT */
/* Identifier followed by an equal sign and value or text expression
   (VALUE_OR_TEXT_EXPRESSION) */

/* VALUE_OR_TEXT_EXPRESSION */
/* Either a value expression (VALUE_EXPRESSION) or text expression
   (TEXT_EXPRESSION) */

/* FOR_STATEMENT */
/* Keyword for, followed by identifier, right part of for statement
   (FOR_RIGHT_PART), keyword do, and a statement (STATEMENT) */

/* FOR_RIGHT_PART */
/* Either assignment operator followed by value for list elements
   (VALUE_FOR_LIST_ELEMENTS),
   or: reference assign operator followed by reference for list elements
   (REFERENCE_FOR_LIST_ELEMENTS) */

/* VALUE_FOR_LIST_ELEMENTS */
/* A comma-separated list of value for list elements (VALUE_FOR_LIST_ELEMENT) */

/* VALUE_FOR_LIST_ELEMENT */
/* Either: a value expression (VALUE_EXPRESSION) followed by optional while
   (OPT_WHILE),
   or: arithmetic expression (ARITHMETIC_EXPRESSION), followed by keyword
   step, arithmetic expression, keyword until, and an arithmetic expression */

/* REFERENCE_FOR_LIST_ELEMENTS */
/* A comma-separated list of reference for list elements
   (REFERENCE_FOR_LIST_ELEMENT) */

/* REFERENCE_FOR_LIST_ELEMENT */
/* A reference expression (REFERENCE_EXPRESSION), followed by optional while
   (OPT_WHILE) */

/* OPT_WHILE */
/* Empty */

/* ARRAY_DECLARATION */
/* Optional type (OPT_TYPE), keyword array, and array segments
   (ARRAY_SEGMENTS) */

/* ARRAY_SEGMENTS */
/* A comma-separated list of array segments (ARRAY_SEGMENT) */

/* ARRAY_SEGMENT */
/* A list of identifiers (IDENTIFIER_LIST). followed by a list of bound pairs
   (BOUND_PAIR_LIST) in parentheses */

/* BOUND_PAIR_LIST */
/* A comma-separated list of bound pairs (BOUND_PAIR) */

/* BOUND_PAIR */
/* Two arithmetic expressions separated with a colon */

%%


int main( void )
{
	int ret;
	/* yydebug = 1;*/
	printf( "Author: Paweł Gęsiński\n" );
	printf( "yytext              Token type      Token value as string\n\n" );
	ret = yyparse();
	return ret;
}

void yyerror(const char *txt)
{
	printf("Syntax error %s\n", txt);
}

void found(const char *nonterminal, const char *value)
{ /* info on syntax structures found */
	printf( "======== FOUND: %s %s%s%s ========\n", nonterminal, 
		(*value) ? "'" : "", value, (*value) ? "'" : "" );
}
