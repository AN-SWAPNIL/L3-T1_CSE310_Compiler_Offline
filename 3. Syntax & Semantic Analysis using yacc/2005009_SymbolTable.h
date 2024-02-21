#include "2005009_ScopeTable.h"
using namespace std;

class SymbolTable
{
private:
    ScopeTable *currentScope;
    int size;

public:
    SymbolTable(int size)
    {
        this->size = size;
        currentScope = new ScopeTable(size, 1);
    }

    ~SymbolTable()
    {
        while(currentScope != NULL){
            ScopeTable *temp = currentScope;
            cout << "\tScopeTable# " << temp->getId() << " deleted" << endl;
            currentScope = currentScope->getParentScope();
            delete temp;
        }
    }

    int getSize() { return size; }

    ScopeTable* getCurrentScope() { return currentScope; }

    void _enter()
    {
        int newID = currentScope->getCounter() + 1;
        int id = currentScope->getId() + (newID);
        ScopeTable *newScope = new ScopeTable(size, id, currentScope);
        currentScope->setCounter(newID);
        currentScope = newScope;
    }

    bool _exit()
    {
        if(currentScope->getParentScope() == NULL)
            return false;
        ScopeTable *temp = currentScope;
        currentScope = currentScope->getParentScope();
        delete temp;
        return true;
    }

    bool _insert(string name, string type, SymbolInfo *pointTo = NULL)
    {
        return currentScope->_insert(name, type, pointTo);
    }

    bool _remove(string name)
    {
        return currentScope->_delete(name);
    }

    SymbolInfo* _lookUp(string name)
    {
        ScopeTable *temp = currentScope;
        while(temp != NULL){
            SymbolInfo *info = temp->_lookUp(name);
            if(info != NULL)
                return info;
            temp = temp->getParentScope();
        }
        return NULL;
    }

    void _printCurrentScope(ofstream &out)
    {
        currentScope->_print(out);
    }

    void _printAllScope(ofstream &out)
    {
        ScopeTable *temp = currentScope;
        while(temp != NULL){
            temp->_print(out);
            temp = temp->getParentScope();
        }
    }

};