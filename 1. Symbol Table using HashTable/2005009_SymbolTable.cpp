#include "2005009_ScopeTable.cpp"
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
        currentScope = new ScopeTable(size, "1");
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
        string id = currentScope->getId() + "." + to_string(newID);
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

    bool _insert(string name, string type)
    {
        return currentScope->_insert(name, type);
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

    void _printCurrentScope()
    {
        currentScope->_print();
    }

    void _printAllScope()
    {
        ScopeTable *temp = currentScope;
        while(temp != NULL){
            temp->_print();
            temp = temp->getParentScope();
        }
    }

};