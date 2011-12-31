#ifndef UTIL_H
#define UTIL_H

#include <string>
using namespace std;

void clearLine();
unsigned int hash(string str);
int time_after(int now, int m);
string urlwrap(string url, string text);
string yahooURL(string code);

#endif  /* UTIL_H */
