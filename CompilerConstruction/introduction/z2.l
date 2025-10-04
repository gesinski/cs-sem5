%{
#include <stdlib.h>
int sum = 0;
%}
%%
[char | int] {printf("=%d\n", sum); sum = 0;}
[0-9]+ {sum += atoi(yytext); printf(yytext);}
[^0-9]+ { printf("+"); }
%%
