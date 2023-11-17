#include <iostream>
#include <queue>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <bitset>
#include <set>
#include <cstring>
#include <iterator>
#include <iomanip>
#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
template <size_t desb, size_t srcb>
bitset<desb> &cut(bitset<desb> &des, const bitset<srcb> &src, int pos, int bits)
{
    if ((void *)&des == (void *)&src || bits > des.size() || pos > src.size())
    {
        return des;
    }
    if ((pos + bits - 1) >= src.size())
        bits = src.size() - pos;
    if (bits > des.size())
        bits = des.size();
    for (int i = 0; i < bits; ++i)
    {
        des[i] = src[i + pos]; // index: from right to left
    }
    return des;
}

enum Registers
{
    $zero = 0,
    $at,
    $v0,
    $v1,
    $a0,
    $a1,
    $a2,
    $a3,
    $t0,
    $t1,
    $t2,
    $t3,
    $t4,
    $t5,
    $t6,
    $t7,
    $s0,
    $s1,
    $s2,
    $s3,
    $s4,
    $s5,
    $s6,
    $s7,
    $t8,
    $t9,
    $k0,
    $k1,
    $gp,
    $sp,
    $s8,
    $fp = 30,
    $ra = 31
};
deque<string> ReFine(string filename)
{
    ifstream inFile;
    deque<string> reFineInFile;
    string line;
    inFile.open(filename.c_str(), ios::in);
    if (inFile)
    {
        while (getline(inFile, line))
        {
            line.erase(0, line.find_first_not_of("\r\t\n ")); // delete meaningless spaces
            line.erase(line.find_last_not_of("\r\t\n ") + 1);
            if (line.substr(0, 5) == ".text") // distinguish .text part
            {
                while (getline(inFile, line))
                {
                    line.erase(0, line.find_first_not_of("\r\t\n "));
                    line.erase(line.find_last_not_of("\r\t\n ") + 1);
                    if (line.length() == 0 || line.find('#') == 0) // delete comments
                    {
                        continue;
                    }
                    else
                    {
                        int comment = line.find('#'); // delete comments
                        if (comment != -1)
                        {
                            line = line.substr(0, comment);
                        }
                        line.erase(line.find_last_not_of("\r\t\n ") + 1);
                        if (line.length() != 0)
                        {
                            reFineInFile.push_back(line); // put into the refine=instructions+labels
                        }
                    }
                }
            }
        }
    }
    else
    {
        cout << "error1"; // if the file doesn't open successfully
    }
    inFile.close();
    return reFineInFile;
}
deque<string> pass1_step1(deque<string> filename)
{
    int PC = 0;
    string line;
    deque<string> instructions;
    while (!filename.empty())
    {
        line = filename.front();
        filename.pop_front();
        PC++;
        if (line.find(':') != -1) // find instrctions after ':'
        {
            if (line.find(':') != line.length() - 1)
            {
                string ist = line.substr(line.find(':') + 1);
                ist.erase(0, ist.find_first_not_of("\r\t\n ")); // delete redundant spaces
                ist.erase(ist.find_last_not_of("\r\t\n ") + 1);
                instructions.push_back(ist);
            }
        }
        else
        {
            instructions.push_back(line); // add instructions directly
        }
    }
    return instructions;
}
map<string, int> pass1_step2(deque<string> filename)
{
    map<string, int> LabelTable;
    int PC = 0;
    string line;
    while (!filename.empty())
    {
        line = filename.front();
        filename.pop_front();
        PC++;
        if (line.find(':') != -1) // find labels
        {
            if (line.find(':') == line.length() - 1) // labels with \n at the end
            {
                LabelTable.insert(pair<string, int>(line.substr(0, line.length() - 1), PC));
                PC--;
            }
            else
            {
                LabelTable.insert(pair<string, int>(line.substr(0, line.find(':')), PC));
            }
        }
    }
    return LabelTable;
}
string decimalToBinary(enum Registers rg) // change register number into binary code(length=5)
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
    while (ans.length() < 5) // sign extension
    {
        ans = "0" + ans;
    }
    return ans;
}
string decimalToBinary(int rg) // change int into binary format
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
    while (ans.length() < 16) // sign extension
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
string toLabeladdress(int rg) // return label address in binary format
{
    int remainder;
    string ans = "";
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
    // several parts in the instructions
    string first;
    string write = ""; // indicate each line of the output.txt
    Registers rd;
    Registers rs;
    Registers rt;
    string function;
    string opcode;
    int sa;
    stringstream ss; // change string to int
    int immediate;
    string label;
    int Label;
    while (!instructions.empty())
    {
        string each_line = instructions.front();
        instructions.pop_front();
        each_instruction.push_back(each_line.substr(0, each_line.find(' ')));
        each_line.erase(0, each_line.find(' ')); // erase the first part of the instruction
        while (each_line.length() != 0)          // split the whole line
        {
            if (each_line.find(',') != -1) // split the line with ','
            {
                each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                each_instruction.push_back(each_line.substr(0, each_line.find(',')));
                each_line.erase(0, each_line.find(',') + 1);
            }
            else // the last part of the line
            {
                each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                if (each_line.find('(') != -1) // case with ()
                {
                    each_line.erase(0, each_line.find_first_not_of("\r\t\n "));
                    each_instruction.push_back(each_line.substr(0, each_line.find('(')));
                    each_line.erase(0, each_line.find('(') + 1);
                    each_instruction.push_back(each_line.substr(0, each_line.find(')')));
                    break;
                }
                else // case without () at the end
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
        // specify each line using those separate parts
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
        outFile << write << endl; // add to the output.txt
        write = "";
        ss.clear();
        currentPC++;
    }
    outFile.close();
}

// all pointers
int *real_mem;
int *real_reg;
int *static_data;
int *dynamic_data;
int *pc;
int *ptr;
signed int *lo;
signed int *hi;
signed int *zero;
signed int *at;
signed int *v0;
signed int *v1;
signed int *a0;
signed int *a1;
signed int *a2;
signed int *a3;
signed int *t0;
signed int *t1;
signed int *t2;
signed int *t3;
signed int *t4;
signed int *t5;
signed int *t6;
signed int *t7;
signed int *s0;
signed int *s1;
signed int *s2;
signed int *s3;
signed int *s4;
signed int *s5;
signed int *s6;
signed int *s7;
signed int *t8;
signed int *t9;
signed int *k0;
signed int *k1;
signed int *gp;
signed int *sp;
signed int *fp;
signed int *ra;

// files
ifstream testinfile;
ofstream testoutfile;

// check points
set<int> init_checkpoints(string checkpoint_file)
{
    ifstream checkptsFile;
    set<int> checkpoints;
    checkptsFile.open(checkpoint_file.c_str(), ios::in);
    int tmp;
    string line;
    while (getline(checkptsFile, line))
    {
        stringstream ss;
        // cout << line << endl;
        ss << line;
        ss >> tmp;
        ss.clear();
        checkpoints.insert(tmp);
    }
    return checkpoints;
}
void checkpoint_memory(int ins_count, set<int> checkpoints)
{
    if (!checkpoints.count(ins_count))
        return;
    string name = "memory_" + to_string(ins_count) + ".bin";
    FILE *fp = fopen(name.c_str(), "wb");

    fwrite(real_mem, 1, 0x600000, fp);
}
void checkpoint_register(int ins_count, set<int> checkpoints)
{
    if (!checkpoints.count(ins_count))
        return;
    string name = "register_" + to_string(ins_count) + ".bin";
    FILE *Fp = fopen(name.c_str(), "wb");
    fwrite(zero, 4, 1, Fp);
    fwrite(at, 4, 1, Fp);
    fwrite(v0, 4, 1, Fp);
    fwrite(v1, 4, 1, Fp);
    fwrite(a0, 4, 1, Fp);
    fwrite(a1, 4, 1, Fp);
    fwrite(a2, 4, 1, Fp);
    fwrite(a3, 4, 1, Fp);
    fwrite(t0, 4, 1, Fp);
    fwrite(t1, 4, 1, Fp);
    fwrite(t2, 4, 1, Fp);
    fwrite(t3, 4, 1, Fp);
    fwrite(t4, 4, 1, Fp);
    fwrite(t5, 4, 1, Fp);
    fwrite(t6, 4, 1, Fp);
    fwrite(t7, 4, 1, Fp);
    fwrite(s0, 4, 1, Fp);
    fwrite(s1, 4, 1, Fp);
    fwrite(s2, 4, 1, Fp);
    fwrite(s3, 4, 1, Fp);
    fwrite(s4, 4, 1, Fp);
    fwrite(s5, 4, 1, Fp);
    fwrite(s6, 4, 1, Fp);
    fwrite(s7, 4, 1, Fp);
    fwrite(t8, 4, 1, Fp);
    fwrite(t9, 4, 1, Fp);
    fwrite(k0, 4, 1, Fp);
    fwrite(k1, 4, 1, Fp);
    fwrite(gp, 4, 1, Fp);
    fwrite(sp, 4, 1, Fp);
    fwrite(fp, 4, 1, Fp);
    fwrite(ra, 4, 1, Fp);

    fwrite(pc, 4, 1, Fp);
    fwrite(hi, 4, 1, Fp);
    fwrite(lo, 4, 1, Fp);

    fclose(Fp);
}

int compare_files(string filename1, string filename2)
{
    ifstream File1;
    File1.open(filename1.c_str(), ios::in);
    ifstream File2;
    File2.open(filename2.c_str(), ios::in);
    char char1 = File1.get();
    char char2 = File2.get();

    while (char1 != EOF && char2 != EOF)
    {
        if (char1 != char2)
        {
            return -1;
        }
        char1 = File1.get();
        char2 = File2.get();
    }
    File1.close();
    File2.close();
    return 0;
}
map<string, Registers> setRegmap()
{
    map<string, Registers> regmap;
    regmap.insert(pair<string, Registers>("$zero", $zero));
    regmap.insert(pair<string, Registers>("$at", $at));
    regmap.insert(pair<string, Registers>("$v0", $v0));
    regmap.insert(pair<string, Registers>("$v1", $v1));
    regmap.insert(pair<string, Registers>("$a0", $a0));
    regmap.insert(pair<string, Registers>("$a1", $a1));
    regmap.insert(pair<string, Registers>("$a2", $a2));
    regmap.insert(pair<string, Registers>("$a3", $a3));
    regmap.insert(pair<string, Registers>("$t0", $t0));
    regmap.insert(pair<string, Registers>("$t1", $t1));
    regmap.insert(pair<string, Registers>("$t2", $t2));
    regmap.insert(pair<string, Registers>("$t3", $t3));
    regmap.insert(pair<string, Registers>("$t4", $t4));
    regmap.insert(pair<string, Registers>("$t5", $t5));
    regmap.insert(pair<string, Registers>("$t6", $t6));
    regmap.insert(pair<string, Registers>("$t7", $t7));
    regmap.insert(pair<string, Registers>("$s0", $s0));
    regmap.insert(pair<string, Registers>("$s1", $s1));
    regmap.insert(pair<string, Registers>("$s2", $s2));
    regmap.insert(pair<string, Registers>("$s3", $s3));
    regmap.insert(pair<string, Registers>("$s4", $s4));
    regmap.insert(pair<string, Registers>("$s5", $s5));
    regmap.insert(pair<string, Registers>("$s6", $s6));
    regmap.insert(pair<string, Registers>("$s7", $s7));
    regmap.insert(pair<string, Registers>("$t8", $t8));
    regmap.insert(pair<string, Registers>("$t9", $t9));
    regmap.insert(pair<string, Registers>("$k0", $k0));
    regmap.insert(pair<string, Registers>("$k1", $k1));
    regmap.insert(pair<string, Registers>("$gp", $gp));
    regmap.insert(pair<string, Registers>("$sp", $sp));
    regmap.insert(pair<string, Registers>("$s8", $s8));
    regmap.insert(pair<string, Registers>("$fp", $fp));
    regmap.insert(pair<string, Registers>("$ra", $ra));
    return regmap;
}
map<int, signed int *> setPtrmap()
{
    map<int, signed int *> Ptrmap;
    Ptrmap.insert(pair<int, signed int *>(0, zero));
    Ptrmap.insert(pair<int, signed int *>(1, at));
    Ptrmap.insert(pair<int, signed int *>(2, v0));
    Ptrmap.insert(pair<int, signed int *>(3, v1));
    Ptrmap.insert(pair<int, signed int *>(4, a0));
    Ptrmap.insert(pair<int, signed int *>(5, a1));
    Ptrmap.insert(pair<int, signed int *>(6, a2));
    Ptrmap.insert(pair<int, signed int *>(7, a3));
    Ptrmap.insert(pair<int, signed int *>(8, t0));
    Ptrmap.insert(pair<int, signed int *>(9, t1));
    Ptrmap.insert(pair<int, signed int *>(10, t2));
    Ptrmap.insert(pair<int, signed int *>(11, t3));
    Ptrmap.insert(pair<int, signed int *>(12, t4));
    Ptrmap.insert(pair<int, signed int *>(13, t5));
    Ptrmap.insert(pair<int, signed int *>(14, t6));
    Ptrmap.insert(pair<int, signed int *>(15, t7));
    Ptrmap.insert(pair<int, signed int *>(16, s0));
    Ptrmap.insert(pair<int, signed int *>(17, s1));
    Ptrmap.insert(pair<int, signed int *>(18, s2));
    Ptrmap.insert(pair<int, signed int *>(19, s3));
    Ptrmap.insert(pair<int, signed int *>(20, s4));
    Ptrmap.insert(pair<int, signed int *>(21, s5));
    Ptrmap.insert(pair<int, signed int *>(22, s6));
    Ptrmap.insert(pair<int, signed int *>(23, s7));
    Ptrmap.insert(pair<int, signed int *>(24, t8));
    Ptrmap.insert(pair<int, signed int *>(25, t9));
    Ptrmap.insert(pair<int, signed int *>(26, k0));
    Ptrmap.insert(pair<int, signed int *>(27, k1));
    Ptrmap.insert(pair<int, signed int *>(28, gp));
    Ptrmap.insert(pair<int, signed int *>(29, sp));
    Ptrmap.insert(pair<int, signed int *>(30, fp));
    Ptrmap.insert(pair<int, signed int *>(31, ra));
    return Ptrmap;
}

void store_text(string filename, int *real_mem)
{
    ifstream inFile;
    string line;

    int index = 0;
    inFile.open(filename.c_str(), ios::in);
    if (inFile)
    {
        while (getline(inFile, line))
        {
            bitset<32> each_ins(line);
            // cout << line << endl;
            // cout << each_ins << endl;
            real_mem[index] = each_ins.to_ulong();
            index++;
            // cout << index << endl;
        }
    }
}

int store_data(string filename, int *real_mem)
{
    // cout << "real_mem in store_data" << real_mem << endl;
    int index = 0;
    int count;
    int ctr;
    int store;
    int countbyte = 0;
    int counthalf = 0;
    stringstream toint;
    queue<char> lineque;
    string type;
    ifstream inFile;
    string line;
    inFile.open(filename.c_str(), ios::in);
    if (inFile)
    {
        while (getline(inFile, line))
        {
            line.erase(0, line.find_first_not_of("\r\t\n "));
            line.erase(line.find_last_not_of("\r\t\n ") + 1);
            if (line.substr(0, 5) == ".data")
            {
                while (getline(inFile, line))
                {
                    line.erase(0, line.find_first_not_of("\r\t\n "));
                    line.erase(line.find_last_not_of("\r\t\n ") + 1);
                    if (line.substr(0, 5) == ".text")
                        break;
                    if (line.length() == 0 || line.find('#') == 0)
                    {
                        continue;
                    }
                    else
                    {
                        int comment = line.find('#'); // delete comments
                        if (comment != -1)
                        {
                            line = line.substr(0, comment);
                        }
                        line.erase(line.find_last_not_of("\r\t\n ") + 1);
                        if (line.length() != 0)
                        {
                            line.erase(0, line.find('.') + 1);
                            type = line.substr(0, line.find(' '));
                            line.erase(0, line.find(' ') + 1);
                            if (type == "ascii")
                            {
                                line.erase(0, line.find('\"') + 1);
                                line.erase(line.find_last_not_of('\"') + 1);


                                for (int i = 0; i < line.length(); i++)
                                {
                                    if (line.at(i) == '\\')
                                    {
                                        i++;
                                        if (line.at(i) == 'n')
                                            lineque.push('\n');
                                        else if (line.at(i)=='t'){
                                            lineque.push('\t');
                                        }else if (line.at(i)=='r'){
                                            lineque.push('\r');
                                        }else if (line.at(i)=='v'){
                                            lineque.push('\v');
                                        }else if (line.at(i)=='f'){
                                            lineque.push('\f');
                                        }else if (line.at(i)=='b'){
                                            lineque.push('\b');
                                        }else if (line.at(i)=='\''){
                                            lineque.push('\'');
                                        }else if (line.at(i)=='\"'){
                                            lineque.push('\"');
                                        }else if (line.at(i)=='\\'){
                                            lineque.push('\\');
                                        }else if (line.at(i)=='0'){
                                            lineque.push('\0');
                                        }
                                    }
                                    else
                                    {
                                        lineque.push(line.at(i));
                                    }
                                }
                                while (!lineque.empty())
                                {
                                    if (lineque.size() >= 4)
                                    {
                                        count = 0;
                                        for (int i = 0; i < 4; i++)
                                        {
                                            real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + (lineque.front() << (count%4) * 8);
                                            count++;
                                            lineque.pop();
                                        }
                                        index++;
                                    }
                                    else
                                    {
                                        count = 0;
                                        int lqs = lineque.size();
                                        for (int i = 0; i < lqs; i++)
                                        {
                                            real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + (lineque.front() << (count%4) * 8);
                                            count++;
                                            lineque.pop();
                                        }
                                    }
                                     if(lineque.empty()&&(count%4)==0) index--;
                                }

                            }
                            else if (type == "asciiz")
                            {
                                line.erase(0, line.find('\"') + 1);
                                line.erase(line.find_last_not_of('\"') + 1);


                                for (int i = 0; i < line.length(); i++)
                                {
                                    if (line.at(i) == '\\')
                                    {
                                        i++;
                                        if (line.at(i) == 'n')
                                            lineque.push('\n');
                                        else if (line.at(i)=='t'){
                                            lineque.push('\t');
                                        }else if (line.at(i)=='r'){
                                            lineque.push('\r');
                                        }else if (line.at(i)=='v'){
                                            lineque.push('\v');
                                        }else if (line.at(i)=='f'){
                                            lineque.push('\f');
                                        }else if (line.at(i)=='b'){
                                            lineque.push('\b');
                                        }else if (line.at(i)=='\''){
                                            lineque.push('\'');
                                        }else if (line.at(i)=='\"'){
                                            lineque.push('\"');
                                        }else if (line.at(i)=='\\'){
                                            lineque.push('\\');
                                        }else if (line.at(i)=='0'){
                                            lineque.push('\0');
                                        }
                                    }

                                    else
                                    {
                                        lineque.push(line.at(i));
                                    }
                                }
                                lineque.push('\0');
                                while (!lineque.empty())
                                {
                                    if (lineque.size() >= 4)
                                    {
                                        count = 0;
                                        for (int i = 0; i < 4; i++)
                                        {
                                            real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + (lineque.front() << (count%4) * 8);
                                            count++;
                                            lineque.pop();
                                        }
                                        index++;
                                    }
                                    else
                                    {
                                        count = 0;
                                        int lqs = lineque.size();
                                        for (int i = 0; i < lqs; i++)
                                        {
                                            real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + (lineque.front() << (count%4) * 8);
                                            count++;
                                            lineque.pop();
                                        }
                                    }
                                    if(lineque.empty()&&(count%4)==0) index--;
                                }
                            }
                            else if (type == "word")
                            {
                                line.erase(0, line.find_first_not_of("\r\t\n "));
                                line.erase(line.find_last_not_of("\r\t\n ") + 1);
                                // cout << line << endl;
                                while (line.find(',') != -1)
                                {
                                    string part_line = line.substr(0, line.find(','));
                                    toint << part_line;
                                    toint >> store;
                                    toint.clear();
                                    real_mem[index + (0x500000 - 0x400000) / 4] = store;
                                    index++;
                                    line.erase(0, line.find(',') + 1);
                                }
                                toint << line;
                                toint >> store;
                                toint.clear();
                                real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + store;
                            }

                            else if (type == "byte")
                            {
                                line.erase(0, line.find_first_not_of("\r\t\n "));
                                line.erase(line.find_last_not_of("\r\t\n ") + 1);

                                while (line.find(',') != -1)
                                {
                                    string part_line = line.substr(0, line.find(','));
                                    toint << part_line;
                                    toint >> store;
                                    toint.clear();
                                    store = store << countbyte * 8;
                                    real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + store;
                                    countbyte++;
                                    if (countbyte == 4)
                                    {
                                        countbyte = 0;
                                        index++;
                                    }
                                    line.erase(0, line.find(',') + 1);
                                }
                                toint << line;
                                toint >> store;
                                toint.clear();
                                store = store << countbyte * 8;
                                real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + store;

                            }

                            else if (type == "half")
                            {
                                line.erase(0, line.find_first_not_of("\r\t\n "));
                                line.erase(line.find_last_not_of("\r\t\n ") + 1);

                                while (line.find(',') != -1)
                                {

                                    string part_line = line.substr(0, line.find(','));
                                    toint << part_line;
                                    toint >> store;
                                    toint.clear();
                                    store = (store << counthalf * 16);
                                    real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + store;
                                    counthalf++;
                                    if (counthalf == 2)
                                    {
                                        counthalf = 0;
                                        index++;
                                    }
                                    line.erase(0, line.find(',') + 1);
                                }
                                toint << line;
                                toint >> store;
                                toint.clear();
                                store = store << counthalf * 16;
                                real_mem[index + (0x500000 - 0x400000) / 4] = real_mem[index + (0x500000 - 0x400000) / 4] + store;
                            } //如果length不为0
                            else
                            {
                                cout << "error" << endl;
                            }
                            index++;
                            // cout << "\n";
                        }
                    }
                } // 处理data 2nd while
            }     //读data后的每一行
            if (line.substr(0, 5) == ".text")
                break;
        } // 1st while end
    }     // if file
    cout<<"stack end:"<<real_mem+0x100000/4+index;
    return index;
}

void execution(int *ptr, map<int, signed int *> ptrmap, string filename4, string filename5, int *pc)
{
    char *chp = (char *)real_mem;
    short *shp=(short*)real_mem;
    int instruction = ptr[0]; // ptr=real address
    int *Rs;
    int *Rt;
    int *Rd;
    int Sa;
    int Immediate;
    int Target;
    stringstream ss;
    string line;
    bitset<32> bin_ins(instruction);
    bitset<6> opcode;
    cut(opcode, bin_ins, 26, 6);
    bitset<6> function;
    cut(function, bin_ins, 0, 6);
    bitset<5> rs;
    cut(rs, bin_ins, 21, 5);
    bitset<5> rt;
    cut(rt, bin_ins, 16, 5);
    bitset<5> rd;
    cut(rd, bin_ins, 11, 5);
    bitset<16> immediate;
    cut(immediate, bin_ins, 0, 16);
    bitset<1> sign;
    bitset<5> sa;
    cut(sa, bin_ins, 6, 5);
    bitset<26> target;
    cut(target, bin_ins, 0, 26);
    int op = opcode.to_ulong();
    if (op == 0)
    { // R type
        int func = function.to_ulong();
        switch (func)
        {
        case 32: // add
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] + Rt[0];
            break;
        case 33: // addu
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] + Rt[0];
            break;
        case 36: // and
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] & Rt[0];
            cout<<Rd[0]<<endl;
            break;
        case 26: // div
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            hi[0] = Rs[0] % Rt[0];
            lo[0] = Rs[0] / Rt[0];
            break;
        case 27: // divu
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            hi[0] = (unsigned)Rs[0] % (unsigned)Rt[0];
            lo[0] = (unsigned)Rs[0] / (unsigned)Rt[0];
            break;
        case 9: // jalr
            Rs = ptrmap[(int)rs.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = pc[0];
            pc[0] = Rs[0];
            break;
        case 8: // jr
            Rs = ptrmap[(int)rs.to_ulong()];
            pc[0] = Rs[0];
            break;
        case 16: // mfhi
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = hi[0];
            break;
        case 18: // mflo
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = lo[0];
            break;
        case 17: // mthi
            Rs = ptrmap[(int)rs.to_ulong()];
            hi[0] = Rs[0];
            break;
        case 19: // mtlo
            Rs = ptrmap[(int)rs.to_ulong()];
            lo[0] = Rs[0];
            break;
        case 24: // mult
        {
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            long long result = Rs[0] * Rt[0];
            bitset<64> multans(result);
            bitset<32> high;
            bitset<32> low;
            cut(high, multans, 32, 32);
            cut(low, multans, 0, 32);
            hi[0] = high.to_ulong();
            lo[0] = low.to_ulong();
            break;
        }
        case 25: // multu
        {
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            long long result = Rs[0] * Rt[0];
            bitset<64> multans(result);
            bitset<32> high;
            bitset<32> low;
            cut(high, multans, 32, 32);
            cut(low, multans, 0, 32);
            hi[0] = high.to_ulong();
            lo[0] = low.to_ulong();
            break;
        }

        case 39: // nor
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = ~(Rs[0] | Rt[0]);
            break;
        case 37: // or
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] | Rt[0];
            break;

        case 0: // sll
            Rd = ptrmap[(int)rd.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            /*cut(sign, sa, 4, 1);
            if (sign.to_ulong() == 0)
            {
                Sa = sa.to_ulong();
            }
            else
            {
                Sa = sa.to_ulong() - 32;
            }*/
            Sa = sa.to_ulong();
            Rd[0] = Rt[0] << Sa;
            break;
        case 4: // sllv
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rt[0] << (Rs[0]&31);
            break;
        case 42: // slt
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            if (Rs[0] < Rt[0])
                Rd[0] = 1;
            else
            {
                Rd[0] = 0;
            }
            break;
        case 43: // sltu
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            if ((unsigned int)Rs[0] < (unsigned int)Rt[0])
                Rd[0] = 1;
            else
            {
                Rd[0] = 0;
            }
            break;
        case 3: // sra
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Sa = sa.to_ulong();
            Rd[0] = Rt[0] >> Sa;
            break;
        case 7: // srav
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rt[0] >> (Rs[0]&31);
            break;
        case 2: // srl
            Rd = ptrmap[(int)rd.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            /*cut(sign, sa, 4, 1);
            if (sign.to_ulong() == 0)
            {
                Sa = sa.to_ulong();
            }
            else
            {
                Sa = sa.to_ulong() - 32;
            }*/
            Sa = sa.to_ulong();
            Rd[0] = (unsigned)Rt[0] >> Sa;

            break;
        case 6: // srlv
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = (unsigned)Rt[0] >> (Rs[0]&31);
            break;
        case 34: // sub
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] - Rt[0];
            break;
        case 35: // subu
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] - Rt[0];
            break;
        case 12: // syscall
        {
            cout << "enter syscall" << endl;
            int choice = v0[0];
            char c;
            string fn;
            char ba, bb, bc, bd;
            int i = 0;
            long long real_address;
            switch (choice)
            {
            case 1: // print_int
                cout << "a0:" << a0[0] << endl;
                testoutfile << a0[0];
                break;

            case 4: // print_string
            {
                cout<<a0[0]-0x400000+i<<endl;
                while(chp[a0[0]-0x400000+i]!='\0'){
                    //cout<<a0[0]-0x400000+i<<endl;
                    testoutfile << noskipws << chp[a0[0]-0x400000+i];
                    cout<<chp[a0[0]-0x400000+i];
                    i++;
                }

                break;
            }
            case 5: // read_int

                getline(testinfile, line);
                cout<<line;
                ss << line;
                int num;
                ss >> num;
                ss.clear();
                v0[0] = num;
                break;
            case 8: // read_string
            {
                getline(testinfile,line);
                cout<<line<<endl;
                //cout<<(char)testinfile.get()<<endl;
                if(line.length()<a1[0]){
                    for(int i=0;i<line.length();i++){
                        chp[i+a0[0]-0x400000]=line.at(i);
                    }
                }else{
                for(int i=0;i<a1[0];i++){
                    chp[i+a0[0]-0x400000]=line.at(i);
                }}
//                for (int i = 0; i < a1[0]; i++)
//                {
//                    c = testinfile.get();
//                    cout<<"get from file:"<<c<<endl;
//                    if (c == '\n')
//                        break;
//                    chp[i + a0[0] - 0x400000] = c;
//                }

                break;
            }
            case 9: // sbrk
            {
                cout<<"dynamic begin:"<<dynamic_data<<endl;
                real_address = (dynamic_data - real_mem)*4 + 0x400000;
                cout<<"difference:"<<dynamic_data-real_mem<<endl;
                dynamic_data = dynamic_data + a0[0];
                cout<<"old dynamic:"<<real_address<<endl;
                v0[0] = real_address;
                break;
            }
            case 10: // exit

                testoutfile.close();
                testinfile.close();
                exit(0);
                break;
            case 11: // print_char
                cout << "a0:" << (char)a0[0] << endl;
                testoutfile << (char)a0[0];
                break;
            case 12: // read_char
                //c = testinfile.get();
                getline(testinfile, line);
                v0[0] = line.at(0);
                break;
            case 13: // open
                while(chp[a0[0]-0x400000+i]!='\0'){
                    //cout<<a0[0]-0x400000+i<<endl;
                    ss << noskipws << chp[a0[0]-0x400000+i];
                    cout<<chp[a0[0]-0x400000+i];
                    i++;
                }
                fn = ss.str();

                a0[0] = open(fn.c_str(), a1[0],a2[0]);
                cout<<"file descripter:"<<a0[0]<<endl;



                break;
            case 14: // read
            {
                int *re = real_mem + (a1[0] - 0x400000) / 4;
                a0[0] = read(a0[0], re, a2[0]);
                break;
            }
            case 15: // write
            {
                char *wr = chp + a1[0] - 0x400000;
                a0[0] = write(a0[0], wr, a2[0]);

                break;
            }
            case 16: // close
                close(a0[0]);
                break;
            case 17: // exit2
                testoutfile.close();
                testinfile.close();
                exit(a0[0]);
                break;
            default:
                break;
            } // switch syscall
            break;
        }        // end of syscall
        case 38: // xor
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rd = ptrmap[(int)rd.to_ulong()];
            Rd[0] = Rs[0] ^ Rt[0];

            break;
        default:
            break;
        }
    }
    else if (op == 2)
    { // j
        Target = target.to_ulong() << 2;
        pc[0] = Target;
    }
    else if (op == 3)
    { // jal
        ra[0] = pc[0];
        Target = target.to_ulong() << 2;
        pc[0] = Target;
    }
    else
    { // I type
        switch (op)
        {
        case 8: // addi
        {

            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Rt[0] = Rs[0] + Immediate;
            break;
        }
        case 24: // addiu
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Immediate = immediate.to_ulong();
            Rt[0] = Rs[0] + Immediate;
            break;
        case 12: // andi
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            Immediate = immediate.to_ulong();
            Rt[0] = Rs[0] & Immediate;
            break;

        case 4: // beq
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            if (Rs[0] == Rt[0])
            {
                pc[0] = pc[0] + Immediate * 4;
            }
            break;
        case 1:
            Rt = ptrmap[(int)rt.to_ulong()];
            Rs = ptrmap[(int)rs.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            if (Rt[0] == 1)
            {
                // bgez
                if (Rs[0] >= 0)
                    pc[0] = pc[0] + Immediate * 4;
            }
            else
            {
                // bltz
                if (Rs[0] < 0)
                    pc[0] = pc[0] + Immediate * 4;
            }
            break;
        case 7: // bgtz
            Rs = ptrmap[(int)rs.to_ulong()];

            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            if (Rs[0] > 0)
            {
                pc[0] = pc[0] + Immediate * 4;
            }
            break;
        case 6: // blez
            Rs = ptrmap[(int)rs.to_ulong()];

            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            if (Rs[0] <= 0)
            {
                pc[0] = pc[0] + Immediate * 4;
            }
            break;

        case 32: // lb
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
                Rt[0] = chp[Rs[0] + Immediate - 0x400000];
                Rt[0]=Rt[0]<<24;
                Rt[0]=Rt[0]>>24;
//            Rt[0] = real_mem[(Rs[0] + Immediate - 0x400000) / 4] << 24;
//            Rt[0] = Rt[0] >> 24;
            //}
//            else{
//                char *loadb=(char*)Rs;
//                Rt[0]=loadb[Immediate];
//                Rt[0]=Rt[0]<<24;
//                Rt[0]=Rt[0]>>24;
//                cout<<"lb:"<<Rt[0]<<endl;

//                bitset<32> cur(Rs[0]);
//                bitset<8> wantbyte;
//                cut(wantbyte,cur,Immediate*8,8);
//                cut(sign,wantbyte,7,1);
//                if(sign.to_ulong()==0){
//                    Rt[0]=wantbyte.to_ulong();
//                }else{
//                    Rt[0]=wantbyte.to_ulong()-65536;
//                }

           // }
            break;
        case 36: // lbu
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
                Rt[0] = chp[Rs[0] + Immediate - 0x400000];
                Rt[0]=Rt[0]<<24;
                Rt[0]=(unsigned)Rt[0]>>24;

            break;
        case 33: // lh
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
            Rt[0] = shp[(Rs[0] + Immediate - 0x400000) / 2];
            Rt[0]=Rt[0]<<16;
            Rt[0]=Rt[0]>>16;
           // }
//            else{
//                short* loadh=(short*)Rs;
//                Rt[0]=loadh[Immediate/2];
//                Rt[0]=Rt[0]<<16;
//                Rt[0]=Rt[0]>>16;
//                cout<<"lh:"<<Rt[0]<<endl;

//            }
            break;
        case 37: // lhu
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
                Rt[0] = shp[(Rs[0] + Immediate - 0x400000) / 2];
                Rt[0]=Rt[0]<<16;
                Rt[0]=(unsigned)Rt[0]>>16;
           // }
            //Rt[0] = real_mem[(Rs[0] + Immediate - 0x400000) / 4] << 16;
            //Rt[0] = (unsigned)Rt[0] >> 16;}
//            else{
//                short* loadhu=(short*)Rs;
//                Rt[0]=loadhu[Immediate/2];
//                Rt[0]=Rt[0]<<16;
//                Rt[0]=(unsigned)Rt[0]>>16;
////                bitset<32> cur(Rs[0]);
////                bitset<16> wanthalf;
////                cut(wanthalf,cur,Immediate*16,16);
////                Rt[0]=wanthalf.to_ulong();

//            }
            break;

        case 11: // sltiu
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            if (Rs[0] < (unsigned)Immediate)
                Rt[0] = 1;
            else
            {
                Rt[0] = 0;
            }
            break;
        case 41: // sh
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
                shp[(Rs[0] + Immediate - 0x400000)/2] = Rt[0]&65535;
            //}
            //real_mem[(Rs[0] + Immediate - 0x400000) / 4]=real_mem[(Rs[0] + Immediate - 0x400000) / 4]>>16;
            //real_mem[(Rs[0] + Immediate - 0x400000) / 4]=real_mem[(Rs[0] + Immediate - 0x400000) / 4]<<16;
            //real_mem[(Rs[0] + Immediate - 0x400000) / 4] = real_mem[(Rs[0] + Immediate - 0x400000) / 4]+Rt[0] & 65535;}
//            else{
//                short *storeh=(short*)Rs;
//                storeh[Immediate/2]=Rt[0]&65535;
//            }
            break;
        case 14: // xori

            Immediate = immediate.to_ulong();

            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];

            Rt[0] = Rs[0] ^ Immediate;
            break;
        case 34: // lwl
        {Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }


            char *lwlptr=(char*)Rt;
            for(int r=0;r<=3-Immediate%4;r++){
                lwlptr[3-Immediate%4-r]=chp[Rs[0]-0x400000+Immediate+(3-Immediate%4)-r];
            }
            break;}
        case 38: // lwr
        {Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            char *lwrptr=(char*)Rt;
            for(int r=0;r<=Immediate%4;r++){
                lwrptr[3-r]=chp[Rs[0]-0x400000+Immediate-r];
            }
            break;}

        case 42: // swl
           { Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            char *swlptr=(char*)Rt;
            for(int r=3-Immediate%4;r>=0;r--){
                chp[Rs[0]+Immediate-0x400000+r]=swlptr[r];
            }
            break;}

        case 46: // swr
           { Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            char *swrptr=(char*)Rt;
            for(int r=Immediate%4;r>=0;r--){
                chp[Rs[0]+Immediate-0x400000+r]=swrptr[3+r-Immediate%4];
            }
            break;}
        case 15: // lui
            Immediate = (immediate.to_ulong()) << 16;
            Rt = ptrmap[(int)rt.to_ulong()];
            Rt[0] = Immediate;
            break;
        case 13: // ori
            Rt = ptrmap[(int)rt.to_ulong()];
            Rs = ptrmap[(int)rs.to_ulong()];
            Immediate = (immediate.to_ulong());
            Rt[0] = Rs[0] | Immediate;
            break;
        case 43: // sw
        {
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
            real_mem[(Rs[0] + Immediate - 0x400000) / 4] = Rt[0];
        //}
//            else{
//                Rs[0]=Rt[0];
//            }
            break;
        }
        case 10: // slti
        {

            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            if (Rs[0] < Immediate)
                Rt[0] = 1;
            else
            {
                Rt[0] = 0;
            }
            break;
        }
        case 5: // bne
        {
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            if (Rs[0] != Rt[0])
            {
                pc[0] = pc[0] + Immediate * 4;
            }
            break;
        }
        case 35: // lw
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
            Rt[0] = real_mem[(Rs[0] + Immediate - 0x400000) / 4];
           // }
//            else{
//                Rt[0]=Rs[0];
//            }
            break;
        case 40: // sb
            cut(sign, immediate, 15, 1);
            if (sign.to_ulong() == 0)
            {
                Immediate = immediate.to_ulong();
            }
            else
            {
                Immediate = immediate.to_ulong() - 65536;
            }
            Rs = ptrmap[(int)rs.to_ulong()];
            Rt = ptrmap[(int)rt.to_ulong()];
            //if(rs.to_ulong()>=28 && rs.to_ulong()<=31){
                chp[Rs[0] + Immediate - 0x400000] = Rt[0]&255;
            //}
//            else{
//                char *storeb=(char*)Rs;
//                storeb[Immediate]=Rt[0]&255;
//                //Rs[0]=Rs[0]>>8;
//                //Rs[0]=Rs[0]<<8;
//                //Rs[0]=Rs[0]+ (Rt[0] & 255);
//            }
            break;

        default:
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    // if (argc < 6)
    //{
    // cout << "Please enter an input file, an output file, and expected output file \n";
    // return 0;
    // }
    real_mem = (int *)malloc(1572864 * sizeof(int));
    memset(real_mem, 0, 1572864 * sizeof(int));
    real_reg = (int *)malloc(sizeof(int) * 32);
    memset(real_reg, 0, sizeof(int) * 32);
    pc = (int *)malloc(4);
    pc[0] = 0x400000;
    lo = (int *)malloc(4);
    memset(lo, 0, 4);
    hi = (int *)malloc(4);
    memset(hi, 0, 4);
    zero = &real_reg[0];
    at = &real_reg[1];
    v0 = &real_reg[2];
    v1 = &real_reg[3];
    a0 = &real_reg[4];
    a1 = &real_reg[5];
    a2 = &real_reg[6];
    a3 = &real_reg[7];
    t0 = &real_reg[8];
    t1 = &real_reg[9];
    t2 = &real_reg[10];
    t3 = &real_reg[11];
    t4 = &real_reg[12];
    t5 = &real_reg[13];
    t6 = &real_reg[14];
    t7 = &real_reg[15];
    s0 = &real_reg[16];
    s1 = &real_reg[17];
    s2 = &real_reg[18];
    s3 = &real_reg[19];
    s4 = &real_reg[20];
    s5 = &real_reg[21];
    s6 = &real_reg[22];
    s7 = &real_reg[23];
    t8 = &real_reg[24];
    t9 = &real_reg[25];
    k0 = &real_reg[26];
    k1 = &real_reg[27];
    gp = &real_reg[28];
    gp[0] = 0x508000;
    sp = &real_reg[29];
    sp[0] = 0xa00000;
    fp = &real_reg[30];
    fp[0] = 0xa00000;
    ra = &real_reg[31];

    map<string, int> LabelTable;
    map<string, Registers> regmap = setRegmap();
    map<int, signed int *> ptrmap = setPtrmap();
    set<int> checkpoints;
    deque<string> instructions;
    deque<string> refine;
    string filename1 = argv[1]; // mips file
    refine = ReFine(filename1);
    instructions = pass1_step1(refine); // get insturctions
    LabelTable = pass1_step2(refine);   // get all labels
    string filename2 = "output.txt";
    pass2(instructions, filename2, regmap, LabelTable); // write output.txt
    string filename3 = argv[2];
    string filename4 = argv[3];
    checkpoints = init_checkpoints(filename4);
    string filename5 = argv[4];
    testinfile.open(filename5.c_str(), ios::in);
    string filename6 = argv[5];
    testoutfile.open(filename6.c_str(), ios::out);
    if (compare_files(filename2, filename3) == 0)
    {
        cout << "ALL PASSED! CONGRATS :)" << endl;
    }
    else
    {
        cout << "YOU DID SOMETHING WRONG :(" << endl;
    }
    store_text(filename3, real_mem);
    cout<<"real_mem"<<real_mem<<endl;
    // cout << "real_mem: " << real_mem << endl;
    int start_dynamic = store_data(filename1, real_mem);
    dynamic_data=real_mem+0x100000/4+start_dynamic;
    cout<<"dynamic here:"<<dynamic_data<<endl;

    // dynamic_data = real_mem + (0x50000 - 0x40000) / 4 + start_dynamic;
    // cout << "here" << endl;
    // cout << real_mem << endl;

    while (true)
    {
        ptr = real_mem + (pc[0] - 0x400000) / 4;
        // cout << ptr[0] << endl;
        int line_num = (pc[0] - 0x400000) / 4;
        pc[0] = pc[0] + 4;
        execution(ptr, ptrmap, filename5, filename6, pc);
        set<int>::iterator it;
        it = find(checkpoints.begin(), checkpoints.end(), line_num);
        if (it != checkpoints.end())
        {
            checkpoint_memory(line_num, checkpoints);
            checkpoint_register(line_num, checkpoints);
        }

    }
    testoutfile.close();
    testinfile.close();
    return 0;
}
