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
#include "record.h"
using namespace std;

double Record::trade() const
{
    return ( high + low ) * volume / 2;
}

void Record::print() const {
    cout << code << " " << date << " " << start << " " << high << " " << low << " " << end << " " << volume << endl;
}

// [from,to)の期間をrecordsが包含するか
static bool includes( vector<Record>& records, int from, int to )
{
    if ( records[0].date > from ) return false;
    if ( to != -1 && records.back().date < to ) return false;
    return true;
}

double ratio(double from, double to, double C)
{
    return (to + C) / (from + C);
}

// pair< 日数,pair<上昇率,補正上昇率> >
pair<int,pair<double,double> > calcUpStreak( vector<Record>& records )
{
    const double C = 20;
    if ( records.size() == 0 ) return make_pair(0, make_pair(UNDEF, UNDEF));
    int days = 0;
    for ( int i = records.size() - 1; i >= 1; i -- ) {
        Price cur = records[i].end;
        Price prev = records[i - 1].end;
        if ( cur == 0 || prev == 0 )
            return make_pair(0, make_pair(0.0, 0.0));
        if ( cur > prev ) {
            if ( days >= 0 ) {
                days ++;
                continue;
            }
        }
        else if ( cur < prev ) {
            if ( days <= 0 ) {
                days --;
                continue;
            }
        }
        return make_pair(days, make_pair(ratio(records[i].end, records.back().end, 0), ratio(records[i].end, records.back().end, C)));
    }
    return make_pair(days, make_pair(ratio(records[0].end, records.back().end, 0), ratio(records[0].end, records.back().end, C)));
}

// records[ret] <= dayとなる最大のret
int indexOfDate( vector<Record>& records, int day )
{
    int lo = 0, hi = records.size();
    if ( records[lo].date > day ) return -1;
    while ( lo + 1 < hi ) {
        int mid = ( lo + hi ) / 2;
        if ( records[mid].date < day ) lo = mid;
        else hi = mid;
    }
    return lo;
}

// [from,to)の期間の変化
double calcChange( vector<Record>& records, int from, int to )
{
    if ( !includes( records, from, to ) ) return UNDEF;
    int idx0 = indexOfDate( records, from );
    int idx1 = to == -1 ? records.size() - 1 : indexOfDate( records, to );
    if ( idx0 == -1 || idx1 == -1 || records[idx0].end == 0 || records[idx1].end == 0 ) return UNDEF;
    double Z = 1;
    for (int i = idx0; i < idx1; i++)
        Z *= records[i].split;
    return 1.0 * records[idx1].end * Z / records[idx0].end;
}

// [from,to)の期間の高値*2, 安値*2 (終値・終日)
vector<PeakPrice> calcPeak( vector<Record>& records, int from, int to )
{
    if ( !includes( records, from, to ) ) return vector<PeakPrice>(4);
    int start = indexOfDate( records, from );
    if ( start == -1 ) return vector<PeakPrice>(4);
    Price hi[2] = {0}, lo[2] = {1<<30,1<<30}, prev_hi[2] = {0}, prev_lo[2] = {1<<30,1<<30};
    int hi_day[2] = {-1,-1}, lo_day[2] = {-1,-1}, prev_hi_day[2] = {-1,-1}, prev_lo_day[2] = {-1,-1};
    for ( int i = start + 1; i < records.size(); i ++ ) {
        if ( to != -1 && records[i].date >= to ) break;
        if ( hi[0] < records[i].end ) {
            prev_hi[0] = hi[0];
            prev_hi_day[0] = hi_day[0];
            hi[0] = records[i].end;
            hi_day[0] = records[i].date;
        }
        if ( lo[0] > records[i].end ) {
            prev_lo[0] = lo[0];
            prev_lo_day[0] = lo_day[0];
            lo[0] = records[i].end;
            lo_day[0] = records[i].date;
        }
        if ( hi[1] < records[i].high ) {
            prev_hi[1] = hi[1];
            prev_hi_day[1] = hi_day[1];
            hi[1] = records[i].high;
            hi_day[1] = records[i].date;
        }
        if ( lo[1] > records[i].low ) {
            prev_lo[1] = lo[1];
            prev_lo_day[1] = lo_day[1];
            lo[1] = records[i].low;
            lo_day[1] = records[i].date;
        }
    }
    vector<PeakPrice> res(4);
    res[0] = PeakPrice( hi[0], hi_day[0], prev_hi[0], prev_hi_day[0] );
    res[1] = PeakPrice( hi[1], hi_day[1], prev_hi[1], prev_hi_day[1] );
    res[2] = PeakPrice( lo[0], lo_day[0], prev_lo[0], prev_lo_day[0] );
    res[3] = PeakPrice( lo[1], lo_day[1], prev_lo[1], prev_lo_day[1] );
    return res;
}

// 前回高値(安値)のdate
int calcLastPeak(vector<Record>& records, int max_days)
{
    double Z = 1;
    Price price = records.back().end;
    if (price <= 0) return UNDEF;
    int mode = 0;
    for (int i = 1; i <= max_days && i < (int)records.size(); i ++) {
        int idx = records.size() - 1 - i;
        Z /= records[idx].split;
        if (records[idx].end <= 0) continue;
        if (records[idx].end * Z == price) {
            if (mode == 0) return UNDEF;
            return mode * records[idx].date;
        }
        if (records[idx].end * Z > price) {
            if (mode == 0) mode = -1;
            else if (mode > 0) return mode * records[idx].date;
        }
        else {
            if (mode == 0) mode = 1;
            else if (mode < 0) return mode * records[idx].date;
        }
    }
    return UNDEF;
}

double calcSumLogTradeVolume(vector<Record>& records, int from, int to)
{
    if ( !includes(records, from, to) ) return UNDEF;
    int idx0 = indexOfDate(records, from);
    int idx1 = to == -1 ? records.size() - 1 : indexOfDate(records, to);
    if (idx0 == -1 || idx1 == -1) return UNDEF;
    double res = 0;
    for (int i = idx0; i <= idx1; i++) {
        if (records[i].end == 0) continue;
        res += log(records[i].volume) + log((records[i].start + records[i].end) / 2.0);
    }
    return res;
}
