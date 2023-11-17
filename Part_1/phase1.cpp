#include "mainlib.h"
using namespace std;
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
            line.erase(0, line.find_first_not_of("\r\t\n ")); //delete meaningless spaces
            line.erase(line.find_last_not_of("\r\t\n ") + 1);
            if (line.substr(0, 5) == ".text") //distinguish .text part
            {
                while (getline(inFile, line))
                {
                    line.erase(0, line.find_first_not_of("\r\t\n "));
                    line.erase(line.find_last_not_of("\r\t\n ") + 1);
                    if (line.length() == 0 || line.find('#') == 0) //delete comments
                    {
                        continue;
                    }
                    else
                    {
                        int comment = line.find('#'); //delete comments
                        if (comment != -1)
                        {
                            line = line.substr(0, comment);
                        }
                        line.erase(line.find_last_not_of("\r\t\n ") + 1);
                        if (line.length() != 0)
                        {
                            reFineInFile.push_back(line); //put into the refine=instructions+labels
                        }
                    }
                }
            }
        }
    }
    else
    {
        cout << "error"; //if the file doesn't open successfully
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
        if (line.find(':') != -1) //find instrctions after ':'
        {
            if (line.find(':') != line.length() - 1)
            {
                string ist = line.substr(line.find(':') + 1);
                ist.erase(0, ist.find_first_not_of("\r\t\n ")); //delete redundant spaces
                ist.erase(ist.find_last_not_of("\r\t\n ") + 1);
                instructions.push_back(ist);
            }
        }
        else
        {
            instructions.push_back(line); //add instructions directly
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
        if (line.find(':') != -1) //find labels
        {
            if (line.find(':') == line.length() - 1) //labels with \n at the end
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