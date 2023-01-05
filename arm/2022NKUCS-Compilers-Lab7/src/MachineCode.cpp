#include "MachineCode.h"
#include<vector>
#include<Ast.h>
using namespace std;
extern vector<VarDef_entry> globalVarList;
extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    //操作数要么为寄存器，要么为立即数
    this->type = tp;
    if(tp == MachineOperand::IMM)
        this->val = val;
    else 
        this->reg_no = val;
}

MachineOperand::MachineOperand(std::string label,int type)
{
    //操作数为标签，用于控制流
    this->type = type;
    this->label = label;
    
}





bool MachineOperand::operator==(const MachineOperand&a) const
{
    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand&a) const
{
    if(this->type == a.type)
    {
        if(this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output() 
{
    /* HINT：print operand
    * Example:
    * immediate num 1 -> print #1;
    * register 1 -> print r1;
    * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    //有三类标签：块标签 全局变量地址标签 函数标签
    case LABEL:
        if (this->label.substr(0, 2) == ".L")
            fprintf(yyout, "%s", this->label.c_str());
        else
            fprintf(yyout, "addr_%s", this->label.c_str());
        break;
    case FUNCLABEL:
        fprintf(yyout, "%s", this->label.c_str());
    default:
        break;
    }
}

void MachineInstruction::PrintCond()
{
    // TODO
    
    switch (cond)
    {
    case LT:
        fprintf(yyout, "lt");
        break;
    case GT:
        fprintf(yyout,"gt");
        break;
    case EQ:
        fprintf(yyout,"eq");
        break;
    case NE:
        fprintf(yyout,"ne");
        break;
    case GE:
        fprintf(yyout,"ge");
        break;
    case LE:
        fprintf(yyout,"le");
        break;
    default:
        break;
    }
}

BinaryMInstruction::BinaryMInstruction(
    MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    // TODO: 
    // Complete other instructions
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::DIV:
        fprintf(yyout, "\tsdiv ");
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    default:
        break;
    }
}

LoadMInstruction::LoadMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}

void LoadMInstruction::output()
{
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
    MachineOperand* src1, MachineOperand* src2, MachineOperand* src3, 
    int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = -1;
    this->cond = cond;
    if(src1->isParam()){
        isParam=true;
        ParamNo=src1->getStackParam();
    }
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    if (src3)
        this->use_list.push_back(src3);
    src1->setParent(this);
    src2->setParent(this);
    if (src3)
        src3->setParent(this);
}

void StoreMInstruction::output()
{
    
    // TODO
    fprintf(yyout, "\tstr ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    // store address
    //str src [reg]
    if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
        fprintf(yyout, "[");
    this->use_list[1]->output();
    //str src [reg #num]
    if (this->use_list.size() > 2) {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }
    if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

MovMInstruction::MovMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src,
    int cond)
{
    // TODO
    this->parent=p;
    this->type=MachineInstruction::MOV;
    this->op=op;
    this->cond=cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
    
}

void MovMInstruction::output() 
{
    // TODO
    fprintf(yyout, "\tmov");
    PrintCond();
    fprintf(yyout, " ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, 
    int cond)
{
    // TODO
    this->parent=p;
    this->type=MachineInstruction::BRANCH;
    this->op=op;
    this->cond=cond;
    this->use_list.push_back(dst);
    dst->setParent(this);

}

void BranchMInstruction::output()
{
    // TODO
    switch(op){
        case B:
            fprintf(yyout,"\tb");
            break;
        case BX:
            fprintf(yyout,"\tbx");
            break;
        case BL:
            fprintf(yyout,"\tbl");
            break;
    }
    PrintCond();
    fprintf(yyout, " ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock* p, 
    MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    // TODO
    this->parent=p;
    this->type=MachineInstruction::CMP;
    this->op=-1;
    this->cond=cond;
   
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output()
{
    // TODO
    // Jsut for reg alloca test
    // delete it after test
    fprintf(yyout,"\tcmp ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, 
    MachineOperand* src,
    int cond)
{
    // TODO
    this->parent=p;
    this->type=MachineInstruction::STACK;
    this->op=op;
    this->cond=cond;
    this->use_list.push_back(src);
    src->setParent(this);
}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p,
                                     int op,
                                     std::vector<MachineOperand*> srcs,
                                     MachineOperand* src,
                                     MachineOperand* src1,
                                     int cond) {
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    if (srcs.size()) {
        for (auto it = srcs.begin(); it != srcs.end(); it++) {
            this->use_list.push_back(*it);
        }
    }
    if (src) {
        this->use_list.push_back(src);
        src->setParent(this);
    }
    if (src1) {
        this->use_list.push_back(src1);
        src1->setParent(this);
    }
}

void StackMInstrcuton::output()
{
    // TODO
    if(!this->use_list.empty()){
        switch(op){
            case PUSH:
                fprintf(yyout,"\tpush");
                break;
            case POP:
                fprintf(yyout,"\tpop");
        }
        fprintf(yyout, " {");
        auto size = use_list.size();
        if (size <= 16) {
            this->use_list[0]->output();
            for (int i = 1; i < (int)use_list.size(); i++) {
                fprintf(yyout, ", ");
                this->use_list[i]->output();
            }
        }
        fprintf(yyout, "}\n");
    }
}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p; 
    this->sym_ptr = sym_ptr; 
    this->stack_size = 0;
};

std::vector<MachineOperand*> MachineFunction::getSavedRegs() {
    std::vector<MachineOperand*> regs;
    for (auto it = saved_regs.begin(); it != saved_regs.end(); it++) {
        auto reg = new MachineOperand(MachineOperand::REG, *it);
        regs.push_back(reg);
    }
    return regs;
}

void MachineBlock::output()
{
    
    for(auto iter : inst_list){
        //函数跳转到父函数返回
        //恢复父函数的状态
        //在bx lr前面插入 pop指令
        if(iter->isBX()){
            auto fp = new MachineOperand(MachineOperand::REG,11);
            auto lr = new MachineOperand(MachineOperand::REG,14);
            auto pop_inst=new StackMInstrcuton(this,StackMInstrcuton::POP,parent->getSavedRegs(),fp,lr);
            pop_inst->output();
        }
        if(iter->isStr()){
          if(((StoreMInstruction*)iter)->isParam==true){
            int off= (((int)parent->getSavedRegs().size())+2+((StoreMInstruction*)iter)->ParamNo)*4;
            auto size = new MachineOperand(MachineOperand::IMM, off);
            auto fp = new MachineOperand(MachineOperand::REG, 11);
            auto r3 = new MachineOperand(MachineOperand::REG, 3);
            (new LoadMInstruction(nullptr,r3,fp,size))->output();
          }
        }
        iter->output();
        
    }
}

void MachineFunction::output()
{   
    
    const char *func_name = this->sym_ptr->toStr().c_str();
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);

    // TODO
    /* Hint:
    *  1. Save fp
    *  2. fp = sp
    *  3. Save callee saved register
    *  4. Allocate stack space for local variable */

   
    fprintf(yyout, ".L%d:\n", block_list[0]->getNo());
    auto fp = new MachineOperand(MachineOperand::REG, 11);
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto lr = new MachineOperand(MachineOperand::REG, 14);

    //插入push指令 保存寄存器
   (new StackMInstrcuton(nullptr, StackMInstrcuton::PUSH, getSavedRegs(), fp,lr))->output();
    
    //切换栈帧
    //mov fp,sp
   (new MovMInstruction(nullptr, MovMInstruction::MOV, fp, sp))->output();

    //sub sp sp #num 为局部变量分配栈内空间
    int off = AllocSpace(0);
    if (off > 0) {
        auto size = new MachineOperand(MachineOperand::IMM, off);
        if (off < -255 || off > 255) {
            auto r4 = new MachineOperand(MachineOperand::REG, 4);
            (new LoadMInstruction(nullptr,r4, size))->output();
            (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, r4))->output();
        } else {
            (new BinaryMInstruction(nullptr, BinaryMInstruction::SUB, sp, sp, size))->output();
        }
    }


    block_list[0]->output();
    for(int i=1; i<(int)block_list.size();i++){
        fprintf(yyout, ".L%d:\n", block_list[i]->getNo());
        block_list[i]->output();
    }
   
    
}


void MachineUnit::PrintGlobalDecl()
{
   
    // TODO:
    // You need to print global variable/const declarition code;
     std::vector<int> constIndex;
     if(globalVarList.size()>0){
        fprintf(yyout, "\t.data\n");
        for(int i=0;i<(int)globalVarList.size();i++){
            IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(globalVarList[i].id->getSymPtr());
            if(se->getType()==TypeSystem::constIntType){
                constIndex.push_back(i);
            }
            else{
            fprintf(yyout, "\t.global %s\n", se->toStr().c_str());
            fprintf(yyout, "\t.align 4\n");
            fprintf(yyout, "\t.size %s, %d\n", se->toStr().c_str(), 4);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            fprintf(yyout, "\t.word %d\n", (int)se->getValue());
            }
        }
     }
     if(!constIndex.empty()){
        fprintf(yyout, "\t.section .rodata\n");
        for (auto i : constIndex) {
            IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(globalVarList[i].id->getSymPtr());
            fprintf(yyout, "\t.global %s\n", se->toStr().c_str());
            fprintf(yyout, "\t.align 4\n");
            fprintf(yyout, "\t.size %s, %d\n", se->toStr().c_str(), 4);
            fprintf(yyout, "%s:\n", se->toStr().c_str());
            fprintf(yyout, "\t.word %d\n", (int)se->getValue());
        }
     }
}

void MachineUnit::output()
{
    // TODO
    /* Hint:
    * 1. You need to print global variable/const declarition code;
    * 2. Traverse all the function in func_list to print assembly code;
    * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    fprintf(yyout, "\t.text\n");
    for(auto iter : func_list){
        iter->output();
    }
    for (auto s : globalVarList) {
      IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(s.id->getSymPtr());
        fprintf(yyout, "addr_%s:\n", se->toStr().c_str());
        fprintf(yyout, "\t.word %s\n", se->toStr().c_str());
    }
}

void MachineInstruction::insertBefore(MachineInstruction* inst) {
    auto& inst_list = parent->getInsts();
    auto target = std::find(inst_list.begin(), inst_list.end(), this);
    inst_list.insert(target, inst);
}

void MachineInstruction::insertAfter(MachineInstruction* inst) {
    auto& inst_list = parent->getInsts();
    auto target = std::find(inst_list.begin(), inst_list.end(), this);
    inst_list.insert(++target, inst);
}