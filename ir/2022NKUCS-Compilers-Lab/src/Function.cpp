#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <list>

extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);

    //入口basicblock，每个函数的第一个基本块
    entry = new BasicBlock(this);

    //函数类型的变量标识符
    sym_ptr = s;

    //上一级是unit
    parent = u;
}

Function::~Function()
{
    // auto delete_list = block_list;
    // for (auto &i : delete_list)
    //     delete i;
    // parent->removeFunc(this);
}


// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

//输出函数
void Function::output() const
{
    FunctionType* funcType = dynamic_cast<FunctionType*>(sym_ptr->getType());
    Type *retType = funcType->getRetType();
    std::string fparamType=funcType->paramsToStr();
    std::vector<SymbolEntry*> params = funcType->getParamsSe();
   
    if (!params.size())
        fprintf(yyout, "define %s %s() {\n", retType->toStr().c_str(),
                sym_ptr->toStr().c_str());
    else {
        fprintf(yyout, "define %s %s(", retType->toStr().c_str(),
                sym_ptr->toStr().c_str());
        for (long unsigned int i = 0; i < params.size(); i++) {
            if (i)
                fprintf(yyout, ", ");
            fprintf(yyout, "%s %s", params[i]->getType()->toStr().c_str(),
                    params[i]->toStr().c_str());
        }
        fprintf(yyout, ") {\n");
    }

    //基本块集合
    std::set<BasicBlock *> v;

    //双向链表
    std::list<BasicBlock *> q;
    q.push_back(entry);
    v.insert(entry);
    //递归输出基本块
   
    while (!q.empty())
    {
        
        auto bb = q.front();
        q.pop_front();
        bb->output();
        //广度优先遍历
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            //如果没有访问过程序流图中的此节点，则把该节点放入到队列中
            if (v.find(*succ) == v.end())
            {
                v.insert(*succ);
                q.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}
