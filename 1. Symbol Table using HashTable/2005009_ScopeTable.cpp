#include "2005009_SymbolInfo.cpp"
using namespace std;

class ScopeTable
{
private:
    ScopeTable *parentScope;
    int counter, size, infoCount;
    string id;
    SymbolInfo **table;

    unsigned long hashFunction(string key)
    {
        unsigned long long hash = 0;

        for(int i = 0; i < key.length(); i++){
            hash = (key[i] + (hash << 6) + (hash << 16) - hash) % size;
        }

        return hash % size;
    }

public:
    ScopeTable(int size, string id, ScopeTable *parentScope = NULL)
    {
        this->size = size;
        this->id = id;
        this->parentScope = parentScope;
        counter = infoCount = 0;
        table = new SymbolInfo*[size];
        for(int i = 0; i < size; i++)
            table[i] = NULL;
    }

    ~ScopeTable()
    {
        for(int i = 0; i < size; i++){
            SymbolInfo *temp = table[i];
            while(temp != NULL){
                SymbolInfo *temp2 = temp;
                temp = temp->getNext();
                delete temp2;
            }
        }
        delete[] table;
    }

    ScopeTable* getParentScope() { return parentScope; }

    void setParentScope(ScopeTable *parentScope) { this->parentScope = parentScope; }

    string getId() { return id; }

    int getCounter() { return counter; }

    void setCounter(int counter) { this->counter = counter; }

    int getInfoCount() { return infoCount; }

    bool _insert(string name, string type)
    {
        int index = hashFunction(name), tcount = 1;

        SymbolInfo *temp = table[index];
        SymbolInfo *prev = NULL;
        while(temp != NULL){
            if(temp->getName() == name)
                return false;
            prev = temp;
            temp = temp->getNext();
            tcount++;   
        }

        SymbolInfo *newSymbol = new SymbolInfo(name, type, index+1, tcount, id);
        if(prev==NULL)
            table[index] = newSymbol;
        else 
            prev->setNext(newSymbol);
        infoCount++;
        return true;
    }

    SymbolInfo* _lookUp(string name)
    {
        int index = hashFunction(name);

        SymbolInfo *temp = table[index];
        while(temp != NULL){
            if(temp->getName() == name)
                return temp;
            temp = temp->getNext();
        }

        return NULL;
    }

    bool _delete(string name)
    {
        int index = hashFunction(name);

        SymbolInfo *temp = table[index];
        SymbolInfo *prev = NULL;
        while(temp != NULL){
            if(temp->getName() == name){
                SymbolInfo *temp2 = temp->getNext();
                if(prev == NULL)
                    table[index] = temp2;
                else
                    prev->setNext(temp2);
                delete temp;
                while (temp2 != NULL)
                {
                    temp2->setCol(temp2->getCol() - 1);
                    temp2 = temp2->getNext();
                }
                infoCount--;
                return true;
            }
            prev = temp;
            temp = temp->getNext();
        }

        return false;
    }

    void _print()
    {
        cout << '\t' << "ScopeTable# " << id << endl;
        for(int i = 0; i < size; i++){
            cout << '\t' << i+1;
            SymbolInfo *temp = table[i];
            while(temp != NULL){
                cout << " --> (" << temp->getName() << "," << temp->getType() << ")";
                temp = temp->getNext();
            }
            cout << endl;
        }
    }

};