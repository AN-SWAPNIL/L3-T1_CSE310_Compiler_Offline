#include <bits/stdc++.h>
using namespace std;

class SymbolInfo
{
private:
    string name, type;
    SymbolInfo* next;
    int row, col;
    string tableID;

public:
    SymbolInfo(string name, string type, int row, int col, string tableID)
    {
        this->name = name;
        this->type = type;
        this->row = row;
        this->col = col;
        this->tableID = tableID;
        next = NULL;
    }   

    ~SymbolInfo(){};
    
    string getName() { return name; }
    
    string getType() { return type; }
    
    SymbolInfo* getNext() { return next; }
    
    void setNext(SymbolInfo* next) { this->next = next; }
    
    int getRow() { return row; }
    
    void setRow(int row) { this->row = row; }
    
    int getCol() { return col; }
    
    void setCol(int col) { this->col = col; }

    string getTableID() { return tableID; }

};
