#include "dictionary.h"

Dictionary::Dictionary()
{
    keyset.clear();
    words.clear();
    normalForms = NULL;
    wordTags = NULL;
    allTags.clear();
}

Dictionary::~Dictionary()
{
    for(uint i = 0; i < words.size(); i++){
        normalForms[i].clear();
        wordTags[i].clear();
    }
    delete [] normalForms;
    delete [] wordTags;
    keyset.clear();
    words.clear();
    allTags.clear();
}
    
void Dictionary::addWord(QString word)
{
    keyset.push_back(word.toUtf8().data());
}

void Dictionary::addTags(QString tags)
{
    allTags.append(tags);
}

void Dictionary::build()
{
    words.build(keyset);
    keyset.clear();
    normalForms = new QVector<QString>[words.size()];
    wordTags = new QVector<int>[words.size()];
}

void Dictionary::insert(QString form, QString normalform, QString tags)
{
    marisa::Agent agent;
    agent.set_query(form.toUtf8().data());
    if(words.lookup(agent)){
        uint id = agent.key().id();
        int index = 0;
        normalForms[id].append(normalform);
        if((index = allTags.indexOf(tags)) != -1)
            wordTags[id].append(index);
    }
}

QList<StringPair> Dictionary::values(QString key)
{
    marisa::Agent agent;
    agent.set_query(key.toUtf8().data());
    QList<StringPair> res;
    if(words.lookup(agent)){
        uint id = agent.key().id();
        QVector<QString>::const_iterator itr1 = normalForms[id].begin();
        QVector<int>::const_iterator itr2 = wordTags[id].begin();
        for( ; itr1 != normalForms[id].end(); itr1++, itr2++)
            res.append(StringPair(*itr1, allTags[*itr2]));
    }
    return res;
}
