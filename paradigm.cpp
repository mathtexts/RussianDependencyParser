#include "paradigm.h"
#include <algorithm>
#include <iostream>

Paradigm::Paradigm()
{
    size = 0;
    array = NULL;
}

Paradigm::Paradigm(const Paradigm& other)
{
    size = other.size;
    array = new int[size * 3];
    for(int i = 0; i < size * 3; i++)
        array[i] = other.array[i];
}

void Paradigm::load(ifstream& ifs)
{
    ifs>>size;
    array = new int[size * 3];
    for(int i = 0; i < size * 3; i++)
        ifs>>array[i];
}

void Paradigm::save(ofstream& ofs) const
{
    ofs<<size<<endl;
    for(int i = 0; i < size * 3; i++)
        ofs<<array[i]<<" ";
    ofs<<endl;
}

string Paradigm::LCSubstr(const string& str1, const string& str2)
{
    int m = str1.size();
    int n = str2.size();
    int z = 0;
    string res;
    int L[m][n];

    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++){
            if(str1[i] == str2[j]){
                if(i == 0 || j == 0)
                    L[i][j] = 1;
                else
                    L[i][j] = L[i - 1][j - 1] + 1;
                if(L[i][j] > z){
                    z = L[i][j];
                    res = str1.substr(i - z + 1, z);
                }
            }else{
                L[i][j] = 0;
            }
        }
    return res;
}

bool Paradigm::operator==(const Paradigm& other) const
{
    if(size != other.size)
        return false;
    for(int i = 0; i < size * 3; i++)
        if(array[i] != other.array[i])
            return false;
    return true;
}

void Paradigm::build(vector<strPair>& lines, vector<string>& prefixes, 
                     vector<string>& suffixes, vector<string>& tags)
{
    size = lines.size();
    array = new int[size * 3];
    string stem = lines.front().first;
    string temp;
    size_t pos;
    vector<string>::iterator it;

    for(vector<strPair>::iterator i = lines.begin() + 1; i != lines.end(); i++)
        stem = LCSubstr(stem, (*i).first);
    for(vector<strPair>::iterator i = lines.begin(); i != lines.end(); i++){
        int j = i - lines.begin();
        pos = (*i).first.find(stem);
        temp = (*i).first.substr(0, pos);
        it = find(prefixes.begin(), prefixes.end(), temp);
        if(it != prefixes.end()){
            array[j] = it - prefixes.begin();
        }else{
            prefixes.push_back(temp);
            array[j] = prefixes.size() - 1;
        }

        temp = (*i).first.substr(pos + stem.size());
        it = find(suffixes.begin(), suffixes.end(), temp);
        if(it != suffixes.end()){
            array[size + j] = it - suffixes.begin();
        }else{
            suffixes.push_back(temp);
            array[size + j] = suffixes.size() - 1;
        }

        it = find(tags.begin(), tags.end(), (*i).second);
        if(it != tags.end()){
            array[2 * size + j] = it - tags.begin();
        }else{
            tags.push_back((*i).second);
            array[2 * size + j] = tags.size() - 1;
        }
    }
}
