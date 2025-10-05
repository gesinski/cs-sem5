%{
#include <stdio.h>
int keywords = 0;
int variables = 0;
int string = 0;
int numbers = 0;
int arithmetics = 0;
int comments = 0;
int lines = 0;
%}
%%
"//".* { comments++;}
"/*"([^*]|\*+[^*/])*"*/" { 
    comments++; 
    for (int i = 0; yytext[i]; ++i)
    if (yytext[i] == '\n') ++lines;
}  
[0-9]+ {numbers++;}
\"([^"\\]|\\.)*\" {
    string++;
    for (int i = 0; yytext[i]; ++i)
    if (yytext[i] == '\n') ++lines;
}
\n {lines++;}
(char|int|void|for|if|else|return) {keywords++;}
(\+\+|\-\-|\+|\-|\*|\/) {arithmetics++;}
[a-zA-Z_][a-zA-Z0-9_]* {variables++;}
[ \t]+ ;
. ;
%%
int yywrap() {
    printf("keywords = %d\n", keywords);
    printf("numbers = %d\n", numbers);
    printf("arithmetics = %d\n", arithmetics);
    printf("string = %d\n", string);
    printf("comments = %d\n", comments);
    printf("variables = %d\n", variables);
    printf("lines = %d\n", lines);
    return(1);
}
