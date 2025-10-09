%{
#include <stdio.h> /* printf() */
#include <string.h> /* strcpy */
#include <stdlib.h> /* atoi(), atof() */
#include "common.h" /* MAX_STR_LEN */
#include "simula.tab.h" /* declaration of terminals */

/* printing information about the terminal found */
int process_token(const char *text, const char *TokenType,
                  const char *TokenVal, const int TokenID); 

int str_beg = 0;  /* start line of a string */
int comm_beg = 0; /* start line of a comment */
%}
 /* unsigned integer constant (with an optional radix) */
uint	[0-9]+|(2|4|8|16)[Rr][0-9A-Fa-f]+
  
%option yylineno

/* declaration of start constions */
/* (start condition INITIAL does not need declaration) */
/* ..................... */
%x END_COMMENT DIRECT_COMMENT STRING
%%

 /* removal of white spaces */

 /* detection of keywords */
(?i:activate)   return process_token(yytext, "KW_ACTIVATE", "", KW_ACTIVATE);
 /* Note that for KW_END you should do a bit more */

 /* multicharacter operators */
":=" return process_token(yytext, "ASSIGN", "", ASSIGN);

 /* identifiers */

 /* unsigned integer constant */


 /* unsigned real constant */

 /* direct comments */

 /* strings */

 /* single character operators and punctuation */

%%

/* Name:	process_token
 * Purpose:     Prints information about the detected item, and passes
 *		it up with optional item value, if it exists.
 * Parameters:  text            - (i) matched text;
 *              TokenType       - (i) string representing token type name;
 *              TokenVal        - (i) token value if exists;
 *              TokenID         - (i) token type identifier declared with
 *					%token in simula.y file.
 * Returns:     Token type identifier (TokenID).
 * Notes:       Information about the detected token is printed in 3 columns.
 *		The first one contains the matched text.
 *		The second one contains the token type name.
 *		The third one contains the token value, if relevant.
 *
 *		Don't forget to pass the returned value to the parser
 *		by placing the invocation in a return statement.
 */
int process_token(const char *text, const char *TokenType,
                  const char *TokenVal, const int TokenID) {
  int l;
  switch (TokenID) {

  case INTEGER_CONST:
    yylval.i = atoi(text); break;

  case REAL_CONST:
    yylval.d = atof(text); break;

  case IDENT:
    strncpy(yylval.s, text, MAX_STR_LEN); break;

  case TEXT_CONST:
    l = strlen(text);
    strncpy(yylval.s, text + 1, l - 2 <= MAX_STR_LEN ? l - 2 : MAX_STR_LEN);
    /* replace double double quotes */
    for (int i = 0; yylval.s[i]; i++) {
      if (yylval.s[i] == '"') {
	if (yylval.s[i+1] == '"') {
	  memmove(yylval.s + i, yylval.s + i + 1, strlen(yylval.s + i));
	}
      }
    }
    printf("%-20.20s%-15s %s\n", text, TokenType, yylval.s);
    return TokenID;;

  case CHARACTER_CONST:
    yylval.i = text[1]; break;

  }
  printf("%-20.20s%-15s %s\n", text, TokenType, TokenVal);
  return TokenID;
}/*process_token*/


int
yywrap(void) {
  /* This function is called when the end of the current input stream
     is detected */
  if (YY_START == DIRECT_COMMENT) {
    /* We are still in a comment */
    fprintf(stderr, "Unfinished comment started in line %d\n", comm_beg);
  }
  else if (YY_START == END_COMMENT) {
    /* We are still in an end comment */
    fprintf(stderr, "Missing semicolon after END in line %d\n", comm_beg);
  }
  else if (YY_START == STRING) {
    /* We are still in a text constant */
    fprintf(stderr, "Unfinished string opened in line %d\n", str_beg);
  }
  return 1; /* necessary so that the analysis does not restart from scratch */
}
