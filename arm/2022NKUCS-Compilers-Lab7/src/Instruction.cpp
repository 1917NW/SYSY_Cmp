#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include "Function.h"
#include "Type.h"
extern FILE* yyout;
int nowCmpCond=-1;
Operand* RetRes;
Operand* CmpRes;
Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
   
    switch (opcode)
    {
    case E:
        op = "eq";
        break;
    case NE:
        op = "ne";
        break;
    case L:
        op = "slt";
        break;
    case LE:
        op = "sle";
        break;
    case G:
        op = "sgt";
        break;
    case GE:
        op = "sge";
        break;
    default:
        op = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock*true_branch, BasicBlock*false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if(operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

ZextInstruction::ZextInstruction(Operand* dst,
                                 Operand* src,
                                 BasicBlock* insert_bb)
    : Instruction(ZEXT, insert_bb) {
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

ZextInstruction::~ZextInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void ZextInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];

    
    fprintf(yyout, "  %s = zext %s %s to i32\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str());
}

XorInstruction::XorInstruction(Operand* dst,
                               Operand* src,
                               BasicBlock* insert_bb)
    : Instruction(XOR, insert_bb) {
       
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
    
}

XorInstruction::~XorInstruction() {
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void XorInstruction::output() const {
    Operand* dst = operands[0];
    Operand* src = operands[1];
    fprintf(yyout, "  %s = xor %s %s, true\n", dst->toStr().c_str(),
            src->getType()->toStr().c_str(), src->toStr().c_str());
}

CallInstruction::CallInstruction(Operand* dst,
                                 SymbolEntry* func,
                                 std::vector<Operand*> params,
                                 BasicBlock* insert_bb)
    : Instruction(CALL, insert_bb), func(func), dst(dst) {
    operands.push_back(dst);
    if (dst)
        dst->setDef(this);
    for (auto param : params) {
        operands.push_back(param);
        param->addUse(this);
    }

   
}


void CallInstruction::output() const {
    fprintf(yyout, "  ");
    if (operands[0])
        fprintf(yyout, "%s = ", operands[0]->toStr().c_str());

    FunctionType* type = (FunctionType*)(func->getType());
    fprintf(yyout, "call %s %s(", type->getRetType()->toStr().c_str(),
            func->toStr().c_str());

    for (unsigned int i = 1; i < operands.size(); i++) {
        if (i != 1)
            fprintf(yyout, ", ");
        fprintf(yyout, "%s %s", operands[i]->getType()->toStr().c_str(),
                operands[i]->toStr().c_str());
    }
    fprintf(yyout, ")\n");
}

CallInstruction::~CallInstruction() {
    if (operands[0]) {
        operands[0]->setDef(nullptr);
        if (operands[0]->usersNum() == 0)
            delete operands[0];
    }
    for (long unsigned int i = 1; i < operands.size(); i++)
        operands[i]->removeUse(this);
}

MachineOperand* Instruction::genMachineOperand(Operand* ope)
{
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if(se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
    else if(se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
  
    else if(se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if(id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str());
        else if(id_se->isParam()){
            auto number=id_se->getParamNo();
            if(number<4)
                mope = new MachineOperand(MachineOperand::REG, number);
            else{
                mope = new MachineOperand(MachineOperand::REG, 3);
                mope->setParam();
                mope->setStackParam(number-4);
            }
        }
        else{
            exit(0);    
        }
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
   
    auto cur_func = builder->getFunction();
    
    int offset = cur_func->AllocSpace(4);
    
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if(operands[1]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloc())
    {
        // example: load r1, [fp, #4]
        auto dst = genMachineOperand(operands[0]);
        //src1 == fp
        auto fp = genMachineReg(11);
        auto off = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());
        cur_inst = new LoadMInstruction(cur_block, dst, fp, off);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
   
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    MachineOperand* src;
    //??????str function() dst
    //????????????str r0 dst
    if(operands[1]==RetRes){
        src = genMachineReg(0);
    }
    else
        src = genMachineOperand(operands[1]);

    //store Imm, dst = load dst, Immm
    if(operands[1]->getEntry()->isConstant()){
        auto dst = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block,dst, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*dst);
    }

    //store to local
    //store src,[fp,#num]
    if(operands[0]->getEntry()->isTemporary()&&operands[0]->getDef()&&operands[0]->getDef()->isAlloc()){
        
       
        //fp
        auto dst1 = genMachineReg(11);

        //#num
        auto dst2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());
        cur_inst = new StoreMInstruction(cur_block, src, dst1, dst2);
        cur_block->InsertInst(cur_inst);
       
    }

    //store to global
    else if(operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal()){
        
        //load addr_global_reg, addr_global
        auto dst = genMachineOperand(operands[0]);
        auto addr_global_reg = genMachineVReg();
        cur_inst=new LoadMInstruction(cur_block,addr_global_reg,dst);
        cur_block->InsertInst(cur_inst);

        //store src, [addr_global_reg]
        cur_inst = new StoreMInstruction(cur_block,
                                             src, addr_global_reg);
        cur_block->InsertInst(cur_inst);
    }
    
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO:
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
    * The source operands of ADD instruction in ir code both can be immediate num.
    * However, it's not allowed in assembly code.
    * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
    * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/
    MachineInstruction* cur_inst = nullptr;
    if(src1->isImm())
    {
        //src1????????????
        //??????load reg src1
        auto src1_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, src1_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*src1_reg);
    }

    //?????????2???????????????????????????????????????2??????????????????8??????????????????255????????????-255??????????????????????????????load???????????????????????????
    if(src2->isImm())
    {   
        auto value=((ConstantSymbolEntry*)(operands[2]->getEntry()))->getValue() ;
        // ???????????????????????????????????????????????????load
        if (value>255 || value <-255 ||opcode >= BinaryInstruction::MUL){
            auto src2_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, src2_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*src2_reg);
        }
    }
    
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    case SUB:
         cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
        break;
    case MUL:
         cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
        break;
    case DIV:
         cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        break;
    case MOD:
        //a=b%c?????????
        //t1=b/c
        //t2=t1*c
        //a=b-t2
        {   

            auto t1=genMachineVReg();
            cur_inst=new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, t1, src1, src2);
            cur_block->InsertInst(cur_inst);

            auto t2=genMachineVReg();
            cur_inst=new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, t2, t1, src2);
            cur_block->InsertInst(cur_inst);

            cur_inst=new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, t2);
        }
        break;
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    CmpRes=operands[0];
    auto cur_block = builder->getBlock();
    MachineOperand* src1;
    MachineOperand* src2;
    //?????????cmp function() src2
    //??????cmp src1 function()
    //??????cmp function() function() 
    if(operands[1]==RetRes)
        src1=genMachineReg(0);
    else
        src1 = genMachineOperand(operands[1]);

    if(operands[2]==RetRes)
        src2=genMachineReg(0);
    else
        src2 = genMachineOperand(operands[2]);

    MachineInstruction* cur_inst = nullptr;
    //cmp??????????????????????????????
    if(src1->isImm())
    {
        auto src1_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, src1_reg, src1);
        cur_block->InsertInst(cur_inst);
        //copy_constructor
        src1 = new MachineOperand(*src1_reg);
    }
    //cmp???????????????????????????????????????????????????8????????????255?????????-255???????????????load??????
    auto value=((ConstantSymbolEntry*)(operands[2]->getEntry()))->getValue();
    if (src2->isImm() && (value >255 || value <-255) ) {
        auto src2_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, src2_reg, src2);
        cur_block->InsertInst(cur_inst);
        src2 = new MachineOperand(*src2_reg);
    }
        //???????????????????????????
        nowCmpCond=opcode;
        
        //cmp src1,src2
        cur_inst = new CmpMInstruction(cur_block, src1, src2, opcode);
        cur_block->InsertInst(cur_inst);

}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    //b .Lb
    auto cur_block = builder->getBlock();
    MachineOperand* dst = genMachineLabel(branch->getNo());
    auto cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
     auto cur_block = builder->getBlock();
    
    //bcond truebranch
    MachineOperand* dst =  genMachineLabel(true_branch->getNo());
    auto cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst, nowCmpCond);
    cur_block->InsertInst(cur_inst);
    
    //b falsebranch
    dst = genMachineLabel(false_branch->getNo());
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::B, dst);
    cur_block->InsertInst(cur_inst);
}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    /* HINT:
    * 1. Generate mov instruction to save return value in r0
    * 2. Restore callee saved registers and sp, fp
    * 3. Generate bx instruction */

    auto cur_block = builder->getBlock();
   //??????????????????
   if(operands.size()>0){
        MachineOperand* dst;
        MachineOperand* src;
        MachineInstruction* cur_inst;
        //dst r0
        dst = new MachineOperand(MachineOperand::REG, 0);
        
        src = genMachineOperand(operands[0]);
        if (src->isImm()) {
            auto value = ((ConstantSymbolEntry*)(operands[0]->getEntry()))->getValue();
            //ldr r0,Imm(>255 <-255)
            if (value > 255 || value <= -255) {
            auto r0 = new MachineOperand(MachineOperand::REG, 0);
            cur_block->InsertInst(new LoadMInstruction(cur_block, r0, src));
            }else{
                //mov r0 src
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
                cur_block->InsertInst(cur_inst);
            }
            
        }
        //mov r0,r1
        //????????? ret fucntion();
        //???????????????mov???????????????function?????????????????????r0???
        else{
            if(operands[0]!=RetRes){
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
                cur_block->InsertInst(cur_inst);
            }
       
        }
   }

    //add sp,sp,#num
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto cur_func = builder->getFunction();
    auto size =
        new MachineOperand(MachineOperand::IMM, cur_func->AllocSpace(0));
    
   
    auto add_sp_MI = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, size);
    cur_block->InsertInst(add_sp_MI);
    
    //bx lr
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    auto bx_MI = new BranchMInstruction(cur_block, BranchMInstruction::BX, lr);
    cur_block->InsertInst(bx_MI);
}


void ZextInstruction::genMachineCode(AsmBuilder* builder){
    //TODO
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    
    MachineOperand* src;
    MachineInstruction* cur_inst;
    //??????????????????cmp???????????????
    //movcond reg 1
    //movrecond reg 0
    if(operands[1]==CmpRes){
        src=genMachineImm(1);
        cur_inst =
        new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src,nowCmpCond);
        cur_block->InsertInst(cur_inst);
        //???nowCmpCond????????????
        if(nowCmpCond==CmpInstruction::E)
            nowCmpCond=CmpInstruction::NE;
        else if(nowCmpCond==CmpInstruction::NE)
            nowCmpCond=CmpInstruction::E;
        if(nowCmpCond==CmpInstruction::L)
            nowCmpCond=CmpInstruction::GE;
        else if(nowCmpCond==CmpInstruction::GE)
            nowCmpCond=CmpInstruction::L;
        if(nowCmpCond==CmpInstruction::G)
            nowCmpCond=CmpInstruction::LE;
        else if(nowCmpCond==CmpInstruction::LE)
            nowCmpCond=CmpInstruction::G;
        src=genMachineImm(0);
        cur_inst =new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src,nowCmpCond);
        cur_block->InsertInst(cur_inst);
    }
    else{
    //mov reg1 reg2
     src = genMachineOperand(operands[1]);
     cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
     cur_block->InsertInst(cur_inst);
    }
}

void XorInstruction::genMachineCode(AsmBuilder* builder){
    //TODO
    //???src??????????????????
    //cmp src 0 
    //moveq dst 1 
    //movne dst 0
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    if(operands[1]!=CmpRes){
        MachineOperand*  src = genMachineOperand(operands[1]);

    auto zero= genMachineImm(0);
    
    auto cmp_inst=new CmpMInstruction(cur_block, src, zero, CmpMInstruction::NE);
    cur_block->InsertInst(cmp_inst);                
    }
    auto trueRes = genMachineImm(1);
    auto falseRes = genMachineImm(0);
    auto cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, trueRes, MachineInstruction::EQ);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, falseRes, MachineInstruction::NE);
    cur_block->InsertInst(cur_inst);

}

void CallInstruction::genMachineCode(AsmBuilder* builder){
    //TODO
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst;

    //??????????????????r0 r1 r2 r3
    int regNo=0;
    MachineOperand* src;
    int i=1;
    bool morethan4 = ((int)operands.size()>5);
    if(!morethan4){
    for( ;i<(int)operands.size();i++){
        auto dst=genMachineReg(regNo++);
        src=genMachineOperand(operands[i]);
        if(src->isImm()){
            cur_inst = new LoadMInstruction(cur_block, dst, src);
        }
        else{
            if(operands[i]==RetRes)
                src=genMachineReg(0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
        }
        cur_block->InsertInst(cur_inst);
    }
    }
    else{
    for( ;i<5;i++){
        auto dst=genMachineReg(regNo++);
        src=genMachineOperand(operands[i]);
        if(src->isImm()){
            cur_inst = new LoadMInstruction(cur_block, dst, src);
        }
        else{
            if(operands[i]==RetRes)
                src=genMachineReg(0);
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
        }
        cur_block->InsertInst(cur_inst);
    }
    //push ??????
    int push_stack_arg=0;
    int count=i;
    for(i=((int)operands.size()-1);i>=count;i--){
            push_stack_arg++;
            auto dst = genMachineVReg();
            src = genMachineOperand(operands[i]);
            if(src->isImm()){
                cur_inst = new LoadMInstruction(cur_block, dst, src);
                src=new MachineOperand(*dst);
            }
            else{
                if(operands[i]==RetRes)
                    src=genMachineReg(0);
                cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src);
                src=new MachineOperand(*dst);
            }
            
            cur_block->InsertInst(cur_inst);
            cur_inst=(new StackMInstrcuton(cur_block, StackMInstrcuton::PUSH,  src));
            cur_block->InsertInst(cur_inst);
        }
    //pop ??????
    MachineOperand* sp = genMachineReg(13);
    int off = push_stack_arg * 4;
    auto size = new MachineOperand(MachineOperand::IMM, off);
    cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, size);
    cur_block->InsertInst(cur_inst);
    }

    

    //bx func_name
    auto func_label = new MachineOperand(func->toStr(),MachineOperand::FUNCLABEL);
    cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BL, func_label);
    cur_block->InsertInst(cur_inst);
    RetRes=operands[0];

}