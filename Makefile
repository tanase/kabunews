CC=g++
OBJDIR=obj
EKISPRESSO_LIBDIR=../ekispresso_lib
CFLAGS=-O3 -I$(EKISPRESSO_LIBDIR) -std=c++0x
CFLAGS=-I$(EKISPRESSO_LIBDIR) -std=c++0x -g
LDFLAGS=-O3 -lsqlite3
LDFLAGS=-lsqlite3 -g
OBJS=record.o article.o newsSource.o genArticle.o layout.o loadFile.o utf8.o analysis.o util.o env.o

generate: main.o $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

analyze: analyze_main.o $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^


.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o generate analyze
