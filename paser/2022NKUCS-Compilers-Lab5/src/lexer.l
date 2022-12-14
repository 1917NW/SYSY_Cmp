%option noyywrap
%{
    #define YY_NO_UNPUT
    #define YY_NO_INPUT

    #include "parser.h"
    #include <ostream>
    #include <fstream>
    #include <iostream>
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
OCTONARY 0([0-7]*)
HEXADECIMIAL (0x|0X)([0-9A-Fa-f]*)
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
"float" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("FLOAT\tfloat");
    return FLOAT;
}

"const" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("CONSt\tconst");
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
"while" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("WHILE\twhile");
    return WHILE;
};
"continue" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("CONTINUE\tcontinue");
    return CONTINUE;
};


"return" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RETURN\treturn");
    return RETURN;
}

"==" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("EQ\t==");
    return EQ;
};

"<=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LOE\t<=");
    return LOE;
};

">=" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("GOE\t>=");
    return GOE;
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
};

"<" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LESS\t<");
    return LESS;
};

">" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("MORE\t<");
    return MORE;
};

"&&" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("AND\t&&");
    return AND;
}

"||" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("ORL\t||");
    return OR;
};

"!" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("NOT\t!");
    return NOT;
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

"," {
    if(dump_tokens)
        DEBUG_FOR_LAB4("COMMA\t,");
    return COMMA;
};


"[" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("LSQUARE\t[");
    return LSQUARE;
};

"]" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("RSQUARE\t]");
    return RSQUARE;
};
";" {
    if(dump_tokens)
        DEBUG_FOR_LAB4("SEMICOLON\t;");
    return SEMICOLON;
}
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

    if(yytext[0]=='-'){        
    yylval.itype = -1*atoi(yytext+1);
    cout<<"-"<<endl;
    }
    else if(yytext[0]=='+'){
     yylval.itype = atoi(yytext+1);
     cout<<"+"<<endl;
    }
    else{
     yylval.itype = atoi(yytext);
    }
    return INTEGER;
}

{HEXADECIMIAL} {
    int num;
    sscanf(yytext, "%x", &num);
    yylval.itype=num;
    return INTEGER;
}

{OCTONARY} {
    int num;
    sscanf(yytext, "%o", &num);
    yylval.itype=num;
    return INTEGER;
}

{blockcommentbegin} {BEGIN BLOCKCOMMENT;}

<BLOCKCOMMENT>{blockcommentelement} {}
<BLOCKCOMMENT>{blockcommentend} {BEGIN INITIAL;}

{linecommentbegin} {BEGIN LINECOMMENT;}
<LINECOMMENT>{linecommentelement} {}
<LINECOMMENT>{linecommentend} {BEGIN INITIAL;}

"putint" {
    if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;

    //?????????????????????
    SymbolTable* top=identifiers;
    while(top->getPrev()){
        top=top->getPrev();
    }

    //??????void putint(int)?????????
    Type *funcType;
    vector<Type*> vtp;

    vtp.push_back(TypeSystem::intType);
    funcType = new FunctionType(TypeSystem::voidType,vtp);
    SymbolEntry *se = new IdentifierSymbolEntry(funcType,yytext, top->getLevel());

    //???????????????
    top->install(yytext,se);
    return ID;
}

"getint" {
     if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;

   
    //?????????????????????
    SymbolTable* top=identifiers;
    while(top->getPrev()){
        top=top->getPrev();
    }

    //??????void getint()?????????
    Type *funcType;
    vector<Type*> vtp;
    
    funcType = new FunctionType(TypeSystem::intType,vtp);
    SymbolEntry *se = new IdentifierSymbolEntry(funcType,yytext, top->getLevel());

    //???????????????
    top->install(yytext,se);
    
    return ID;
}

"putch" {
     if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;

   
    //?????????????????????
    SymbolTable* top=identifiers;
    while(top->getPrev()){
        top=top->getPrev();
    }

    //??????void putch(int)?????????
    Type *funcType;
    vector<Type*> vtp;
    
    vtp.push_back(TypeSystem::intType);
    
    funcType = new FunctionType(TypeSystem::voidType,vtp);
    SymbolEntry *se = new IdentifierSymbolEntry(funcType,yytext, top->getLevel());

    //???????????????
    top->install(yytext,se);
    

    return ID;
}

"getch" {
     if(dump_tokens)
        DEBUG_FOR_LAB4(yytext);
    char *lexeme;
    lexeme = new char[strlen(yytext) + 1];
    strcpy(lexeme, yytext);
    yylval.strtype = lexeme;

   
    //?????????????????????
    SymbolTable* top=identifiers;
    while(top->getPrev()){
        top=top->getPrev();
    }

    //??????int getch()?????????
    Type *funcType;
    vector<Type*> vtp;
    
    funcType = new FunctionType(TypeSystem::intType,vtp);
    SymbolEntry *se = new IdentifierSymbolEntry(funcType,yytext, top->getLevel());

    //???????????????
    top->install(yytext,se);
    

    return ID;
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
