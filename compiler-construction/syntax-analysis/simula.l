%{
#include <stdio.h> /* printf() */
#include <string.h> /* strcpy */
#include <stdlib.h> /* atoi(), atof() */
#include "common.h" /* MAX_STR_LEN */
#include <ctype.h>
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
[ \t\r\n]+   ;

 /* detection of keywords */

(?i:activate)     return process_token(yytext, "KW_ACTIVATE", "", KW_ACTIVATE);
(?i:after)        return process_token(yytext, "KW_AFTER", "", KW_AFTER);
(?i:and)          return process_token(yytext, "KW_AND", "", KW_AND);
(?i:array)        return process_token(yytext, "KW_ARRAY", "", KW_ARRAY);
(?i:at)           return process_token(yytext, "KW_AT", "", KW_AT);
(?i:before)       return process_token(yytext, "KW_BEFORE", "", KW_BEFORE);
(?i:begin)        return process_token(yytext, "KW_BEGIN", "", KW_BEGIN);
(?i:boolean)      return process_token(yytext, "KW_BOOLEAN", "", KW_BOOLEAN);
(?i:character)    return process_token(yytext, "KW_CHARACTER", "", KW_CHARACTER);
(?i:class)        return process_token(yytext, "KW_CLASS", "", KW_CLASS);
(?i:delay)        return process_token(yytext, "KW_DELAY", "", KW_DELAY);
(?i:do)           return process_token(yytext, "KW_DO", "", KW_DO);
(?i:else)         return process_token(yytext, "KW_ELSE", "", KW_ELSE);
(?i:end)           { BEGIN(END_COMMENT); comm_beg = yylineno; return process_token(yytext, "KW_END", "", KW_END);}
<INITIAL>(?i:end)      { BEGIN(END_COMMENT); comm_beg = yylineno; }
<END_COMMENT>[^;\n]*    ; 
<END_COMMENT>\n {yywrap(); BEGIN(INITIAL);}
<END_COMMENT>";"         { BEGIN(INITIAL); return process_token(";", ";", "", ';'); }
(?i:eq)           return process_token(yytext, "KW_EQ", "", KW_EQ);
(?i:eqv)          return process_token(yytext, "KW_EQV", "", KW_EQV);
(?i:external)     return process_token(yytext, "KW_EXTERNAL", "", KW_EXTERNAL);
(?i:false)        return process_token(yytext, "KW_FALSE", "", KW_FALSE);
(?i:for)          return process_token(yytext, "KW_FOR", "", KW_FOR);
(?i:ge)           return process_token(yytext, "KW_GE", "", KW_GE);
(?i:go)           return process_token(yytext, "KW_GO", "", KW_GO);
(?i:goto)         return process_token(yytext, "KW_GOTO", "", KW_GOTO);
(?i:gt)           return process_token(yytext, "KW_GT", "", KW_GT);
(?i:hidden)       return process_token(yytext, "KW_HIDDEN", "", KW_HIDDEN);
(?i:if)           return process_token(yytext, "KW_IF", "", KW_IF);
(?i:imp)          return process_token(yytext, "KW_IMP", "", KW_IMP);
(?i:in)           return process_token(yytext, "KW_IN", "", KW_IN);
(?i:inner)        return process_token(yytext, "KW_INNER", "", KW_INNER);
(?i:inspect)      return process_token(yytext, "KW_INSPECT", "", KW_INSPECT);
(?i:integer)      return process_token(yytext, "KW_INTEGER", "", KW_INTEGER);
(?i:is)           return process_token(yytext, "KW_IS", "", KW_IS);
(?i:label)        return process_token(yytext, "KW_LABEL", "", KW_LABEL);
(?i:long)         return process_token(yytext, "KW_LONG", "", KW_LONG);
(?i:lt)           return process_token(yytext, "KW_LT", "", KW_LT);
(?i:name)         return process_token(yytext, "KW_NAME", "", KW_NAME);
(?i:ne)           return process_token(yytext, "KW_NE", "", KW_NE);
(?i:new)          return process_token(yytext, "KW_NEW", "", KW_NEW);
(?i:none)         return process_token(yytext, "KW_NONE", "", KW_NONE);
(?i:not)          return process_token(yytext, "KW_NOT", "", KW_NOT);
(?i:notext)       return process_token(yytext, "KW_NOTEXT", "", KW_NOTEXT);
(?i:or)           return process_token(yytext, "KW_OR", "", KW_OR);
(?i:otherwise)    return process_token(yytext, "KW_OTHERWISE", "", KW_OTHERWISE);
(?i:prior)        return process_token(yytext, "KW_PRIOR", "", KW_PRIOR);
(?i:procedure)    return process_token(yytext, "KW_PROCEDURE", "", KW_PROCEDURE);
(?i:protected)    return process_token(yytext, "KW_PROTECTED", "", KW_PROTECTED);
(?i:qua)          return process_token(yytext, "KW_QUA", "", KW_QUA);
(?i:reactivate)   return process_token(yytext, "KW_REACTIVATE", "", KW_REACTIVATE);
(?i:real)         return process_token(yytext, "KW_REAL", "", KW_REAL);
(?i:ref)          return process_token(yytext, "KW_REF", "", KW_REF);
(?i:short)        return process_token(yytext, "KW_SHORT", "", KW_SHORT);
(?i:step)         return process_token(yytext, "KW_STEP", "", KW_STEP);
(?i:switch)       return process_token(yytext, "KW_SWITCH", "", KW_SWITCH);
(?i:text)         return process_token(yytext, "KW_TEXT", "", KW_TEXT);
(?i:then)         return process_token(yytext, "KW_THEN", "", KW_THEN);
(?i:this)         return process_token(yytext, "KW_THIS", "", KW_THIS);
(?i:to)           return process_token(yytext, "KW_TO", "", KW_TO);
(?i:true)         return process_token(yytext, "KW_TRUE", "", KW_TRUE);
(?i:until)        return process_token(yytext, "KW_UNTIL", "", KW_UNTIL);
(?i:value)        return process_token(yytext, "KW_VALUE", "", KW_VALUE);
(?i:virtual)      return process_token(yytext, "KW_VIRTUAL", "", KW_VIRTUAL);
(?i:when)         return process_token(yytext, "KW_WHEN", "", KW_WHEN);
(?i:while)        return process_token(yytext, "KW_WHILE", "", KW_WHILE);
(?i:comment)[^;]*\;      ;

 /* multicharacter operators */
":="    return process_token(yytext, "ASSIGN", "", ASSIGN);
":-"    return process_token(yytext, "REF_ASSIGN", "", REF_ASSIGN);
"<="    return process_token(yytext, "KW_LE", "", KW_LE);
">="    return process_token(yytext, "KW_GE", "", KW_GE);
"<>"    return process_token(yytext, "KW_NE", "", KW_NE);
"=="    return process_token(yytext, "KW_EQ", "", KW_EQ);

 /* identifiers */
 [A-Za-z][A-Za-z0-9]* return process_token(yytext, "IDENT", yytext, IDENT);

 /* unsigned integer constant */
[0-9]+ return process_token(yytext, "INTEGER_CONST", yytext, INTEGER_CONST);

 /* unsigned real constant */
 [0-9]*\.[0-9]+ return process_token(yytext, "REAL_CONST", yytext, REAL_CONST);

 /* direct comments */
! { 
    comm_beg = yylineno;  
    BEGIN(DIRECT_COMMENT);
}
<DIRECT_COMMENT>[^;\n]+ ; 
<DIRECT_COMMENT>;\n? { BEGIN(INITIAL);} 
<DIRECT_COMMENT>\n  {yywrap(); BEGIN(INITIAL); }  

 /* strings */
 \"([^\"\n]|\"\")*\"   return process_token(yytext, "TEXT_CONST", "", TEXT_CONST);

 /* single character operators and punctuation */
'[^']' return process_token(yytext, "CHARACTER_CONST", yytext, CHARACTER_CONST);
"+"     return process_token(yytext, "+", "", '+');
"-"     return process_token(yytext, "-", "", '-');
"*"     return process_token(yytext, "*", "", '*');
"/"     return process_token(yytext, "/", "", '/');
"="     return process_token(yytext, "=", "", '=');
"<"     return process_token(yytext, "<", "", '<');
">"     return process_token(yytext, ">", "", '>');
":"     return process_token(yytext, ":", "", ':');
"'"     return process_token(yytext, "'", "", '\'');
";"     return process_token(yytext, ";", "", ';');
","     return process_token(yytext, ",", "", ',');
"("     return process_token(yytext, "(", "", '(');
")"     return process_token(yytext, ")", "", ')');
"{"     return process_token(yytext, "{", "", '{');
"}"     return process_token(yytext, "}", "", '}');
"."     return process_token(yytext, ".", "", '.');
"["     return process_token(yytext, "[", "", '[');
"]"     return process_token(yytext, "]", "", ']');

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
