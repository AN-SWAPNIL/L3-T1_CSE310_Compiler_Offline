#include <bits/stdc++.h>
using namespace std;

enum SymbolType
{
    FUNCTION_DEFINITION,
    FUNCTION_DECLARATION,
    ARRAY,
    NONE
};

enum TypeSpecifier
{
    _INT,
    _FLOAT,
    _CHAR,
    _VOID,
    _ERROR,
    _NONE
};

struct Jump {
    int Next;
    int True;
    int False;
};

class SymbolInfo
{
private:
    string name, type;
    SymbolInfo* next = NULL;
    SymbolInfo* pointTo = NULL;
    int row, col, arraySize=0;
    string tableID;
    TypeSpecifier typeSpecifier = TypeSpecifier::_NONE;
    vector<SymbolInfo*> children;
    vector<SymbolInfo*> parameters;
    vector<SymbolInfo*> declarations;
    vector<SymbolInfo*> arrayList;
    SymbolType symbolType = SymbolType::NONE;
    int offset = 0;
    string asmCode = "";
    Jump jmp = {-1,-1,-1};
    bool isCondition = false;
    int scopeID = 0;

public:
    bool isInScope = false;
    
    SymbolInfo(string name, string type, int row, int col, string tableID="", SymbolInfo* pointTo=NULL)
    {
        this->name = name;
        this->type = type;
        this->row = row;
        this->col = col;
        this->tableID = tableID;
        this->pointTo = pointTo;
        next = NULL;
    }

    ~SymbolInfo(){};

    void setPointTo(SymbolInfo* pointTo) { this->pointTo = pointTo; }

    SymbolInfo* getPointTo() { return pointTo; }

    void setTypeSpecifier(TypeSpecifier type) { this->typeSpecifier = type; }

    TypeSpecifier getTypeSpecifier() { return typeSpecifier; }

    void setParameters(vector<SymbolInfo*> parameters) { this->parameters = parameters; }

    vector<SymbolInfo*> getParameters() { return parameters; }

    void addParameter(SymbolInfo* parameter) { parameters.push_back(parameter); }

    void setDeclarations(vector<SymbolInfo*> declarations) { this->declarations = declarations; }

    vector<SymbolInfo*> getDeclarations() { return declarations; }

    void addDeclaration(SymbolInfo* declaration) { declarations.push_back(declaration); }

    void setArrayList(vector<SymbolInfo*> arrayList) { this->arrayList = arrayList; }

    vector<SymbolInfo*> getArrayList() { return arrayList; }

    void addArray(SymbolInfo* arrayEl) { arrayList.push_back(arrayEl); }

    void setSymbolType(SymbolType symbolType) { this->symbolType = symbolType; }

    SymbolType getSymbolType() { return symbolType; }

    void setArraySize(int arraySize) { this->arraySize = arraySize; }

    int getArraySize() { return arraySize; }

    string getRule() { return tableID; }

    SymbolInfo* addChild(SymbolInfo* symbolInfo) { children.push_back(symbolInfo); return this; }

    void setChildren(vector<SymbolInfo*> children) { this->children = children; }

    vector<SymbolInfo*> getChildren() { return children; }

    int getStartLine() { return row; }

    int getEndLine() { return col; }
    
    string getName() { return name; }
    
    string getType() { return type; }
    
    SymbolInfo* getNext() { return next; }
    
    void setNext(SymbolInfo* next) { this->next = next; }
    
    int getRow() { return row; }
    
    void setRow(int row) { this->row = row; }
    
    int getCol() { return col; }
    
    void setCol(int col) { this->col = col; }

    string getTableID() { return tableID; }

    void setScopeID(int scID) { scopeID = scID; }

    int getScopeID() { return scopeID; }

    void setOffset(int offset) { this->offset = offset; }

    int getOffset() { return offset; }

    void setJump(int Next, int True, int False) {
        if(True == -1) True = jmp.True;
        if(False == -1) False = jmp.False;
        if(Next == -1) Next = jmp.Next;
        jmp = {Next, True, False}; 
    }

    Jump getJump() { return jmp; }

    void setIsCondition(bool isCondition) { this->isCondition = isCondition; }

    bool getIsCondition() { return isCondition; }

    void printTree(ofstream &out, int depth)
    {
        // cout << depth << endl;
        for (int i = 0; i < depth; i++)
            out << " ";
        
        string sline = std::to_string(this->getStartLine());
        string eline = std::to_string(this->getEndLine());
        if(this->getChildren().size())
            out << this->getType() << " : " << this->getRule() + "\t<Line: " + sline + "-" + eline + ">" << endl;
        else
            out << this->getType() << " : " << this->getRule() + "\t<Line: " + sline + ">" << endl;

        for (SymbolInfo* child : this->getChildren())
            child->printTree(out, depth + 1);
    }

};
