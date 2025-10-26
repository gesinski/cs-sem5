/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SIMULA_TAB_H_INCLUDED
# define YY_YY_SIMULA_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    KW_ACTIVATE = 258,             /* KW_ACTIVATE  */
    KW_AFTER = 259,                /* KW_AFTER  */
    KW_AND = 260,                  /* KW_AND  */
    KW_ARRAY = 261,                /* KW_ARRAY  */
    KW_AT = 262,                   /* KW_AT  */
    KW_BEFORE = 263,               /* KW_BEFORE  */
    KW_BEGIN = 264,                /* KW_BEGIN  */
    KW_BOOLEAN = 265,              /* KW_BOOLEAN  */
    KW_CHARACTER = 266,            /* KW_CHARACTER  */
    KW_CLASS = 267,                /* KW_CLASS  */
    KW_COMMENT = 268,              /* KW_COMMENT  */
    KW_DELAY = 269,                /* KW_DELAY  */
    KW_DO = 270,                   /* KW_DO  */
    KW_ELSE = 271,                 /* KW_ELSE  */
    KW_END = 272,                  /* KW_END  */
    KW_EQ = 273,                   /* KW_EQ  */
    KW_EQV = 274,                  /* KW_EQV  */
    KW_EXTERNAL = 275,             /* KW_EXTERNAL  */
    KW_FALSE = 276,                /* KW_FALSE  */
    KW_FOR = 277,                  /* KW_FOR  */
    KW_GE = 278,                   /* KW_GE  */
    KW_GO = 279,                   /* KW_GO  */
    KW_GOTO = 280,                 /* KW_GOTO  */
    KW_GT = 281,                   /* KW_GT  */
    KW_HIDDEN = 282,               /* KW_HIDDEN  */
    KW_IF = 283,                   /* KW_IF  */
    KW_IMP = 284,                  /* KW_IMP  */
    KW_IN = 285,                   /* KW_IN  */
    KW_INNER = 286,                /* KW_INNER  */
    KW_INSPECT = 287,              /* KW_INSPECT  */
    KW_INTEGER = 288,              /* KW_INTEGER  */
    KW_IS = 289,                   /* KW_IS  */
    KW_LABEL = 290,                /* KW_LABEL  */
    KW_LONG = 291,                 /* KW_LONG  */
    KW_LT = 292,                   /* KW_LT  */
    KW_NAME = 293,                 /* KW_NAME  */
    KW_NE = 294,                   /* KW_NE  */
    KW_NEW = 295,                  /* KW_NEW  */
    KW_NONE = 296,                 /* KW_NONE  */
    KW_NOT = 297,                  /* KW_NOT  */
    KW_NOTEXT = 298,               /* KW_NOTEXT  */
    KW_OR = 299,                   /* KW_OR  */
    KW_OTHERWISE = 300,            /* KW_OTHERWISE  */
    KW_PRIOR = 301,                /* KW_PRIOR  */
    KW_PROCEDURE = 302,            /* KW_PROCEDURE  */
    KW_PROTECTED = 303,            /* KW_PROTECTED  */
    KW_QUA = 304,                  /* KW_QUA  */
    KW_REACTIVATE = 305,           /* KW_REACTIVATE  */
    KW_REAL = 306,                 /* KW_REAL  */
    KW_REF = 307,                  /* KW_REF  */
    KW_SHORT = 308,                /* KW_SHORT  */
    KW_STEP = 309,                 /* KW_STEP  */
    KW_SWITCH = 310,               /* KW_SWITCH  */
    KW_TEXT = 311,                 /* KW_TEXT  */
    KW_THEN = 312,                 /* KW_THEN  */
    KW_THIS = 313,                 /* KW_THIS  */
    KW_TO = 314,                   /* KW_TO  */
    KW_TRUE = 315,                 /* KW_TRUE  */
    KW_UNTIL = 316,                /* KW_UNTIL  */
    KW_VALUE = 317,                /* KW_VALUE  */
    KW_VIRTUAL = 318,              /* KW_VIRTUAL  */
    KW_WHEN = 319,                 /* KW_WHEN  */
    KW_WHILE = 320,                /* KW_WHILE  */
    INTEGER_CONST = 321,           /* INTEGER_CONST  */
    TEXT_CONST = 322,              /* TEXT_CONST  */
    CHARACTER_CONST = 323,         /* CHARACTER_CONST  */
    IDENT = 324,                   /* IDENT  */
    ASSIGN = 325,                  /* ASSIGN  */
    REF_ASSIGN = 326,              /* REF_ASSIGN  */
    INT_DIV = 327,                 /* INT_DIV  */
    EXPO = 328,                    /* EXPO  */
    REF_EQ = 329,                  /* REF_EQ  */
    REF_NE = 330,                  /* REF_NE  */
    POWER = 331,                   /* POWER  */
    REAL_CONST = 332,              /* REAL_CONST  */
    KW_LE = 333                    /* KW_LE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 12 "simula.y"

        char s[ MAX_STR_LEN + 1 ]; /* text field for idents etc. */
        int i; /* integer field */
        double d; /* floating point field */

#line 148 "simula.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_SIMULA_TAB_H_INCLUDED  */
