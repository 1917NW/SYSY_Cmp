%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;

    int yylex();
    int yyerror( char const * );
    Type* Var_type;
    bool isConst=false;
    bool isVoid=true;
    vector<Type*>* NowFuncParamsType;
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
    
    VarDef_entry* vde;
    vector<VarDef_entry> * vvde;

    FunctionParam* fpa;
    vector<FunctionParam> * vfpa;

    vector<ExprNode*>* vepl;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token IF ELSE WHILE CONTINUE
%token INT VOID CONST FLOAT
%token LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE SEMICOLON COMMA
%token ADD SUB OR AND LESS MORE ASSIGN LOE EQ GOE NE ANL ORL NOT 
%token MUL DIV MOD
%token RETURN

%nterm <stmttype> Stmts Stmt AssignStmt ExprStmt BlockStmt IfStmt ReturnStmt DeclStmt FuncDef WhileStmt NullStmt
%nterm <exprtype> Exp AddExp Cond LOrExp PrimaryExp LVal RelExp LAndExp MulExp UnaryExp EqExp CallExp
%nterm <type> Type
%nterm <vde> VarDef ConstDef
%nterm <vvde> VarDefList ConstDefList
%nterm <fpa> FuncFParam
%nterm <vfpa> FuncFParams
%nterm <vepl> ExpList

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
    | ExprStmt{$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}
    | NullStmt {$$=$1;}
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

//???????????????
ExprStmt:
    Exp SEMICOLON{
        $$=new ExprStmt($1);
    };

//????????????
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;

//?????????
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
        |LBRACE RBRACE { $$ = new CompoundStmt(nullptr);}

    ;

//if??????
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;

//while??????
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3,$5);
    }

//?????????
NullStmt: SEMICOLON {
        $$ = new NullStmt();
}


//????????????
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;

//?????????
Exp
    :
    AddExp {$$ = $1;}
    ;

//??????
Cond
    :
    LOrExp {$$ = $1;}
    ;

//???????????????
ExpList:
    Exp {$$=new vector<ExprNode*>();$$->push_back($1);}
    |ExpList COMMA Exp {$$=$1;$$->push_back($3);}
    |%empty {$$=nullptr;}

//?????????????????????
CallExp:
    ID LPAREN ExpList RPAREN{
        //????????????????????????????????????
        SymbolEntry* se;
        se=identifiers->lookup($1);
        $$=new CallExpr(se,$3);
    }


//????????????????????????ID???????????????????????????????????????
PrimaryExp
    :
    LVal {
        $$ = $1;
    }
    | INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    |LPAREN Exp RPAREN
    {
        $$ = $2;
    }
    |CallExp {$$=$1;}
    
    ;

//???????????????
UnaryExp
    :
    PrimaryExp{$$=$1;}
    | ADD UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
    }
    | SUB UnaryExp {
         SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    | NOT UnaryExp{
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    ;

//???????????????
MulExp:
    UnaryExp {$$ = $1;}
    |
    MulExp MUL UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    |
    MulExp DIV UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    |
    MulExp MOD UnaryExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }

//???????????????
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {//ADD ??? SUB???????????????
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;

//???????????????
RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp MORE AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MORE, $1, $3);
    }
    |
    RelExp LOE AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LOE, $1, $3);
    }
    |
    RelExp GOE AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::GOE, $1, $3);
    }
    ;

//???????????????
EqExp:
    RelExp {$$ = $1;}
    |EqExp EQ RelExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQ, $1, $3);
    }
    |EqExp NE RelExp {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NE, $1, $3);
    }

//??????????????????
LAndExp
    :
    EqExp {$$ = $1;}
    |
    LAndExp AND EqExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;

//??????????????????
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;

//??????
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

//????????????
ConstDef
    :ID ASSIGN Exp {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(Var_type, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new VarDef_entry(new Id(se),$3);
        delete []$1;
    }
    ;
//??????????????????
ConstDefList
    : ConstDef {
        $$=new vector<VarDef_entry>();$$->push_back(*($1));
        }
    | ConstDefList COMMA ConstDef {
        $$=$1;$$->push_back(*($3));
    }
    ;

//????????????
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

//??????????????????
VarDefList
    : VarDefList COMMA VarDef{$$=$1;$$->push_back(*($3));delete $3;}
    | VarDef {$$=new vector<VarDef_entry>();$$->push_back(*($1));}

//????????????
DeclStmt
    :
    Type VarDefList SEMICOLON {
        $$ = new DeclStmt($2);
    }
    |
    CONST{isConst=true;} Type ConstDefList SEMICOLON {
        
        $$ = new DeclStmt($4);
    }

    ;

//????????????
FuncFParam:
    Type ID{
        //ToDo
        SymbolEntry *se;
        se=new IdentifierSymbolEntry(Var_type,$2,identifiers->getLevel());

        //???????????????????????????????????????????????????????????????
        identifiers->install($2,se);

        //???????????????????????????????????????
        NowFuncParamsType->push_back($1);
        $$=new FunctionParam(new Id(se));
        delete []$2;
    }
    |CONST {isConst=true;}
    Type ID{
        SymbolEntry *se;
        se=new IdentifierSymbolEntry(Var_type,$4,identifiers->getLevel());
        identifiers->install($4,se);
        NowFuncParamsType->push_back($3);
        $$=new FunctionParam(new Id(se));
        delete []$4;
    }
    |%empty {$$=nullptr;}
    ;
    
//??????????????????
FuncFParams
    : FuncFParam {if($1!=nullptr){$$=new vector<FunctionParam>();$$->push_back(*($1));}else $$=nullptr;}
    | FuncFParams COMMA FuncFParam {
        $$=$1;$$->push_back(*($3));
        delete $3;
    }
    

//????????????
FuncDef
    :
    Type ID {
        Type *funcType;
        funcType = new FunctionType($1,{});

        //?????????????????????????????????
        NowFuncParamsType=((FunctionType*)funcType)->GetParamsType();
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());

        //??????????????????????????????????????????????????????
        identifiers->install($2, se);

        //??????????????????????????????????????????
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN FuncFParams RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);

        
        //??????????????????
        //FunctionDef??????????????????se,$5?????????????????????$7?????????
        $$ = new FunctionDef(se, $7,$5);

        //???????????????????????????????????????
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
