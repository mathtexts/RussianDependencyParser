#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <QVector>
#include <QString>
#include <QList>
#include <QPair>
#include <QByteArray>
#include <marisa.h>

typedef QPair<QString, QString> StringPair;
typedef unsigned int uint;

class Dictionary{
    marisa::Keyset kWords;
    marisa::Trie words;
    QVector<uint> *normalForms;
    QVector<uint> *wordTags;
    marisa::Keyset kAllNormalForms;
    marisa::Trie allNormalForms;
    marisa::Keyset kAllTags;
    marisa::Trie allTags;
public:
    Dictionary();
    virtual ~Dictionary();
    void addWord(QString word);
    void addNormalForm(QString normalform);
    void addTags(QString tags);
    void build();
    void insert(QString form, QString normalform, QString tags);
    QList<StringPair> values(QString key);
};

#endif
