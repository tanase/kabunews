#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include "newsSource.h"
#include "article.h"
#include "env.h"
#include "genArticleJP.h"
#include "genArticleEN.h"
#include "genArticleHelper.h"

using namespace std;

// +n
static string nPointUp2String(Price n)
{
    stringstream ss;
    if (n >= 0) {
        ss << "<span class=\"up\">+" << n << "</span>";
    }
    else {
        ss << "<span class=\"down\">" << n << "</span>";
    }
    return ss.str();
}

vector<Summary> generateSummary(NewsSource& source)
{
    vector<Summary> summaries;
    for (int i = 0; i < source.stocks.size(); i ++) {
        Stock& stock = source.stocks[i];
        if (codeIsSpecial(stock.code.code)) {
            stringstream ss;
            ss << commaNumeralF(stock.valueToday.end) << " " << nPointUp2String(stock.changeToday());
            summaries.push_back(Summary(stock.code, ss.str(), stock.changeToday()));
        }
    }
    return summaries;
}

pair<Ranking, Ranking> generateYearWinnerLoserRankings(NewsSource& source)
{
    // 年初来騰落ランキング、全銘柄中売買高上位10%のみ
    vector< pair<double,int> > targets;
    for (int i = 0; i < source.stocks.size(); i++) {
        Stock& stock = source.stocks[i];
        if (stock.ratioThisYear != UNDEF) {
            targets.push_back(make_pair(-stock.sumLogTradeVolumeYear, i));
        }
    }
    sort(targets.begin(), targets.end());
    /*
    for (int i = 0; i < 10; i++) {
        cerr << i << ": " << source.stocks[targets[i].second].code.shortName << endl;
    }
    */
    
    vector< pair<double,Code> > data;
    for (int i = 0; i < targets.size() * 0.1; i++) {
        Stock& stock = source.stocks[targets[i].second];
        data.push_back(make_pair(stock.ratioThisYear, stock.code));
    }
    
    sort(data.begin(), data.end());
    
    Ranking losers(data.begin(), data.begin() + 20);
    reverse(data.begin(), data.end());
    Ranking winners(data.begin(), data.begin() + 20);

    for (int i = 0; i < winners.codes.size(); i++) {
        cerr << i << ": " << winners.codes[i].first << "\t" << winners.codes[i].second.shortName << endl;
    }
    for (int i = 0; i < losers.codes.size(); i++) {
        cerr << i << ": " << losers.codes[i].first << "\t" << losers.codes[i].second.shortName << endl;
    }

    return make_pair(winners, losers);
}

vector<Article> generateArticles(NewsSource& source)
{
    vector<Article> articles;
    int today = source.today;
    cerr << "today = " << today << endl;

    int num_yearHigh = source.numSatisfy_yearHigh();
    double entropy_yearHigh = calcEntropy(num_yearHigh, source.stocks.size());
    int num_yearLow = source.numSatisfy_yearLow();
    double entropy_yearLow = calcEntropy(num_yearLow, source.stocks.size());

    cerr << "num_yearHigh = " << num_yearHigh << endl;
    cerr << "num_yearLow = " << num_yearLow << endl;

    for (int i = 0; i < source.stocks.size(); i ++) {
        Stock& stock = source.stocks[i];
        if (stock.valueToday.date < today) continue;
        try {
            Article article;
            if (Env::lang == "jp")
                article = genArticleJP(source, stock, entropy_yearHigh, entropy_yearLow);
            else if (Env::lang == "en")
                article = genArticleEN(source, stock, entropy_yearHigh, entropy_yearLow);
            if (!article.isNull())
                articles.push_back(article);
        }
        catch (...) {
            cerr << "ERROR CAUGHT\n";
        }
    }

    sort(articles.begin(), articles.end());

    return articles;
}
