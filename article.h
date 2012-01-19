#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>
#include "newsSource.h"
using namespace std;

class Article {
public:
    Article();
    Article(Code& code_, string heading_, string sub_heading_, string content_, double score_, Price changeToday_);
    Code   code;
    string heading;
    string sub_heading;
    string content;
    double score;
    Price changeToday;

    bool operator<( const Article& o ) const;
    bool isNull() const { return code.code == ""; }

    // for layout
    string prefix() const;
    string prefixSimple() const;

    unsigned int hash() const;
    void print() const;
    void dump( ofstream& out ) const;
};

class Summary {
public:
    Summary();
    Summary(Code& code_, string content_, Price changeToday_);
    Code code;
    string content;
    Price changeToday;

    unsigned int hash() const;
};

class Ranking {
public:
    typedef vector< pair<double,Code> > RatioCodeArray;
    typedef RatioCodeArray::const_iterator Iter;
    Ranking();
    Ranking(Iter b, Iter e);
    RatioCodeArray codes;
};

#endif  /* ARTICLE_H */
