#include "model.h"
#include <QString>
#include <QTextCodec>
#include <QByteArray>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QList>
#include <QHash>
#include <QDebug>
#include <stdio.h>

QTextStream err(stdout);

Model::Model(const char *mapfile, const char *wordsfile, const char *tagsfile, 
        const char *endsfile) 
{
    countTagsPair.clear();
    words.load(wordsfile);
    tags.load(tagsfile);
    ends.load(endsfile);

    wordsMap = new Map[words.size()];
    endsMap = new Map[ends.size()];

    FILE *file = fopen(mapfile, "r");
    if(file == NULL){
        fprintf(stderr, "ERROR: map file not found\n");
    }

    for(uint i = 0; i < words.size(); i++){
        uchar temp;
        fread(&temp, 1, 1, file);
        wordsMap[i].size = temp * 2;
        if(temp == 1){
            fread(&wordsMap[i].first, sizeof(uint), 1, file);
            fread(&wordsMap[i].second, sizeof(uint), 1, file);
        }else{
            wordsMap[i].mapArray = new uint[wordsMap[i].size];
            for(uchar j = 0; j < wordsMap[i].size; j += 2){
                fread(wordsMap[i].mapArray + j, sizeof(uint), 1, file);
                fread(wordsMap[i].mapArray + j + 1, sizeof(uint), 1, file);
            }
        }
    }
    for(uint i = 0; i < ends.size(); i++){
        uchar temp;
        fread(&temp, 1, 1, file);
        endsMap[i].size = temp * 2;
        if(temp == 1){
            fread(&endsMap[i].first, sizeof(uint), 1, file);
            fread(&endsMap[i].second, sizeof(uint), 1, file);
        }else{
            endsMap[i].mapArray = new uint[endsMap[i].size];
            for(uchar j = 0; j < endsMap[i].size; j += 2){
                fread(endsMap[i].mapArray + j, sizeof(uint), 1, file);
                fread(endsMap[i].mapArray + j + 1, sizeof(uint), 1, file);
            }
        }
    }
    fclose(file);
}

bool
Model::save(const QString& filename, QTextStream& out) {
    QFile fout(filename);
    if (!fout.open(QIODevice::WriteOnly | QIODevice::Text)) {
        out << "ERROR: cannot open model file" << endl;
        return false;
    }
    QTextStream sfout(&fout);
    sfout.setCodec("CP1251");

    for (QHash<StringPair, ulong>::const_iterator itr = countTagsPair.begin(); itr != countTagsPair.end(); ++itr) {
        sfout << itr.key().first << " " << itr.key().second << " " << itr.value() << " ";
    }
    sfout << "----------";
    fout.flush();
    fout.close();
    return true;
}

bool
Model::load(const QString& filename, QTextStream& out) {
    QFile fin(filename);
    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "ERROR: model file not found" << endl;
        return false;
    }
    QTextStream sfin(&fin);
    sfin.setCodec("CP1251");

    countTagsPair.clear();
    QString tmp1, tmp2;
    int tmp3;
    while (true) {
        sfin >> tmp1;
        if (tmp1 == "----------") {
            break;
        }
        sfin >> tmp2 >> tmp3;
        countTagsPair.insert(StringPair(tmp1, tmp2), tmp3);
    }
    fin.close();
    return true;
}

bool
Model::train(const QString& filename, QTextStream& out) {
    QFile fin(filename);

    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "ERROR: input file not found" << endl;
        return false;
    }

    QString prevTag = "NONE";

    QTextStream sfin(&fin);
    sfin.setCodec("UTF-8");
    out.setCodec("UTF-8");
    while (!sfin.atEnd()) {
        QString line = sfin.readLine();
        if (line == "----------") {
            prevTag = "NONE";
            continue;
        }

        QStringList words = line.split(" ");
        ++countWords;
        ++countTagsPair[StringPair(prevTag, words[2])];
        prevTag = words[2];
    }
    return true;
}

double
Model::test(const QString& filename, QTextStream& out) {
    QFile fin(filename);

    if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
        out << "ERROR: input file not found" << endl;
        return false;
    }

    QString curTag, prevTag = "NONE";
    ulong countAll = 0, countWrong = 0;

    QTextStream sfin(&fin);
    sfin.setCodec("UTF-8");
    out.setCodec("UTF-8");
    while (!sfin.atEnd()) {
        QString line = sfin.readLine();
        if (line == "----------") {
            prevTag = "NONE";
            continue;
        }

        QStringList words = line.split(" ");
        curTag = predict(prevTag, words[0]).second;
        if (curTag != "PNKT" && curTag != "NUMB" && curTag != "LATN" && curTag != "UNKN") {
            if (words[2] == "UNKN")
                continue;
            ++countAll;
            if (words[2] != "UNKN" && words[2] != curTag) {
                out << words[0] << " : " << words[2] << " != " << curTag << endl;
                ++countWrong;
            }
        }
        prevTag = curTag;
    }
    out << (countAll - countWrong) / (1. * countAll) << endl;
    return true;
}

void
Model::print(QTextStream& out) {
    out.setCodec("UTF-8");
    QHash<StringPair, ulong>::const_iterator i;
    QHash<QString, ulong> count;
    for (i = countTagsPair.begin(); i != countTagsPair.end(); ++i) {
        out << i.key().first << " before " << i.key().second << " : " << i.value() << endl;
        count[i.key().first] += i.value();
    }

    QHash<QString, ulong>::const_iterator j;
    for (j = count.begin(); j != count.end(); ++j) {
        out << j.key() << " " << j.value() << endl;
    }
}

StringPair
Model::predict(const QString& prevTag, const QString& curWord) {
    QList<ulong> probs;
    QList<StringPair> variants = getTags(curWord, probs);
    uint maxVariant = 0;
    if (variants[maxVariant].second == "PNKT" || variants[maxVariant].second == "NUMB" ||
            variants[maxVariant].second == "LATN" || variants[maxVariant].second == "UNKN") {
        return variants[maxVariant];
    }
    Q_ASSERT(probs.size() == variants.size());
    for (int i = 0; i < variants.size(); ++i) {
        if (probs[i] * countTagsPair[StringPair(prevTag, variants[i].second)] > probs[maxVariant] * countTagsPair[StringPair(prevTag, variants[maxVariant].second)]) {
            maxVariant = i;
        }
    }
    return variants[maxVariant];
}

QList<StringPair>
Model::getTags(const QString& word, QList<ulong>& probs) {
    err.setCodec("UTF-8");
    QList<StringPair> result;
    result.clear();
    probs.clear();
    QChar yo = QString::fromUtf8("Ё")[0];
    QChar ye = QString::fromUtf8("Е")[0];
    result = getNFandTags(word.toUpper().replace(yo, ye, Qt::CaseInsensitive));
    if (result.size() > 0) {
        for (QList<StringPair>::iterator i = result.begin(); i != result.end(); ++i) {
            probs.append(1);
        }
        return result;
    }
    if (word[0].isPunct()) {
        result.append(StringPair(word, "PNCT"));
        probs.append(1);
    } else if (word[0].isNumber()) {
        result.append(StringPair(word, "NUMB"));
        probs.append(1);
    } else if (word[0].toLatin1() != 0) {
        result.append(StringPair(word, "LATN"));
        probs.append(1);
    } else {
        QVector<QPair<QString, uint> > predictedTags = getTagsAndCount(word.toUpper().right(3));
        if (predictedTags.size() == 0) {
            result.append(StringPair(word, "UNKN"));
            probs.append(1);
        }
        for (QVector<QPair<QString, uint> >::const_iterator itr = predictedTags.begin(); itr != predictedTags.end(); ++itr) {
            result.append(StringPair(word, (*itr).first));
            probs.append((*itr).second);
        }
    }
    return result;
}

QList<StringPair> Model::getNFandTags(const QString& key) const
{
    QByteArray temp = key.toUtf8();
    marisa::Agent agent;
    agent.set_query(temp.data());
    QList<StringPair> res;
    if(words.lookup(agent)){
        uint id = agent.key().id();
        if(wordsMap[id].size > 2){
            for(uchar i = 0; i < wordsMap[id].size; i += 2){
                marisa::Agent aNormalForm;
                aNormalForm.set_query(wordsMap[id].mapArray[i]);
                words.reverse_lookup(aNormalForm);
                QString nf = QString::fromUtf8(aNormalForm.key().ptr(), aNormalForm.key().length());
                marisa::Agent aTags;
                aTags.set_query(wordsMap[id].mapArray[i + 1]);
                tags.reverse_lookup(aTags);
                QString t = QString::fromUtf8(aTags.key().ptr(), aTags.key().length());
                res.append(StringPair(nf, t));
            }
        }else{
            marisa::Agent aNormalForm;
            aNormalForm.set_query(wordsMap[id].first);
            words.reverse_lookup(aNormalForm);
            QString nf = QString::fromUtf8(aNormalForm.key().ptr(), aNormalForm.key().length());
            marisa::Agent aTags;
            aTags.set_query(wordsMap[id].second);
            tags.reverse_lookup(aTags);
            QString t = QString::fromUtf8(aTags.key().ptr(), aTags.key().length());
            res.append(StringPair(nf, t));
        }
    }
    return res;
}

QVector<QPair<QString, uint> > Model::getTagsAndCount(const QString& key) const
{
    QByteArray temp = key.toUtf8();
    marisa::Agent agent;
    agent.set_query(temp.data());
    QVector<QPair<QString, uint> > res;
    if(ends.lookup(agent)){
        uint id = agent.key().id();
        if(endsMap[id].size > 2){
            for(uchar i = 0; i < endsMap[id].size; i += 2){
                marisa::Agent aTags;
                aTags.set_query(endsMap[id].mapArray[i]);
                tags.reverse_lookup(aTags);
                QString t = QString::fromUtf8(aTags.key().ptr(), aTags.key().length());
                res.append(QPair<QString, uint>(t, endsMap[id].mapArray[i + 1]));
            }
        }else{
            marisa::Agent aTags;
            aTags.set_query(endsMap[id].first);
            tags.reverse_lookup(aTags);
            QString t = QString::fromUtf8(aTags.key().ptr(), aTags.key().length());
            res.append(QPair<QString, uint>(t, endsMap[id].second));
        }
    }
    return res;
}

Model::~Model()
{
    countTagsPair.clear();

    for(uint i = 0; i < words.size(); i++)
        if(wordsMap[i].size > 2)
            delete [] wordsMap[i].mapArray;
    delete [] wordsMap;

    for(uint i = 0; i < ends.size(); i++)
        if(endsMap[i].size > 2)
            delete [] endsMap[i].mapArray;
    delete [] endsMap;

    words.clear();
    tags.clear();
    ends.clear();

}
