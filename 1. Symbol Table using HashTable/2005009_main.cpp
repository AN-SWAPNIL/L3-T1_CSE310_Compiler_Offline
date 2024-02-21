#include "2005009_SymbolTable.cpp"
using namespace std;


int main(int argc,char *argv[]){
	FILE *fp;

    if(argc<2){
        printf("Please provide input file.\n");
        exit(1);
    }

	if((fp=fopen(argv[1],"r"))==NULL){
		printf("Cannot Open Input File.\n");
		exit(1);
	}
    
    fclose(fp);

    freopen("argv[1]", "r", stdin);
    freopen("output.txt", "w", stdout);

    int size, counter = 0;
    
    string line, command[3];
    getline(cin, line);
    stringstream sin(line);
    sin >> size;

    SymbolTable st(size);
    cout << "\tScopeTable# 1 created" << endl;

    while (!cin.eof())
    {
        int count = 0;
        getline(cin, line);
        stringstream sin(line);

        cout << "Cmd " << ++counter << ": " << line << endl;     
        while(sin >> line) {
            if(count < 3) {
                command[count] = line;
            }
            count++;
        }

        if(command[0] == "I") {
            if(count!=3) {
                cout << "\tWrong number of arugments for the command I" << endl;
                continue;
            }
            string name = command[1], type = command[2];
            bool result = st._insert(name, type);
            SymbolInfo *info = st._lookUp(name);
            if(result)
                cout << "\tInserted  at position <" << info->getRow() << ", " << info->getCol() <<"> of ScopeTable# " << info->getTableID() << endl;
            else
                cout << "\t\'" << name << "\' already exists in the current ScopeTable# " << info->getTableID() << endl;
        }
        else if(command[0] == "L") {
            if(count!=2) {
                cout << "\tWrong number of arugments for the command L" << endl;
                continue;
            }
            string name = command[1];
            SymbolInfo *info = st._lookUp(name);
            if(info != NULL)
                cout << "\t\'" << name << "\' found at position <" << info->getRow() << ", " << info->getCol() << "> of ScopeTable# " << info->getTableID() << endl;
            else
                cout << "\t\'" << name << "\' not found in any of the ScopeTables" << endl;
        }
        else if(command[0] == "D") {
            if(count!=2) {
                cout << "\tWrong number of arugments for the command D" << endl;
                continue;
            }
            string name = command[1];
            SymbolInfo *info = st._lookUp(name);
            bool result = st._remove(name);
            if(result)
                cout << "\tDeleted \'" << name << "\' from position <" << info->getRow() << ", " << info->getCol() << "> of ScopeTable# " << info->getTableID() << endl;
            else
                cout << "\tNot found in the current ScopeTable# " << st.getCurrentScope()->getId() << endl;
        }
        else if(command[0] == "P") {
            if(count!=2) {
                cout << "\tWrong number of arugments for the command P" << endl;
                continue;
            }
            if(command[1] == "A")
                st._printAllScope();
            else if(command[1] == "C")
                st._printCurrentScope();
            else
                cout << "\tInvalid argument for the command P" << endl;
        }
        else if(command[0] == "S") {
            if(count!=1) {
                cout << "\tWrong number of arugments for the command S" << endl;
                continue;
            }
            st._enter();
            cout << "\tScopeTable# " << st.getCurrentScope()->getId() << " created" << endl;
        }
        else if(command[0] == "E") {
            if(count!=1) {
                cout << "\tWrong number of arugments for the command E" << endl;
                continue;
            }
            string id = st.getCurrentScope()->getId();
            bool result = st._exit();
            if(!result)
                cout << "\tScopeTable# " << id << " cannot be deleted" << endl;
            else
                cout << "\tScopeTable# " << id << " deleted" << endl;
        }
        else if(command[0] == "Q") {
            if(count!=1) {
                cout << "\tWrong number of arugments for the command Q" << endl;
                continue;
            }
            return 0;
        }
        else {
            cout << "\tInvalid command" << endl;
        } 
    }
    return 0;
}