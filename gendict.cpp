#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "paradigm.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dawgdic/dawg-builder.h>
#include <dawgdic/dictionary-builder.h>

typedef unsigned int uint;
typedef unsigned char uchar;

void rpl(char *str)
{
    char *temp;
    while((temp = strstr(str, "Ё")))
        strncpy(temp, "Е", strlen("Е"));
}

int main(int argc, char **argv)
{
    if(argc != 3){
        printf("Usage: genhashtable dictfile.txt dictdir\n");
        return 0;
    }

    vector<Paradigm> paradigms;
    vector<string> prefixes;
    vector<string> suffixes;
    vector<string> tags;
    vector<strPair> lines;
    vector<string> wordsV;
    map<string, int> endsM;
    dawgdic::DawgBuilder words_builder;
    dawgdic::Dawg words_dawg;
    dawgdic::Dictionary words;
    dawgdic::DawgBuilder ends_builder;
    dawgdic::Dawg ends_dawg;
    dawgdic::Dictionary ends;

    FILE *file = fopen(argv[1], "r");

    if(file == NULL){
        fprintf(stderr, "ERROR: input file not found\n");
        return 1;
    }

    char *line = NULL;
    char lineParts[3][256];
    size_t length = 0;
    int partsCount = 0;

    cout<<"Processing source dictionary..."<<endl;
    while(getline(&line, &length, file) != -1){
        partsCount = 0;
        int j = 0, i = 0;
        bool flag = false;
        while(line[i] != '\n'){
            if(line[i] == ' ' || line[i] == '\t'){
                lineParts[partsCount][j] = '\0';
                partsCount++;
                j = 0;
                i++;
                while(line[i] == ' ' || line[i] == '\t')
                    i++;
            }else{
                flag = true;
                lineParts[partsCount][j] = line[i];
                i++;
                j++;
            }
        }
        if(flag){
            lineParts[partsCount][j] = '\0';
            partsCount++;
        }
        if(partsCount < 1){
            free(line);
            line = NULL;
            int index = -1;
            Paradigm cur;
            cur.build(lines, prefixes, suffixes, tags);
            for(vector<Paradigm>::iterator i = paradigms.begin(); 
                    i != paradigms.end(); i++)
                if(*i == cur){
                    index = i - paradigms.begin();
                    break;
                }
            if(index == -1){
                paradigms.push_back(cur);
                index = paradigms.size() - 1;
            }
            for(vector<strPair>::iterator i = lines.begin(); i != lines.end(); i++){
                string w = (*i).first + " " + to_string(index) + " " + 
                    to_string(i - lines.begin());
                wordsV.push_back(w);
            }
            lines.clear();
            continue;
        }
        if(partsCount == 1){
            lines.clear();
            free(line);
            line = NULL;
            continue;
        }
        if(partsCount == 3){
            int l = strlen(lineParts[1]);
            lineParts[1][l] = ',';
            strcpy(lineParts[1] + l + 1, lineParts[2]);
            lineParts[1][l + 1 + strlen(lineParts[2])] = '\0';
        }

        rpl(lineParts[0]);
        lines.push_back(make_pair(lineParts[0], lineParts[1]));

        if(strlen(lineParts[0]) > 3){
            string e = lineParts[0] + strlen(lineParts[0]) - 3;
            string eh;
            vector<string>::iterator it = find(tags.begin(), tags.end(), lineParts[1]);
            if(it != tags.end()){
                eh = e + " " + to_string(it - tags.begin());
            }else{
                tags.push_back(lineParts[1]);
                eh = e + " " + to_string(tags.size() - 1);
            }
            if(endsM.find(eh) == endsM.end())
                endsM[eh] = 0;
            else
                endsM[eh]++;
        }

        free(line);
        line = NULL;
    }
    fclose(file);
    free(line);

    struct stat sb;
    if(stat(argv[2], &sb) == -1)
        if(mkdir(argv[2], 0777) == -1){
            fprintf(stderr, "ERROR: couldn`t create dictionary directory\n");
            return 1;
        }

    cout<<"Writing prefixes..."<<endl;
    string dir = argv[2];
    string path = dir + "/prefixes";
    ofstream ofs(path);
    ofs<<prefixes.size()<<endl;
    for(vector<string>::iterator i = prefixes.begin(); i != prefixes.end(); i++)
        ofs<<*i<<endl;
    ofs.close();
    prefixes.clear();

    cout<<"Writing suffixes..."<<endl;
    path = dir + "/suffixes";
    ofs.open(path);
    ofs<<suffixes.size()<<endl;
    for(vector<string>::iterator i = suffixes.begin(); i != suffixes.end(); i++)
        ofs<<*i<<endl;
    ofs.close();
    suffixes.clear();

    cout<<"Writing tags..."<<endl;
    path = dir + "/tags";
    ofs.open(path);
    ofs<<tags.size()<<endl;
    for(vector<string>::iterator i = tags.begin(); i != tags.end(); i++)
        ofs<<*i<<endl;
    ofs.close();
    tags.clear();

    cout<<"Writing paradigms..."<<endl;
    path = dir + "/paradigms";
    ofs.open(path);
    ofs<<paradigms.size()<<endl;
    for(vector<Paradigm>::iterator i = paradigms.begin(); i != paradigms.end(); i++)
        (*i).save(ofs);
    ofs.close();
    paradigms.clear();

    cout<<"Sorting words vector..."<<endl;
    sort(wordsV.begin(), wordsV.end());
    cout<<"Writing words DAWG..."<<endl;
    for(vector<string>::iterator i = wordsV.begin(); i != wordsV.end(); i++)
        if(!words_builder.Insert((*i).c_str())){
            cerr<<"Couldn`t add word \""<<*i<<"\" to dictionary"<<endl;
            return 1;
        }
    words_builder.Finish(&words_dawg);
    if(!dawgdic::DictionaryBuilder::Build(words_dawg, &words)){
        cerr<<"Couldn`t create words dictionary"<<endl;
        return 1;
    }
    wordsV.clear();
    path = dir + "/words.dawg";
    ofs.open(path);
    words.Write(&ofs);
    ofs.close();

    cout<<"Writing ends DAWG..."<<endl;
    for(map<string, int>::iterator i = endsM.begin(); i != endsM.end(); i++){
        string en = i->first + " " + to_string(i->second);
        if(!ends_builder.Insert(en.c_str())){
            cerr<<"Couldn`t add ending \""<<en<<"\" to dictionary"<<endl;
            return 1;
        }
    }
    ends_builder.Finish(&ends_dawg);
    if(!dawgdic::DictionaryBuilder::Build(ends_dawg, &ends)){
        cerr<<"Couldn`t create endings dictionary"<<endl;
        return 1;
    }
    endsM.clear();
    path = dir + "/ends.dawg";
    ofs.open(path);
    ends.Write(&ofs);
    ofs.close();

    cout<<"Done."<<endl;
    return 0;
}
