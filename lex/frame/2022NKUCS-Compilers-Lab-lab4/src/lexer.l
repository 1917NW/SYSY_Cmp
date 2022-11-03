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
    //在分号的结束关闭isDeclare，是为了支持多变量的声明，比如int a,b,c;
    bool isDeclare=false;

    //引入isLeftValue是为了支持多变量声明时的初始化，比如int a=c,b=d;
    bool isLeftValue=true;

    //引入isFunctionArg是为了对"()"计数，并且对于函数定义里面变量声明进行区分
    int isFunctionArg=0;
    
   struct symbol_descriptor{
        string id;
        int rank; //作用域等级，用于寻找变量时区分重名变量
        int start;//变量声明的行
        int end;//变量作用域结束的行
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

    void add_id(string id,int rank){
        symbol_descriptor sd;
        sd.id=id;
        sd.rank=rank;
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
Float (([0-9]*(\.[0-9]+)|[0-9]+\.?)([eE][+-]?[0-9]+)?)


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

"float" {
   
    isDeclare=true;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("FLOAT\tfloat\t"+to_string(yylineno));
    #else
        return FLOAT;
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
        return CONST;
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
    isLeftValue=false;
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

"," {
    //如果现在是在'('内，则不把isLeftValue置为true,而是维持原状
    //否则，则重置isLeftValue
    //如果是函数内部的变量声明，则为true
    //如果不是函数内部的变量声明，则与赋值运算符=相关
    if(!isFunctionArg)
    {
        isLeftValue=true;
    }
     #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("COMMA\t,\t"+to_string(yylineno));
    #else
        return COMMA;
    #endif
}



";" {
    //关闭isDeclare
    //打开isLeftValue
    isDeclare=false;
    isLeftValue=true;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("SEMICOLON\t;\t"+to_string(yylineno));
    #else
        return SEMICOLON;
    #endif
}

"[" {
  #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LSQUARE\t[\t"+to_string(yylineno));
    #else
        return LSQUARE;
    #endif
}

"]" {
  #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RSQUARE\t]\t"+to_string(yylineno));
    #else
        return RSQUARE;
    #endif
}

"(" {
    //记录'('的个数
   isFunctionArg++;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LPAREN\t(\t"+to_string(yylineno));
    #else
        return LPAREN;
    #endif
}

")" {
    isFunctionArg--;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("RPAREN\t)\t"+to_string(yylineno));
    #else
    return RPAREN;
    #endif
}

"{" {
    //在进入语句块时，先关闭函数声明
    isDeclare=false;
    rank_now++;
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4("LBRACE\t{\t"+to_string(yylineno));
    #else
        return LBRACE;
    #endif
}

"}" {
    //遇到"}",就更新符号表中的同等级下未定义的符号的作用域末尾
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
    string t_num="ONUM\t";
    int num;
    sscanf(yytext, "%o", &num);
    t_num+=to_string(num);
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(t_num+"\t"+to_string(yylineno));
    #else
        return NUM;
    #endif
}
 
{HEXADECIMIAL} {
    string t_num="HNUM\t";

    int num;
    sscanf(yytext,"%x",&num);

    t_num+=to_string(num);
    #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(t_num+"\t"+to_string(yylineno));
    #else
        return NUM;
    #endif
}

{Float} {
    string t_num="FLOAT\t";
    t_num+=yytext;

    float num;
    sscanf(yytext, "%f", &num);

     #ifdef ONLY_FOR_LEX
        DEBUG_FOR_LAB4(t_num+"\t"+to_string(yylineno));
     #else
        return NUM;
     #endif
    }

{blockcommentbegin} {BEGIN BLOCKCOMMENT;}

<BLOCKCOMMENT>{blockcommentelement} {if(strcmp(yytext,"\n")==0)yylineno++;}
<BLOCKCOMMENT>{blockcommentend} {BEGIN INITIAL;}

{linecommentbegin} {BEGIN LINECOMMENT;}
<LINECOMMENT>{linecommentelement} {}
<LINECOMMENT>{linecommentend} {BEGIN INITIAL;yylineno++;}


{ID} {
    string text="";
    text+=yytext;
    string address;
    //如果是左值且是声明语句，添加到符号表，并且返回最后一个元素的索引
    if(isLeftValue && isDeclare){
        //如果为函数形参声明
        if(isFunctionArg){
            int index=find_id(text,rank_now+1);
            if(index!=-1)
            {
                if(symbol_table[index].end==unknown){
                    address="重复定义";
                }
                else{
                    add_id(text,rank_now+1);
                    int index=symbol_table.size()-1;
                    address=to_string(reinterpret_cast<long long>(&symbol_table[index]))+"\tindex:"+to_string(index);
                }
            }
            else{
                add_id(text,rank_now+1);
                int index=symbol_table.size()-1;
                address=to_string(reinterpret_cast<long long>(&symbol_table[index]))+"\tindex:"+to_string(index);
            }
        }
         //如果为正常的声明语句
        else{
            int index=find_id(text,rank_now);
            if(index!=-1)
            {
                if(symbol_table[index].end==unknown){
                   address="重复定义";
                    
                }
                else{
                    add_id(text,rank_now);
                    int index=symbol_table.size()-1;
                    address=to_string(reinterpret_cast<long long>(&symbol_table[index]))+"\tindex:"+to_string(index);
                }
            }
            else{
                add_id(text,rank_now);
                int index=symbol_table.size()-1;
                address=to_string(reinterpret_cast<long long>(&symbol_table[index]))+"\tindex:"+to_string(index);
            }
        }
    }
//如果为右值，或者为左值且是非声明标识符,直接查找
int i=0;
if(!isDeclare || !isLeftValue){
    
    for(i=rank_now;i>=0;i--){
        int n=find_id(text,i);
        
        if(n!=-1 and symbol_table[n].end==unknown){
            address=to_string(reinterpret_cast<long long>(&symbol_table[n]))+"\tindex:"+to_string(n);
            break;
        }
    }
    
}

    #ifdef ONLY_FOR_LEX

    if(i==-1){
        DEBUG_FOR_LAB4(to_string(i)+"ID\t"+text+" 没有定义");
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
