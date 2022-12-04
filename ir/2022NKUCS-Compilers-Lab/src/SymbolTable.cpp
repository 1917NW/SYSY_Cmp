#include "SymbolTable.h"
#include <iostream>
#include <sstream>

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value = value;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << value;
    return buffer.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope,int kind) : SymbolEntry(type, kind), name(name)
{
    this->scope = scope;
    addr = nullptr;
}
std::string IdentifierSymbolEntry::toStr()
{
    return "@" + name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    // Todo
    SymbolTable * temp=identifiers;
    while(temp!=nullptr){
    if(temp->symbolTable.find(name)!=temp->symbolTable.end()){
        return temp->symbolTable[name];
    }
    temp=temp->prev;
    }
    return nullptr;
}

SymbolEntry* SymbolTable::lookup_now(std::string name)
{
    // Todo
    SymbolTable * temp=identifiers;
    if(temp!=nullptr){
    if(temp->symbolTable.find(name)!=temp->symbolTable.end()){
        return temp->symbolTable[name];
        }
    }
    return nullptr;
}

SymbolEntry* SymbolTable::lookup_top(std::string name)
{
    // Todo
    SymbolTable * top=identifiers;
    while(top->prev!=nullptr){
        top=top->prev;
    }

    if(top->symbolTable.find(name)!=top->symbolTable.end()){
        return top->symbolTable[name];
    }

    return nullptr;
}

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
static SymbolTable sysyTable2;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;

SymbolTable *sysyTable = &sysyTable2;