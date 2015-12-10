#ifndef __PARADIGM_H__
#define __PARADIGM_H__

#include <string>
#include <vector>
#include <utility>
#include <fstream>

using namespace std;

typedef pair<string, string> strPair;

class Paradigm{
    int size;
    int *array;
public:
    Paradigm();
    Paradigm(const Paradigm& other);
    virtual ~Paradigm() { delete [] array; }
    bool operator==(const Paradigm& other) const;
    void build(vector<strPair>& lines, vector<string>& prefixes, 
               vector<string>& suffixes, vector<string>& tags);
    static string LCSubstr(const string& str1, const string& str2);
    int getPrefix(int i) const { return array[i]; }
    int getSuffix(int i) const { return array[size + i]; }
    int getTags(int i) const { return array[size * 2 + i]; }
    void save(ofstream& ofs) const;
    void load(ifstream& ifs);
};

#endif
