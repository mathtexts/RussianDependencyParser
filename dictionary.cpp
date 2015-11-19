#include "dictionary.h"

Dictionary::Dictionary()
{
    kWords.clear();
    words.clear();
    normalForms = NULL;
    wordTags = NULL;
    kAllTags.clear();
    allTags.clear();
    kAllNormalForms.clear();
    allNormalForms.clear();
}

Dictionary::~Dictionary()
{
    for(uint i = 0; i < words.size(); i++){
        normalForms[i].clear();
        wordTags[i].clear();
    }
    delete [] normalForms;
    delete [] wordTags;
    kWords.clear();
    words.clear();
    kAllTags.clear();
    allTags.clear();
    kAllNormalForms.clear();
    allNormalForms.clear();
}
    
void Dictionary::addWord(QString word)
{
    kWords.push_back(word.toUtf8().data());
}

void Dictionary::addTags(QString tags)
{
    kAllTags.push_back(tags.toUtf8().data());
}

void Dictionary::addNormalForm(QString normalform)
{
    kAllNormalForms.push_back(normalform.toUtf8().data());
}

void Dictionary::build()
{
    words.build(kWords);
    kWords.clear();
    allTags.build(kAllTags);
    kAllTags.clear();
    allNormalForms.build(kAllNormalForms);
    kAllNormalForms.clear();
    normalForms = new QVector<uint>[words.size()];
    wordTags = new QVector<uint>[words.size()];
}

void Dictionary::insert(QString form, QString normalform, QString tags)
{
    uint id = 0, tid = 0, nfid = 0;

    marisa::Agent aForm;
    aForm.set_query(form.toUtf8().data());
    if(words.lookup(aForm))
        id = aForm.key().id();
    else
        return;

    marisa::Agent aTags;
    aTags.set_query(tags.toUtf8().data());
    if(allTags.lookup(aTags))
        tid = aTags.key().id();
    else
        return;

    marisa::Agent aNormalForm;
    aNormalForm.set_query(normalform.toUtf8().data());
    if(allNormalForms.lookup(aNormalForm))
        nfid = aNormalForm.key().id();
    else
        return;

    wordTags[id].append(tid);
    normalForms[id].append(nfid);
}

QList<StringPair> Dictionary::values(QString key)
{
    marisa::Agent agent;
    agent.set_query(key.toUtf8().data());
    QList<StringPair> res;
    if(words.lookup(agent)){
        uint id = agent.key().id();
        QVector<uint>::const_iterator itr1 = normalForms[id].begin();
        QVector<uint>::const_iterator itr2 = wordTags[id].begin();
        for( ; itr1 != normalForms[id].end(); itr1++, itr2++){
            marisa::Agent aNormalForm;
            aNormalForm.set_query(*itr1);
            allNormalForms.reverse_lookup(aNormalForm);
            QString nf = QString::fromUtf8(aNormalForm.key().ptr(), aNormalForm.key().length());
            marisa::Agent aTags;
            aTags.set_query(*itr2);
            allTags.reverse_lookup(aTags);
            QString tags = QString::fromUtf8(aTags.key().ptr(), aTags.key().length());
            res.append(StringPair(nf, tags));
        }
    }
    return res;
}
