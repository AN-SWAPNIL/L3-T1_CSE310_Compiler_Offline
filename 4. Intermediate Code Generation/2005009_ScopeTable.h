#include "2005009_SymbolInfo.h"
using namespace std;

class ScopeTable
{
private:
    ScopeTable *parentScope;
    int counter, size, infoCount;
    int id;
    SymbolInfo **table;
    int tableOffset = 0;

    unsigned long hashFunction(string key)
    {
        unsigned long long hash = 0;

        for(int i = 0; i < key.length(); i++){
            hash = (key[i] + (hash << 6) + (hash << 16) - hash);
        }

        return hash % size;
    }

public:
    ScopeTable(int size, int id, ScopeTable *parentScope = NULL)
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
        // for(int i = 0; i < size; i++){
        //     SymbolInfo *temp = table[i];
        //     while(temp != NULL){
        //         SymbolInfo *temp2 = temp;
        //         temp = temp->getNext();
        //         delete temp2;
        //     }
        // }
        delete[] table;
    }

    ScopeTable* getParentScope() { return parentScope; }

    void setParentScope(ScopeTable *parentScope) { this->parentScope = parentScope; }

    int getId() { return id; }

    int getCounter() { return counter; }

    void setCounter(int counter) { this->counter = counter; }

    int getInfoCount() { return infoCount; }

    bool _insert(string name, string type, SymbolInfo *pointTo)
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

        pointTo->setScopeID(id);
        SymbolInfo *newSymbol = new SymbolInfo(name, type, index+1, tcount, to_string(id), pointTo);
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

    vector<SymbolInfo*> getVariableList() {
        vector<SymbolInfo*> list;
        for(int i = 0; i < size; i++){
            SymbolInfo *temp = table[i];
            while(temp != NULL) {
                SymbolInfo* var = temp->getPointTo(); 
                if(var->getSymbolType() == SymbolType::NONE || var->getSymbolType() == SymbolType::ARRAY)
                    list.push_back(var);
                temp = temp->getNext();
            }
        }
        return list;
    }

    void setTableOffset(int tableOffset) { this->tableOffset = tableOffset; }

    int getTableOffset() { return tableOffset; }

    void _print(ofstream &out)
    {
        out << '\t' << "ScopeTable# " << id << endl;
        for(int i = 0; i < size; i++){
            SymbolInfo *temp = table[i];
            if(temp == NULL)
                continue;
            out << '\t' << i+1;
            if(temp->getPointTo()!=NULL) out << "--> ";
            while(temp != NULL){
                if(temp->getPointTo() == NULL)
                    out << " --> (" << temp->getName() << "," << temp->getType() << ")";
                else {
                    SymbolInfo *pointTo = temp->getPointTo();
                    out << "<" << pointTo->getName() << ",";
                    if(pointTo->getSymbolType() == SymbolType::FUNCTION_DECLARATION || pointTo->getSymbolType() == SymbolType::FUNCTION_DEFINITION)
                        out << "FUNCTION,";
                    if(pointTo->getSymbolType() == SymbolType::ARRAY)
                        out << "ARRAY,";
                    if(pointTo->getTypeSpecifier() == TypeSpecifier::_INT)
                        out << "INT";
                    else if(pointTo->getTypeSpecifier() == TypeSpecifier::_FLOAT)
                        out << "FLOAT";
                    else if(pointTo->getTypeSpecifier() == TypeSpecifier::_VOID)
                        out << "VOID";
                    else if(pointTo->getTypeSpecifier() == TypeSpecifier::_CHAR)
                        out << "CHAR";
                    else if(pointTo->getTypeSpecifier() == TypeSpecifier::_ERROR)
                        out << "ERROR";
                    else
                        out << "NAN";
                    out << "> ";
                }
                temp = temp->getNext();
            }
            out << endl;
        }
    }

};