#include "imdbTAD.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BLOCK 30
#define MINVOTES 100000 //minima cantidad de votos para entrar en mostPopularList
#define MP_MAX 100
#define NULL_EXIT(imdb, ret, value) { if ((imdb) == NULL) { \
                                        errno=(value); \
                                        return (ret); } \
                                    }
#define NULL_EXIT_VOID(imdb, value) { if((imdb) == NULL) { \
                                        errno=(value); \
                                        return; } \
                                    }
typedef struct tNodeYear *tListYear;
typedef struct tNodeMPM *tListMPM; //Most Popular Movies
typedef struct tNodeMPS *tListMPS; //Most Popular Series
typedef struct tNodeGenre *tListGenre;
typedef struct tNodeGenreRating *tListGenreRating;

//Q6 RELATED STRUCT
typedef struct tNodeGenreRating{
    char * genre;
    float minRating;
    float maxRating;
    struct tNodeGenreRating *tail;
} tNodeGenreRating;

//Q5 Y Q6 RELATED STRUCT
typedef struct tNodeLimitedYear{
    unsigned int minYear;
    unsigned int maxYear;
    tListGenreRating firstGenreRating;
    tListGenreRating currentLimitedGenre;
    unsigned int seriesSize;    // Dimension de lista de Q5
    tListMPS seriesFirst;       // Lista de q5
    int vecInitialized;         // Flag del vector, marca si esta inicializado o no
    tListMPS vec[MP_MAX];       // Vector dinamico de tlistMPS, punteros a struct
} tNodeLimitedYear;

//Q2 RELATED STRUCT
typedef struct tNodeGenre{
    char *genre;
    unsigned int cantMovies;
    tListGenre tail;
} tNodeGenre;

//Q3 RELATED STRUCT AND INFO SAVER
typedef struct tNode{
    char *title;
    float rating;
    size_t votes;
} tNode;

//Q1 Q2 Q3 RELATED STRUCT
typedef struct tNodeYear{
    unsigned int year;
    unsigned int numMovies;
    unsigned int numSeries;
    tListYear tail;
    tNode mostVotedSeries;
    tNode mostVotedMovie;
    tListGenre firstGenre;
} tNodeYear;

//Q4 RELATED STRUCT
typedef struct tElemMPM{
    char *title;
    unsigned int startYear;
    float rating;
    unsigned long votes;
} tElemMPM;

//Q4 RELATED STRUCT
typedef struct tNodeMPM{
    tElemMPM head;
    tListMPM tail;
} tNodeMPM;

//Los siguientes dos structs son parecidos a los anteriores dos structs
//difieren en que los de abajo contienen endYear al tratarse de series
//mientras que los de arriba representan peliculas por lo que no tienen ese campo

//Q5 RELATED STRUCT
typedef struct tElemMPS{
    char *title;
    unsigned int startYear;
    unsigned int endYear;
    float rating;
    unsigned long votes;
} tElemMPS;

//Q5 RELATED STRUCT
typedef struct tNodeMPS{
    tElemMPS head;
    tListMPS tail;
} tNodeMPS;

//Q4 RELATED STRUCT
typedef struct tMPData{
    unsigned int movieSize; //el tamano de la lista de mostPopular, maximo 100
    tListMPM movieFirst;
    int vecInitialized;
    tListMPM vec[MP_MAX];
} tMPData;

//MAIN STRUCT
typedef struct imdbCDT{
    tListYear first;
    tListYear currentYear;
    tMPData MPData;
    tNodeLimitedYear limitedYear;
    tListGenre currentGenre;
} imdbCDT;

imdbADT new () {
    return calloc(1, sizeof(imdbCDT));
}

static char * copy(const char *copyFrom) {
    unsigned int i = 0, j = 0;
    char *copyTo = NULL;
    for (; copyFrom[j] != '\0'; i++, j++) {
        if (i % BLOCK == 0) {
            copyTo = realloc(copyTo, sizeof(char) * (BLOCK + i));
            if(copyTo==NULL || errno == ENOMEM) {
                errno=ENOMEM;
                return NULL;
            }
        }
        copyTo[i] = copyFrom[j];
    }
    copyTo = realloc(copyTo, sizeof(char) * (i + 1));
    if(copyTo==NULL || errno == ENOMEM) {
        errno=ENOMEM;
        return NULL;
    }
    copyTo[i] = '\0';
    return copyTo;
}

//Q2 related function) En esta funcion vemos si en la lista existe el genero aumenta el contador.
// Si no existia el genero lo agrega
static tListGenre addRecGenre(tListGenre first, char *genre) {
    int c;
    if (first == NULL || (c = strcmp(genre, first->genre)) < 0) {
        tListGenre newGenre = malloc(sizeof(tNodeGenre));
        if(newGenre==NULL || errno == ENOMEM) {
            errno=ENOMEM;
            return first;
        }
        newGenre->genre = copy(genre);
        //No seteamos errno en ENOMEM de vuelta porque viene seteado desde copy
        if((newGenre->genre)==NULL || errno == ENOMEM)
            return first;
        newGenre->tail = first;
        newGenre->cantMovies = 1;
        return newGenre;
    }
    if (c == 0) {
        (first->cantMovies)++;
        return first;
    }
    first->tail = addRecGenre(first->tail, genre);
    return first;
}

//Q6 related function) En esta funcion por cada genero que tenga la pelicula agarramos su rating
//y revisamos si en cada genero supera el rating max o min ya establecidos y si asi es, lo updateamos.
//si el genero no existia, lo agregamos
static tListGenreRating addRecGenreRating(tListGenreRating first, char *genre, float rating) {
    int c;
    if (first == NULL || (c = strcmp(genre, first->genre)) < 0) {
        tListGenreRating newNode = malloc(sizeof(tNodeGenreRating));
        if(newNode==NULL || errno == ENOMEM) {
            errno=ENOMEM;
            return first;
        }
        newNode->genre = copy(genre);
        if(newNode->genre==NULL || errno == ENOMEM)
            return first;

        newNode->tail = first;
        newNode->maxRating = rating;
        newNode->minRating = rating;
        return newNode;
    }
    if (c == 0) {
        if (rating > first->maxRating)
            first->maxRating = rating;
        else if (rating < first->minRating)
            first->minRating = rating;
        return first;
    }
    first->tail = addRecGenreRating(first->tail, genre, rating);
    return first;
}

//las 3 funciones de abajo son de la query 4
static tListMPM addRecMPM(tListMPM list, tElemMPM elem) { //va a estar ordenada de men a may rating
    if (list == NULL || list->head.rating > elem.rating || ((list->head.rating == elem.rating) && (elem.votes <= list->head.votes))) {
        tListMPM newnode = malloc(sizeof(tNodeMPM));
        if(newnode==NULL || errno == ENOMEM) {
            errno=ENOMEM;
            return list;
        }
        newnode->head = elem;
        newnode->tail = list;
        return newnode;
    }
    list->tail = addRecMPM(list->tail, elem);
    return list;
}

static tListMPM deletefirstM(tListMPM first) {
    tListMPM aux = first->tail;
    free(first->head.title);
    free(first);
    return aux;
}

static void MPM(imdbADT imdb, tNode newNode, unsigned int startYear) {
    if (newNode.votes < MINVOTES)
        return;

    if (imdb->MPData.movieSize == MP_MAX && ((imdb->MPData.movieFirst->head.rating > newNode.rating) || ((imdb->MPData.movieFirst->head.rating == newNode.rating) && imdb->MPData.movieFirst->head.votes >= newNode.votes)))
        return;

    tNodeMPM newNodeMPM;
    newNodeMPM.head.votes = newNode.votes;
    newNodeMPM.head.rating = newNode.rating;
    newNodeMPM.head.startYear = startYear;
    newNodeMPM.head.title = copy(newNode.title);
    if(newNodeMPM.head.title==NULL || errno == ENOMEM)
        return;

    if (imdb->MPData.movieSize == MP_MAX)
        imdb->MPData.movieFirst = deletefirstM(imdb->MPData.movieFirst);
    else
        imdb->MPData.movieSize++;
    imdb->MPData.movieFirst = addRecMPM(imdb->MPData.movieFirst, newNodeMPM.head );
}


//las 3 funciones de abajo son de la query 5
static tListMPS addRecMPS(tListMPS list, tElemMPS elem) { //va a estar ordenada de men a may rating
    if (list == NULL || list->head.rating > elem.rating || ((list->head.rating == elem.rating) && (elem.votes <= list->head.votes))) {
        tListMPS newnode = malloc(sizeof(tNodeMPS));
        if(newnode==NULL || errno == ENOMEM) {
            errno=ENOMEM;
            return list;
        }
        newnode->head = elem;
        newnode->tail = list;
        return newnode;
    }
    list->tail = addRecMPS(list->tail, elem);
    return list;
}


static tListMPS deletefirstS(tListMPS first) {
    tListMPS aux = first->tail;
    free(first->head.title);
    free(first);
    return aux;
}

static void MPS(imdbADT imdb, tNode newNode, unsigned int startYear, unsigned int endYear)
{
    // R : se chequea que la serie no haya terminado en este periodo de años, osea que el endig year sea menor que el end posta
    if (newNode.votes < MINVOTES || ( endYear != 0 && endYear < imdb->limitedYear.minYear) )
        return;
    if (imdb->limitedYear.seriesSize == MP_MAX && ((imdb->limitedYear.seriesFirst->head.rating > newNode.rating) || ((imdb->limitedYear.seriesFirst->head.rating == newNode.rating) && imdb->limitedYear.seriesFirst->head.votes >= newNode.votes)))
        return;
    // R : cambie el tipo de nodo que creabamos, estabamos gastando memoria al pedo porque siempre usabamos .head
    tElemMPS newNodeMPS;
    newNodeMPS.votes = newNode.votes;
    newNodeMPS.rating = newNode.rating;
    newNodeMPS.startYear = startYear;
    newNodeMPS.endYear = endYear;
    newNodeMPS.title = copy(newNode.title);
    if(newNodeMPS.title==NULL || errno == ENOMEM)
        return;
    if (imdb->limitedYear.seriesSize == MP_MAX)
        imdb->limitedYear.seriesFirst = deletefirstS(imdb->limitedYear.seriesFirst);
    else
        imdb->limitedYear.seriesSize++;
    imdb->limitedYear.seriesFirst = addRecMPS(imdb->limitedYear.seriesFirst, newNodeMPS);
}

static tNode addType(tNode first, tNode node) {
    if (node.votes > first.votes) {
        node.title= copy(node.title);
        free(first.title);
        if(node.title==NULL || errno == ENOMEM)
            return first;
        return node; //cambio el nodo, lo supero en votos
    }
    return first; //no lo supero en votos, devuelvo el que ya estaba
}

static tListYear addRec(tListYear first, unsigned int year, char *type, tNode node, char **genres, int dim) {
    if (first == NULL || year > first->year) {
        tListYear newYear = calloc(1, sizeof(tNodeYear));
        if(newYear==NULL || errno == ENOMEM) {
            errno=ENOMEM;
            return first;
        }
        node.title= copy(node.title);
        if(node.title==NULL || errno == ENOMEM)
            return first;

        if (!strcmp(type, TYPEMOVIE)) {
            newYear->mostVotedMovie = node;
            newYear->numMovies++;
            for (int i = 0; i < dim; i++) {
                newYear->firstGenre = addRecGenre(newYear->firstGenre, genres[i]);
                if(errno==ENOMEM)
                    return newYear;
            }
        }
        else if (!strcmp(type, TYPESERIES)) {
            newYear->mostVotedSeries = node;
            newYear->numSeries++;
        }
        else{
            free(newYear);

            return NULL;
        }
        newYear->year = year;
        newYear->tail = first;

        return newYear;
    }
    if (year == first->year) {
        if (strcmp(type, TYPEMOVIE) == 0) { // se agrega una pelicula
            first->numMovies++;
            for (int i = 0; i < dim; ++i) {
                first->firstGenre = addRecGenre(first->firstGenre, genres[i]);
                if(errno==ENOMEM)
                    return first;

            }
            first->mostVotedMovie = addType(first->mostVotedMovie, node);
            if(errno==ENOMEM)
                return first;

        }
        else if (strcmp(type, TYPESERIES) == 0) { // se agrega una serie
            first->mostVotedSeries = addType(first->mostVotedSeries, node);
            first->numSeries++;
        }
        return first; // si no era ni pelicula ni serie, simplemente devuelvo lo que estaba pues no hay nada a insertar
    }
    first->tail = addRec(first->tail, year, type, node, genres, dim);
    return first;
}

// donde dim es la dimension del vector de generos
void add(imdbADT imdb, char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    tNode newNode;
    newNode.votes = votes;
    newNode.rating = rating;
    newNode.title = title;

    imdb->first = addRec(imdb->first, startYear, type, newNode, genres, dim);
    if(errno==ENOMEM)
        return;
    if (!strcmp(type, TYPEMOVIE)) {
        MPM(imdb, newNode, startYear);
        if(errno==ENOMEM)
            return;
    }

    if (!strcmp(type, TYPESERIES) && ((imdb->limitedYear.minYear == NO_RESTRICTION || startYear >= imdb->limitedYear.minYear) && (imdb->limitedYear.maxYear == NO_RESTRICTION || startYear <= imdb->limitedYear.maxYear))) {
        for (int i = 0; i < dim; ++i)
            imdb->limitedYear.firstGenreRating = addRecGenreRating(imdb->limitedYear.firstGenreRating, genres[i], rating);

        MPS(imdb, newNode, startYear, endYear);
        if(errno==ENOMEM)
            return;
    }
}

void receiveYears(imdbADT imdb, int startYear, int endYear) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    imdb->limitedYear.minYear = startYear;
    imdb->limitedYear.maxYear = endYear;
}

void toBegin(imdbADT imdb) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    imdb->currentYear = imdb->first;
}

int hasNext(imdbADT imdb) {
    return (imdb != NULL && imdb->currentYear != NULL);
}

int next(imdbADT imdb) {
    if (!hasNext(imdb)) {
        errno=ERROR_ITERATION;
        return ERROR_ITERATION;
    }

    imdb->currentYear = imdb->currentYear->tail;
    return OK;
}

static tListYear searchYear(tListYear first, unsigned int year) {
    if (first == NULL || first->year < year)
        return NULL;
    if (first->year == year)
        return first;
    return searchYear(first->tail, year);
}

void toBeginGenre(imdbADT imdb, unsigned int year) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    tListYear aux = searchYear(imdb->first, year);
    if (aux != NULL) {
        imdb->currentGenre = aux->firstGenre;
    }
    else
        imdb->currentGenre = NULL;
}

int hasNextGenre(imdbADT imdb) {
    return (imdb != NULL && imdb->currentGenre != NULL);
}

int nextGenre(imdbADT imdb) {
    if (!hasNextGenre(imdb))
       return errno=ERROR_ITERATION;

    imdb->currentGenre = imdb->currentGenre->tail;
    return OK;
}

unsigned int getYear(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NOTOK, ERROR_ITERATION)

    return imdb->currentYear->year;
}

char *getGenre(imdbADT imdb) {
    NULL_EXIT(imdb, NULL, ERROR_NULL)
    NULL_EXIT(imdb->currentGenre, NULL, ERROR_ITERATION)

    char *genre = copy(imdb->currentGenre->genre);
    if (genre == NULL || errno==ENOMEM)
        return NULL;
    
    return genre;
}

unsigned int getGenreCant(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    NULL_EXIT(imdb->currentGenre, NOTOK, ERROR_ITERATION)

    return imdb->currentGenre->cantMovies;
}

void toBeginLimitedGenres(imdbADT imdb) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    imdb->limitedYear.currentLimitedGenre = imdb->limitedYear.firstGenreRating;
}

int hasNextLimitedGenres(imdbADT imdb) {
    return (imdb!=NULL && imdb->limitedYear.currentLimitedGenre != NULL);
}

int nextLimitedGenres(imdbADT imdb) {
    if (!hasNextLimitedGenres(imdb)) {
        errno=ERROR_ITERATION;
        return ERROR_ITERATION;
    }
    imdb->limitedYear.currentLimitedGenre = imdb->limitedYear.currentLimitedGenre->tail;
    return OK;
}

char *getLimitedGenre(imdbADT imdb) {
    NULL_EXIT(imdb, NULL, ERROR_NULL)
    NULL_EXIT(imdb->limitedYear.currentLimitedGenre, NULL, ERROR_ITERATION)
    
    char *genre = copy(imdb->limitedYear.currentLimitedGenre->genre);
    if (genre == NULL || errno==ENOMEM)
        return NULL;
    
    return genre;
}

float getLimitedGenreMin(imdbADT imdb) {
    NULL_EXIT(imdb, ERROR_NULL, ERROR_NULL)
    NULL_EXIT(imdb->limitedYear.currentLimitedGenre, ERROR_ITERATION, ERROR_ITERATION)
    return imdb->limitedYear.currentLimitedGenre->minRating;
}

float getLimitedGenreMax(imdbADT imdb) {
    NULL_EXIT(imdb, ERROR_NULL, ERROR_NULL)
    NULL_EXIT(imdb->limitedYear.currentLimitedGenre, ERROR_ITERATION, ERROR_ITERATION)
    return imdb->limitedYear.currentLimitedGenre->maxRating;
}

unsigned int getYearNumMovies(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NOTOK, ERROR_ITERATION)
    return imdb->currentYear->numMovies;
}

unsigned int getYearNumSeries(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NOTOK, ERROR_ITERATION)

    return imdb->currentYear->numSeries;
}

char *getMostVotedMovieTitle(imdbADT imdb) {
    NULL_EXIT(imdb, NULL, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NULL, ERROR_ITERATION)
    char *title = copy(imdb->currentYear->mostVotedMovie.title);
    if (title == NULL || errno == ENOMEM)
        return NULL;
    
    return title;
}

float getMostVotedMovieRating(imdbADT imdb) {
    NULL_EXIT(imdb, ERROR_NULL, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, ERROR_ITERATION, ERROR_ITERATION)

    return imdb->currentYear->mostVotedMovie.rating;
}

size_t getMostVotedMovieVotes(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NOTOK, ERROR_ITERATION)
    return imdb->currentYear->mostVotedMovie.votes;
}

char *getMostVotedSeriesTitle(imdbADT imdb) {
    NULL_EXIT(imdb, NULL, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NULL, ERROR_ITERATION)
    char *title = copy(imdb->currentYear->mostVotedSeries.title);
    if (title == NULL || errno==ENOMEM)
        return NULL;

    return title;
}

float getMostVotedSeriesRating(imdbADT imdb) {
    NULL_EXIT(imdb, ERROR_NULL, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, ERROR_ITERATION, ERROR_ITERATION)
    return imdb->currentYear->mostVotedSeries.rating;
}

size_t getMostVotedSeriesVotes(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    NULL_EXIT(imdb->currentYear, NOTOK, ERROR_ITERATION)
    return imdb->currentYear->mostVotedSeries.votes;
}

static void copyTop100SeriesListToVecIterative(tListMPS *vec,unsigned int size,tListMPS list) {
    // Relleno vector con iterador auxiliar
    tListMPS aux = list;
    while ( aux != NULL)
    {
        vec[size] = aux;
        size--;
        aux = aux->tail;
    }
}

//por si se cargan mas datos luego de la creacion del vector
void prepareTop100Series(imdbADT imdb) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    copyTop100SeriesListToVecIterative(imdb->limitedYear.vec,imdb->limitedYear.seriesSize - 1,imdb->limitedYear.seriesFirst);
    imdb->limitedYear.vecInitialized = 1;
}

unsigned int getSizeTop100Series(imdbADT imdb) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    return imdb->limitedYear.seriesSize;
}

static int checkInitialized(int flag) {
    if(flag==0) {
        errno=NOT_INITIALIZED;
        return NOTOK;
    }
    return OK;
}

static int checkPositionSerie(imdbADT imdb,int position) {
    if (position <= 0 || position > imdb->limitedYear.seriesSize) {
        errno = ERROR_POS;
        return ERROR_POS;
    }
    return OK;
}

static int checkPositionMovie(imdbADT imdb,int position) {
    if (position <= 0 || position > imdb->MPData.movieSize) {
        errno = ERROR_POS;
        return ERROR_POS;
    }
    return OK;
}

unsigned int getMPSStartYear(imdbADT imdb,int position) {
    NULL_EXIT(imdb,NOTOK, ERROR_NULL)
    if ( checkPositionSerie(imdb,position) == ERROR_POS || !checkInitialized(imdb->limitedYear.vecInitialized) )
        return NOTOK;
    return imdb->limitedYear.vec[position - 1]->head.startYear;
}

unsigned int getMPSEndYear(imdbADT imdb,int position) {
    NULL_EXIT(imdb,NOTOK, ERROR_NULL)
    if ( checkPositionSerie(imdb,position) == ERROR_POS || !checkInitialized(imdb->limitedYear.vecInitialized) )
        return NOTOK;
    return imdb->limitedYear.vec[position - 1]->head.endYear;
}

float getMPSRating(imdbADT imdb,int position) {
    NULL_EXIT(imdb,ERROR_NULL, ERROR_NULL)
    if ( checkPositionSerie(imdb,position) == ERROR_POS || !checkInitialized(imdb->limitedYear.vecInitialized) )
        return NOTOK;
    return imdb->limitedYear.vec[position - 1]->head.rating;
}

size_t getMPSVotes(imdbADT imdb,int position) {
    NULL_EXIT(imdb,NOTOK, ERROR_NULL)
    if ( checkPositionSerie(imdb,position) == ERROR_POS || !checkInitialized(imdb->limitedYear.vecInitialized) )
        return NOTOK;
    return imdb->limitedYear.vec[position - 1]->head.votes;
}

char *getMPSTitle(imdbADT imdb,int position) {
    NULL_EXIT(imdb,NULL, ERROR_NULL)
    if ( checkPositionSerie(imdb,position) == ERROR_POS || !checkInitialized(imdb->limitedYear.vecInitialized) )
        return NULL;
    char * string=copy(imdb->limitedYear.vec[position - 1]->head.title);
    if(string==NULL || errno==ENOMEM)
        return NULL;
    return string;
}

unsigned int getsizeTop100Movies(imdbADT imdb) {
    NULL_EXIT(imdb,NOTOK, ERROR_NULL)
    return imdb->MPData.movieSize;
}

static void copyTop100MoviesListToVecIterative(tListMPM *vec, unsigned int size, tListMPM list) {
    tListMPM aux = list;
    while( aux != NULL) {
        vec[size] = aux;
        size--;
        aux = aux->tail;
    }
}

void prepareTop100Movies(imdbADT imdb) {
    NULL_EXIT_VOID(imdb, ERROR_NULL)
    copyTop100MoviesListToVecIterative(imdb->MPData.vec,imdb->MPData.movieSize - 1,imdb->MPData.movieFirst);
    imdb->MPData.vecInitialized = 1;
}

size_t getMPMVotes(imdbADT imdb,int position) {
    NULL_EXIT(imdb,NOTOK, ERROR_NULL)
    if ( checkPositionMovie(imdb,position) == ERROR_POS || !checkInitialized(imdb->MPData.vecInitialized) )
        return NOTOK;
    return imdb->MPData.vec[position - 1]->head.votes;
}

float getMPMRating(imdbADT imdb,int position) {
    NULL_EXIT(imdb,ERROR_NULL, ERROR_NULL)
    if ( checkPositionMovie(imdb,position) == ERROR_POS || !checkInitialized(imdb->MPData.vecInitialized) )
        return NOTOK;
    return imdb->MPData.vec[position - 1]->head.rating;
}

unsigned int getMPMYear(imdbADT imdb,int position) {
    NULL_EXIT(imdb, NOTOK, ERROR_NULL)
    if ( checkPositionMovie(imdb,position) == ERROR_POS || !checkInitialized(imdb->MPData.vecInitialized) )
        return NOTOK;
    return imdb->MPData.vec[position - 1]->head.startYear;
}

char * getMPMTitle(imdbADT imdb,int position) {
    NULL_EXIT(imdb,NULL, ERROR_NULL)
    if (checkPositionMovie(imdb,position) == ERROR_POS || !checkInitialized(imdb->MPData.vecInitialized))
        return NULL;
    char * string = copy(imdb->MPData.vec[position-1]->head.title);
    if(string==NULL || errno==ENOMEM)
        return NULL;
    return string;
}

static void freeRecGenre(tListGenre genre) {
    if (genre == NULL)
        return;
    freeRecGenre(genre->tail);
    free(genre->genre);
    free(genre);
}

static void freeRecYear(tListYear year) {
    if (year == NULL)
        return;
    freeRecYear(year->tail);
    freeRecGenre(year->firstGenre);
    free(year->mostVotedSeries.title);
    free(year->mostVotedMovie.title);
    free(year);
}

static void freeMP(tListMPM mostPopular) {
    if(mostPopular == NULL)
        return;
    freeMP(mostPopular->tail);
    free(mostPopular->head.title);
    free(mostPopular);
}

static void freeGenreRating(tListGenreRating genreRating) {
    if(genreRating==NULL)
        return;
    freeGenreRating(genreRating->tail);
    free(genreRating->genre);
    free(genreRating);
}

static void freeSeries(tListMPS list) {
    if(list==NULL)
        return;
    freeSeries(list->tail);
    free(list->head.title);
    free(list);
}

void freeIMDB(imdbADT imdb) {
    if(imdb==NULL)
        return;
    freeRecYear(imdb->first);
    freeMP(imdb->MPData.movieFirst);
    freeGenreRating(imdb->limitedYear.firstGenreRating);
    freeSeries(imdb->limitedYear.seriesFirst);
    free(imdb);
}

