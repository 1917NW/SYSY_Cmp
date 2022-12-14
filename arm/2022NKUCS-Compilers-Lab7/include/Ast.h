#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Operand.h"
#include<vector>
#include<iostream>
#include "Type.h"
using namespace std;
class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node
{
private:
    static int counter;
    int seq;
protected:
    std::vector<Instruction*> true_list;
    std::vector<Instruction*> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<Instruction*> &list, BasicBlock*bb);
    std::vector<Instruction*> merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2);
public:
    Node();
    int getSeq() const {return seq;};
    static void setIRBuilder(IRBuilder*ib) {builder = ib;};
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    
    std::vector<Instruction*>& trueList() {return true_list;}
    std::vector<Instruction*>& falseList() {return false_list;}
};

class ExprNode : public Node
{
protected:
    int kind;
    enum{BIN,UN,OT};
    SymbolEntry *symbolEntry;
      // The result of the subtree is stored into dst.
    //表达式类型
    Type* type;
public:
    Operand *dst; 
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){kind=BIN;};
    Operand* getOperand() {return dst;};
    SymbolEntry* getSymPtr() {return symbolEntry;};
    virtual Type* getType(){return type;}
    virtual int getValue(){return 0;}
   
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, MUL,DIV,MOD, AND, OR, LESS, GREATER,EQ,NE,LOE,GOE};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){
       if(op>=BinaryExpr::AND && op<=BinaryExpr::GOE){
        type=TypeSystem::boolType;
       }
       else if(op>=BinaryExpr::ADD && op<=BinaryExpr::MOD){
        type=TypeSystem::intType;
       }
       SymbolEntry *temp = new TemporarySymbolEntry(type, SymbolTable::getLabel()); dst = new Operand(temp);
       kind=BIN;
        };
    void output(int level);
    void typeCheck();
    void genCode();
};

class UnaryExpr:public ExprNode
{
private:
    int op;
    ExprNode * expr;
public:
    enum{ADD,SUB,NOT};
    UnaryExpr(SymbolEntry* se,int op,ExprNode* expr):ExprNode(se),op(op),expr(expr){kind=UN;
        if(op==NOT){
            type=TypeSystem::boolType;
        }
        else
            type=TypeSystem::intType;
        SymbolEntry *temp = new TemporarySymbolEntry(type, SymbolTable::getLabel()); dst = new Operand(temp);
    };
    void output(int level);
    void genCode();
    void typeCheck();
};


class CallExpr:public ExprNode{
private:
    vector<ExprNode*>* epl;
public:
    CallExpr(SymbolEntry* se,vector<ExprNode*>* epl):ExprNode(se),epl(epl){type=((FunctionType*)se->getType())->getRetType();SymbolEntry *temp = new TemporarySymbolEntry(((FunctionType*)se->getType())->getRetType(), SymbolTable::getLabel()); dst = new Operand(temp);};
    void output(int level);
    void typeCheck();
    void genCode();
    
};

class Constant : public ExprNode
{
public:
    int value=0;
    Constant(SymbolEntry *se) : ExprNode(se){dst = new Operand(se);type=se->getType();};
    Constant(SymbolEntry *se,int value) : ExprNode(se),value(value){dst = new Operand(se);type=se->getType();};
    void output(int level);
    void typeCheck();
    void genCode();
    int getValue(){return value;}
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){SymbolEntry *temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel()); dst = new Operand(temp);type=se->getType();};
    void output(int level);
    void typeCheck();
    void genCode();
};


//int2bool, bool2int
class ImplicitCastExpr:public ExprNode{
private:
    ExprNode* expr;
 public:
    //把表达式节点的类型转换为dstType类型
    ImplicitCastExpr(ExprNode* expr, Type* dstType)
        : ExprNode(nullptr), expr(expr) {
        type = dstType;
        symbolEntry = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        dst = new Operand(symbolEntry);
    };
    void output(int level);
    void typeCheck();
    void genCode();
};

class StmtNode : public Node
{};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class WhileStmt:public StmtNode
{
    private:
        ExprNode *cond;
        StmtNode* stmt;
        BasicBlock* cond_bb;
        BasicBlock* end_bb;
    public:
        WhileStmt(ExprNode *cond, StmtNode* stmt):cond(cond),stmt(stmt){};
        void output(int level);
        void typeCheck();
        void genCode();
};

class NullStmt:public StmtNode
{
    public:
    NullStmt(){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class VarDef_entry{
public:
    Id* id;
    ExprNode* ep;
    VarDef_entry(Id* id,ExprNode* ep):id(id),ep(ep){};
    void output(int level);
};

class DeclStmt : public StmtNode
{
private:
    vector<VarDef_entry> * p;
public:
    DeclStmt(vector<VarDef_entry>* p) : p(p){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ExprStmt : public StmtNode{
private:
    ExprNode* ep;
public:
    ExprStmt(ExprNode* ep):ep(ep){};
    void output(int level);
    void typeCheck();
    void genCode();
};
class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FunctionParam{
    
public:
    Id *id;
    FunctionParam(Id* id):id(id){};
    void output(int level);
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    vector<FunctionParam>* functionParams;
    StmtNode *stmt;
public:
    FunctionDef(SymbolEntry *se, StmtNode *stmt,vector<FunctionParam>* functionParams) : se(se), functionParams(functionParams),stmt(stmt){};
    void output(int level);
    void typeCheck();
    void genCode();
};


class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
    void typeCheck();
    void genCode(Unit *unit);
};

#endif
