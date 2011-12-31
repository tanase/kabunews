#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <string>
using namespace std;

#define UNDEF (99999999)
typedef double Price;

struct PeakPrice {
    Price price;
    int   date;
    Price prevPrice;
    int   prevDate;
    PeakPrice() :
    price(-1), date(-1), prevPrice(-1), prevDate(-1) {}
    PeakPrice(Price price_, int date_, Price prevPrice_, int prevDate_ ) :
    price(price_), date(date_), prevPrice(prevPrice_), prevDate(prevDate_) {}
};

class Record {
public:
    string code;
    int date;
    Price start;
    Price high;
    Price low;
    Price end;
    long long volume;
    double split;
    Record()
        : code("INVALID"), start(-1), high(-1), low(-1), end(-1), volume(-1) {}
    Record(string _code, int _date, Price _start, Price _high, Price _low, Price _end, long long _volume, double _split)
        : code(_code), date(_date), start(_start), high(_high), low(_low), end(_end), volume(_volume), split(_split) {}
    bool isset() const { return code != "INVALID"; }
    bool hasVal() const { return end > 0; }
    void print() const;
    long long trade() const;

    Price getStart() const {return start * split;}
    Price getHigh() const {return high * split;}
    Price getLow() const {return low * split;}
    Price getEnd() const {return end * split;}
};

pair<int,pair<double,double> > calcUpStreak( vector<Record>& records );
double calcChange( vector<Record>& records, int from, int to = -1 );
vector<PeakPrice> calcPeak( vector<Record>& records, int from, int to = -1 );
int calcLastPeak( vector<Record>& records, int max_days = 1000 );
int indexOfDate( vector<Record>& records, int day );
double calcSumLogTradeVolume(vector<Record>& records, int from, int to=-1);

#endif  /* RECORD_H */
