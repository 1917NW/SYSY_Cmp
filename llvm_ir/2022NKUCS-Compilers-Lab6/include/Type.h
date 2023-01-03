#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>
#include "SymbolTable.h"

class Type
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC, PTR};
    bool is_const;
public:
    Type(int kind,bool is_const=false) : kind(kind),is_const(is_const) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    virtual int getSize() const {return 0;}
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size,bool is_const) : Type(Type::INT,is_const), size(size){};
    std::string toStr();
    int getSize()  {return size;}
};

class VoidType : public Type
{
public:
    VoidType(bool is_const) : Type(Type::VOID,is_const){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
    std::vector<SymbolEntry*> paramsSe;
   
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    Type* getRetType() {return returnType;};

    void setParmaSe(std::vector<SymbolEntry*> paramsSe){this->paramsSe=paramsSe;}
    std::vector<SymbolEntry*> getParamsSe() { return paramsSe; };

    std::string toStr();
    std::vector<Type*>* GetParamsType(){return &paramsType;};
    std::string paramsToStr();
};

class PointerType : public Type
{
private:
    Type *valueType;
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
    static IntType constInt;
public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
    static Type *constIntType;
};

#endif
