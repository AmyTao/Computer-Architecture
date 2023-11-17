#include "mainlib.h"
using namespace std;

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

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        cout << "Please enter an input file, an output file, and expected output file \n";
        return 0;
    }
    map<string, int> LabelTable;
    map<string, Registers> regmap = setRegmap();
    deque<string> instructions;
    deque<string> refine;
    string filename1 = argv[1]; //mips file
    refine = ReFine(filename1);
    instructions = pass1_step1(refine); //get insturctions
    LabelTable = pass1_step2(refine);   //get all labels
    string filename2 = argv[2];
    pass2(instructions, filename2, regmap, LabelTable); //write output.txt
    string filename3 = argv[3];
    if (compare_files(filename2, filename3) == 0)
    {
        cout << "ALL PASSED! CONGRATS :)" << endl;
    }
    else
    {
        cout << "YOU DID SOMETHING WRONG :(" << endl;
    }
    return 0;
}