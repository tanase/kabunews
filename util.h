#ifndef UTIL_H
#define UTIL_H

#include <string>
using namespace std;

void clearLine();
unsigned int hash(string str);
int time_after(int now, int m);
int nMonthsAgo(int today, int n);
string urlwrap(string url, string text);
string quoteURL(string code);
bool isLeapYear(int year);

#endif  /* UTIL_H */
