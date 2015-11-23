#include <QVector>
#include <marisa.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;
typedef unsigned char uchar;

uint getIndexOf(const char *key, const marisa::Trie& trie)
{
    uint res = 0;
    marisa::Agent agent;
    agent.set_query(key);
    if(trie.lookup(agent))
        res = agent.key().id();
    return res;
}

int main(int argc, char **argv)
{
    marisa::Trie words;
    marisa::Trie tags;
    marisa::Trie ends;
    marisa::Keyset kWords;
    marisa::Keyset kTags;
    marisa::Keyset kEnds;
    QVector<uint> *wordsToTags;
    QVector<uint> *wordsToNormalForms;
    QVector<uint> *endsToTags;
    QVector<uint> *endsToCount;

    if (argc != 6) {
        printf("Usage: genhashtable dictfile.txt map.bin words.trie tags.trie ends.trie\n");
        return 0;
    }

    FILE *file = fopen(argv[1], "r");

    if (file == NULL) {
        fprintf(stderr, "ERROR: input file not found\n");
        return 1;
    }

    char *line = NULL;
    char lineParts[3][256];
    char form[256], tag[256];
    char normalForm[256];
    bool isfirst = false;
    size_t length = 0;
    int partsCount = 0;

    //first iteration
    while (getline(&line, &length, file) != -1) {
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
        if (partsCount >= 2) {
            strcpy(form, lineParts[0]); 
            strcpy(tag, lineParts[1]);
            if (partsCount == 3){
                int l = strlen(tag);
                tag[l] = ',';
                strcpy(tag + l + 1, lineParts[2]);
                tag[l + 1 + strlen(lineParts[2])] = '\0';
            }
            kWords.push_back(form);
            kTags.push_back(tag);
            if(strlen(form) > 3)
                kEnds.push_back(form + strlen(form) - 3);
        }
        free(line);
        line = NULL;
    }

    words.build(kWords);
    words.save(argv[3]);
    tags.build(kTags);
    tags.save(argv[4]);
    ends.build(kEnds);
    ends.save(argv[5]);
    wordsToTags = new QVector<uint>[words.size()];
    wordsToNormalForms = new QVector<uint>[words.size()];
    endsToTags = new QVector<uint>[ends.size()];
    endsToCount = new QVector<uint>[ends.size()];

    fseek(file, 0, SEEK_SET);
    free(line);
    line = NULL;
    //second iteration
    while (getline(&line, &length, file) != -1) {
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
        if (isfirst) {
            if (partsCount >= 2 && strncmp(lineParts[1], "VERB,", 5) && 
                  strncmp(lineParts[1], "PRTF,", 5) &&
                  strncmp(lineParts[1], "PRTS,", 5) &&
                  strncmp(lineParts[1], "GRND,", 5)){
                strcpy(normalForm, lineParts[0]);
            }
            isfirst = false;
        }
        if (partsCount > 2 && !strncmp(lineParts[1], "INFN,", 5)) {
            strcpy(normalForm, lineParts[0]);
        }
        if (partsCount < 1) {
            free(line);
            line = NULL;
            continue;
        }
        if (partsCount == 1) {
            isfirst = true;
            free(line);
            line = NULL;
            continue;
        }
        strcpy(form, lineParts[0]); 
        strcpy(tag, lineParts[1]);
        if (partsCount == 3){
            int l = strlen(tag);
            tag[l] = ',';
            strcpy(tag + l + 1, lineParts[2]);
            tag[l + 1 + strlen(lineParts[2])] = '\0';
        }

        uint temp1, temp2;
        int index;
        temp1 = getIndexOf(form, words);
        temp2 = getIndexOf(tag, tags);

        wordsToNormalForms[temp1].append(getIndexOf(normalForm, words));
        wordsToTags[temp1].append(temp2);
        if(strlen(form) > 3){
            temp1 = getIndexOf(form + strlen(form) - 3, ends);
            if((index = endsToTags[temp1].indexOf(temp2)) == -1){
                endsToTags[temp1].append(temp2);
                endsToCount[temp1].append(1);
            }else{
                endsToCount[temp1][index]++;
            }
        }

        free(line);
        line = NULL;
    }
    fclose(file);
    free(line);
    uint wc = words.size();
    uint ec = ends.size();
    words.clear();
    ends.clear();
    tags.clear();

    file = fopen(argv[2], "w");
    for(uint i = 0; i < wc; i++){
        uchar size = (uchar)wordsToNormalForms[i].size();
        fwrite(&size, 1, 1, file);
        for(uchar j = 0; j < size; j++){
            fwrite(&wordsToNormalForms[i][j], sizeof(uint), 1, file);
            fwrite(&wordsToTags[i][j], sizeof(uint), 1, file);
        }
        wordsToNormalForms[i].clear();
        wordsToTags[i].clear();
    }
    delete [] wordsToNormalForms;
    delete [] wordsToTags;
    for(uint i = 0; i < ec; i++){
        uchar size = (uchar)endsToTags[i].size();
        fwrite(&size, 1, 1, file);
        for(uchar j = 0; j < size; j++){
            fwrite(&endsToTags[i][j], sizeof(uint), 1, file);
            fwrite(&endsToCount[i][j], sizeof(uint), 1, file);
        }
        endsToTags[i].clear();
        endsToCount[i].clear();
    }
    delete [] endsToTags;
    delete [] endsToCount;
    fclose(file);

    return 0;
}
