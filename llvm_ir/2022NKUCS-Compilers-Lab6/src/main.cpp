#include <iostream>
#include <string.h>
#include <unistd.h>
#include "Ast.h"
#include "Unit.h"
using namespace std;

Ast ast;
Unit unit;
extern FILE *yyin;
extern FILE *yyout;
extern SymbolTable *sysyTable;
extern vector<VarDef_entry> globalVarList;

int yyparse();

char outfile[256] = "a.out";
bool dump_tokens;
bool dump_ast;
bool dump_ir;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "iato:")) != -1)
    {
        switch (opt)
        {
        case 'o':
            strcpy(outfile, optarg);
            break;
        case 'a':
            dump_ast = true;
            break;
        case 't':
            dump_tokens = true;
            break;
        case 'i':
            dump_ir = true;
            break;
        default:
            fprintf(stderr, "Usage: %s [-o outfile] infile\n", argv[0]);
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (optind >= argc)
    {
        fprintf(stderr, "no input file\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fopen(argv[optind], "r")))
    {
        fprintf(stderr, "%s: No such file or directory\nno input file\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
    if (!(yyout = fopen(outfile, "w")))
    {
        fprintf(stderr, "%s: fail to open output file\n", outfile);
        exit(EXIT_FAILURE);
    }
    //词法分析+语法分析生成语法树
    yyparse();
    if(dump_ast)
    ast.output();
    
    //类型检查
    ast.typeCheck();



    //生成程序流图
    ast.genCode(&unit);


    for(auto globalVar:globalVarList){
        string name;
        IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(globalVar.id->getSymPtr());
        name=se->toStr();
        string value="0";
        if(globalVar.ep){
        value=globalVar.ep->getOperand()->toStr();
        }
        
        string type="i32";
        fprintf(yyout,"%s = global %s %s, align 4\n",name.c_str(),type.c_str(),value.c_str());
    }
    //根据程序流图生成中间代码
    if(dump_ir)
        unit.output();

    
    if(sysyTable->lookup("putint")){

        SymbolEntry* se=sysyTable->lookup("putint");
        std::string funcName,returnType,fparamType;
        FunctionType* functionType=(FunctionType*)(se->getType());
        funcName=se->toStr();
        returnType=functionType->getRetType()->toStr();
        fparamType=functionType->paramsToStr();
        fprintf(yyout,"declare %s %s(%s)\n",returnType.c_str(),funcName.c_str(),fparamType.c_str());
    }

     if(sysyTable->lookup("getint")){

        SymbolEntry* se=sysyTable->lookup("getint");
        std::string funcName,returnType,fparamType;
        FunctionType* functionType=(FunctionType*)(se->getType());
        funcName=se->toStr();
        returnType=functionType->getRetType()->toStr();
        fparamType=functionType->paramsToStr();
        fprintf(yyout,"declare %s %s(%s)\n",returnType.c_str(),funcName.c_str(),fparamType.c_str());
    }

     if(sysyTable->lookup("putch")){

        SymbolEntry* se=sysyTable->lookup("putch");
        std::string funcName,returnType,fparamType;
        FunctionType* functionType=(FunctionType*)(se->getType());
        funcName=se->toStr();
        returnType=functionType->getRetType()->toStr();
        fparamType=functionType->paramsToStr();
        fprintf(yyout,"declare %s %s(%s)\n",returnType.c_str(),funcName.c_str(),fparamType.c_str());
    }
     if(sysyTable->lookup("getch")){

        SymbolEntry* se=sysyTable->lookup("getch");
        std::string funcName,returnType,fparamType;
        FunctionType* functionType=(FunctionType*)(se->getType());
        funcName=se->toStr();
        returnType=functionType->getRetType()->toStr();
        fparamType=functionType->paramsToStr();
        fprintf(yyout,"declare %s %s(%s)\n",returnType.c_str(),funcName.c_str(),fparamType.c_str());
    }
    return 0;

    
}
