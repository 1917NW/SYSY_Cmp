%option noyywrap
%top{
    #include<map>
    #include<iomanip>
    #include<fstream>
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

int main(){
    std::ifstream input("./testin");
    std::ofstream output("./testout");

    yyFlexLexer lexer(&input);
    
    lexer.yylex();
    output<<std::setw(8)<<lines<<std::setw(8)<<words<<std::setw(8)<<chars<<std::endl;
    return 0;
}