#include <algorithm>
#include <cctype>
#include <cmath>
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
#include "env.h"
#include "newsSource.h"
using namespace std;

Code::Code()
{}

Code::Code( string code_, string exchange_, string shortName_, string industry_ ) :
    code(code_), shortName(shortName_), exchange(exchange_), industry(industry_)
{}

string Code::to_s() const
{
    stringstream ss;
    ss << shortName << "[" << exchange << "]" << "(" << code << "," << industry << ")";
    return ss.str();
}

bool Code::operator<(const Code& other) const {
    if (code < other.code) return true;
    if (code > other.code) return false;
    if (exchange < other.exchange) return true;
    return false;
}


Price Stock::changeToday() const
{
    if (valueYesterday.end <= 0 || valueToday.end <= 0) return UNDEF;
    if (valueYesterday.split != valueToday.split) return UNDEF;
    return valueToday.end - valueYesterday.end;
}

Price Stock::changeYesterday() const
{
    if (valueDayBeforeYesterday.end <= 0 || valueYesterday.end <= 0) return UNDEF;
    if (valueDayBeforeYesterday.split != valueYesterday.split) return UNDEF;
    return valueYesterday.end - valueDayBeforeYesterday.end;
}

double Stock::ratioToday() const
{
    if (valueYesterday.end <= 0 || valueToday.end <= 0) return UNDEF;
    return 1.0 * valueToday.getEnd() / valueYesterday.getEnd();
}

double Stock::ratioYesterday() const
{
    if (valueDayBeforeYesterday.end <= 0 || valueYesterday.end <= 0) return UNDEF;
    return 1.0 * valueYesterday.getEnd() / valueDayBeforeYesterday.getEnd();
}

double Stock::ratioToday_modified() const
{
    if (valueYesterday.end <= 0) return UNDEF;
    const double C = 20;
    return 1.0 * (valueToday.getEnd() + C) / (valueYesterday.getEnd() + C);
}

double Stock::volumeChangeToday() const
{
    if ( valueYesterday.volume <= 0 ) return UNDEF;
    // 前日の取引額が1億を下回るようでは出来高が増えたことを論じない
    if ( valueYesterday.trade() < 10000 * 10000 ) return 0;
    return 1.0 * valueToday.volume / valueYesterday.volume;
}

NewsSource::NewsSource(SQLite3Wrapper& db_, int today_, string symbolPrefix_)
    : db(db_), today(today_), symbolPrefix(symbolPrefix_)
{
    getCodes();
}

void NewsSource::getCodes() {
    codes.clear();
    Table table;
    stringstream ss;
    ss << "SELECT * FROM stockproperties";
    db.select( ss.str(), table );
    vector<int> res;
    for ( int i = 0; i < (int)table.size(); i ++ ) {
        codes.push_back( Code( string( table[i][0] ), string( table[i][1] ), string( table[i][2] ), string( table[i][4] ) ) );
    }
}

enum {CODE = 0, DATE, START, HIGH, LOW, END, VOLUME, SPLIT};

vector<Record> NewsSource::getRecords(const string code, int today)
{
    vector<Record> res;
    Table table;
    stringstream ss;
    ss << "SELECT * FROM records WHERE code='" << code << "' AND date <=" << today
       << " ORDER BY date ASC";
    db.select( ss.str(), table );
    for ( int i = 0; i < (int)table.size(); i ++ ) {
        string code = table[i][CODE];
        res.push_back( Record(table[i][CODE], integer(table[i][DATE]), floating(table[i][START]), floating(table[i][HIGH]), floating(table[i][LOW]), floating(table[i][END]), longlong(table[i][VOLUME]), floating(table[i][SPLIT])) );
    }
    return res;
}

bool codeIsSpecial( string code )
{
    if (Env::country == "us") return false; // TODO
    if (code.size() < 4) return true;
    return false;
}

static bool codeIsBetween( string code, string from, string to )
{
    if ( codeIsSpecial( code ) ) return true;
    if ( code.size() < from.size() ) return false;
    if ( code.size() > to.size() ) return false;
    if ( from <= code && ( code.size() < to.size() || code < to ) ) return true;
    return false;
}

static bool codeStartsWith(string code, string prefix)
{
    if (prefix != "" && code.substr(0, prefix.size()) != prefix) return false;
    return true;
}

void NewsSource::collect()
{
    stocks.clear();
    
    for ( int i = 0; i < (int)codes.size(); i ++ ) {
        Code& code = codes[i];
        //if ( !codeIsBetween( code.code, minCode, maxCode ) ) continue;
        if (!codeStartsWith(code.code, symbolPrefix)) continue;

        vector<Record> records = getRecords(code.code, today);
        if ( records.size() == 0 || records.back().date < today ) {
            cerr << code.code << " data missing ?" << "\r";
            continue;
        }
        
        pair<int,pair<double,double> > streak = calcUpStreak( records );
        
        int year = today / 10000 * 10000;
        vector<PeakPrice> peaks = calcPeak( records, year );
        vector<PeakPrice> peaks_lastyear = calcPeak( records, year - 10000, year );
        
        Stock stock( code );
        stock.streakDays = streak.first;
        stock.streakIncrease = streak.second.first;
        stock.streakIncrease_modified = streak.second.second;
        stock.ratioThisYear = calcChange( records, year );
        stock.ratioLastYear = calcChange( records, year - 10000, year );
        stock.yearHigh[0] = peaks[0];
        stock.yearHigh[1] = peaks[1];
        stock.yearLow[0] = peaks[2];
        stock.yearLow[1] = peaks[3];
        stock.lastYearHigh[0] = peaks_lastyear[0];
        stock.lastYearHigh[1] = peaks_lastyear[1];
        stock.lastYearLow[0] = peaks_lastyear[2];
        stock.lastYearLow[1] = peaks_lastyear[3];

        stock.lastPeakDate = calcLastPeak(records);
        stock.sumLogTradeVolumeYear = calcSumLogTradeVolume(records, year);

        stock.valueToday = records.back();
        if (records.size() > 1) stock.valueYesterday = records[records.size() - 2];
        if (records.size() > 2) stock.valueDayBeforeYesterday = records[records.size() - 3];
        
        stocks.push_back( stock );

        //fprintf( stderr, "%5s %6.1f high=%8.0f(%d) %s %s%-20s\n", code.code.c_str(), 100 * stock.ratioThisYear, stock.yearHigh[0].price, stock.yearHigh[0].date, stock.yearHigh[0].date == today ? "!" : "", " ", code.shortName.c_str() );
        //fprintf( stderr, "%5s %6.1f low=%8.0f(%d) %s %s%-20s\n", code.code.c_str(), 100 * stock.ratioThisYear, stock.yearLow[0].price, stock.yearLow[0].date, stock.yearLow[0].date == today ? "!" : "", " ", code.shortName.c_str() );
//        if (stock.lastPeakDate != UNDEF)
//            fprintf(stderr, "%5s lastPeak = %d\n", code.code.c_str(), stock.lastPeakDate);
    }
}

string NewsSource::getNameOfCode(string code) const {
    for (int i = 0; i < codes.size(); i++)
        if (codes[i].code == code) return codes[i].shortName;
    return "";
}

int NewsSource::numSatisfy_yearHigh() const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if ( stocks[i].yearHigh[0].date == today ) res ++;
    }
    return res;
}

int NewsSource::numSatisfy_yearLow() const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if ( stocks[i].yearLow[0].date == today ) res ++;
    }
    return res;
}

int NewsSource::numSatisfy_streakDays( int days ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if ( ( days > 0 && stocks[i].streakDays >= days ) || ( days < 0 && stocks[i].streakDays <= days ) )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_streakIncrease( double increase ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if (stocks[i].streakIncrease == UNDEF) continue;
        if ( ( increase > 1 && stocks[i].streakIncrease >= increase ) || ( increase < 1 && stocks[i].streakIncrease <= increase ) )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_streakIncrease_modified( double increase ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if (stocks[i].streakIncrease_modified == UNDEF) continue;
        if ( ( increase > 1 && stocks[i].streakIncrease_modified >= increase ) || ( increase < 1  && stocks[i].streakIncrease_modified <= increase ) )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_ratioThisYear( double change ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if ( ( change > 1 && stocks[i].ratioThisYear >= change ) || ( change < 1 && stocks[i].ratioThisYear <= change ) )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_ratioToday( double change ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        double ch = stocks[i].ratioToday();
        if ( ch == UNDEF ) continue;
        if ( ( change > 1 && ch >= change ) || ( change < 1 && ch <= change ) )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_ratioToday_modified( double change ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        double ch = stocks[i].ratioToday_modified();
        if ( ch == UNDEF ) continue;
        if ( ( change > 1 && ch >= change ) || ( change < 1 && ch <= change ) )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_trade( long long trade ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        long long t = ( stocks[i].valueToday.high + stocks[i].valueToday.low ) * stocks[i].valueToday.volume / 2;
        if ( t >= trade ) res ++;
    }
    return res;
}

int NewsSource::numSatisfy_volumeChangeToday( double change ) const {
    int res = 0;
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        double ch = stocks[i].volumeChangeToday();
        if ( ch == UNDEF ) continue;
        if ( ch >= change )
            res ++;
    }
    return res;
}

int NewsSource::numSatisfy_lastPeak( int last ) const {
    int res = 0;
    if ( last == UNDEF ) return stocks.size();
    for ( int i = 0; i < (int)stocks.size(); i ++ ) {
        if ( stocks[i].lastPeakDate == UNDEF ) continue;
        if ( last > 0 && stocks[i].lastPeakDate > 0 && last >= stocks[i].lastPeakDate ) res ++;
        if ( last < 0 && stocks[i].lastPeakDate < 0 && last <= stocks[i].lastPeakDate ) res ++;
    }
    return res;
}
