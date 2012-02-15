CC=g++
OBJDIR=obj
EKISPRESSO_LIBDIR=../ekispresso_lib
CFLAGS=-O3 -I$(EKISPRESSO_LIBDIR) -std=c++0x
CFLAGS=-I$(EKISPRESSO_LIBDIR) -std=c++0x -g
LDFLAGS=-O3 -lsqlite3
LDFLAGS=-lsqlite3 -g
OBJS=record.o article.o newsSource.o genArticle.o genArticleJP.o genArticleEN.o genArticleHelper.o layout.o loadFile.o utf8.o analysis.o util.o env.o strings.o

generate: main.o $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

analyze: analyze_main.o $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

genArticleJP.o: genArticleJP.cpp genArticleJP.h article.h
	$(CC) $(CFLAGS) -c $< -o $@

genArticleEN.o: genArticleEN.cpp genArticleEN.h article.h
	$(CC) $(CFLAGS) -c $< -o $@

genArticleHelper.o: genArticleHelper.cpp genArticleHelper.h
	$(CC) $(CFLAGS) -c $< -o $@

layout.o: layout.cpp layout.h
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o generate analyze
