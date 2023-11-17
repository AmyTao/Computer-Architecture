#ifndef _mainlib_h
#define _mainlib_h

#include <iostream>
#include <queue>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

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

string decimalToBinary(enum Registers rg);
string decimalToBinary(int rg);
string toLabeladdress(int rg);
deque<string> ReFine(string filename);
deque<string> pass1_step1(deque<string> filename);
map<string, int> pass1_step2(deque<string> filename);
void pass2(deque<string> instructions, string filename, map<string, Registers> regmap,map<string, int> ltp);

#endif