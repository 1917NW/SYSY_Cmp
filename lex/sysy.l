%option noyywrap
%top{
    #include<math.h>
}

%{
    int chars=0,words=0,lines=0;
%}

word [a-zA-Z]+
line \n
char .

%%

{word} {words++;chars+=strlen(yytext);}
{line} {lines++;}
{char} {chars++;}

%%

int main(int argc,char* argv[]){
    if(argc>1){
        yyin=fopen(argv[1],"r");
        if(argc>2){
            yyout=fopen(argv[2],"w");
        }
    }
    yylex();
    fprintf(yyout,"%8d%8d%8d\n",lines,words,chars);
    return 0;
}