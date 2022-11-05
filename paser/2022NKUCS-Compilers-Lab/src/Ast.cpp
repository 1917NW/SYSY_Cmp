#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;

Node::Node()
{
    seq = counter++;
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void CallExpr::output(int level){
   
    std::string name,type;
    name=symbolEntry->toStr();
    type=symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cCallExpr \t function: %s %s\n", level, ' ',name.c_str(),type.c_str());
    //输出表达式
    
    for(int i=0;i<(int)epl->size();i++){
        (*epl)[i]->output(level+4);
    }
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "+";
            break;
        case SUB:
            op_str = "-";
            break;
        case AND:
            op_str = "&&";
            break;
        case OR:
            op_str = "||";
            break;
        case LESS:
            op_str = "<";
            break;
        case MUL:
            op_str = "*";
            break;
        case DIV:
            op_str = "/";
            break;
        case MOD:
            op_str = "%";
            break;
        case MORE:
            op_str = ">";
            break;
        case LOE:
            op_str = "<=";
            break;
        case GOE:
            op_str = ">=";
            break;
        case EQ:
            op_str="==";
            break;
        case NE:
            op_str="!=";
            break;
      
        
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void UnaryExpr::output(int level){
      std::string op_str;
      switch(op){
        case ADD:
            op_str='+';
            break;
        case SUB:
            op_str='-';
            break;
        case NOT:
            op_str="!";
            break;
      }
      fprintf(yyout, "%*cUnaryExpr\tprefix: %s\n", level, ' ', op_str.c_str());
      expr->output(level+4);
}
void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    bool isConst;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    isConst=dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->isConst();
    if(!isConst)
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
    else
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: const %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level + 4);
    stmt2->output(level + 4);
}
void VarDef_entry::output(int level){
    fprintf(yyout, "%*cVarDef\n", level, ' ');
    if(id!=nullptr)
    id->output(level+4);
    if(ep!=nullptr)
    ep->output(level+4);
}
void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    for(auto i:*p){
        i.output(level+4);
    }
}

void ExprStmt::output(int level){
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    ep->output(level+4);
}
void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void WhileStmt::output(int level){
     fprintf(yyout, "%*cWhileStmt\n", level, ' ');
     cond->output(level+4);
     stmt->output(level+4);
}

void NullStmt::output(int level){
     fprintf(yyout, "%*cNullStmt\n", level, ' ');
}
void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}
void FunctionParam::output(int level){
    fprintf(yyout, "%*cParmVarDecl : \n", level, ' ');
    id->output(level+4);
}
void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(functionParams!=nullptr){
        for(auto i:*functionParams)
            i.output(level+4);
    }
    stmt->output(level + 4);
}
