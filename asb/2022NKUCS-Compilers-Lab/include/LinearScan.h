/**
 * linear scan register allocation
 */

#ifndef _LINEARSCAN_H__
#define _LINEARSCAN_H__
#include <set>
#include <map>
#include <vector>
#include <list>

class MachineUnit;
class MachineOperand;
class MachineFunction;


class LinearScan
{
private:
    struct Interval
    {
        int start;
        int end;
        bool spill; // whether this vreg should be spilled to memory
        int disp;   // displacement in stack
        int rreg;   // the real register mapped from virtual register if the vreg is not spilled to memory
        std::set<MachineOperand *> defs;
        std::set<MachineOperand *> uses;

        bool fpu;
    };
    MachineUnit *unit;
    MachineFunction *func;
    std::vector<int> regs;
    std::map<MachineOperand *, std::set<MachineOperand *>> du_chains;

    //未分配寄存器的虚拟寄存器对应的活跃区间的集合
    std::vector<Interval*> intervals;

    //已分配寄存器的虚拟寄存器对应活跃区间的集合
    std::vector<Interval*> active;
    static bool compareStart(Interval*a, Interval*b);
    static bool compareEnd(Interval* a, Interval* b);
    void expireOldIntervals(Interval *interval);
    void spillAtInterval(Interval *interval);
    void makeDuChains();
    void computeLiveIntervals();
    bool linearScanRegisterAllocation();
    void modifyCode();
    void genSpillCode();
    
public:
    LinearScan(MachineUnit *unit);
    void allocateRegisters();
};

#endif