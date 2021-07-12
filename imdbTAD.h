#ifndef TPFINAL2_IMDBTAD_H
#define TPFINAL2_IMDBTAD_H

#define OK 1
#define NOTOK !OK
#define ERROR_CODE -1
#define NO_RESTRICTION -1

#include <stdio.h>

typedef struct imdbCDT * imdbADT;

void add(imdbADT imdb,char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear);
void toBegin(imdbADT imdb);
int hasNext(imdbADT imdb);
void toBeginLimitedGenres(imdbADT imdb);
int hasNextLimitedGenres(imdbADT imdb);
unsigned int getYear(imdbADT imdb);
void toBeginGenre(imdbADT imdb, int year);
int hasNextGenre(imdbADT imdb);
char * getGenre(imdbADT imdb, unsigned int * cant);
char * getLimitedGenre(imdbADT imdb, float * min, float * max);

#endif //TPFINAL2_IMDBTAD_H
