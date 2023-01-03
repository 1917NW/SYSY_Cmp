#ifndef __IRBUILDER_H__
#define __IRBUILDER_H__

class Unit;
class Function;
class BasicBlock;

class IRBuilder
{
private:
    Unit *unit;
    BasicBlock *insertBB;   // The current basicblock that instructions should be inserted into.

public:
    IRBuilder(Unit*unit) : unit(unit){};
    //设置当前应该插入的基本块
    void setInsertBB(BasicBlock*bb){insertBB = bb;};
    Unit* getUnit(){return unit;};
    BasicBlock* getInsertBB(){return insertBB;};
};

#endif