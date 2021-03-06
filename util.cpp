#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "util.h"
#include "env.h"

void clearLine()
{
    fprintf(stderr, "%-200s\r", "");
}

static unsigned int __hash[] = {
    0x358be750,0x6ca5e171,0x6bef0b6a,0x60feed35,0x2d3b7a24,0x1c498735,0x7b88e23,0x403a6f22,0x7d63a1f3,0x1d7bf764,0x512acb8d,0x2dc72ffd,0x8797307,0x62b680f2,0x44a204c7,0x3dc5cb3e,0x487a811f,0x6e83d0d8,0x590a354b,0x4fbc2373,0x4299481c,0x618f2931,0x66dc9c13,0x40a7d8e7,0x454b5896,0x2cdfc58e,0x52a4c647,0x3425eb79,0x2135e036,0xde22e3b,0x2948bb94,0x3fc64bff,0x7ae8296e,0x4ab634ea,0x58837b7a,0x331b5e4d,0x21455e4b,0x68717ac3,0x551dd22a,0x4fe3aab9,0x25f349c6,0x301702ce,0x475a5198,0x39c9b580,0x4129bdf9,0x70d74b08,0x565fc720,0x5c4aa5fd,0x433105ec,0x7f356d34,0x4e60022c,0x3e8c263d,0x3f511b5d,0x47bb8d63,0xe6ac532,0x2452d068,0x46ddb279,0x7f0f65f4,0x16ff5f8a,0x56f248ea,0x6ca423cd,0x5dea7ab0,0x235a5758,0x4307f822,0xfee2b3e,0x170435c0,0x4f2db2f,0x70fe3a4d,0x22617b27,0x52f2f872,0x1375efcb,0x72897e90,0x1b98be91,0x3703c0fc,0x3c82dc94,0x4c68ea6f,0x79152474,0x4cfb8d36,0xac861f1,0x7cc9af36,0x70a53d95,0x2ff93a85,0x376cdc41,0x4bd1472c,0x743fe58f,0x70c1e390,0x7c0aabb3,0xfc9cca6,0x5e786ce4,0x318aae6,0x50dbe689,0x2fc04a59,0x3933a1b7,0x34e27b4e,0x6f4c9381,0x78fafa87,0x36e074f7,0x3211a0dd,0x7c95be9e,0x7203d2c8,0x56fa7312,0x1fa753ac,0x2e0cc972,0x2aa7cbce,0x29490957,0x1222d95,0x29e40efc,0x11f02c18,0x18394e93,0x4730f052,0x70c5c5e9,0x4992a7d4,0x3e7a7028,0x5fedc85c,0x3b660ab9,0x6588aaef,0x5c20bf02,0x482cf8e6,0xc659bc9,0x455f437a,0x42d345e4,0x702b5dd4,0x76a0f858,0x742cc859,0x2566f18d,0x35caeab0,0x31b4252b,0x592d3cc5,0x79b401f5,0x4a6ad0c4,0xf6e3713,0x2714f2b4,0x32cdc171,0x14834a88,0x2634ac7b,0x4267a7f0,0x64d6419f,0x5d012fa7,0x53dc6a67,0x33303c46,0x2ffbad5,0x592d1b8e,0x68130942,0x6cd6feea,0x28496b58,0x275b877b,0x9856ea3,0x1c3be60a,0x2f6414c1,0x785c119b,0x1de8fa74,0x2f144a06,0x7d387ea6,0x864e278,0x633176bc,0x2abaaaa1,0x7d6bf0d3,0x9b54fa9,0x251f2a21,0x711f78a8,0x3449c94e,0x312298b3,0x7e37e2f1,0x23d69a3c,0x5a120d81,0x467a268e,0x71563fd6,0x3b26c578,0x1274b53f,0x30ffee23,0x6ec12b0c,0x459709e3,0x83e0cf5,0x22dac783,0x73831806,0x5a594924,0x7dfd528c,0x11c2c308,0x226cb4d7,0x21292f57,0x49893fb,0x5f766b0e,0x7f350758,0x7e543cb,0x1725bdd4,0x68aa1e85,0x6f68df91,0x74034b99,0x7100357e,0x22f77b19,0x370bbb48,0x76e3fe94,0x46849d9b,0x54341bd7,0x74ddd394,0x79227b50,0x47149d05,0x27d891fb,0x2de50eca,0x7b406863,0x6e11e09b,0x39659f7c,0x49ebcc25,0x6f723a3e,0x3847a05a,0x680c6565,0xb6b982b,0x6263ccfa,0x552465b5,0x7d092682,0x68d29578,0x58313c76,0x3a3b5bea,0x289bf8a2,0x1bb2b5f9,0xbe02bba,0xdde601a,0x7ed5915b,0x7090aab3,0x54a48f81,0x244d07dd,0x62a25de0,0x1c07c94,0x249b4ac,0x79ac4123,0x49ab112d,0x2408656a,0x275593cb,0x56273d67,0xef2bcc,0x43bd9966,0x2c487e9,0x5fd8e502,0x11784211,0x7711b47b,0x10b21cf3,0x4d81b3a3,0x6f12ab6,0x22c79d8d,0xd9a082d,0x5af18a16,0x225dd919,0x7259713,0x4f7c32f7,0x18eaa686,0x7025aacd,0x5451717d,0x43aa779,0x35489c43,0x131ebccf,0x20ace1aa,0x334af207,0x32429b3c,0x2495802,0x842955,0x1b1fce93,
};

unsigned int hash(string str)
{
    unsigned int res = 0;
    for (int i = 0; i < str.size(); i++)
        res ^= __hash[(int)str[i]];
    return res;
}

static int get_hour(int t) { return t / 10000; }
static int get_min(int t) { return t / 100 % 100; }
static int get_sec(int t) { return t % 100; }
static int hms_toi(int h, int m, int s) { return h * 10000 + m * 100 + s; }

// now = 122345 (12時23分45秒)
// s秒後
int time_after(int now, int s)
{
    int hour = get_hour(now), minute = get_min(now), second = get_sec(now);
    second += s;
    minute += second / 60;
    second %= 60;
    hour += minute / 60;
    minute %= 60;
    return hms_toi(hour, minute, second);
}

// Nヶ月前(N < 12) あまり細かいことは考えない(20120231とかも出てくる)
int nMonthsAgo(int today, int n)
{
    int mon = today % 10000 / 100;
    if (mon > n) return today - n * 100;
    int year = today / 10000;
    mon = (mon - 1 + 12 - n) % 12 + 1;
    year--;
    return year * 10000 + mon * 100 + today % 100;
}

string urlwrap(string url, string text)
{
    stringstream ss;
//    ss << "<a href=\"" << url << "\" target=\"_blank\">" << text << "</a>";
    ss << "<a href=\"" << url << "\">" << text << "</a>";
    return ss.str();
}

string quoteURL(string code)
{
    return
        Env::country == "us" ?
        "http://finance.yahoo.com/q?s=" + code + "&ql=1" :
        //"http://stocks.finance.yahoo.co.jp/stocks/chart/?code=" + code;
        // 内部サイトに誘導
        "quotes/" + code + ".html";
}

bool isLeapYear(int year)
{
    if (year % 4 == 0 && (year % 400 == 0 || year % 100)) return true;
    return false;
}
