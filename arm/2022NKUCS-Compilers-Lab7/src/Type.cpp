#include "Type.h"
#include <sstream>

IntType TypeSystem::commonInt = IntType(32,false);
IntType TypeSystem::constInt=IntType(32,true);
IntType TypeSystem::commonBool = IntType(1,false);
VoidType TypeSystem::commonVoid = VoidType(false);

Type* TypeSystem::intType = &commonInt;
Type* TypeSystem::voidType = &commonVoid;
Type* TypeSystem::boolType = &commonBool;
Type* TypeSystem::constIntType=&constInt;

std::string IntType::toStr()
{
    std::ostringstream buffer;
    buffer << "i" << size;
    return buffer.str();
}

std::string VoidType::toStr()
{
    return "void";
}

std::string FunctionType::toStr()
{
    std::ostringstream buffer;
    buffer << returnType->toStr() << "()";
    return buffer.str();
}

std::string FunctionType::paramsToStr(){
    std::string paramstr="";
    if(paramsType.size()!=0){
        paramstr=paramsType[0]->toStr();
    }
    for(int i=1;i<(int)paramsType.size();i++){
        paramstr+=","+paramsType[i]->toStr();
    }
    return paramstr;
}

std::string PointerType::toStr()
{
    std::ostringstream buffer;
    buffer << valueType->toStr() << "*";
    return buffer.str();
}
