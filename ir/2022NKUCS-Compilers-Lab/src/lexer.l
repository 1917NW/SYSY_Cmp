%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include "parser.h"
    #include <ostream>
    #include <fstream>
    using namespace std;

    extern FILE *yyin; 
    extern FILE *yyout;
    extern bool dump_tokens;

    void DEBUG_FOR_LAB4(std::string s){
        std::string DEBUG_INFO = "[DEBUG LAB4]: \t" + s + "\n";
        fputs(DEBUG_INFO.c_str(), yyout);
    }
%}

DECIMIAL ([1-9][0-9]*|0)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
%x BLOCKCOMMENT

%%

"int" {
    /*
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    if(dump_tokens)
        DEBUG_FOR_LAB4("INT\tint");
    return INT;
}
"void" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("VOID\tvoid");
    return VOID;
}

"const" {
    return CONST;

}

"if" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("IF\tif");
    return IF;
};
"else" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ELSE\telse");
    return ELSE;
};
"return" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RETURN\treturn");
    return RETURN;
}

"&&" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("AND\t=");
    return AND;
}

"||" {
    return OR;
}

"==" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("EQ\t==");
    return EQ;
};

">=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GOE\t>=");
    return GOE;
};

"<=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LOE\t<=");
    return LOE;
};

"!=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NE\t!=");
    return NE;
};

"=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ASSIGN\t=");
    return ASSIGN;
}

"<" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LESS\t<");
    return LESS;
}

">" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MORE\t<");
    return GREATER;
};



"+" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ADD\t+");
    return ADD;
}

"-" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SUB\t-");
    return SUB;
};

"*" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MUL\t*");
    return MUL;
};

"/" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("DIV\t/");
    return DIV;
};

"%" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MOD\t%");
    return MOD;
};

";" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SEMICOLON\t;");
    return SEMICOLON;
}

"," {
    if(dump_tokens)
        DEBUG_FOR_LAB4("COMMA\t,");
    return COMMA;
};

"(" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LPAREN\t(");
    return LPAREN;
}
")" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RPAREN\t)");
    return RPAREN;
}
"{" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LBRACE\t{");
    return LBRACE;
}
"}" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RBRACE\t}");
    return RBRACE;
}

{DECIMIAL} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    yylval.itype = atoi(yytext);
    return INTEGER;
}

{ID} {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;
    return ID;
}
{EOL} yylineno++;
{WHITE}
%%
