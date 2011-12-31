#include <cmath>
#include <iostream>
#include "analysis.h"

#define VAL(record, idx) (log(record[idx].end))
#define DIFFMAX log(1.45)

// 変化率の相関
double correlation2(vector<Record>& recordsA, vector<Record>& recordsB)
{
    if (recordsA.size() == 0 || recordsB.size() == 0) return 0;
    double saa = 0, sbb = 0, sab = 0, sa = 0, sb = 0;
    int n = 0;
    for (int ia = 0, ib = 0; ia + 1 < recordsA.size() && ib + 1 < recordsB.size(); ) {
        if (recordsA[ia].date == recordsB[ib].date) {
            if (recordsA[ia].hasVal() && recordsB[ib].hasVal() && recordsA[ia+1].hasVal() && recordsB[ib+1].hasVal()) {
                double ta = VAL(recordsA, ia + 1) - VAL(recordsA, ia);
                double tb = VAL(recordsB, ib + 1) - VAL(recordsB, ib);
                if (fabs(ta) < DIFFMAX && fabs(tb) < DIFFMAX) {
                    sa += ta;
                    saa += ta * ta;
                    sb += tb;
                    sbb += tb * tb;
                    n++;
                }
            }
            ia++;
            ib++;
        }
        else if (recordsA[ia].date > recordsB[ib].date) {
            ib++;
        }
        else {
            ia++;
        }
    }

//    if (n < 30) return -2;
    if (n < 100) return -2;

    sa /= n;
    sb /= n;
    
    for (int ia = 0, ib = 0; ia < recordsA.size() && ib < recordsB.size(); ) {
        if (recordsA[ia].date == recordsB[ib].date) {
            if (recordsA[ia].hasVal() && recordsB[ib].hasVal() && recordsA[ia+1].hasVal() && recordsB[ib+1].hasVal()) {
                double ta = VAL(recordsA, ia + 1) - VAL(recordsA, ia);
                double tb = VAL(recordsB, ib + 1) - VAL(recordsB, ib);
                if (fabs(ta) < DIFFMAX && fabs(tb) < DIFFMAX) {
                    sab += (ta - sa) * (tb - sb);
                }
            }
            ia++;
            ib++;
        }
        else if (recordsA[ia].date > recordsB[ib].date) {
            ib++;
        }
        else {
            ia++;
        }
    }

    if (saa < 1e-10 || sbb < 1e-10) return -2;

    saa -= n * sa * sa;
    sbb -= n * sb * sb;

    return sab / sqrt(saa) / sqrt(sbb);
}

// AB間の相関係数
double correlation(vector<Record>& recordsA, vector<Record>& recordsB)
{
    if (recordsA.size() == 0 || recordsB.size() == 0) return 0;
    double saa = 0, sbb = 0, sab = 0, sa = 0, sb = 0;
    int n = 0;
    for (int ia = 0, ib = 0; ia < recordsA.size() && ib < recordsB.size(); ) {
        if (recordsA[ia].date == recordsB[ib].date) {
            if (recordsA[ia].hasVal() && recordsB[ib].hasVal()) {
                sa += VAL(recordsA, ia);
                saa += pow(VAL(recordsA, ia), 2);
                sb += VAL(recordsB, ib);
                sbb += pow(VAL(recordsB, ib), 2);
                n++;
            }
            ia++;
            ib++;
        }
        else if (recordsA[ia].date > recordsB[ib].date) {
            ib++;
        }
        else {
            ia++;
        }
    }

    if (n < 3) return -2;

    sa /= n;
    sb /= n;

    for (int ia = 0, ib = 0; ia < recordsA.size() && ib < recordsB.size(); ) {
        if (recordsA[ia].date == recordsB[ib].date) {
            if (recordsA[ia].hasVal() && recordsB[ib].hasVal()) {
                sab += (VAL(recordsA, ia) - sa) * (VAL(recordsB, ib) - sb);
            }
            ia++;
            ib++;
        }
        else if (recordsA[ia].date > recordsB[ib].date) {
            ib++;
        }
        else {
            ia++;
        }
    }

    saa -= n * sa * sa;
    sbb -= n * sb * sb;
    return sab / sqrt(saa) / sqrt(sbb);
}

