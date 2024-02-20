// #include "2005009_SymbolTable.h"
// using namespace std;

ofstream asmout;
ofstream optout;
SymbolTable* symTable;

int clabel = 1;
int offset = 0;
bool isPrint = false;
string currentFunc = "";

void generate(SymbolInfo *root);

void start_program(SymbolInfo *root) {
    // cout << "start_program start" << endl;
    asmout << ";..........START..........\n\n";
    SymbolInfo* program = NULL;
    if(root->getChildren().size() == 1) {
        program = root->getChildren()[0];
    }
    asmout<<".MODEL SMALL\n.STACK 1000H\n\n.DATA\n\tnumber DB \"00000$\"\n";
    for(SymbolInfo* var: symTable->getCurrentScope()->getVariableList()) {
        if(var->getSymbolType() == SymbolType::ARRAY)
            asmout<<'\t'<<var->getName()<<" DW "<<var->getArraySize()<<" DUP (0000H)\n";
        else
            asmout<<'\t'<<var->getName()<<" DW 1 DUP (0000H)\n";
    }
    asmout<<"\n.CODE\n";
    generate(program);
    if(isPrint) {
        asmout << ";..........LIBRARY_FUNCTIONS..........\n\n";
        asmout << ";..........new_line..........\n\n";
        asmout << "new_line proc\n\tpush ax\n\tpush dx\n\tmov ah,2\n\tmov dl,0Dh\n\tint 21h\n\tmov ah,2\n\tmov dl,0Ah\n\tint 21h\n\tpop dx\n\tpop ax\n\tret\n\tnew_line endp\n";
        asmout << ";..........print_output..........\n\n";
        asmout << "print_output proc  ;print what is in ax\n\tpush ax\n\tpush bx\n\tpush cx\n\tpush dx\n\tpush si\n\tlea si,number\n\tmov bx,10\n\tadd si,4\n\tcmp ax,0\n\tjnge negate\n\tprint:\n\txor dx,dx\n\tdiv bx\n\tmov [si],dl\n\tadd [si],'0'\n\tdec si\n\tcmp ax,0\n\tjne print\n\tinc si\n\tlea dx,si\n\tmov ah,9\n\tint 21h\n\tpop si\n\tpop dx\n\tpop cx\n\tpop bx\n\tpop ax\n\tret\n\tnegate:\n\tpush ax\n\tmov ah,2\n\tmov dl,'-'\n\tint 21h\n\tpop ax\n\tneg ax\n\tjmp print\n\tprint_output endp\n";
    }
    asmout<<"END main\n";
    asmout << ";..........END..........\n\n";
    // cout << "start_program end" << endl;
}

void func_definition(SymbolInfo *root) {
    // cout << "func_definition start" << endl;
    SymbolInfo *funcName = root->getChildren()[1];
    SymbolInfo *parameterList = NULL;
    SymbolInfo *compoundStatement = NULL;
    currentFunc = funcName->getName();
    if(root->getChildren().size() == 6) {
        parameterList = root->getChildren()[3];
        generate(parameterList);
        compoundStatement = root->getChildren()[5];
    }
    else if(root->getChildren().size() == 5) {
        compoundStatement = root->getChildren()[4];
    }
    if(currentFunc=="main") 
        asmout << ";..........MAIN..........\n\n";
    asmout<<currentFunc<<" PROC\n";
    if(currentFunc=="main")
        asmout<<"\tMOV AX, @DATA\n\tMOV DS, AX\n";

    asmout<<"\tPUSH BP\n\tMOV BP, SP\n";
    generate(compoundStatement);
    asmout<<currentFunc<<"_L:\n";
    if(currentFunc=="main") {
        asmout<<"\tMOV AX, 4CH\n\tINT 21H\n";
    }
    asmout<<"\tADD SP, "<< -compoundStatement->getOffset() <<"\n\tPOP BP\n";
    if(currentFunc!="main") asmout << "\tRET\n";
    asmout<<currentFunc<<" ENDP\n\n";
    // cout << "func_definition end" << endl;
}

void var_declaration(SymbolInfo* root) {
    // cout << "var_declaration start" << endl;
    SymbolInfo* declaration_list = root->getChildren()[1];
    generate(declaration_list);

    for(SymbolInfo* declaration : declaration_list->getDeclarations()) {
        if(root->getScopeID()==1) continue;
        if(declaration->getSymbolType() == SymbolType::ARRAY)
            asmout<<"\tSUB SP, " << 2*declaration->getArraySize()<<"\n";
        else
            asmout<<"\tSUB SP, 2\n";
    }
    // cout << "var_declaration end" << endl;
}

void compound_statement(SymbolInfo* root) {
    // cout << "compound_statement start" << endl;
    if(root->getJump().Next == -1) {
        root->setJump(clabel++, -1, -1);
    }
    SymbolInfo* statements = root->getChildren()[1];
    statements->setJump(root->getJump().Next, -1, -1);
    generate(statements);
    // cout << "compound_statement end" << endl;
}

void statements(SymbolInfo* root) {
    // cout << "statements start" << endl;
    SymbolInfo* statements = NULL;
    SymbolInfo* statement = NULL;
    if(root->getChildren().size()==2) {
        statements = root->getChildren()[0];
        statement = root->getChildren()[1];
        statements->setJump(clabel++, -1, -1);
        generate(statements);
    } else
        statement = root->getChildren()[0];
    statement->setJump(root->getJump().Next, -1, -1);
    generate(statement);
    asmout << "L" << root->getJump().Next << ":\n";
    // cout << "statements end" << endl;
}

void statement(SymbolInfo* root) {
    // cout << "statement start" << endl;
    std::string rule = root->getRule();
    std::istringstream iss(rule);
    std::string first_word;
    iss >> first_word;
    if(root->getChildren().size() == 1)
        generate(root->getChildren()[0]);
    else if(root->getChildren().size() == 3) {
        SymbolInfo* expression = root->getChildren()[1];
        generate(expression);
        asmout<<"\tJMP "<<currentFunc<<"_L\n";
    } else if(root->getChildren().size() == 7&&first_word == "FOR") {
        generate(root->getChildren()[2]);
        int loop = clabel++;
        SymbolInfo* expression_statement = root->getChildren()[3];
        SymbolInfo* statement = root->getChildren()[6];
        asmout<<"L"<<loop<<":\n";
        expression_statement->setIsCondition(true);
        expression_statement->setJump(-1, clabel++, root->getJump().Next);
        statement->setJump(clabel++, -1, -1);
        generate(expression_statement);
        asmout<<"L"<<expression_statement->getJump().True<<":\n";
        generate(statement);
        asmout<<"\tADD SP, "<< -root->getOffset()<<"\n";
        generate(root->getChildren()[4]);
        asmout<<"\tJMP L"<<loop<<"\n"; 
    } else if(root->getChildren().size() == 5&&first_word == "IF") {
        SymbolInfo* expression = root->getChildren()[2];
        SymbolInfo* statement = root->getChildren()[4];
        expression->setIsCondition(true);
        expression->setJump(-1, clabel++, root->getJump().Next);
        statement->setJump(root->getJump().Next, -1, -1);
        generate(expression);
        asmout<<"L"<<expression->getJump().True<<":\n";
        generate(statement);
        asmout<<"\tADD SP, "<< -root->getOffset()<<"\n";
    } else if(root->getChildren().size() == 7&&first_word == "IF") {
        SymbolInfo* expression = root->getChildren()[2];
        SymbolInfo* statement1 = root->getChildren()[4];
        SymbolInfo* statement2 = root->getChildren()[6];
        expression->setIsCondition(true);
        expression->setJump(-1, clabel++, clabel++);
        statement1->setJump(expression->getJump().False, -1, -1);
        statement2->setJump(root->getJump().Next, -1, -1);
        generate(expression);
        asmout<<"L"<<expression->getJump().True<<":\n";
        generate(statement1);
        asmout<<"\tJMP L"<<root->getJump().Next<<"\n";
        asmout<<"L"<<expression->getJump().False<<":\n";
        generate(statement2);
        asmout << "\tADD SP, "<< -root->getOffset() << "\n";
    } else if(root->getChildren().size() == 5&&first_word == "WHILE") {
        int loop = clabel++;
        SymbolInfo* expression = root->getChildren()[2];
        SymbolInfo* statement = root->getChildren()[4];
        asmout<<"L"<<loop<<":\n";
        expression->setIsCondition(true);
        expression->setJump(-1, clabel++, root->getJump().Next);
        statement->setJump(root->getJump().Next, -1, -1);
        generate(expression);
        asmout<<"L"<<expression->getJump().True<<":\n";
        generate(statement);
        asmout << "\tADD SP, "<< -root->getOffset() << "\n";
        asmout<<"\tJMP L"<<loop<<"\n";
    } else if(root->getChildren().size() == 5&&first_word == "PRINTLN") {
        isPrint = true;
        SymbolInfo* id = root->getChildren()[2];
        if(root->getScopeID()==1)
            asmout<<"\tMOV AX, "<<id->getName()<<"\n";
        else 
            asmout<<"\tMOV AX, [BP"<<(root->getOffset()>=0?"+":"")<<root->getOffset()<<"]\n";
        asmout<<"\tCALL print_output\n\tCALL new_line\n";
    }
    // cout << "statement end" << endl;
}

void expression_statement(SymbolInfo* root) {
    // cout << "expression_statement start" << endl;
    if(root->getChildren().size() == 2) {
        SymbolInfo* expression = root->getChildren()[0];
        expression->setIsCondition(root->getIsCondition());
        expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(expression);
    }
    // cout << "expression_statement end" << endl;
}

void variable(SymbolInfo* root) {
    // cout << "variable start" << endl;
    SymbolInfo* id = root->getChildren()[0];
    if(root->getChildren().size()==4) {
        SymbolInfo* expression = root->getChildren()[2];
        generate(expression);
        if(root->getScopeID()==1) {
            asmout<< "\tLEA SI, "<< id->getName()<< "\n\tADD SI, CX\n\tADD SI, CX\n\tPUSH BP\n\tMOV BP, SI\n";
        } else {
            asmout<< "\tPUSH BP\n\tMOV BX, CX\n\tADD BX, BX\n\tADD BX, "<< root->getOffset() << "\n\tADD BP, BX\n";
        }
    }
    // cout << "variable end" << endl;
}

void expression(SymbolInfo* root) {
    // cout << "expression start" << endl;
    if(root->getChildren().size() == 1) {
        SymbolInfo* logic_expression = root->getChildren()[0];
        logic_expression->setIsCondition(root->getIsCondition());
        logic_expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(logic_expression);
    } else if(root->getChildren().size() == 3) {
        SymbolInfo* variable = root->getChildren()[0];
        SymbolInfo* logic_expression = root->getChildren()[2];
        generate(logic_expression);
        variable->setIsCondition(false);
        logic_expression->setIsCondition(false);

        if(variable->getSymbolType() != SymbolType::ARRAY) {
            generate(variable);
            if(variable->getScopeID()==1)
                asmout<<"\tMOV "<<variable->getName()<<", CX\n";
            else
                asmout<<"\tMOV [BP"<<(variable->getOffset()>=0?"+":"")<<variable->getOffset()<<"], CX\n";
        }
        else {
            asmout<< "\tPUSH CX\n";
            generate(variable);
            asmout<< "\tPOP AX\n\tPOP CX\n\tMOV [BP], CX\n\tMOV BP, AX\n";
        }
        if(root->getIsCondition())
            asmout<<"\tJMP L"<<root->getJump().True<<"\n";
    }
    // cout << "expression end" << endl;
}


void factor(SymbolInfo* root) {
    // cout << "factor start" << endl;
    if(root->getChildren().size()==1) {
        generate(root->getChildren()[0]);
        if(root->getRule()=="variable") {
            SymbolInfo* id = root->getChildren()[0];
            if(id->getSymbolType() != SymbolType::ARRAY) {
                if(id->getScopeID()==1)
                    asmout<<"\tMOV CX, "<<id->getName();
                else
                    asmout<<"\tMOV CX, [BP"<<(id->getOffset()>=0?"+":"")<<id->getOffset()<<"]";
            }
            else 
                asmout<<"\tMOV CX, [BP]\n\tPOP BP";
        } else
            asmout<<"\tMOV CX, "<<root->getName();
    } else if(root->getChildren().size()==2) {
        SymbolInfo* id = root->getChildren()[0];
        string symbol = root->getChildren()[1]->getName();
        generate(id);
        if(id->getSymbolType() != SymbolType::ARRAY) {
                if(id->getScopeID()==1)
                    asmout<<"\tMOV CX, "<<id->getName()<<"\n";
                else
                    asmout<<"\tMOV CX, [BP"<<(id->getOffset()>=0?"+":"")<<id->getOffset()<<"]\n";
            }
            else 
                asmout<<"\tMOV CX, [BP]\n";
        asmout << "\tMOV AX, CX\n";
        if(symbol == "++")
            asmout<<"\tINC CX\n";
        else if(symbol == "--")
            asmout<<"\tDEC CX\n";
        if(id->getSymbolType() != SymbolType::ARRAY) {
            if(id->getScopeID()==1)
                asmout<<"\tMOV "<<id->getName()<<", CX\n";
            else
                asmout<<"\tMOV [BP"<<(id->getOffset()>=0?"+":"")<<id->getOffset()<<"], CX\n";
        } else {
            asmout<<"\tMOV [BP], CX\n\tPOP BP\n";
        }
        asmout << "\tMOV CX, AX";
    } else if(root->getChildren().size()==3) {
        SymbolInfo* expression = root->getChildren()[1];
        generate(expression);
    } else if(root->getChildren().size()==4) {
        SymbolInfo* id = root->getChildren()[0];
        SymbolInfo* argument_list = root->getChildren()[2];
        generate(id);
        generate(argument_list);
        asmout << "\tCALL " << id->getName() << "\n";
        asmout << "\tADD SP, "<< argument_list->getOffset();
    }
    asmout << "\t\t; Line " << root->getChildren()[0]->getStartLine() << "\n";
    if(root->getIsCondition())
        asmout<<"\tCMP CX, 0\n\tJE L"<<root->getJump().False<<"\n\tJMP L"<<root->getJump().True<<"\n";
    // cout << "factor end" << endl;
}

void logic_expression(SymbolInfo* root) {
    // cout << "logic_expression start" << endl;
    if(root->getChildren().size() == 1) {
        SymbolInfo* rel_expression = root->getChildren()[0];
        rel_expression->setIsCondition(root->getIsCondition());
        rel_expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(rel_expression);
    } else if(root->getChildren().size() == 3) {
        SymbolInfo* rel_expression1 = root->getChildren()[0];
        SymbolInfo* rel_expression2 = root->getChildren()[2];
        rel_expression1->setIsCondition(root->getIsCondition());
        rel_expression2->setIsCondition(root->getIsCondition());
        string symbol = root->getChildren()[1]->getName();
        if(symbol=="||")
            rel_expression1->setJump(-1, root->getJump().True, clabel++);
        else
            rel_expression1->setJump(-1, clabel++, root->getJump().False);
        
        rel_expression2->setJump(-1, root->getJump().True, root->getJump().False);
        generate(rel_expression1);
        if(root->getIsCondition()) {
            if(symbol=="||") 
                asmout<<"L"<<rel_expression1->getJump().False<<":\n";
            else
                asmout<<"L"<<rel_expression1->getJump().True<<":\n";
        } else
            asmout<<"\tPUSH CX\n";
        generate(rel_expression2);
        if(!root->getIsCondition()) {
            asmout<<"\tPOP AX\n";
            if(symbol=="||") {
                int jmpF1 = clabel++;
                int jmpT = clabel++;
                int jmpF2 = clabel++;
                asmout<< "\tCMP AX, 0\n\tJE L"<< jmpF1 << "\n\tJMP L"<< jmpT << "\n";
                asmout<<"L"<< jmpF1 << ":\n\tCMP CX, 0\n\tJE L"<< jmpF2 << "\n";
                asmout<<"L"<< jmpT << ":\n\tMOV CX, 1\n";
                asmout<<"L"<< jmpF2 << ":\n";
            } else {
                int jmpF = clabel++;
                int jmpT = clabel++;
                int jmpN = clabel++;
                asmout<< "\tCMP AX, 0\n\tJE L"<< jmpF << "\n\tCMP CX, 0\n\tJE L"<< jmpF << "\n\tJMP L"<< jmpT << "\n";
                asmout<<"L"<< jmpF << ":\n\tMOV CX, 0\n\tJMP L"<< jmpN << "\n";
                asmout<<"L"<< jmpT << ":\n\tMOV CX, 1\n";
                asmout<<"L"<< jmpN << ":\n";
            }
        }
    }
    // cout << "logic_expression end" << endl;
}

void rel_expression(SymbolInfo* root) {
    // cout << "rel_expression start" << endl;
    if(root->getChildren().size() == 1) {
        SymbolInfo* expression = root->getChildren()[0];
        expression->setIsCondition(root->getIsCondition());
        expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(expression);
    } else if(root->getChildren().size() == 3) {
        SymbolInfo* expression1 = root->getChildren()[0];
        SymbolInfo* expression2 = root->getChildren()[2];
        string symbol = root->getChildren()[1]->getName();
        generate(expression1);
        asmout<<"\tPUSH CX\n";
        generate(expression2);
        asmout<<"\tPOP AX\n";
        if(root->getJump().True == -1||root->getJump().False == -1)
            root->setJump(-1, clabel++, clabel++);
        asmout << "\tCMP AX, CX\n";
        if(symbol == "==") {
            asmout<<"\tJE L"<<root->getJump().True<<"\n";
        } else if(symbol == "!=") {
            asmout<<"\tJNE L"<<root->getJump().True<<"\n";
        } else if(symbol == "<") {
            asmout<<"\tJL L"<<root->getJump().True<<"\n";
        } else if(symbol == ">") {
            asmout<<"\tJG L"<<root->getJump().True<<"\n";
        } else if(symbol == "<=") {
            asmout<<"\tJLE L"<<root->getJump().True<<"\n";
        } else if(symbol == ">=") {
            asmout<<"\tJGE L"<<root->getJump().True<<"\n";
        }
        asmout<<"\tJMP L"<<root->getJump().False<<"\n";
        if(!root->getIsCondition()) {
            int jmpNext = clabel++;
            asmout << "L" << root->getJump().True << ":\n\tMOV CX, 1\n\tJMP L" << jmpNext << "\n";
            asmout << "L" << root->getJump().False << ":\n\tMOV CX, 0\n";
            asmout << "L" << jmpNext << ":\n";
        }
    }
    // cout << "rel_expression end" << endl;
}

void simple_expression(SymbolInfo* root) {
    // cout << "simple_expression start" << endl;
    if(root->getChildren().size() == 1) {
        SymbolInfo* expression = root->getChildren()[0];
        expression->setIsCondition(root->getIsCondition());
        expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(expression);
    } else if(root->getChildren().size() == 3) {
        SymbolInfo* expression1 = root->getChildren()[0];
        SymbolInfo* expression2 = root->getChildren()[2];
        string symbol = root->getChildren()[1]->getName();
        generate(expression1);
        asmout<<"\tPUSH CX\n";
        generate(expression2);
        asmout<<"\tPOP AX\n";
        if(symbol == "+")
            asmout<<"\tADD CX, AX\n";
        else if(symbol == "-")
            asmout<<"\tSUB CX, AX\n\tNEG CX\n";
        if(root->getIsCondition())
            asmout<< "\tCMP CX, 0\n\tJE L"<< root->getJump().False<< "\n\tJMP L"<< root->getJump().True<< "\n";
    }
    // cout << "simple_expression end" << endl;
}

void term(SymbolInfo* root) {
    // cout << "term start" << endl;
    if(root->getChildren().size() == 1) {
        SymbolInfo* unary_expression = root->getChildren()[0];
        unary_expression->setIsCondition(root->getIsCondition());
        unary_expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(unary_expression);
    } else if(root->getChildren().size() == 3) {
        SymbolInfo* term = root->getChildren()[0];
        SymbolInfo* factor = root->getChildren()[2];
        string symbol = root->getChildren()[1]->getName();
        generate(term);
        asmout<<"\tPUSH CX\n";
        generate(factor);
        asmout<<"\tPOP AX\n";
        if(symbol == "*")
            asmout<<"\tIMUL CX\n";
        else if(symbol == "/")
            asmout<<"\tCWD\n\tIDIV CX\n";
        else if(symbol == "%")
            asmout<<"\tCWD\n\tIDIV CX\n\tMOV AX, DX\n";
        asmout << "\tMOV CX, AX\n";
        if(root->getIsCondition())
            asmout<< "\tCMP CX, 0\n\tJE L"<< root->getJump().False<< "\n\tJMP L"<< root->getJump().True<< "\n";
    }
    // cout << "term end" << endl;
}

void unary_expression(SymbolInfo* root) {
    // cout << "unary_expression start" << endl;
    if(root->getChildren().size() == 1) {
        SymbolInfo* unary_expression = root->getChildren()[0];
        unary_expression->setIsCondition(root->getIsCondition());
        unary_expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(unary_expression);
    } else if(root->getChildren().size() == 2) {
        SymbolInfo* unary_operator = root->getChildren()[0];
        SymbolInfo* unary_expression = root->getChildren()[1];
        unary_expression->setIsCondition(root->getIsCondition());
        unary_expression->setJump(-1, root->getJump().True, root->getJump().False);
        generate(unary_expression);
        if(unary_operator->getName() == "-")
            asmout<<"\tNEG CX\n";
        if(unary_operator->getName() == "!"&& !root->getIsCondition()) {
            int jmp1 = clabel++;
            int jmp2 = clabel++;
            asmout<<"\tCMP CX, 0\n\tJNE L"<< jmp1 <<"\n\tMOV CX, 1\n\tJMP L"<< jmp2 <<"\n";
            asmout<<"L"<< jmp1 <<":\n\tMOV CX, 0\n";
            asmout<<"L"<< jmp2 <<":\n";
        }
    }
    // cout << "unary_expression end" << endl;
}

void arguments(SymbolInfo* root) {
    // cout << "arguments start" << endl;
    generate(root->getChildren()[0]);
    if(root->getChildren().size() == 3)
        generate(root->getChildren()[2]);
    asmout << "\tPUSH CX\n";
    // cout << "arguments end" << endl;
}

void generate(SymbolInfo *root)
{
    // cout << root->getType() << " : "<< root->getRule() << " start" <<endl;
    if (root->getType() == "start")
        start_program(root);
    else if (root->getType() == "func_definition")
        func_definition(root);
    else if(root->getType() == "var_declaration") 
        var_declaration(root);
    else if(root->getType() == "compound_statement")
        compound_statement(root);
    else if(root->getType() == "statements")
        statements(root);
    else if(root->getType() == "statement")
        statement(root);
    else if(root->getType() == "expression_statement")
        expression_statement(root);
    else if(root->getType() == "variable")
        variable(root);
    else if(root->getType() == "expression")
        expression(root);
    else if(root->getType() == "logic_expression")
        logic_expression(root);
    else if(root->getType() == "rel_expression")
        rel_expression(root);
    else if(root->getType() == "simple_expression")
        simple_expression(root);
    else if(root->getType() == "term")
        term(root);
    else if(root->getType() == "unary_expression")
        unary_expression(root);
    else if(root->getType() == "factor")
        factor(root);
    else if(root->getType() == "arguments")
        arguments(root);
    else
    {
        // cout << "Unknown type: " << root->getType() << " start"<< endl;
        for (SymbolInfo *child : root->getChildren())
            generate(child);
        // cout << "Unknown type: " << root->getType() << " end"<< endl;
    }
    // cout << root->getType() << " : "<< root->getRule() << " end" <<endl;
}

void asmCodeGenerator(SymbolTable* table, SymbolInfo *root, int errCount)
{
    if (errCount > 0)
    {
        asmout << "; Errors found in the code. Assembly code generation aborted!" << endl;
        // optout << "; Errors found in the code. Assembly code generation aborted!" << endl;
        return;
    }
    symTable = table;
    generate(root);
}

///................Optimization....................

string toUpperCase(string s)
{
	for (int i = 0; i < s.size(); i++)
		s[i] = toupper(s[i]);
	return s;
}

void getCodeList(string file_name, vector<vector<string>> &code)
{
    ifstream fin;
    fin.open(file_name);
    string line;
    while (getline(fin, line))
    {
        string word="";
        vector<string> lineWords;
        for(char c: line) {
            if(c==';') {
                break;
            }
            if(c==','||c==' '||c=='\t') {
                if(word.size()>0) {
                    lineWords.push_back(word);
                    word="";
                }
            } else word += c;
        }
        if(word.size()>0)
            lineWords.push_back(word);
        // if(lineWords.size()>0) 
            code.push_back(lineWords);
    }
    fin.close();
}

void printLine(vector<string> line) {
    for(string s: line) {
        cout << s << ' ';
    }
    cout << endl;
}

void peepholeOptimization(ofstream &fout, vector<vector<string>> code)
{
    cout << "Optimizing Code...." << endl;
    int initialSize = code.size();
    vector<vector<string>> pass;
    pass = code;
    vector<string> temp;
    code.push_back(temp);
    while(pass.size()<code.size()) {
        code = pass;
        pass.clear();
        int sz = code.size(); 
        for(int i=0; i<sz; i++) {
            if(code[i].size()==0) continue;
            if(i<(sz-1)&&code[i+1].size()&&toUpperCase(code[i][0])=="MOV"&&toUpperCase(code[i+1][0])=="MOV") {
                if(code[i][1]==code[i+1][1]) continue;
                if(code[i][1]==code[i+1][2]&&code[i][2]==code[i+1][1]) {
                    pass.push_back(code[i++]);
                    continue;
                }
            }
            if(i<(sz-1)&&code[i+1].size()&&((toUpperCase(code[i][0])=="PUSH"&&toUpperCase(code[i+1][0])=="POP")||(toUpperCase(code[i][0])=="POP"&&toUpperCase(code[i+1][0])=="PUSH"))) {
                if(code[i][1]==code[i+1][1]) {
                    i++;
                    continue;
                }
            }
            if(toUpperCase(code[i][0])=="ADD"||toUpperCase(code[i][0])=="SUB") {
                if(code[i][2]=="0") continue;
            }
            pass.push_back(code[i]);
        }
    }
    int optimizedSize = code.size();
    cout << "Optimized from line number :" << initialSize << " to :" << optimizedSize << endl;
    
    bool flag_data = false, flag_code=false;
    for(vector<string> line: code) {
        if(toUpperCase(line[0])==".DATA") flag_data = true;
        if(toUpperCase(line[0])==".CODE") flag_code = true;
        if(flag_data) {
            if(line.size()>1&&toUpperCase(line[0])!="END"&&toUpperCase(line[1])!="PROC"||toUpperCase(line[0])=="RET") fout << "\t";
        }
        fout << line[0];
        if(flag_code&&line.size()==3) fout << " "<< line[1] << ",";
        else if(line.size()>1) fout << " " << line[1];
        for(int i=2; i<line.size(); i++) fout << " " << line[i];
        fout << endl;
    }
}