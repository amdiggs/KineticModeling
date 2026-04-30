//
//  FileIO.hpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/21/22.
//

#ifndef FileIO_hpp
#define FileIO_hpp

#include <stdio.h>
#include <iostream>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#define MAX_WORD 250
#define MAX_TS 1000




bool Check_File(const char* file);
bool Check_FT(const char* ft);

//needed by Atom constructor
struct Data_2D{
    float** m_dat = NULL;
    int m_num_el = 0;
    int m_num_rows = 0;
    
    Data_2D(const char* file);
    ~Data_2D();
};

struct String_List{
    std::string m_words[25];
    std::string& operator[](const int index);
    int m_num_words = 0;
    String_List(std::string line);
    String_List(const char* line);
    ~String_List();
    void print();
};

const std::regex re_Params(R"(^\s*Parameters:\s*$)");
const std::regex re_NumRes(R"(^\s*num_reservoir\s*=\s*\d+.*$)");
const std::regex re_NumAts(R"(^\s*num_atoms\s*=\s*\d+.*$)");
const std::regex re_MaxIter(R"(^\s*max_iter\s*=\s*\d+.*$)");
const std::regex re_MaxTime(R"(^\s*max_time\s*=\s*\d+.*$)");
const std::regex re_Freq(R"(^\s*freq\s*=\s*\d+.*$)");
const std::regex re_Temp(R"(^\s*temperature\s*=\s*\d+.*$)");
const std::regex re_Reservoir(R"(^\s*Reservoirs:\s*$)");
//const std::regex re_ResLine(R"(^\s*(\d+)?(\\s*-?\d+\.\d+\s*-?\d+\.\d+\s*$)");


bool match(std::string input, std::string m_type);
bool search(std::string input, std::string m_type);
bool match_int(std::string input);
bool match_float(std::string input);
bool match_space(std::string input);
bool match_atom_file(std::string input);
bool match_comment(std::string input);

bool match_MaxIter(const char* line);
bool match_MaxTime(const char* line);
bool match_Num_Atoms(const char* line);
bool match_Num_Res(const char* line);
bool match_Freq(const char* line);
bool match_Temp(const char* line);
bool match_Reservoirs(const char* line);
bool match_Params(const char* line);
bool match_ResLine(const char* line);

unsigned int Get_Num_El(std::string line);

unsigned int Hash(const char* line);

int get_int(std::string int_str);
float get_float(std::string fl_str);

bool ITEM(std::string str, std::string reg_ex);
char* Read_Line(std::ifstream& infile);
char** Read_Input(const char* file,int& num_blocks, int* block_idx, int& num_lines);

void Set_Params(const char* file);

void Write_Dat(float* dat, int num, const char* file_name);

#endif /* FileIO_hpp */
