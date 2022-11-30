%code top{
    #include <iostream>
    #include <assert.h>
    #include<vector>
    #include "parser.h"
    using namespace std;
    extern Ast ast;
    Type* Var_type;
    bool isConst=false;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;

    //支持多变量声明和定义
    VarDef_entry* vde;
    std::vector<VarDef_entry> * vvde;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token IF ELSE
%token INT VOID CONST
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA
%token ADD SUB MUL DIV MOD OR AND LESS ASSIGN 
%token RETURN

%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef
%nterm <exprtype> Exp AddExp MulExp Cond LOrExp PrimaryExp LVal RelExp LAndExp
%nterm <type> Type

%nterm <vde> VarDef ConstDef
%nterm <vvde> VarDefList ConstDefList

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;
BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top = identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;
PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    ;

MulExp:
     PrimaryExp {$$ = $1;}
     |
     MulExp MUL PrimaryExp
     {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
     }
     |
     MulExp DIV PrimaryExp
     {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
     }
     |
     MulExp MOD PrimaryExp
     {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
     }

AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD PrimaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB PrimaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    ;
LAndExp
    :
    RelExp {$$ = $1;}
    |
    LAndExp AND RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::boolType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
Type
    : INT {
          if(!isConst){
        $$ = TypeSystem::intType;
        Var_type=TypeSystem::intType;
        }
        else{
        $$ = TypeSystem::constIntType;
        Var_type=TypeSystem::constIntType;
        isConst=false;
        }
    }
    | VOID {
        $$ = TypeSystem::voidType;
        Var_type=TypeSystem::voidType;
    }
    ;

VarDef
    :ID { 
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(Var_type, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new VarDef_entry(new Id(se),nullptr);
        delete []$1;
        }
    |ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(Var_type, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new VarDef_entry(new Id(se),$3);
        delete []$1;
    }
    ;

//变量定义列表
VarDefList
    : VarDefList COMMA VarDef{$$=$1;$$->push_back(*($3));delete $3;}
    | VarDef {$$=new vector<VarDef_entry>();$$->push_back(*($1));}

//常量定义
ConstDef
    :ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(Var_type, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new VarDef_entry(new Id(se),$3);
        delete []$1;
    }
    ;

//常量定义列表
ConstDefList
    : ConstDef {
        $$=new vector<VarDef_entry>();$$->push_back(*($1));
        }
    | ConstDefList COMMA ConstDef {
        $$=$1;$$->push_back(*($3));
    }
    ;   
DeclStmt
    :
    Type VarDefList SEMICOLON {
         $$ = new DeclStmt($2);
    }
    ;
    |
    CONST {isConst=true;} Type ConstDefList SEMICOLON {
        $$ = new DeclStmt($4);
    }

    ;

FuncDef
    :
    Type ID {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);
        $$ = new FunctionDef(se, $6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;
%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
