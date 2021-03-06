#include "article.h"
#include "util.h"
#include "env.h"

Article::Article() {}

Article::Article(Code& code_, string heading_, string sub_heading_, string content_, double score_, Price changeToday_)
    : code(code_), heading(heading_), sub_heading(sub_heading_), content(content_), score(score_), changeToday(changeToday_)
{
}

bool Article::operator<( const Article& o ) const
{
    if ( score > o.score ) return true;
    if ( score < o.score ) return false;
    return false;
}

string Article::prefix() const
{
    string url = quoteURL(code.code);
    return urlwrap( url, code.shortName ) + " (" + code.code + "," + code.exchange + ")" + (Env::lang == "jp" ? "が" : " ");
}

string Article::prefixSimple() const
{
    return code.shortName + " ";
}

unsigned int Article::hash() const {
    return ::hash(code.code) ^ ::hash(heading) ^ ::hash(sub_heading) ^ ::hash(content);
}

void Article::print() const
{
    cerr << code.to_s() << endl;
    cerr << heading << endl;
    cerr << sub_heading << endl;
    cerr << content << endl;
}

void Article::dump( ofstream& out ) const
{
}

Summary::Summary() {}

Summary::Summary(Code& code_, string content_, Price changeToday_)
    : code(code_), content(content_), changeToday(changeToday_) {}

unsigned int Summary::hash() const {
    return ::hash(content);
}

Ranking::Ranking() {}

Ranking::Ranking(Iter b, Iter e)
{
    for (Iter it = b; it != e; it++)
        codes.push_back(*it);
}
