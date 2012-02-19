#ifndef ANALYSIS_H
#define ANALYSIS_H
#include "record.h"

double correlation(vector<Record>& recordsA, vector<Record>& recordsB);
double correlation2(vector<Record>& recordsA, vector<Record>& recordsB);
vector<double> estimatedPurchasedPrices(vector<Record>& records, int fromDate, int toDate);

#endif  /* ANALYSIS */
