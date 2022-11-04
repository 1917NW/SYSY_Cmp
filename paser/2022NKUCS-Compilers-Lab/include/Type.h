#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC,CONSTINT};
public:
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    std::string toStr();
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class ConstIntType:public Type{
    private:
    int size;
public:
    ConstIntType(int size):Type(Type::CONSTINT),size(size){};
    std::string toStr();

};
class type {
    Type* t;
    bool is_const;
public:
    type(Type* t,bool is_const):t(t),is_const(is_const){};
    Type* getType(){return t;}
    bool isConst(){return is_const;}
};
class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<type> paramsType;
public:
    FunctionType(Type* returnType, std::vector<type> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    std::string toStr();
    std::vector<type>* GetParamsType(){return &paramsType;};
};

class TypeSystem
{
private:
    static IntType commonInt;
    static VoidType commonVoid;
public:
    static Type *intType;
    static Type *voidType;
};

#endif
