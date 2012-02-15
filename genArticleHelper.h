#ifndef GEN_ARTICLE_HELPER_H
#define GEN_ARTICLE_HELPER_H

#include <string>

double calcEntropy(int event, int total);
int bounceLevel(double ratioToday, double ratioYesterday);
std::string commaNumeralF(double n);
std::string formatWithPrecision(double n, int p);
int daysBetween(int date1, int date2);

#endif  /* GEN_ARTICLE_HELPER_H */
