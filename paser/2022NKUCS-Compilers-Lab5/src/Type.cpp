#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(4,false);
IntType TypeSystem::constInt=IntType(4,true);
VoidType TypeSystem::commonVoid = VoidType(false);

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::constIntType=&constInt;

std::string IntType::toStr()
{
    if(isConst())
    return "const int";
    else
    return "int";
}

Type* IntType::getConst(){
    return TypeSystem::constIntType;
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
    if(paramsType.size()!=0){
    for(;i<(int)(paramsType.size()-1);i++){
        buffer<<paramsType[i]->toStr();
        buffer<<",";
    }
        buffer<<paramsType[i]->toStr();
    }
    buffer<<")";
    return buffer.str();
}
