#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4);
VoidType TypeSystem::commonVoid = VoidType();

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;

std::string IntType::toStr()
{
    return "int";
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    int i=0;
    buffer << returnType->toStr() << "(";
    for(;i<(int)(paramsType.size()-1);i++){
        if(paramsType[i].isConst())
        buffer<<"const "<<paramsType[i].getType()->toStr();
        else
        buffer<<paramsType[i].getType()->toStr();
        buffer<<",";
    }
    if(paramsType[i].isConst())
        buffer<<"const "<<paramsType[i].getType()->toStr();
        else
        buffer<<paramsType[i].getType()->toStr();
    buffer<<")";
    return buffer.str();
}
