#include "kind.h"
#include <math.h>
#include "math.h"
#include "lexer.h"
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <stdlib.h>
using std::string;
using std::vector;
using std::endl;
using std::cerr;
using std::cin;
using std::getline;
using ASM::Token;
using ASM::Lexer;
using namespace std;

int temp = 0;
int value[10000000];
int cvalue = 0;
string labels[10000000];
int svalue = 0;
int pc = 0;
string labelname;

void output_value(unsigned int instr){
    char c = instr >> 24;
    cout << c;
    c = instr >> 16;
    cout << c;
    c = instr >> 8;
    cout << c;
    c = instr;
    cout << c;
}

void addin(int temp){
    putchar((temp >> 24) & 0xff);
    putchar((temp >> 16) & 0xff);
    putchar((temp >> 8) & 0xff);
    putchar(temp & 0xff);
}

int main(int argc, char* argv[]){
    vector< vector<Token*> > tokLines;
    try{
        Lexer lexer;
        string line;
        while(getline(cin,line)){
            tokLines.push_back(lexer.scan(line));
        }
        vector<vector<Token*> >::iterator it;
        //pass1
        for(it = tokLines.begin(); it != tokLines.end(); ++it){
            vector<Token*>::iterator it2;
            for(it2 = it->begin(); it2 != it->end(); ++it2){
                string lex = (*it2)->getLexeme();
                if((*it2)->getKind() == ASM::LABEL){
                    int len = (*it2)->getLexeme().length() - 1;
                    labelname = (*it2)->getLexeme().substr(0,len);
                    for(int index = 0; index < svalue; index++){
                        if(labels[index] == labelname){
                            cerr << "ERROR: Label already exit" << endl;
                            return 0;
                        }
                    }
                    labels[svalue] = labelname;
                    value[cvalue] = pc;
                    svalue++;
                    cvalue++;
                }
                else if((*it2)->getKind() == ASM::ID || (*it2)->getKind() == ASM::DOTWORD){
                    pc = pc + 4;
                    break;
                }
            }
        }
        int counter = 0;
        //pass2
        for(it = tokLines.begin(); it != tokLines.end(); ++it){
            vector<Token*>::iterator it2;
            for(it2 = it->begin(); it2 != it->end(); ++it2){
                if ((*it2)->getKind() == ASM::LABEL){
                    continue;
                }
                //dotword
                else if ((*it2)->getKind() == ASM::DOTWORD){
                    counter = counter + 1;
                    ++it2;
                    if((*it2)->getKind() == ASM::INT || (*it2)->getKind() == ASM::HEXINT){
                        int value = (*it2)->toInt();
                        output_value(value);
                    }
                    else if ((*it2)->getKind() == ASM::ID){
                        int open = 0;
                        int temp = 0;
                        for(int index = 0; index < svalue; index++){
                            if ((*it2)->getLexeme() == labels[index]){
                                temp = index;
                                open = 1;;
                                break;
                            }
                        }
                        if(open == 1){
                            if (value[temp] > 4294967295){
                                cerr << "ERROR: overflow" << endl;
                                return 0;
                            }
                            else{
                                addin(temp);
                            }
                        }
                        else{
                            cerr << "ERROR: label not find" << endl;
                            return 0;
                        }
                    }
                    else{
                        cerr << "ERROR: here is a error" << endl;
                        return 0;
                    }
                }
                //jr and jalr
                else if((*it2)->getKind() == ASM::ID){
                    //jr
                    if((*it2)->getLexeme() == "jr"){
                        if((*it).size() != 2){
                            cerr << "ERROR: Not enough or too much in jr" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() == ASM::REGISTER){
                            unsigned int value = (*it2)->toInt();
                            value = value << 21;
                            value = value |= 8;
                            output_value(value);
                        }
                        else{
                            cerr << "ERROR: not a correct jr instruction" << endl;
                            return 0;
                        }
                    }
                    //jalr
                    else if((*it2)->getLexeme() == "jalr"){
                        if((*it).size() != 2){
                            cerr << "ERROR: Not enough or too much jalr" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() == ASM::REGISTER){
                            unsigned int value = (*it2)->toInt();
                            value = value << 21;
                            value = value |= 9;
                            output_value(value);
                        }
                        else{
                            cerr << "ERROR: not a correct jalr instruction" << endl;
                            return 0;
                        }
                    }
                    //add
                    else if((*it2)->getLexeme() == "add"){
                        if((*it).size() != 6){
                            cerr << "ERROR: Size is not correct" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2 = it2 - 5;
                        int value1 = (*(it2+1))->toInt();
                        int value2 = (*(it2+3))->toInt();
                        int value3 = (*(it2+5))->toInt();
                        it2 = it2 + 5;
                        value2 = value2 << 21;
                        value3 = value3 << 16;
                        value1 = value1 << 11;
                        output_value(value2 | value3 | value1 | 32);
                    }
                    //sub
                    else if((*it2)->getLexeme() == "sub"){
                        if((*it).size() != 6){
                            cerr << "Error: Size is not correct" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2 = it2 - 5;
                        int value1 = (*(it2+1))->toInt();
                        int value2 = (*(it2+3))->toInt();
                        int value3 = (*(it2+5))->toInt();
                        it2 = it2 + 5;
                        value2 = value2 << 21;
                        value3 = value3 << 16;
                        value1 = value1 << 11;
                        output_value(value2 | value3 | value1 | 34);
                    }
                    //slt
                    else if((*it2)->getLexeme() == "slt"){
                        if((*it).size() != 6){
                            cerr << "ERROR: Size is not correct" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2 = it2 - 5;
                        int value1 = (*(it2+1))->toInt();
                        int value2 = (*(it2+3))->toInt();
                        int value3 = (*(it2+5))->toInt();
                        it2 = it2 + 5;
                        value2 = value2 << 21;
                        value3 = value3 << 16;
                        value1 = value1 << 11;
                        output_value(value2 | value3 | value1 | 42);
                    }
                    //sltu
                    else if((*it2)->getLexeme() == "sltu"){
                        if((*it).size() != 6){
                            cerr << "ERROR: Size is not correct" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::COMMA){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() != ASM::REGISTER){
                            cerr << "ERROR: Wrong register" << endl;
                            return 0;
                        }
                        it2 = it2 - 5;
                        int value1 = (*(it2+1))->toInt();
                        int value2 = (*(it2+3))->toInt();
                        int value3 = (*(it2+5))->toInt();
                        it2 = it2 + 5;
                        value2 = value2 << 21;
                        value3 = value3 << 16;
                        value1 = value1 << 11;
                        output_value(value2 | value3 | value1 | 43);
                    }
                    //lis
                    else if((*it2)->getLexeme() == "lis"){
                        if(it->end() - it2 != 2){
                            cerr << "ERROR: Not enough or too much in lis" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() == ASM::REGISTER){
                            unsigned int value = (*it2)->toInt();
                            output_value(20 | (value << 11));
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //mflo
                    else if((*it2)->getLexeme() == "mflo"){
                        if((*it).size() != 2){
                            cerr << "ERROR: Not enough or too much in jr" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() == ASM::REGISTER){
                            unsigned int value = (*it2)->toInt();
                            output_value(18 | (value << 11));
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //mfhi
                    else if((*it2)->getLexeme() == "mfhi"){
                        if((*it).size() != 2){
                            cerr << "ERROR: Not enough or too much in jr" << endl;
                            return 0;
                        }
                        it2++;
                        if((*it2)->getKind() == ASM::REGISTER){
                            unsigned int value = (*it2)->toInt();
                            output_value(16 | (value << 11));
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //mult
                    else if((*it2)->getLexeme() == "mult"){
                        if((*it).size() != 4){
                            cerr << "ERROR: Not a correct size" << endl;
                            return 0;
                        }
                        if((*(it2+1))->getKind() == ASM::REGISTER){
                            if((*(it2+2))->getKind() == ASM::COMMA){
                                if((*(it2+3))->getKind() == ASM::REGISTER){
                                    it2++;
                                    int value1 = (*(it2))->toInt();
                                    it2 = it2 + 2;
                                    int value2 = (*(it2))->toInt();
                                    output_value((value1 << 21) | (value2 << 16)| 24);
                                }
                                else{
                                    cerr << "ERROR: Not a register" << endl;
                                    return 0;
                                }
                            }
                            else{
                                cerr << "ERROR: Not a comma" << endl;
                                return 0;
                            }
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //multu
                    else if((*it2)->getLexeme() == "multu"){
                        if((*it).size() != 4){
                            cerr << "ERROR: Not a correct size" << endl;
                            return 0;
                        }
                        if((*(it2+1))->getKind() == ASM::REGISTER){
                            if((*(it2+2))->getKind() == ASM::COMMA){
                                if((*(it2+3))->getKind() == ASM::REGISTER){
                                    it2++;
                                    int value1 = (*(it2))->toInt();
                                    it2 = it2 + 2;
                                    int value2 = (*(it2))->toInt();
                                    output_value((value1 << 21) | (value2 << 16)| 25);
                                }
                                else{
                                    cerr << "ERROR: Not a register" << endl;
                                    return 0;
                                }
                            }
                            else{
                                cerr << "ERROR: Not a comma" << endl;
                                return 0;
                            }
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //div
                    else if((*it2)->getLexeme() == "div"){
                        if((*it).size() != 4){
                            cerr << "ERROR: Not a correct size" << endl;
                            return 0;
                        }
                        if((*(it2+1))->getKind() == ASM::REGISTER){
                            if((*(it2+2))->getKind() == ASM::COMMA){
                                if((*(it2+3))->getKind() == ASM::REGISTER){
                                    it2++;
                                    int value1 = (*(it2))->toInt();
                                    it2 = it2 + 2;
                                    int value2 = (*(it2))->toInt();
                                    output_value((value1 << 21) | (value2 << 16)| 26);
                                }
                                else{
                                    cerr << "ERROR: Not a register" << endl;
                                    return 0;
                                }
                            }
                            else{
                                cerr << "ERROR: Not a comma" << endl;
                                return 0;
                            }
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //divu
                    else if((*it2)->getLexeme() == "divu"){
                        if((*it).size() != 4){
                            cerr << "ERROR: Not a correct size" << endl;
                            return 0;
                        }
                        if((*(it2+1))->getKind() == ASM::REGISTER){
                            if((*(it2+2))->getKind() == ASM::COMMA){
                                if((*(it2+3))->getKind() == ASM::REGISTER){
                                    it2++;
                                    int value1 = (*(it2))->toInt();
                                    it2 = it2 + 2;
                                    int value2 = (*(it2))->toInt();
                                    output_value((value1 << 21) | (value2 << 16)| 27);
                                }
                                else{
                                    cerr << "ERROR: Not a register" << endl;
                                    return 0;
                                }
                            }
                            else{
                                cerr << "ERROR: Not a comma" << endl;
                                return 0;
                            }
                        }
                        else{
                            cerr << "ERROR: Not a register" << endl;
                            return 0;
                        }
                    }
                    //lw
                    else if((*it2)->getLexeme() == "lw"){
                        if((*it).size() != 7){
                            cerr << "ERROR: Not enough arguments" << endl;
                            return 0;
                        }
                        if(!((*(it2+1))->getKind() == ASM::REGISTER &&
                           (*(it2+2))->getKind() == ASM::COMMA &&
                           ((*(it2+3))->getKind() == ASM::INT || (*(it2+3))->getKind() == ASM::HEXINT) &&
                           (*(it2+4))->getKind() == ASM::LPAREN  &&
                           (*(it2+5))->getKind() == ASM::REGISTER &&
                           (*(it2+6))->getKind() == ASM::RPAREN)){
                            cerr << "ERROR: go out of bound" << endl;
                            return 0;
                        }
                        int v1 = atoi(((*(it2 + 1))->getLexeme().substr(1)).c_str());
                        int v2 = atoi(((*(it2 + 5))->getLexeme().substr(1)).c_str());
                        if(v1 < 0 || v1 > 31 || v2 < 0 || v2 > 31){
                            cerr << "ERROR: register go out of bound" << endl;
                            return 0;
                        }
                        int value1 = (*(it2+1))->toInt();
                        int value2 = (*(it2+5))->toInt();
                        int myvalue = 0;
                        if((*(it2+3))->getKind() == ASM::INT){
                            if((*(it2+3))->toInt() >= pow(-2,15) && (*(it2+3))->toInt() <= pow(2,15) - 1){
                                myvalue = (*(it2+3))->toInt();
                                if(myvalue  < -32768 || myvalue > 32767){
                                    cerr << "ERROR: too large" << endl;
                                    return 0;
                                }
                                int value3 = 0;
                                value3 |= value2 << 21;
                                value3 |= value1 << 16;
                                if(myvalue >= 0){
                                    value3 |= myvalue;
                                    value3 |= 35 << 26;
                                    output_value(value3);
                                }
                                else{
                                    value3 |= (~(myvalue*-1) & 0xffff) + 1;
                                    value3 |= 35 << 26;
                                    output_value(value3);
                                }
                            }
                            else{
                                cerr << "ERROR: go out of bound" << endl;
                                return 0;
                            }
                        }
                        else if((*(it2+3))->getKind() == ASM::HEXINT){
                            if((*(it2+3))->toInt() >= 0 && (*(it2+3))->toInt() <= 0xffff){
                                myvalue = (*(it2+3))->toInt();
                                int value3 = 0;
                                value3 |= value2 << 21;
                                value3 |= value1 << 16;
                                if(myvalue >= 0){
                                    value3 |= myvalue;
                                    value3 |= 35 << 26;
                                    output_value(value3);
                                }
                                else{
                                    value3 |= (~(myvalue*-1) & 0xffff) + 1;
                                    value3 |= 35 << 26;
                                    output_value(value3);
                                }
                            }
                            else{
                                cerr << "ERROR: go out of bound" << endl;
                                return 0;
                            }
                        }
                        else{
                            cerr << "ERROR: wrong lw and sw instruction" << endl;
                            return 0;
                        }
                    }
                    //sw
                    else if((*it2)->getLexeme() == "sw"){
                        if((*it).size() != 7){
                            cerr << "ERROR: Not enough arguments" << endl;
                            return 0;
                        }
                        if(!((*(it2+1))->getKind() == ASM::REGISTER &&
                             (*(it2+2))->getKind() == ASM::COMMA &&
                             ((*(it2+3))->getKind() == ASM::INT || (*(it2+3))->getKind() == ASM::HEXINT) &&
                             (*(it2+4))->getKind() == ASM::LPAREN  &&
                             (*(it2+5))->getKind() == ASM::REGISTER &&
                             (*(it2+6))->getKind() == ASM::RPAREN)){
                            cerr << "ERROR: go out of bound" << endl;
                            return 0;
                        }
                        int v1 = atoi(((*(it2 + 1))->getLexeme().substr(1)).c_str());
                        int v2 = atoi(((*(it2 + 5))->getLexeme().substr(1)).c_str());
                        if(v1 < 0 || v1 > 31 || v2 < 0 || v2 > 31){
                            cerr << "ERROR: register go out of bound" << endl;
                            return 0;
                        }
                        int value1 = (*(it2+1))->toInt();
                        int value2 = (*(it2+5))->toInt();
                        int myvalue = 0;
                        if((*(it2+3))->getKind() == ASM::INT){
                            if((*(it2+3))->toInt() >= pow(-2,15) && (*(it2+3))->toInt() <= pow(2,15) - 1){
                                myvalue = (*(it2+3))->toInt();
                                if(myvalue  < -32768 || myvalue > 32767){
                                    cerr << "ERROR: too large" << endl;
                                    return 0;
                                }
                                int value3 = 0;
                                value3 |= value2 << 21;
                                value3 |= value1 << 16;
                                if(myvalue >= 0){
                                    value3 |= myvalue;
                                    value3 |= 43 << 26;
                                    output_value(value3);
                                }
                                else{
                                    value3 |= (~(myvalue*-1) & 0xffff) + 1;
                                    value3 |= 43 << 26;
                                    output_value(value3);
                                }
                            }
                            else{
                                cerr << "ERROR: go out of bound" << endl;
                                return 0;
                            }
                        }
                        else if((*(it2+3))->getKind() == ASM::HEXINT){
                            if((*(it2+3))->toInt() >= 0 && (*(it2+3))->toInt() <= 0xffff){
                                myvalue = (*(it2+3))->toInt();
                                int value3 = 0;
                                value3 |= value2 << 21;
                                value3 |= value1 << 16;
                                if(myvalue >= 0){
                                    value3 |= myvalue;
                                    value3 |= 43 << 26;
                                    output_value(value3);
                                }
                                else{
                                    value3 |= (~(myvalue*-1) & 0xffff) + 1;
                                    value3 |= 43 << 26;
                                    output_value(value3);
                                }
                            }
                            else{
                                cerr << "ERROR: go out of bound" << endl;
                                return 0;
                            }
                        }
                    }
                    //beq and bne
                    else if((*it2)->getLexeme() == "beq" || (*it2)->getLexeme() == "bne"){
                        counter = counter + 1;
                        if(it->end() - it2 != 6){
                            cerr << "ERROR: Size is not correct" << endl;
                            return 0;
                        }
                        if(!((*(it2+1))->getKind() == ASM::REGISTER
                         && (*(it2+2))->getKind() == ASM::COMMA
                         && (*(it2+3))->getKind() == ASM::REGISTER
                         && (*(it2+4))->getKind() == ASM::COMMA
                         && ((*(it2+5))->getKind() == ASM::INT
                         || (*(it2+5))->getKind() == ASM::HEXINT
                         || (*(it2+5))->getKind() == ASM::ID)
                         )){
                            cerr << "ERROR: not a valid beq or bne" << endl;
                            return 0;
                         }
                         if((*(it2+1))->getKind() == ASM::REGISTER){
                           if((*(it2+3))->getKind() == ASM::REGISTER){
                                int i = 0;
                                int lower = pow(-2, 15);
                                int upper = pow(2, 15) - 1;
                                if((*(it2+5))->getKind() == ASM::INT){
                                    if((*(it2+5))->toInt() >= lower && (*(it2+5))->toInt() <= upper){
                                        i = (*(it2+5))->toInt();
                                        if(i < -32768 || i > 32767){
                                            cerr << "ERROR: address excceed the limit" << endl;
                                            return 0;
                                        }
                                        int tem = 0;
                                        int value1 = (*(it2+1))->toInt();
                                        int value2 = (*(it2+3))->toInt();
                                        if(value1 < 0 || value1 > 31 || value2 < 0 || value2 > 31){
                                            cerr << "ERROR: register go out of bound" << endl;
                                            return 0;
                                        }
                                        tem |= value1 << 21;
                                        tem |= value2 << 16;
                                        if(i >= 0){
                                            tem |= i;
                                            if((*it2)->getLexeme() == "beq"){
                                                tem |= 1 << 28;
                                            }
                                            else if((*it2)->getLexeme() == "bne"){
                                                tem |= 5 << 26;
                                            }
                                            output_value(tem);
                                        }
                                        else{
                                            tem |= (~(i*-1) & 0xffff) + 1;
                                            if((*it2)->getLexeme() == "beq"){
                                                tem |= 1 << 28;
                                            }
                                            else if((*it2)->getLexeme() == "bne"){
                                                tem |= 5 << 26;
                                            }
                                            output_value(tem);
                                        }
                                    }
                                    else{
                                        cerr << "ERROR: i is too small or too large" << endl;
                                        return 0;
                                    }
                                }
                                else if((*(it2+5))->getKind() == ASM::HEXINT){
                                    if((*(it2+5))->toInt() >= 0 && (*(it2+5))->toInt() <= 0xffff){
                                        i = (*(it2+5))->toInt();
                                        if(i < -32768 || i > 32767){
                                            cerr << "ERROR: address excceed the limit" << endl;
                                            return 0;
                                        }
                                        int tem = 0;
                                        int value1 = (*(it2+1))->toInt();
                                        int value2 = (*(it2+3))->toInt();
                                        if(value1 < 0 || value1 > 31 || value2 < 0 || value2 > 31){
                                            cerr << "ERROR: register go out of bound" << endl;
                                            return 0;
                                        }
                                        tem |= value1 << 21;
                                        tem |= value2 << 16;
                                        if(i >= 0){
                                            tem |= i;
                                            if((*it2)->getLexeme() == "beq"){
                                                tem |= 1 << 28;
                                            }
                                            else if((*it2)->getLexeme() == "bne"){
                                                tem |= 5 << 26;
                                            }
                                            output_value(tem);
                                        }
                                        else{
                                            tem |= (~(i*-1) & 0xffff) + 1;
                                            if((*it2)->getLexeme() == "beq"){
                                                tem |= 1 << 28;
                                            }
                                            else if((*it2)->getLexeme() == "bne"){
                                                tem |= 5 << 26;
                                            }
                                            output_value(tem);
                                        }
                                    }
                                    else{
                                        cerr << "ERROR: i is too small or too large" << endl;
                                        return 0;
                                    }
                                }
                                ///beq or bne with label   A4P4
                                else if((*(it2+5))->getKind() == ASM::ID){
                                    if((*it2)->getLexeme() == "beq"){
                                        int value1 = atoi(((*(it2 + 1))->getLexeme().substr(1)).c_str());
                                        int value2 = atoi(((*(it2 + 3))->getLexeme().substr(1)).c_str());
                                        if(value1 < 0 || value1 > 31 || value2 < 0 || value2 > 31){
                                            cerr << "ERROR: register go out of bound" << endl;
                                            return 0;
                                        }
                                        int tem = 0;
                                        int open = 0;
                                        for(int index = 0; index < svalue; index++){
                                            if((*(it2+5))->getLexeme() == labels[index]){
                                                open = 1;
                                                tem = index;
                                                break;
                                            }
                                        }
                                        if(open == 0){
                                            cerr << "ERROR: invlide beq." << endl;
                                            return 0;
                                        }
                                        else{
                                            int jump = (value[tem] / 4) - counter;
                                            if(jump < -32768 || jump > 32767){
                                                cerr << "ERROR: address excceed the limit" << endl;
                                                return 0;
                                            }
                                            int num = 4 << 26 | value1 << 21 | value2 << 16 | (jump & 0xffff);
                                            addin(num);
                                            break;
                                        }
                                    }
                                    else if((*it2)->getLexeme()== "bne"){
                                        int value1 = atoi(((*(it2 + 1))->getLexeme().substr(1)).c_str());
                                        int value2 = atoi(((*(it2 + 3))->getLexeme().substr(1)).c_str());
                                        if(value1 < 0 || value1 > 31 || value2 < 0 || value2 > 31){
                                            cerr << "ERROR: register go out of bound" << endl;
                                            return 0;
                                        }
                                        int tem = 0;
                                        int open = 0;
                                        for(int index = 0; index < svalue; index++){
                                            if((*(it2+5))->getLexeme() == labels[index]){
                                                open = 1;
                                                tem = index;
                                                break;
                                            }
                                        }
                                        if(open == 0){
                                            cerr << "ERROR: invlide bne." << endl;
                                            return 0;
                                        }
                                        else{
                                            int jump = value[tem] / 4 - counter;
                                            if(jump < -32768 || jump > 32767){
                                                cerr << "ERROR: address excceed the limit" << endl;
                                                return 0;
                                            }
                                            int num = 5 << 26 | value1 << 21 | value2 << 16 | (jump & 0xffff);
                                            addin(num);
                                            break;
                                        }
                                    }
                                    else{
                                        cerr << "ERROR: not a valid op." << endl;
                                        return 0;
                                    }
                                }
                                else{
                                    cerr << "ERROR: not a valid register" << endl;
                                    return 0;
                                }
                            }
                            else{
                                cerr << "ERROR: not a valid instruction" << endl;
                                return 0;
                            }
                        }
                    }
                }
                else{
                    cerr << "ERROR: invalid instruction" << endl;
                    return 0;
                }
            }
        }
    }
    catch(const string& msg){
        cerr << msg << endl;
    }
    vector<vector<Token*> >::iterator it;
    for(it = tokLines.begin(); it != tokLines.end(); ++it){
        vector<Token*>::iterator it2;
        for(it2 = it->begin(); it2 != it->end(); ++it2){
            delete *it2;
        }
    }
}
