#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
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

void cull(vector<double>& S, double x)
{
    vector<double> newS;
    for (vector<double>::iterator it = S.begin(); it != S.end(); ) {
        if ((rand() & 65535) / 65535.0 < x) {
            it ++;
        }
        else {
            it = S.erase(it);
        }
    }
}

// [fromDate,]間に購入して保持している人の購入価格予想
// サンプリングされたプレーヤー達の購入価格の配列を返す {3500,2800,2000,..}
vector<double> estimatedPurchasedPrices(vector<Record>& records, int fromDate, int toDay)
{
    int startIdx = indexOfDate(records, fromDate-1) + 1;
    int toIdx = indexOfDate(records, toDay) + 1;
    long long maxVolume = 0;
    for (int i = startIdx; i < records.size(); i++) {
        if (records[i].volume > maxVolume) maxVolume = records[i].volume;
    }

    double x = 1.0;
    vector<double> S;
    const int maxSize = 100000;
    
    // 保有：S人、ある日の購入者N人
    // 1. p∈Sが売却 => 買値v(p)を変更
    // 2. 以前からの保有者から購入 S += {v}
    // S >= N => Sですべて賄う。NをどうS上にマップするかという問題になる。
    // 購入価格が安い順にN人が売却と考えてしまおう。
    // S < N => N - S人が新規と考えてしまう

    for (int i = startIdx; i < toIdx; i++) {
        long long N = (long long)(records[i].volume * x);

        cerr << records[i].date << ":" << records[i].end << " => " << N << "(" << x << ")" << endl;
        
        // 新規が多すぎるのでSをまず減らす
        if (N > maxSize) {
            cull(S, 1.0 * maxSize / N);
            x *= 1.0 * maxSize / N;
            N = maxSize;
        }

        //
        vector<double> f;
        Price lo = records[i].low;
        Price hi = records[i].high;
        for (int i = 0; i < (int)N; i++) {
            // [lo, hi]に均等に分布と仮定
            if (N == 1)
                f.push_back((lo + hi) / 2);
            else
                f.push_back(lo + (hi - lo) * i / (N - 1));
        }

        int l = min((int)S.size(), (int)N);

        /* 基本下からだけどある程度ランダム性を入れる */
        vector<int> indices(S.size());
        for (int i = 0; i < S.size(); i++) indices[i] = i;
        // 例えば0.1だと下からピックアップしたうち1割をランダムにする
        const double random_factor = 0.6;
        for (int i = 0; i < l; i++)
            if ((rand() & 65535) / 65535.0 < random_factor) {
                int j = rand() % S.size();
                swap(indices[i], indices[j]);
            }
        for (int i = 0; i < l; i++) {
            S[indices[i]] = f[i];
        }

        for (int i = l; i < N; i++) {
            S.push_back(f[i]);
        }

        sort(S.begin(), S.end());
    }

    return S;
}
