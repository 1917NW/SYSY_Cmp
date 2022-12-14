#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"
#include<list>

extern FILE *yyout;
extern vector<VarDef_entry> globalVarList;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

Type* nowFucntionRetType;
Type* nowFucntionType;

Node::Node()
{
    seq = counter++;
}


//从根节点向下遍历，生成程序流图

//回填技术：遍历list中的所有指令，把所有的条件跳转指令的正确分支设置为bb，把无条件跳转指令的目的块设为bb
void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond()){

            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);

        }
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}

//把list2中的指令放到list1后面，返回合并的指令集合
std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
   
    if(functionParams!=nullptr){
    for(auto& i:(*functionParams)){
       
        IdentifierSymbolEntry* se =
        dynamic_cast<IdentifierSymbolEntry*>(i.id->getSymPtr());
        
        BasicBlock *entry = func->getEntry();
        Instruction *alloca;
        Operand *addr;
        Operand* param=new Operand(se);
        SymbolEntry *addr_se;
        Type *type;
         
        type = new PointerType(se->getType());
        //用一个中间符号代替变量
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
        entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
        se->setAddr(addr); 

        //Operand* temp=se->getAddr();

        new StoreInstruction(addr,param,entry);

    }
 }

    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);
   
    //生成该函数所有的指令以及所有的块
    stmt->genCode();

    
    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
    //组织所有的块，生成该函数的控制流图
      for (auto block = func->begin(); block != func->end(); block++) {
        //获取该块的最后一条指令，即跳转指令
        //根据跳转指令的跳转的目标块，设置程序流图
        Instruction* last = (*block)->rbegin();

        //如果为条件跳转，则有2个后继
        if (last->isCond()) {
            BasicBlock *truebranch, *falsebranch;
            truebranch =
                dynamic_cast<CondBrInstruction*>(last)->getTrueBranch();
            falsebranch =
                dynamic_cast<CondBrInstruction*>(last)->getFalseBranch();

            (*block)->addSucc(truebranch);
            (*block)->addSucc(falsebranch);
            truebranch->addPred(*block);
            falsebranch->addPred(*block);

        } else if (last->isUncond()) {  //如果为无条件跳转，则只需要1个后继
            BasicBlock* dst =
                dynamic_cast<UncondBrInstruction*>(last)->getBranch();
            (*block)->addSucc(dst);
            dst->addPred(*block);
            }

}
        //遍历所有语句块，如果搜索所有没有跳转指令的块
       
    //删除一个块内ret后面的指令以及块
      for (auto b = func->begin(); b != func->end(); b++) {
        auto block = *b;
        bool hasRet = false;
        //遍历一个函数内所有块，把块中ret语句后面的所有指令删除
        for (auto i = block->begin(); i != block->end(); i = i->getNext()) {
            if (hasRet) {
                block->remove(i);
                delete i;
                continue;
            }
            if (i->isRet())
                hasRet = true;
        }

        //把ret后面所有的块都删除
        if (hasRet) {
            while (block->succ_begin() != block->succ_end()) {
                auto rb = *(block->succ_begin());
                block->removeSucc(rb);
                rb->removePred(block);
            }
        }
    }

    //没有ret指令，也没有条件跳转和无条件跳转指令
    list<BasicBlock *> q;
    std::set<BasicBlock *> v;
    v.insert(entry);
    q.push_back(func->getEntry());
    while (!q.empty())
    {
        
        auto bb = q.front();
        q.pop_front();
        Instruction* last_i=(bb)->rbegin();
        if(last_i->isCond()||last_i->isUncond()||last_i->isRet())
            ;
        else
            {
                if(!(((FunctionType*)(se->getType()))->getRetType()==TypeSystem::voidType))
                    cout<<"basicblock has no branch stmt or ret stmt"<<endl;
            }
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
    
}
  
   
    
   
void CallExpr::genCode(){
    BasicBlock *bb = builder->getInsertBB();
    vector<Operand*> params;
    if(epl!=nullptr){
        for(auto& i:*epl){
            i->genCode();
            params.push_back(i->getOperand());
        }
    }
    
    if(((FunctionType*)(symbolEntry->getType()))->getRetType()==TypeSystem::voidType)
        dst=nullptr;
   
    new CallInstruction(dst,symbolEntry,params,bb);
    
}   


void BinaryExpr::genCode()
{
   
    {
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    //逻辑运算表达式
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();

        bb = builder->getInsertBB();
        BasicBlock *truebb, *falsebb, *tempbb;
        //临时块
        truebb = new BasicBlock(func);
        falsebb = new BasicBlock(func);
        tempbb = new BasicBlock(func);
        expr1->trueList().push_back(new CondBrInstruction(truebb, tempbb, expr1->getOperand(), bb));

        //在tempbb中设置跳转块为falsebb
        expr1->falseList().push_back(new UncondBrInstruction(falsebb, tempbb));


        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        dst=expr2->getOperand();

        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if(op == OR)
    {
        // Todo
         BasicBlock *falseBB = new BasicBlock(func);
         expr1->genCode();
         bb = builder->getInsertBB();
         BasicBlock *truebb, *falsebb, *tempbb;
        //临时假块
          truebb = new BasicBlock(func);
         falsebb = new BasicBlock(func);
         tempbb = new BasicBlock(func);
         expr1->trueList().push_back(new CondBrInstruction(truebb, tempbb, expr1->getOperand(), bb));
         expr1->falseList().push_back(new UncondBrInstruction(falsebb, tempbb));

         
         backPatch(expr1->falseList(), falseBB);
         builder->setInsertBB(falseBB);   
        
         expr2->genCode();
         dst=expr2->getOperand();
         false_list=expr2->falseList();
         true_list=merge(expr1->trueList(),expr2->trueList());
    }
    //关系运算表达式
    else if(op >= LESS && op <= GOE)
    {
        // Todo
        
        
        expr1->genCode();
        expr2->genCode();
        
        Operand* src1 = expr1->getOperand();
        Operand* src2 = expr2->getOperand();
        int cmpopcode=-1;
        switch (op)
        {
        case LESS: 
            cmpopcode=CmpInstruction::L;
            break;
        
        case GREATER:
            cmpopcode=CmpInstruction::G;
            break;
        case LOE:
            cmpopcode=CmpInstruction::LE;
            break;
        case GOE:
            cmpopcode=CmpInstruction::GE;
            break;
        case EQ:
            cmpopcode=CmpInstruction::E;
            break;
        case NE:
            cmpopcode=CmpInstruction::NE;
            break;
        }
        new CmpInstruction(cmpopcode, dst, src1, src2, bb);
        
    }
    //算术运算表达式
    else if(op >= ADD && op <= MOD)
    {
        
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        case MUL:
            opcode = BinaryInstruction::MUL;
            break;
        case DIV:
            opcode=BinaryInstruction::DIV;
            break;
        case MOD:
            opcode=BinaryInstruction::MOD;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
        }
    }
}

void UnaryExpr::genCode(){
//做条件表达式
//做运算表达式
    expr->genCode();

    if (op == NOT) {
        BasicBlock* bb = builder->getInsertBB();
        Operand* src = expr->getOperand();
        new XorInstruction(dst, src, bb);
        
    } else if (op == SUB) {
        Operand* src2;
        BasicBlock* bb = builder->getInsertBB();
        Operand* src1 = new Operand(new ConstantSymbolEntry(dst->getType(), 0));
        src2 = expr->getOperand();
        new BinaryInstruction(BinaryInstruction::SUB, dst, src1, src2, bb);
        
    }
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}

void IfStmt::genCode()
{
   
    Function *func;
    BasicBlock *then_bb, *end_bb,*bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);
  
    
    cond->genCode();
    
     bb = builder->getInsertBB();
     BasicBlock *truebb, *falsebb, *tempbb;
        //临时块
     truebb = new BasicBlock(func);
     falsebb = new BasicBlock(func);
     tempbb = new BasicBlock(func);
     
     cond->trueList().push_back(new CondBrInstruction(truebb, tempbb, cond->getOperand(), bb));

     cond->falseList().push_back(new UncondBrInstruction(falsebb, tempbb));

    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    //生成then_bb中的指令
    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();

    //在then_bb插入一条跳到end_bb中的无条件跳转指令
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void NullStmt::genCode(){

}

void WhileStmt::genCode(){
   Function* func;
   BasicBlock* bb,*cond_bb,*stmt_bb,*end_bb;

   func = builder->getInsertBB()->getParent();
   cond_bb=new BasicBlock(func);
   stmt_bb=new BasicBlock(func);
   end_bb=new BasicBlock(func);

   bb = builder->getInsertBB();
   new UncondBrInstruction(cond_bb,bb);

   builder->setInsertBB(cond_bb);

   if(((IntType*)(cond->getType()))->getSize()!=1){
       
        cond=new ImplicitCastExpr(cond,TypeSystem::boolType);
        
    }

   cond->genCode();
   bb = builder->getInsertBB();
     BasicBlock *truebb, *falsebb, *tempbb;
        //临时块
     truebb = new BasicBlock(func);
     falsebb = new BasicBlock(func);
     tempbb = new BasicBlock(func);
     cond->trueList().push_back(new CondBrInstruction(truebb, tempbb, cond->getOperand(), bb));
        //在tempbb中设置跳转块为falsebb
     cond->falseList().push_back(new UncondBrInstruction(falsebb, tempbb));
   backPatch(cond->trueList(),stmt_bb);
   backPatch(cond->falseList(),end_bb);

   builder->setInsertBB(stmt_bb);
   stmt->genCode();
   stmt_bb=builder->getInsertBB();
   new UncondBrInstruction(cond_bb,stmt_bb);

   builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    // Todo
    Function* func;
    BasicBlock *then_bb, *else_bb, *end_bb,*bb ;
    // bb = builder->getInsertBB();
    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    if(((IntType*)(cond->getType()))->getSize()!=1){
       
        cond=new ImplicitCastExpr(cond,TypeSystem::boolType);
        
    }
    cond->genCode();
    
    bb = builder->getInsertBB();
    
     BasicBlock *truebb, *falsebb, *tempbb;
        //临时假块
     truebb = new BasicBlock(func);
     falsebb = new BasicBlock(func);
     tempbb = new BasicBlock(func);
     
     cond->trueList().push_back(new CondBrInstruction(truebb, tempbb, cond->getOperand(), bb));
        //在tempbb中设置跳转块为falsebb
     cond->falseList().push_back(new UncondBrInstruction(falsebb, tempbb));
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), else_bb);

    // new CondBrInstruction(then_bb,else_bb,IfElsecond,bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);
}

void CompoundStmt::genCode()
{
    // Todo
    //llvm ir不以花括号划分块，而是以跳转指令来划分块，跳转指令是块的最后一条指令
    
    if (stmt)
        stmt->genCode();
}

void SeqNode::genCode()
{
    // Todo
    stmt1->genCode();
    stmt2->genCode();
}

void DeclStmt::genCode()
{
   
   if(p!=nullptr){
    for(auto& i:(*p)){
   
        if(i.ep){
    if(i.ep->getType()==TypeSystem::boolType){
                    i.ep=new ImplicitCastExpr(i.ep, TypeSystem::intType);
                }
        }
 
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(i.id->getSymPtr());
    //怎么处理全局变量
    if(se->isGlobal())
    {
        Operand *addr;
        SymbolEntry *addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        se->setAddr(addr);
        globalVarList.push_back(i);
    }
    //局部变量
    else if(se->isLocal())
    {
        Function *func = builder->getInsertBB()->getParent();
        BasicBlock *entry = func->getEntry();
        Instruction *alloca;
        Operand *addr;
        SymbolEntry *addr_se;
        Type *type;
        type = new PointerType(se->getType());
        //用一个中间符号代替变量
        addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
        addr = new Operand(addr_se);
        alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
        entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
        se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        if(i.ep){
             BasicBlock* bb = builder->getInsertBB();
             i.ep->genCode();
             Operand* src = i.ep->getOperand();
             new StoreInstruction(addr, src, bb);
        }
    }
    }
 }
}

void ReturnStmt::genCode()
{
    
    // Todo
    BasicBlock* bb = builder->getInsertBB();
    Operand* src = nullptr;
    if (retValue) {
        retValue->genCode();
        src = retValue->getOperand();
    }
    new RetInstruction(src, bb);
}

void AssignStmt::genCode()

{
     
     if(expr->getType()==TypeSystem::voidType){
             fprintf(stderr, "Void type is not allowed in assginment stmt \n");
        }else{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
     
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
        }
}

void ExprStmt::genCode(){
    ep->genCode();
}

void ImplicitCastExpr::genCode(){
   
  
    if(expr){
        expr->genCode();
    
        BasicBlock* bb = builder->getInsertBB();
        //int2bool
        if (type == TypeSystem::boolType && ((IntType*)(expr->getType()))->getSize()!=1) {
             new CmpInstruction(
            CmpInstruction::NE, this->dst, this->expr->getOperand(),
            new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), bb);
        }

        //bool2int
        if(type==TypeSystem::intType && ((IntType*)(expr->getType()))->getSize()!=32){
            new ZextInstruction(this->dst, expr->getOperand(), bb);
        }
    }
}

//对程序流图进行类型检查
void Ast::typeCheck()
{
    if(root != nullptr)
        root->typeCheck();
    
}


void FunctionDef::typeCheck()
{
    // Todo
    //检查函数的返回类型和参数
    nowFucntionRetType=((FunctionType*)(se->getType()))->getRetType();
   

    //检查语句
    stmt->typeCheck();
   
}

void BinaryExpr::typeCheck()
{
    // Todo
   
     expr1->typeCheck();
    expr2->typeCheck();
    
     if(expr1->getType()==TypeSystem::voidType || expr2->getType()==TypeSystem::voidType){
             fprintf(stderr, "Void type is involved in compution \n");
        }
    else if(op==BinaryExpr::AND || op==BinaryExpr::OR){
     if(((IntType*)(expr1->getType()))->getSize()!=1){
        expr1=new ImplicitCastExpr(expr1,TypeSystem::boolType);
        }

     if(((IntType*)(expr2->getType()))->getSize()!=1){
        expr2=new ImplicitCastExpr(expr2,TypeSystem::boolType);
    }
    }

    if(expr1->getType()==TypeSystem::intType && expr2->getType()==TypeSystem::boolType){
        expr2=new ImplicitCastExpr(expr2,TypeSystem::intType);
    }
    if(expr1->getType()==TypeSystem::boolType && expr2->getType()==TypeSystem::intType){
        expr1=new ImplicitCastExpr(expr1,TypeSystem::intType);
    }
   
}

void UnaryExpr::typeCheck(){
    // Todo
    
    expr->typeCheck();

    Type* t=expr->getType();
    if(expr->getType()==TypeSystem::voidType && (op==NOT || op==SUB)){
             fprintf(stderr, "Void type is involved in compution \n");
        }
    if(op==NOT && t->isInt() && ((IntType*)t)->getSize()!=1){
        
        expr=new ImplicitCastExpr(expr,TypeSystem::boolType);
    }
    if(op==SUB && t->isInt() && ((IntType*)t)->getSize()!=32){
        
        expr=new ImplicitCastExpr(expr,TypeSystem::intType);
    }
    if(op==ADD){
        dst=expr->getOperand();
    }

}

void Constant::typeCheck()
{
    // Todo
}

void Id::typeCheck()
{
    // Todo
}

void IfStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    if(((IntType*)(cond->getType()))->getSize()!=1){
        cond=new ImplicitCastExpr(cond,TypeSystem::boolType);
      
    }
   
    thenStmt->typeCheck();
}

void IfElseStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    if(((IntType*)(cond->getType()))->getSize()!=1){
       ExprNode* temp=cond;
        cond=new ImplicitCastExpr(temp,TypeSystem::boolType);
    }
  
    thenStmt->typeCheck();
    elseStmt->typeCheck();
}

void WhileStmt::typeCheck(){
    // Todo

    cond->typeCheck();
    if(((IntType*)(cond->getType()))->getSize()!=1){
       
        cond=new ImplicitCastExpr(cond,TypeSystem::boolType);
    }

    
    stmt->typeCheck();
}

void NullStmt::typeCheck(){
    //
}

void CompoundStmt::typeCheck()
{
    // Todo
    if(stmt)
        stmt->typeCheck();
}

void SeqNode::typeCheck()
{
    // Todo
    if(stmt1)
        stmt1->typeCheck();
    if(stmt2)
        stmt2->typeCheck();
}

void DeclStmt::typeCheck()
{
    // Todo
    //与赋值语句一样，需要类型转换，比如用bool型初始化一个int型，或者用int型初始化一个bool型
    if(p!=nullptr){
        for(auto i:*p){
            if(i.ep){
                if(i.ep->getType()==TypeSystem::boolType){
                    i.ep=new ImplicitCastExpr(i.ep, TypeSystem::intType);
                    
                }
        }
        }
    }

     if(p!=nullptr){
        for(auto i:*p){
            if(i.ep){
                i.ep->typeCheck();
        }
        }
    }

}

void ReturnStmt::typeCheck()
{
    
    // Todo
    //返回类型是否与函数返回类型一致，不一致的是否能够转换
    if(retValue){
    if(retValue->getType()!=TypeSystem::voidType && nowFucntionRetType==TypeSystem::voidType){
            cout<<"Wrong Function! return non-void type"<<endl;
    }
    else if(nowFucntionRetType==TypeSystem::intType && retValue->getType()==TypeSystem::boolType){
        retValue=new ImplicitCastExpr(retValue,TypeSystem::intType);
    }
    retValue->typeCheck();
    }
   
}

void AssignStmt::typeCheck()
{
    // Todo
    //类型转换，比如用bool型赋值一个int型，或者用int型赋值一个bool型
    expr->typeCheck();
    if(lval->getType()==TypeSystem::intType && expr->getType()==TypeSystem::boolType){
        expr=new ImplicitCastExpr(expr,TypeSystem::intType);
    }
    if(lval->getType()==TypeSystem::boolType && expr->getType()==TypeSystem::intType){
        expr=new ImplicitCastExpr(expr,TypeSystem::boolType);
    }
    
}


void ExprStmt::typeCheck(){
    if(ep)
        ep->typeCheck();
}

void CallExpr::typeCheck(){
    //检查是否与函数参数的类型相同
  
   
    int paramNum;
    if(epl==nullptr)
        paramNum=0;
    else
        paramNum=(*epl).size();

  

    SymbolEntry* se=this->getSymPtr();
    
    
    while(se){
        vector<Type*>* pSe=((FunctionType*)(se->getType()))->GetParamsType();
        int size;
        if(pSe==nullptr)
            size=0;
        else    
            size=(int)((*pSe).size());
        if(size==paramNum)
            break;
        se=dynamic_cast<IdentifierSymbolEntry*>(se)->getNext();
    }

    if(se==nullptr){
        cout<<"num of arguments does not match!"<<endl;
    }

    else if(epl){
    vector<Type*>* params=((FunctionType*)(se->getType()))->GetParamsType();
      this->type=((FunctionType*)(se->getType()))->getRetType();
           for(int i=0;i<(int)(*epl).size();i++){
            (*epl)[i]->typeCheck();
            if((*params)[i]==TypeSystem::intType && (*epl)[i]->getType()==TypeSystem::boolType)
                {
                     (*epl)[i]=new ImplicitCastExpr( (*epl)[i],TypeSystem::intType);
                }
           }
    }
}


void ImplicitCastExpr::typeCheck(){
    if(expr)
        expr->typeCheck();
}

//语法树的输出
void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "+";
            break;
        case SUB:
            op_str = "-";
            break;
        case MUL:
            op_str = "*";
            break;
        case DIV:
            op_str="/";
            break;
        case MOD:
            op_str="%";
            break;
        case AND:
            op_str = "&&";
            break;
        case OR:
            op_str = "||";
            break;
        case LESS:
            op_str = "<";
            break;
        case EQ:
            op_str="==";
            break;
        case NE:
            op_str="!=";
            break;
        case GREATER:
            op_str=">";
            break;
        case GOE:
            op_str=">=";
            break;
        case LOE:
            op_str="<=";
            break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void UnaryExpr::output(int level){
      std::string op_str;
      switch(op){
        case ADD:
            op_str='+';
            break;
        case SUB:
            op_str='-';
            break;
        case NOT:
            op_str="!";
            break;
      }
      fprintf(yyout, "%*cUnaryExpr\tprefix: %s\n", level, ' ', op_str.c_str());
      expr->output(level+4);
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void Id::output(int level)
{
    
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void VarDef_entry::output(int level){
    fprintf(yyout, "%*cVarDef\n", level, ' ');
    
    if(id!=nullptr)
    id->output(level+4);
    
    if(ep!=nullptr)
    ep->output(level+4);
}

void DeclStmt::output(int level)
{
    
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    for(auto i:*p){
        
        i.output(level+4);
    }
    
}

void ExprStmt::output(int level){
     
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    ep->output(level+4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}



void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    if(stmt)
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    if(stmt1)
    stmt1->output(level);
    if(stmt2)
    stmt2->output(level);
}

void NullStmt::output(int level){
     fprintf(yyout, "%*cNullStmt\n", level, ' ');
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    if(cond)
    cond->output(level + 4);
    if(thenStmt)
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    if(cond)
    cond->output(level + 4);
    if(thenStmt)
    thenStmt->output(level + 4);
    if(elseStmt)
    elseStmt->output(level + 4);
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    if(lval)
    lval->output(level + 4);
    if(expr)
    expr->output(level + 4);
   
}



void WhileStmt::output(int level){
     fprintf(yyout, "%*cWhileStmt\n", level, ' ');
     if(cond)
     cond->output(level+4);
     if(stmt)
     stmt->output(level+4);
}

void FunctionParam::output(int level){
    fprintf(yyout, "%*cParmVarDecl : \n", level, ' ');
    id->output(level+4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();

    fprintf(yyout, "%*cFunctionDefine function name: %s type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(functionParams!=nullptr){
        for(auto i:*functionParams){
            i.output(level+4);
        }
    }
    stmt->output(level + 4);
}

void CallExpr::output(int level){
    
    std::string name,type;
    name=symbolEntry->toStr();
    type=symbolEntry->getType()->toStr();
    
    fprintf(yyout, "%*cCallExpr \t function: %s type: %s\n", level, ' ',name.c_str(),type.c_str());
    //输出表达式
    
    if(epl!=nullptr){
    for(int i=0;i<(int)epl->size();i++){
        (*epl)[i]->output(level+4);
        }
    }
     
}

void ImplicitCastExpr::output(int level){
   
    if(expr)
    expr->output(level);
}