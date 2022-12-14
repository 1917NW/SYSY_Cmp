#include "Function.h"
#include "Unit.h"
#include "Type.h"
#include <queue>
#include <list>

using namespace std;
extern FILE* yyout;

Function::Function(Unit *u, SymbolEntry *s)
{
    u->insertFunc(this);
    entry = new BasicBlock(this);
    sym_ptr = s;
    parent = u;
}

Function::~Function()
{
    /*auto delete_list = block_list;
    for (auto &i : delete_list)
        delete i;
    parent->removeFunc(this);*/
}

// remove the basicblock bb from its block_list.
void Function::remove(BasicBlock *bb)
{
    block_list.erase(std::find(block_list.begin(), block_list.end(), bb));
}

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
    std::set<BasicBlock *> bbset;

    //双向链表
    std::list<BasicBlock *> bbque;
    bbque.push_back(entry);
    bbset.insert(entry);
    //递归输出基本块
   
    while (!bbque.empty())
    {
        
        auto bb = bbque.front();
        bbque.pop_front();
        bb->output();
        //广度优先遍历
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            //如果没有访问过程序流图中的此节点，则把该节点放入到队列中
            if (bbset.find(*succ) == bbset.end())
            {
                bbset.insert(*succ);
                bbque.push_back(*succ);
            }
        }
    }
    fprintf(yyout, "}\n");
}


void Function::genMachineCode(AsmBuilder* builder) 
{
    
    auto cur_unit = builder->getUnit();
    auto cur_func = new MachineFunction(cur_unit, this->sym_ptr);
    builder->setFunction(cur_func);
    std::map<BasicBlock*, MachineBlock*> map;

    list<BasicBlock*> bbque;
    std::set<BasicBlock *> bbset;
    bbset.insert(entry);
    bbque.push_back(entry);
    
    while (!bbque.empty())
    {
        
        auto bb = bbque.front();
        
        bb->genMachineCode(builder);
        
        map[bb]=builder->getBlock();
        bbque.pop_front();
        
        //广度优先遍历
        for (auto succ = bb->succ_begin(); succ != bb->succ_end(); succ++)
        {
            //如果没有访问过程序流图中的此节点，则把该节点放入到队列中
            if (bbset.find(*succ) == bbset.end())
            {
                bbset.insert(*succ);
                bbque.push_back(*succ);
            }
        }
        
    }
    
    // Add pred and succ for every block
    for(auto block : block_list)
    {
        
        auto mblock = map[block];
        for (auto pred = block->pred_begin(); pred != block->pred_end(); pred++)
            mblock->addPred(map[*pred]);
        for (auto succ = block->succ_begin(); succ != block->succ_end(); succ++)
            mblock->addSucc(map[*succ]);
    }
    
    cur_unit->InsertFunc(cur_func);

}
