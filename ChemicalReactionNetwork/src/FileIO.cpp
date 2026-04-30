//
//  FileIO.cpp
//  OpenGL
//
//  Created by Andrew Diggs on 8/21/22.
//

#include "FileIO.hpp"
#include "Reax.hpp"
#include <cstdlib>
#include <cstring>
#include <math.h>

extern Params* params;

std::string r_int = "[[:digit:]]+";
std::string r_float = "[[:digit:]]+\\.[[:digit:]]+";
std::string r_space = "[[:blank:]]";
std::string r_exp_float = "[[:digit:]]+\\.[[:digit:]]+e\\+[[:digit:]]+";
std::string r_comment = "^#.*";
std::string dump_file = ".dump";
std::string dat_file = ".dat";

int get_int(const char* line);
float get_float(const char* line);
float3 get_float3(const char* line);

String_List::String_List(std::string line){
    unsigned int word_indx = 0;
    for(int i = 0; i < line.size(); i++){
        char word[100] = "";
        if(std::isspace(line[i])){
            continue;
        }
        else{
            word_indx = 0;
            while((i < line.length()) && (!std::isspace(line[i]))){
                word[word_indx] = line[i];
                word_indx++;
                i++;
            }
            m_words[m_num_words].assign(word);
            m_num_words++;
        }
       
    }
}

String_List::String_List(const char* line){
    unsigned int word_indx = 0;
    for(int i = 0; i < strlen(line); i++){
        char word[100] = "";
        if(std::isspace(line[i])){
            continue;
        }
        else{
            word_indx = 0;
            while((i < strlen(line)) && (!std::isspace(line[i]))){
                word[word_indx] = line[i];
                word_indx++;
                i++;
            }
            m_words[m_num_words].assign(word);
            m_num_words++;
        }
       
    }
}

String_List::~String_List(){}

std::string& String_List::operator[](const int index){
    return this->m_words[index];
}

bool Check_File(const char* file){
    std::ifstream infile(file, std::ios_base::in);
    if (!infile.is_open()){
        return false;
    }
    else{
        infile.close();
        return true;
    }
}
int FT_Hash(const char* ft){
    int val = 0;
    for(int i =0; i<strlen(ft); i++){
        val +=ft[i];
    }
    return val % 10;
}


bool match(std::string input, std::string m_type){
    std::regex reg (m_type);
    return std::regex_match(input,reg);
}

bool search(std::string input, std::string m_type){
    std::regex reg (m_type);
    return std::regex_search(input,reg);
}

bool match_int(std::string input){
    return match(input, r_int);
}
bool match_float(std::string input){
    return match(input, r_float) || match(input, r_exp_float);
    
}
bool match_space(std::string input){
    return match(input, r_space);
}


bool match_comment(std::string input){
    return  match(input, r_comment);
}

bool match_atom_file(std::string input){
    return search(input, dump_file) || search(input, dat_file);
}

bool match_number(std::string input){
    return match_int(input) || match_float(input) || match(input, r_exp_float);
}


bool Comment_or_Blank(const char* line){
    return match_comment(line) || match_space(line);
}

bool match_MaxIter(const char* line){return std::regex_match(line,re_MaxIter);}
bool match_MaxTime(const char* line){return std::regex_match(line,re_MaxTime);}
bool match_Num_Atoms(const char* line){return std::regex_match(line,re_NumAts);}
bool match_Num_Res(const char* line){return std::regex_match(line,re_NumRes);}
bool match_Reservoirs(const char* line){return std::regex_match(line,re_Reservoir);}
bool match_Freq(const char* line){return std::regex_match(line,re_Freq);}
bool match_Temp(const char* line){return std::regex_match(line,re_Temp);}
bool match_Params(const char* line){return std::regex_match(line,re_Params);}
bool match_ResLine(const char* line){
    String_List sl(line);
    int count = 0;
    for(int i = 0; i < sl.m_num_words; i++){
        std::string w = sl[i];
        if(match_number(w)){count++;}
    }
    return count == 3;
}



unsigned int Get_Num_El(std::string line){
    bool ws = true;
    unsigned int count = 0;
    for(int i = 0; i < line.size(); i++){
        if(std::isspace(line[i]) && ws){
            continue;
        }
        else if(!ws && !std::isspace(line[i])){
            continue;
        }
        else if(std::isspace(line[i]) && !ws){
            ws = true;
            count++;
        }
        else if(ws && !std::isspace(line[i])){
            ws = false;
        }
    }
    if(!ws){
        count ++;
    }
    return count;
}


void Get_Vals(std::string line, float* tmp, int* num){
    unsigned int count = 0;
    char word[100];
    unsigned int num_vals = 0;
    for(int i = 0; i < line.size(); i++){
        count = 0;
        while(!std::isspace(line[i]) && (i < line.size())){
            word[count] = line[i];
            count++;
            i++;
        }
        if(count){
            tmp[num_vals] = std::atof(word);
            num_vals++;
        }
    }
    *num = num_vals;
    
}

int get_int(const char* line)
{
    unsigned int count = 0;
    bool get = false;
    char word[100];
    unsigned int num_char = std::strlen(line);
    for(int i = 0; i < num_char; i++){
        char c = line[i];
        if(get){
            if(std::isalnum(c) || (c == '-')){
                word[count] = c;
                count++;
            }
        }
        else if(c == '='){get = true;}
    }
    if(count == 0){printf("%s\n does not contain an int\n", line); exit(24);}
    return std::atoi(word);
}

float get_float(const char* line)
{
    unsigned int count = 0;
    bool get = false;
    char word[100];
    unsigned int num_char = std::strlen(line);
    for(int i = 0; i < num_char; i++){
        char c = line[i];
        if(get){
            if(std::isalnum(c) || (c == '-')){
                word[count] = c;
                count++;
            }
        }
        else if(c == '='){get = true;}
    }
    if(count == 0){printf("%s\n does not contain a float\n", line); exit(24);}
    return std::atof(word);
}

float3 get_float3(const char* line)
{
    float tmp[3];
    unsigned int count = 0;
    char word[100];
    unsigned int num_vals = 0;
    unsigned int num_char = std::strlen(line);
    for(int i = 0; i < num_char; i++){
        count = 0;
        while(!std::isspace(line[i]) && (i < num_char)){
            word[count] = line[i];
            count++;
            i++;
        }
        if(count){
            tmp[num_vals] = std::atof(word);
            num_vals++;
        }
    }
    float x, y, z;
    if(num_vals == 3){
        x = tmp[0];
        y = tmp[1];
        z = tmp[2];
    }
    else{printf("%s\n is not formatted for a float3\n", line); exit(24);}
    return float3(x,y,z);
}

//Why do I have this?
unsigned int Hash(const char* word){
    unsigned int hash_int = 0;
    for (int i = 0; i<std::strlen(word); i++){
        hash_int+=word[i];
    }
    return hash_int;
}


char* Read_Line(std::ifstream& in_file){
    char* line = nullptr;
    char temp[100];
    in_file.getline(temp, 100);
    int num_c = (int)in_file.gcount();
    line = (char*)malloc(num_c*sizeof(char));
    for(int i = 0; i< num_c; i++){
        line[i] = temp[i];
    }
    return line;
}


void Set_Params(const char* file){
    std::ifstream infile(file, std::ios_base::in);
    if (!infile.is_open()){
        std::cout << "File did not open!!" << std::endl;
        exit(9);
    }
    
    int num_res = 0;
    std::string line;
    const char* c_line;
    while (std::getline(infile,line)){
        c_line = line.c_str();
        if(Comment_or_Blank(c_line)){continue;}
        else if(match_MaxIter(c_line)){params-> MAX_ITER = get_int(c_line);}
        else if(match_MaxTime(c_line)){params-> MAX_TIME = get_int(c_line);}
        else if(match_Num_Atoms(c_line)){params->NUM_ATOMS = get_int(c_line);}
        else if(match_Num_Res(c_line)){num_res = get_int(c_line);}
        else if(match_Temp(c_line)){params->TEMP = get_float(c_line);}
        else if(match_Freq(c_line)){params->ATTEMPT_FREQUENCY = get_float(c_line);}
        else if(match_Reservoirs(c_line)){
            for(int i = 0; i < num_res; i++){
                std::getline(infile,line);
                float3 bar_line = get_float3(line.c_str());
                params->BARRIERS[i] = bar_line;
                params->NUM_RES++;
            }
        }
    }
}
