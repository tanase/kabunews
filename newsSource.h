#ifndef NEWSSOURCE_H
#define NEWSSOURCE_H

#include "record.h"
#include "sqlite3Wrapper.h"
using namespace std;
using namespace sqlite3wrapper;

struct Code {
    string code;
    string exchange;            /* 所属 */
    string shortName;           /* 銘柄名 */
    string industry;            /* 業種 */

    Code();
    Code( string code_, string exchange_, string shortName_, string industry_ );
    
    void print(string eol) const;
    bool operator<(const Code& other) const;
};

bool codeIsSpecial( string code );


class Stock {
public:
    Stock( Code& code_ )
        : code(code_) {}
    Code code;
    int streakDays;             /* 連続上げ(下げ)日数 */
    double streakIncrease;      /* 連続上昇(下降)率 */
    double streakIncrease_modified; /* (補正)連続上昇(下降)率 */
    double ratioThisYear;       /* 年初来変動 */
    double ratioLastYear;       /* 昨年変動 */
    PeakPrice yearHigh[2];      /* 年初来高値(終値・終日) */
    PeakPrice yearLow[2];       /* 年初来安値(終値・終日) */
    PeakPrice lastYearHigh[2];  /* 昨年高値(終値・終日) */
    PeakPrice lastYearLow[2];   /* 昨年安値(終値・終日) */

    int lastPeakDate;           /* いつ以来の(前日比上昇? 高値:安値) 論外は99999999、安値の場合は(-1)倍(ex. -20110322)、前回ピークが見つからない(上場来高値？)場合は1(or -1) */

    Record valueToday;          /* 今日の値動き */
    Record valueYesterday;      /* 昨日の値動き */
    Record valueDayBeforeYesterday; /* その前の日 */

    double sumLogTradeVolumeYear; /* 年初来 log(売買高)の和 */

    Price changeToday() const;
    double ratioToday() const;
    double ratioToday_modified() const;
    double volumeChangeToday() const;

    Price changeYesterday() const;
    double ratioYesterday() const;
};

class NewsSource {
public:
    vector<Code> codes;
    void getCodes();
    string symbolPrefix;
    
    int today;
    SQLite3Wrapper& db;
    NewsSource(SQLite3Wrapper& db_, int today_, string symbolPrefix_ = "");
    vector<Stock> stocks;
    vector<Record> getRecords(const string code, int today);
    void collect();
    string getNameOfCode(string code) const;

    // 条件を満たす銘柄数
    int numSatisfy_yearHigh() const;
    int numSatisfy_yearLow() const;
    int numSatisfy_streakDays( int days ) const;
    int numSatisfy_streakIncrease( double increase ) const;
    int numSatisfy_streakIncrease_modified( double increase ) const;
    int numSatisfy_ratioThisYear( double change ) const;
    int numSatisfy_ratioToday( double change ) const;
    int numSatisfy_ratioToday_modified( double change ) const;
    int numSatisfy_trade( long long trade ) const;
    int numSatisfy_volumeChangeToday( double change ) const;
    int numSatisfy_lastPeak( int date ) const;
};

#endif  /* NEWSSOURCE_H */
