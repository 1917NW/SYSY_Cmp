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

//表达式语句
ExprStmt:
    Exp SEMICOLON{
        $$=new ExprStmt($1);
    };

//赋值语句
AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON {
        $$ = new AssignStmt($1, $3);
    }
    ;

//块语句
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

//if语句
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }
    ;

//while语句
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3,$5);
    }

//空语句
NullStmt: SEMICOLON {
        $$ = new NullStmt();
}

//返回语句
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    ;

//表达式
Exp
    :
    AddExp {$$ = $1;}
    ;

//条件
Cond
    :
    LOrExp {$$ = $1;}
    ;

//表达式列表
ExpList:
    Exp {$$=new vector<ExprNode*>();$$->push_back($1);}
    |ExpList COMMA Exp {$$=$1;$$->push_back($3);}
    |%empty {$$=nullptr;}

//函数调用表达式
CallExp:
    ID LPAREN ExpList RPAREN{
        //获取函数名对应的符号表项
        SymbolEntry* se;
        se=identifiers->lookup($1);
        $$=new CallExpr(se,$3);
    }


//最基础的表达式：ID和数字，以及函数调用表达式
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

//一元表达式
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

//乘法表达式
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

//加法表达式
AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {//ADD 和 SUB都是左结合
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

//关系表达式
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

//相等表达式
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

//逻辑与表达式
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

//逻辑或表达式
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

//类型
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

//变量定义
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

//声明语句
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

//函数形参
FuncFParam:
    Type ID{
        //ToDo
        SymbolEntry *se;
        se=new IdentifierSymbolEntry(Var_type,$2,identifiers->getLevel());

        //把函数形参名放入到符号表链的第二个符号表中
        identifiers->install($2,se);

        //向形参列表中加入形参的类型
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
    
//函数形参列表
FuncFParams
    : FuncFParam {if($1!=nullptr){$$=new vector<FunctionParam>();$$->push_back(*($1));}else $$=nullptr;}
    | FuncFParams COMMA FuncFParam {
        $$=$1;$$->push_back(*($3));
        delete $3;
    }
    

//函数定义
FuncDef
    :
    Type ID {
        Type *funcType;
        funcType = new FunctionType($1,{});

        //获取当前函数的形参列表
        NowFuncParamsType=((FunctionType*)funcType)->GetParamsType();
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());

        //把函数名放到符号表链的第一个符号表中
        identifiers->install($2, se);

        //进入符号表链中的第二个符号表
        identifiers = new SymbolTable(identifiers);
    }
    LPAREN FuncFParams RPAREN
    BlockStmt
    {
        SymbolEntry *se;
        se = identifiers->lookup($2);
        assert(se != nullptr);

        
        //函数语句节点
        //FunctionDef包含函数原型se,$5函数形参列表，$7语句块
        $$ = new FunctionDef(se, $7,$5);

        //删除符号表链中第二个符号表
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
