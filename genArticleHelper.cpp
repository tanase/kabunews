#include "genArticleHelper.h"
#include "record.h"
#include "util.h"
#include <cmath>
#include <sstream>
#include <string>
#include <cassert>

using namespace std;

// 反発、反落など
#define MINIMUM_BOUNCE_THRES 0.01
#define BIG_BOUNCE_THRES     0.05
int bounceLevel(double ratioToday, double ratioYesterday)
{
    if (ratioToday == UNDEF || ratioYesterday == UNDEF) return 0;
    if (ratioToday > (1 + MINIMUM_BOUNCE_THRES) && ratioYesterday < 1) {
        if (ratioToday > 1 + BIG_BOUNCE_THRES) {
            if (ratioYesterday < 1 - BIG_BOUNCE_THRES)
                return +3;
            return +2;
        }
        return +1;
    }
    else if (ratioToday < 1 - MINIMUM_BOUNCE_THRES && ratioYesterday > 1) {
        if (ratioToday < 1 - BIG_BOUNCE_THRES) {
            if (ratioYesterday > 1 + BIG_BOUNCE_THRES)
                return -3;
            return -2;
        }
        return -1;
    }
    return 0;
}

double calcEntropy(int event, int total)
{
    const int C = 5;
    if (event < C) event = C;
    return log((double)total / event);
}


int daysBetween(int date1, int date2)
{
    const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int y1 = date1/10000,     y2 = date2/10000;
    int m1 = date1%10000/100, m2 = date2%10000/100;
    int d1 = date1%100,       d2 = date2%100;
    if (y1 == y2 && m1 == m2 && d1 == d2) return 0;
    for (int res = 1; res < 10000; res ++) {
        d1 ++;
        if (d1 > days[m1-1] + (m1 == 2 && isLeapYear(y1) ? 1 : 0)) {
            d1 = 1;
            m1 ++;
            if (m1 == 13) {
                m1 = 1;
                y1 ++;
            }
        }
        if (y1 == y2 && m1 == m2 && d1 == d2) return res;
    }
    return UNDEF;
}

string commaNumeral(int n)
{
    stringstream ss;
    ss << n;
    string t = ss.str();
    string res;
    int len = t.size();
    for (int i = 0; i < len; i ++) {
        res += t[i];
        if (i < len - 1 && (len - 1 - i) % 3 == 0) res += ",";
    }
    return res;
}

string commaNumeralF(double n)
{
    stringstream ss;
    ss << n;
    string t = ss.str();
    string res;
    int len = t.size();
    for (int i = 0; i < len; i++)
        if (t[i] == '.') {
            len = i;
            break;
        }
    for (int i = 0; i < t.size(); i ++) {
        res += t[i];
        if (i < len - 1 && (len - 1 - i) % 3 == 0) res += ",";
    }
    return res;
}

// p=2: 0.1456 => 0.14, 1.462 => 1.5, 1778 => 1800
string formatWithPrecision(double n, int p)
{
    assert(n > 0);
    
    int c = 0;
    while (n < 100)
        n *= 10, c ++;
    long long nn = n;
    long long t;
    for (t = 1; t * 10 < n; t *= 10)
        continue;
    stringstream ss;
    ss << (nn + t / 100 * 5) / (t / 10) * (t / 10);
    string res = "0000000" + ss.str();
    if (res.substr(res.size() - c) == string(c, '0'))
        res.erase(res.size() - c, c);
    else {
        res = res.substr(0, res.size() - c) + "." + res.substr(res.size() - c);
        while (res[res.size() - 1] == '0')
            res.erase(res.size() - 1, 1);
    }
    while (res[0] == '0' && res.size() > 1 && res[1] != '.')
        res.erase(0, 1);
    return res;
}
