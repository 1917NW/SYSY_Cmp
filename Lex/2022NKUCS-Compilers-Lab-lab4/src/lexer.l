%option noyywrap
%{
    /*
    * You will need to comment this line in lab5.
    在lab5里面把#define ONLY_FOR_LEX注释掉
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string>

    #ifdef ONLY_FOR_LEX
    #include <ostream>
    #include <fstream>
    #include<cmath>
    #include<vector>
    
    #define unknown -1
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;
    int rank_now=0;
    bool isDeclare=false;
    
   struct symbol_descriptor{
        string id;
        int rank;
        int start;
        int end;
    };

    vector<symbol_descriptor> symbol_table;

    int find_id(string id,int rank){

        for(int i=symbol_table.size()-1;i>=0;i--){
            if(symbol_table[i].id==id && symbol_table[i].rank==rank){
                return i;
            }
        }
        return -1;
    }

    void add_id(string id){
        symbol_descriptor sd;
        sd.id=id;
        sd.rank=rank_now;
        sd.start=yylineno;
        sd.end=unknown;
        symbol_table.push_back(sd);
    }
    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
    #endif
%}

DECIMIAL ([1-9][0-9]*|0)
OCTONARY 0([0-7]*)
HEXADECIMIAL (0x|0X)([0-9A-Ea-e]*)

ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
blockcommentelement  .|\n
blockcommentend "*/"
blockcommentbegin \/\*

linecommentelement .
linecommentend \n
linecommentbegin "//"

%x BLOCKCOMMENT
%x LINECOMMENT

%%


"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   A1: as the input of parser
    *   Q2: What is "INT" actually?
    *   A2: token
    */
    isDeclare=true;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("INT\tint\t"+to_string(yylineno));
    #else
        return INT;
    #endif
}

"void" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("VOID\tvoid\t"+to_string(yylineno));
    #else
        return VOID;
    #endif 
}

"const" {
#ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("CONST\tconst\t"+to_string(yylineno));
    #else
        return RETURN;
    #endif
}

"if" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("IF\tif\t"+to_string(yylineno));
    #else
        return IF;
    #endif
}

"else" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ELSE\telse\t"+to_string(yylineno));
    #else
        return ELSE;
    #endif
}

"while" {
#ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("WHILE\twhile\t"+to_string(yylineno));
    #else
        return WHILE;
    #endif
}

"continue" {
#ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("CONTINUE\tcontinue\t"+to_string(yylineno));
    #else
        return CONTINUE;
    #endif
}

"return" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RETURN\treturn\t"+to_string(yylineno));
    #else
        return RETURN;
    #endif
}




"==" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("EQ\t==\t"+to_string(yylineno));
    #else
        return EQ;
    #endif
}

"<=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LOE\t<=\t"+to_string(yylineno));
    #else
        return LOE;
    #endif
}

">=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MOE\t>=\t"+to_string(yylineno));
    #else
        return MOE;
    #endif
}

"!=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("NOE\t!=\t"+to_string(yylineno));
    #else
        return NOE;
    #endif
}

"=" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ASSIGN\t=\t"+to_string(yylineno));
    #else
        return ASSIGN;
    #endif
}

"<" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LESS\t<\t"+to_string(yylineno));
    #else
        return LESS;
    #endif
}

">" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MORE\t>\t"+to_string(yylineno));
    #else
        return MORE;
    #endif
}


"&&" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ANL\t&&\t"+to_string(yylineno));
    #else
        return ANL;
    #endif
}
"||" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ORL\t||\t"+to_string(yylineno));
    #else
        return ORL;
    #endif
}

"!" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("NOT\t!\t"+to_string(yylineno));
    #else
        return NOT;
    #endif
}


"+" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("ADD\t+\t"+to_string(yylineno));
    #else
        return ADD;
    #endif
}

"-" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SUB\t-\t"+to_string(yylineno));
    #else
        return SUB;
    #endif
}

"*" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MUL\t*\t"+to_string(yylineno));
    #else
        return MUL;
    #endif
}

"/" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("DIV\t/\t"+to_string(yylineno));
    #else
        return DIV;
    #endif
}

"%" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("MOD\t%\t"+to_string(yylineno));
    #else
        return MOD;
    #endif
}


";" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SEMICOLON\t;\t"+to_string(yylineno));
    #else
        return SEMICOLON;
    #endif
}


"(" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LPAREN\t(\t"+to_string(yylineno));
    #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RPAREN\t)\t"+to_string(yylineno));
    #else
    return RPAREN;
    #endif
}
"{" {
    rank_now++;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LBRACE\t{\t"+to_string(yylineno));
    #else
        return LBRACE;
    #endif
}
"}" {
    for(int i=symbol_table.size()-1;i>=0;i--){
        if(symbol_table[i].rank==rank_now && symbol_table[i].end==unknown)
            symbol_table[i].end=yylineno;
        else
            break;
    }
    rank_now--;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RBRACE\t}\t"+to_string(yylineno));
    #else
        return RBRACE;
    #endif
}


{DECIMIAL} {
    string num="NUM\t";num=num+yytext;
     #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(num);
     #else
        return NUM;
     #endif
    }

{OCTONARY} {
    string num="ONUM\t";
    int dnum=0;
    for(int i=strlen(yytext)-1,count=0;i>=1;i--,count++){
        dnum+=(yytext[i]-'0')*pow(8,count);
    }
    num+=to_string(dnum);
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(num+"\t"+to_string(yylineno));
    #else
        return NUM;
    #endif
}
 
{HEXADECIMIAL} {
    string num="HNUM\t";
    int dnum=0;
    for(int i=strlen(yytext)-1,count=0;i>=2;i--,count++){
        if(isdigit(yytext[i]))
        dnum+=(yytext[i]-'0')*pow(16,count);

        if(yytext[i]>=65 and yytext[i]<=70){
            dnum+=(yytext[i]-'A'+10)*pow(16,count);
        }
        else if(yytext[i]>=97 and yytext[i]<=101){
             dnum+=(yytext[i]-'a'+10)*pow(16,count);
     }
    }
    num+=to_string(dnum);
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(num+"\t"+to_string(yylineno));
    #else
        return NUM;
    #endif
}

{blockcommentbegin} {BEGIN BLOCKCOMMENT;}
<BLOCKCOMMENT>{blockcommentelement} {}
<BLOCKCOMMENT>{blockcommentend} {BEGIN INITIAL;}

{linecommentbegin} {BEGIN LINECOMMENT;}
<LINECOMMENT>{linecommentelement} {}
<LINECOMMENT>{linecommentend} {BEGIN INITIAL;}

{ID} {
    string text="";
    text+=yytext;
    if(isDeclare){
       
        int index=find_id(text,rank_now);
        if(index!=-1)
        {
            if(symbol_table[index].end==unknown){
                DEBUG_FOR_LAB4("重复定义");
            }
            else{
                add_id(text);
            }
        }
        else
            add_id(text);
        isDeclare=false;
    }
    
    string address;

    int i;
    for(i=rank_now;i>=0;i--){
        int n=find_id(text,i);
        if(n!=-1){
            address=to_string(reinterpret_cast<long long>(&symbol_table[n]));
            break;
        }
    }
    
    #ifdef ONLY_FOR_LEX

    if(i==-1){
        DEBUG_FOR_LAB4("ID\t"+text+" 没有定义");
    }
    else{
        string id="ID\t";
        id=id+yytext;
        DEBUG_FOR_LAB4(id+"\t"+to_string(yylineno)+"\t"+address);
    }
    #else
        return ID;
    #endif
}

{EOL} yylineno++;

{WHITE}
%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Arguments Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    if(!(yyout = fopen(argv[3], "w"))){
        fprintf(stderr, "No such file or directory: %s", argv[3]);
        exit(EXIT_FAILURE);
    }

    yylex();
    return 0;
}
#endif
