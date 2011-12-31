#ifndef GEN_ARTICLE_H
#define GEN_ARTICLE_H

#include "article.h"

extern vector<Summary> generateSummary(NewsSource& source);
extern pair<Ranking, Ranking> generateYearWinnerLoserRankings(NewsSource& source);
extern vector<Article> generateArticles(NewsSource& source);
extern vector<Article> generateArticles_test(NewsSource& source);

#endif  /* GEN_ARTICLE_H */
