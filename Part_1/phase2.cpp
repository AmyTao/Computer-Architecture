#include "mainlib.h"
using namespace std;
string decimalToBinary(enum Registers rg) //change register number into binary code(length=5)
{
    int num = rg;
    int remainder;
    string ans = "";
    while (num != 0)
    {
        remainder = num % 2;
        num = num / 2;
        ans = to_string(remainder) + ans;
    }
    while (ans.length() < 5) //sign extension
    {
        ans = "0" + ans;
    }
    return ans;
}
string decimalToBinary(int rg) //change int into binary format
{
    int remainder;
    string ans = "";
    bool flag = false;
    if (rg < 0)
    {
        flag = true;
        rg = -rg;
    }
    while (rg != 0)
    {
        remainder = rg % 2;
        rg = rg / 2;
        ans = to_string(remainder) + ans;
    }
    while (ans.length() < 16) //sign extension
    {
        ans = "0" + ans;
    }
    if (flag)
    {
        string newans = "";
        reverse(ans.begin(), ans.end());
        int index = ans.find_first_not_of('0');
        reverse(ans.begin(), ans.end());
        for (int i = 0; i < 16; i++)
        {
            if (i < 15 - index)
            {
                if (ans.at(i) == '0')
                {
                    newans += "1";
                }
                else
                {
                    newans += "0";
                }
            }
            else
            {
                newans += ans.at(i);
            }
        }
        ans = newans;
    }
    return ans;
}
string toLabeladdress(int rg) //return label address in binary format
{
    int remainder;
    string ans = "";
    bool flag = false;
    if (rg < 0)
    {
        flag = true;
        rg = -rg;
    }
    while (rg != 0)
    {
        remainder = rg % 2;
        rg = rg / 2;
        ans = to_string(remainder) + ans;
    }
    while (ans.length() < 32)
    {
        ans = "0" + ans;
    }
    ans = ans.substr(4, 26);
    return ans;
}
void pass2(deque<string> instructions, string filename, map<string, Registers> regmap, map<string, int> ltp)
{
    int currentPC = 1;
    ofstream outFile;
    outFile.open(filename.c_str(), ios::out | ios::trunc);
    deque<string> each_instruction;
    //several parts in the instructions
    string first;
    string write = ""; //indicate each line of the output.txt
    Registers rd;
    Registers rs;
    Registers rt;
    string function;
    string opcode;
    int sa;
    stringstream ss; //change string to int
    int immediate;
    string label;
    int Label;
    while (!instructions.empty())
    {
        string each_line = instructions.front();
        instructions.pop_front();
        each_instruction.push_back(each_line.substr(0, each_line.find(' ')));
        each_line.erase(0, each_line.find(' ')); //erase the first part of the instruction
        while (each_line.length() != 0)          //split the whole line
        {
            if (each_line.find(',') != -1) //split the line with ','
            {
                each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                each_instruction.push_back(each_line.substr(0, each_line.find(',')));
                each_line.erase(0, each_line.find(',') + 1);
            }
            else //the last part of the line
            {
                each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                if (each_line.find('(') != -1) //case with ()
                {
                    each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                    each_instruction.push_back(each_line.substr(0, each_line.find('(')));
                    each_line.erase(0, each_line.find('(') + 1);
                    each_instruction.push_back(each_line.substr(0, each_line.find(')')));
                    break;
                }
                else //case without () at the end
                {
                    each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                    each_line.erase(each_line.find_last_not_of("\r\t\n ") + 1);
                    each_instruction.push_back(each_line);
                    break;
                }
            }
        }
        first = each_instruction.front();
        each_instruction.pop_front();
        //specify each line using those separate parts
        if (first == "add")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100000";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "addu")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100001";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "and")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100100";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "div")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "011010";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + "00000" + "00000" + function;
        }
        else if (first == "divu")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "011011";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + "00000" + "00000" + function;
        }
        else if (first == "jalr")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "001001";
            write = opcode + decimalToBinary(rs) + "00000" + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "jr")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "001000";
            write = opcode + decimalToBinary(rs) + "00000" + "00000" + "00000" + function;
        }
        else if (first == "mfhi")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "010000";
            write = opcode + "00000" + "00000" + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "mflo")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "010010";
            write = opcode + "00000" + "00000" + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "mthi")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "010001";
            write = opcode + decimalToBinary(rs) + "00000" + "00000" + "00000" + function;
        }
        else if (first == "mtlo")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "010011";
            write = opcode + decimalToBinary(rs) + "00000" + "00000" + "00000" + function;
        }
        else if (first == "mult")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "011000";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + "00000" + "00000" + function;
        }
        else if (first == "multu")
        {
            opcode = "000000";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "011001";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + "00000" + "00000" + function;
        }
        else if (first == "nor")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100111";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "or")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100101";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "sll")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> sa;
            each_instruction.pop_front();
            string Sa = decimalToBinary(sa).substr(11);
            function = "000000";
            write = opcode + "00000" + decimalToBinary(rt) + decimalToBinary(rd) + Sa + function;
        }
        else if (first == "sllv")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "000100";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "slt")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "101010";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "sltu")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "101011";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "sra")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> sa;
            each_instruction.pop_front();
            string Sa = decimalToBinary(sa).substr(11);
            function = "000011";
            write = opcode + "00000" + decimalToBinary(rt) + decimalToBinary(rd) + Sa + function;
        }
        else if (first == "srav")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "000111";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "srl")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> sa;
            each_instruction.pop_front();
            string Sa = decimalToBinary(sa).substr(11);
            function = "000010";
            write = opcode + "00000" + decimalToBinary(rt) + decimalToBinary(rd) + Sa + function;
        }
        else if (first == "srlv")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "000110";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "sub")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100010";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "subu")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100011";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "syscall")
        {
            write = "00000000000000000000000000001100";
        }
        else if (first == "xor")
        {
            opcode = "000000";
            rd = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            function = "100110";
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(rd) + "00000" + function;
        }
        else if (first == "addi")
        {
            opcode = "001000";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "addiu")
        {
            opcode = "001001";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "andi")
        {
            opcode = "001100";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "beq")
        {
            opcode = "000100";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            label = each_instruction.front();
            each_instruction.pop_front();
            Label = ltp[label];
            Label -= (currentPC + 1);
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(Label);
        }
        else if (first == "bgez")
        {
            opcode = "000001";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            label = each_instruction.front();
            each_instruction.pop_front();
            Label = ltp[label];
            Label -= (currentPC + 1);
            write = opcode + decimalToBinary(rs) + "00001" + decimalToBinary(Label);
        }
        else if (first == "bgtz")
        {
            opcode = "000111";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            label = each_instruction.front();
            each_instruction.pop_front();
            Label = ltp[label];
            Label -= (currentPC + 1);
            write = opcode + decimalToBinary(rs) + "00000" + decimalToBinary(Label);
        }
        else if (first == "blez")
        {
            opcode = "000110";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            label = each_instruction.front();
            each_instruction.pop_front();
            Label = ltp[label];
            Label -= (currentPC + 1);
            write = opcode + decimalToBinary(rs) + "00000" + decimalToBinary(Label);
        }
        else if (first == "bltz")
        {
            opcode = "000001";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            label = each_instruction.front();
            each_instruction.pop_front();
            Label = ltp[label];
            Label -= (currentPC + 1);
            write = opcode + decimalToBinary(rs) + "00000" + decimalToBinary(Label);
        }
        else if (first == "bne")
        {
            opcode = "000101";
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            label = each_instruction.front();
            each_instruction.pop_front();
            Label = ltp[label];
            Label -= (currentPC + 1);
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(Label);
        }
        else if (first == "lb")
        {
            opcode = "100000";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lbu")
        {
            opcode = "100100";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lh")
        {
            opcode = "100001";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lhu")
        {
            opcode = "100101";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lui")
        {
            opcode = "001111";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + "00000" + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lw")
        {
            opcode = "100011";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "ori")
        {
            opcode = "001101";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "sb")
        {
            opcode = "101000";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "slti")
        {
            opcode = "001010";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "sltiu")
        {
            opcode = "001011";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "sh")
        {
            opcode = "101001";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "sw")
        {
            opcode = "101011";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "xori")
        {
            opcode = "001110";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lwl")
        {
            opcode = "100010";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "lwr")
        {
            opcode = "100110";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "swl")
        {
            opcode = "101010";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "swr")
        {
            opcode = "101110";
            rt = regmap[each_instruction.front()];
            each_instruction.pop_front();
            ss << each_instruction.front();
            ss >> immediate;
            each_instruction.pop_front();
            rs = regmap[each_instruction.front()];
            each_instruction.pop_front();
            write = opcode + decimalToBinary(rs) + decimalToBinary(rt) + decimalToBinary(immediate);
        }
        else if (first == "j")
        {
            opcode = "000010";
            Label = 4194304;
            Label += (ltp[each_instruction.front()] - 1) * 4;
            each_instruction.pop_front();
            label = toLabeladdress(Label);
            write = opcode + label;
        }
        else if (first == "jal")
        {
            opcode = "000011";
            Label = 4194304;
            Label += (ltp[each_instruction.front()] - 1) * 4;
            each_instruction.pop_front();
            label = toLabeladdress(Label);
            write = opcode + label;
        }
        else
        {
            write = "wrong";
        }
        outFile << write << endl; //add to the output.txt
        write = "";
        ss.clear();
        currentPC++;
    }
    outFile.close();
}
