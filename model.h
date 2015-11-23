#ifndef __MODEL_H__
#define __MODEL_H__

#include <QTextStream>
#include <QHash>
#include <QtGlobal>
#include <QVector>
#include "marisa.h"

typedef QPair<QString, QString> StringPair;
typedef unsigned char uchar;
typedef unsigned int uint;

struct Map{
    uchar size;
    union{
        uint *mapArray;
        struct{
            uint first;
            uint second;
        };
    };
};

class Model {
    public:
        Model(const char *mapfile, const char *wordsfile, const char *tagsfile, const char *endsfile);
        virtual ~Model();
        bool train(const QString &filename, QTextStream &out);
        double test(const QString &filename, QTextStream &out);
        void print(QTextStream &out);
        StringPair predict(const QString &prevTag, const QString &curWord);
        QList<StringPair> getTags(const QString &word, QList<ulong> &probs);
        bool save(const QString &filename, QTextStream &out);
        bool load(const QString &filename, QTextStream &out);

    private:
        QHash<StringPair, ulong> countTagsPair;
        ulong countWords;

        marisa::Trie words;
        marisa::Trie tags;
        marisa::Trie ends;

        Map *wordsMap;
        Map *endsMap;

        QList<StringPair> getNFandTags(const QString& key) const;
        QVector<QPair<QString, uint> > getTagsAndCount(const QString& key) const;
};

#endif
