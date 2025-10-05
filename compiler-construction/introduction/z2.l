%{
int keywords = 0;
%}
%%
[char | int | void | for | if | else]+ {keywords++;}
[^A-Za-z] {}
%%
int yywrap() {
    printf("keywords = %d\n", keywords);
    return(1);
}
