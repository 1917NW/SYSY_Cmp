#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC};
    bool is_const;
public:
    Type(int kind,bool is_const=false) : kind(kind),is_const(is_const) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
    bool isConst() {return is_const;}
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size,bool is_const) : Type(Type::INT,is_const), size(size){};
    std::string toStr();
    Type* getConst();
};

class VoidType : public Type
{
public:
    VoidType(bool is_const) : Type(Type::VOID,is_const){};
    std::string toStr();
};


// class type {
//     Type* t;
//     bool is_const;
// public:
//     type(Type* t,bool is_const):t(t),is_const(is_const){};
//     Type* getType(){return t;}
//     bool isConst(){return is_const;}
// };

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    std::string toStr();
    std::vector<Type*>* GetParamsType(){return &paramsType;};
};

class TypeSystem
{
private:
    static IntType commonInt;
    static VoidType commonVoid;
    static IntType constInt;
public:
    static Type *intType;
    static Type *voidType;
    static Type *constIntType;
};

#endif
