#include "imdbTAD.h"
#include <stdlib.h>
#include <string.h>
#define ADDED 1
#define NOT_ADDED !ADDED
#define BLOCK 30
#define MINVOTES 100000 //minima cantidad de votos para entrar en mostPopularList
#define MP_MAX 100

typedef struct tNodeYear *tListYear;
typedef struct tNodeMPM *tListMPM; //Most Popular Movies
typedef struct tNodeMPS *tListMPS; //Most Popular Series
typedef struct tNodeGenre *tListGenre;
typedef struct tNodeGenreRating *tListGenreRating;

//Q6 RELATED STRUCT
typedef struct tNodeGenreRating{
    char *genre;
    float minRating;
    float maxRating;
    struct tNodeGenreRating *tail;
} tNodeGenreRating;

//Q5 Y Q6 RELATED STRUCT
typedef struct tNodeLimitedYear{
    int minYear;
    int maxYear;
    tListGenreRating firstGenreRating;
    tListGenreRating currentLimitedGenre;
    unsigned int seriesSize; //dim de lista de q5
    tListMPS seriesFirst;    //lista de q5
    char vecInitialized;// flag del vector, marca si esta inicializado o no
    tListMPS* vec;// vector dinamico de tlistMPS, punteros a struct
    unsigned int iterativeVecIndex;// indice del vector, herramienta para iterar
} tNodeLimitedYear;

//Q2 RELATED STRUCT
typedef struct tNodeGenre{
    char *genre;
    unsigned int cantMovies;
    struct tNodeGenre *tail;
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

//los siguientes dos structs son parecidos a los anteriores dos structs
//difieren en que los de abajo contienen endYear al tratarse de series
//mientras que los de arriba representan peliculas por lo que no tienen ese campo

//Q5 RELATED STRUCT
typedef struct tElemMPS{
    char *title;
    unsigned startYear;
    unsigned endYear;
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
    int vecinitialized;
    int veciterativeindex;
    tListMPM *vec;
} tMPData;

//MAIN STRUCT
typedef struct imdbCDT{
    tListYear first;
    tListYear currentYear;
    tMPData MPData;
    tNodeLimitedYear limitedYear;
    tListGenre currentGenre;
} imdbCDT;

imdbADT new ()
{
    return calloc(1, sizeof(imdbCDT));
}

static char *
copy(const char *copyFrom)
{
    unsigned int i = 0, j = 0;
    char *copyTo = NULL;
    for (; copyFrom[j] != '\0'; i++, j++)
    {
        if (i % BLOCK == 0)
        {
            copyTo = realloc(copyTo, sizeof(char) * (BLOCK + i));
            //Checkear ERRORES
        }
        copyTo[i] = copyFrom[j];
    }
    copyTo = realloc(copyTo, sizeof(char) * (i + 1));
    //Checkear ERRORES
    copyTo[i] = '\0';
    return copyTo;
}

//Q2 related function) En esta funcion vemos si en la lista existe el genero aumenta el contador.
// Si no existia el genero lo agrega
static tListGenre addRecGenre(tListGenre first, char *genre)
{
    int c;
    if (first == NULL || (c = strcmp(genre, first->genre)) < 0)
    {
        tListGenre newGenre = malloc(sizeof(tNodeGenre));
        //todo Checkear ERRORES
        newGenre->genre = copy(genre);
        //todo Checkear ERRORES
        newGenre->tail = first;
        newGenre->cantMovies = 1;
        return newGenre;
    }
    if (c == 0)
    {
        (first->cantMovies)++;
        return first;
    }
    first->tail = addRecGenre(first->tail, genre);
    return first;
}

//Q6 related function) En esta funcion por cada genero que tenga la pelicula agarramos su rating
//y revisamos si en cada genero supera el rating max o min ya establecidos y si asi es, lo updateamos.
//si el genero no existia, lo agregamos
static tListGenreRating addRecGenreRating(tListGenreRating first, char *genre, float rating)
{
    int c;
    if (first == NULL || (c = strcmp(genre, first->genre)) < 0)
    {
        tListGenreRating newNode = malloc(sizeof(tNodeGenreRating));
        //todo check ERRORES
        newNode->genre = copy(genre);
        //todo check ERRORES
        newNode->tail = first;
        newNode->maxRating = rating;
        newNode->minRating = rating;
        return newNode;
    }
    if (c == 0)
    {
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
static tListMPM addRecMPM(tListMPM list, tElemMPM elem /*, int * controlFlag*/) //va a estar ordenada de men a may rating
{
    if (list == NULL || list->head.rating > elem.rating || ((list->head.rating == elem.rating) && (elem.votes <= list->head.votes)))
    {
        tListMPM newnode = malloc(sizeof(tNodeMPM));
        /**controlFlag=checkNULL(newnode);
        if(*controlFlag==ERROR_CODE)
            return NULL;*/
        newnode->head = elem;
        newnode->tail = list;
        return newnode;
    }
    list->tail = addRecMPM(list->tail, elem);
    return list;
}

static tListMPM deletefirstM(tListMPM first)
{
    tListMPM aux = first->tail;
    free(first->head.title);
    free(first);
    return aux;
}

void MPM(imdbADT imdb, tNode newNode, unsigned int startYear)
{ //meto int * controlflag?
    if (newNode.votes < MINVOTES)
        return;

    if (imdb->MPData.movieSize == MP_MAX && ((imdb->MPData.movieFirst->head.rating > newNode.rating) || ((imdb->MPData.movieFirst->head.rating == newNode.rating) && imdb->MPData.movieFirst->head.votes >= newNode.votes)))
        return;

    tNodeMPM newNodeMPM;
    newNodeMPM.head.votes = newNode.votes;
    newNodeMPM.head.rating = newNode.rating;
    newNodeMPM.head.startYear = startYear;
    newNodeMPM.head.title = copy(newNode.title /*, controlFlag*/);

    if (imdb->MPData.movieSize == MP_MAX)
        imdb->MPData.movieFirst = deletefirstM(imdb->MPData.movieFirst);
    else
        imdb->MPData.movieSize++;
    imdb->MPData.movieFirst = addRecMPM(imdb->MPData.movieFirst, newNodeMPM.head /*, controlFlag*/);
}


//las 3 funciones de abajo son de la query 5
static tListMPS addRecMPS(tListMPS list, tElemMPS elem /*, int * controlFlag*/) //va a estar ordenada de men a may rating
{
    if (list == NULL || list->head.rating > elem.rating || ((list->head.rating == elem.rating) && (elem.votes <= list->head.votes)))
    {
        tListMPS newnode = malloc(sizeof(tNodeMPS));
        /**controlFlag=checkNULL(newnode);
        if(*controlFlag==ERROR_CODE)
            return NULL;*/
        newnode->head = elem;
        newnode->tail = list;
        return newnode;
    }
    list->tail = addRecMPS(list->tail, elem);
    return list;
}


static tListMPS deletefirstS(tListMPS first)
{
    tListMPS aux = first->tail;
    free(first->head.title);
    free(first);
    return aux;
}


void MPS(imdbADT imdb, tNode newNode, unsigned int startYear, unsigned int endYear)
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
    newNodeMPS.title = copy(newNode.title /*, controlFlag*/);
    if (imdb->limitedYear.seriesSize == MP_MAX)
        imdb->limitedYear.seriesFirst = deletefirstS(imdb->limitedYear.seriesFirst);
    else
        imdb->limitedYear.seriesSize++;
    imdb->limitedYear.seriesFirst = addRecMPS(imdb->limitedYear.seriesFirst, newNodeMPS /*, controlFlag*/);
}

static tNode addType(tNode first, tNode node /*int * controlFlag*/)
{
    if (node.votes > first.votes)
    {
        free(first.title);
        //TODO CAMBIAR FLAG A ADDED
        return node; //cambio el nodo, lo supero en votos
    }
    //todo *controlFlag=NOT_ADDED;
    return first; //no lo supero en votos, devuelvo el que ya estaba
}

static tListYear addRec(tListYear first, unsigned int year, char *type, tNode node, char **genres, int dim)
{
    if (first == NULL || year > first->year)
    {
        tListYear newYear = calloc(1, sizeof(tNodeYear));
        //TODO check ERROR

        if (!strcmp(type, TYPEMOVIE))
        {
            newYear->mostVotedMovie = node;
            newYear->numMovies++;
            for (int i = 0; i < dim; i++)
            {
                newYear->firstGenre = addRecGenre(newYear->firstGenre, genres[i]);
                //todo check ERROR
            }
        }
        else if (!strcmp(type, TYPESERIES))
        {
            newYear->mostVotedSeries = node;
            newYear->numSeries++;
        }
        else
        {
            free(newYear);
            //TODO CAMBIAR FLAG A NOT ADDED
            return NULL;
        }
        newYear->year = year;
        newYear->tail = first;
        //TODO CAMBIAR FLAG A ADDED
        return newYear;
    }
    if (year == first->year)
    {
        if (strcmp(type, TYPEMOVIE) == 0)
        { // se agrega una pelicula
            first->numMovies++;
            for (int i = 0; i < dim; ++i)
            {
                first->firstGenre = addRecGenre(first->firstGenre, genres[i]);
                //todo check ERROR
            }
            first->mostVotedMovie = addType(first->mostVotedMovie, node);
            //todo check ERROR
        }
        else if (strcmp(type, TYPESERIES) == 0)
        { // se agrega una serie
            first->mostVotedSeries = addType(first->mostVotedSeries, node);
            first->numSeries++;
        }
        return first; // si no era ni pelicula ni serie, simplemente devuelvo lo que estaba pues no hay nada a insertar
    }
    first->tail = addRec(first->tail, year, type, node, genres, dim);
    return first;
}

// donde dim es la dimension del vector de generos
void add(imdbADT imdb, char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear)
{
    tNode newNode;
    newNode.votes = votes;
    newNode.rating = rating;
    newNode.title = copy(title);
    imdb->first = addRec(imdb->first, startYear, type, newNode, genres, dim);
    if (!strcmp(type, TYPEMOVIE))
    {
        MPM(imdb, newNode, startYear);
    }
    if (!strcmp(type, TYPESERIES) && ((imdb->limitedYear.minYear == NO_RESTRICTION || startYear >= imdb->limitedYear.minYear) && (imdb->limitedYear.maxYear == NO_RESTRICTION || startYear <= imdb->limitedYear.maxYear)))
    {
        for (int i = 0; i < dim; ++i)
        {
            imdb->limitedYear.firstGenreRating = addRecGenreRating(imdb->limitedYear.firstGenreRating, genres[i], rating);
        }
        MPS(imdb, newNode, startYear, endYear);
    }
}

//HACER FUNCION RECIEVE YEARS Q5 Y Q6 related function
void receiveYears(imdbADT imdb, int startYear, int endYear)
{
    imdb->limitedYear.minYear = startYear;
    imdb->limitedYear.maxYear = endYear;
}

void toBegin(imdbADT imdb)
{
    imdb->currentYear = imdb->first;
}

int hasNext(imdbADT imdb)
{
    return imdb->currentYear != NULL;
}

int next(imdbADT imdb)
{
    if (!hasNext(imdb))
        //todo tema errores
        imdb->currentYear = imdb->currentYear->tail;
    return OK;
}

static tListYear searchYear(tListYear first, unsigned int year)
{
    if (first == NULL || first->year > year)
        return NULL;
    if (first->year == year)
        return first;
    return searchYear(first->tail, year);
}

void toBeginGenre(imdbADT imdb, unsigned int year)
{
    tListYear aux = searchYear(imdb->first, year);
    if (aux != NULL)
    {
        imdb->currentGenre = aux->firstGenre;
    }
    else
        imdb->currentGenre = NULL;
}

int hasNextGenre(imdbADT imdb)
{
    return imdb->currentGenre != NULL;
}

int nextGenre(imdbADT imdb)
{
    if (!hasNextGenre(imdb))
        //todo tema errores
        imdb->currentGenre = imdb->currentGenre->tail;
    return OK;
}

unsigned int getYear(imdbADT imdb)
{
    if (imdb->currentYear == NULL) {
        //todo tema errores
    }
    return imdb->currentYear->year;
}

char *getGenre(imdbADT imdb)
{
    if (imdb->currentGenre == NULL)
        return NULL;
    char *genre = copy(imdb->currentGenre->genre);
    if (genre == NULL )
    { //todo ver el tema errores erno

        return NULL;
    }
    return genre;
}

unsigned int getGenreCant(imdbADT imdb)
{
    if (imdb->currentGenre == NULL) {
        //todo errores
    }
    return imdb->currentGenre->cantMovies;
}

void toBeginLimitedGenres(imdbADT imdb)
{
    imdb->limitedYear.currentLimitedGenre = imdb->limitedYear.firstGenreRating;
}

int hasNextLimitedGenres(imdbADT imdb)
{
    return imdb->limitedYear.currentLimitedGenre != NULL;
}

int nextLimitedGenres(imdbADT imdb)
{
    if (!hasNextLimitedGenres(imdb)) {
        //todo tema errores
    }
    imdb->limitedYear.currentLimitedGenre = imdb->limitedYear.currentLimitedGenre->tail;
    return OK;
}

char *getLimitedGenre(imdbADT imdb)
{
    if (imdb->limitedYear.currentLimitedGenre == NULL)
        return NULL;
    char *genre = copy(imdb->limitedYear.currentLimitedGenre->genre);
    if (genre == NULL)
    { //todo ver el tema errores erno

        return NULL;
    }
    return genre;
}

float getLimitedGenreMin(imdbADT imdb)
{
    if (imdb->limitedYear.currentLimitedGenre == NULL){
        //todo ver errores
    }
    return imdb->limitedYear.currentLimitedGenre->minRating;
}

float getLimitedGenreMax(imdbADT imdb)
{
    if (imdb->limitedYear.currentLimitedGenre == NULL){
        //todo ver errores
    }
    return imdb->limitedYear.currentLimitedGenre->maxRating;
}

unsigned int getYearNumMovies(imdbADT imdb)
{
    if (imdb->currentYear == NULL){
        //todo tema errores
    }
    return imdb->currentYear->numMovies;
}
unsigned int getYearNumSeries(imdbADT imdb)
{
    if (imdb->currentYear == NULL){
        //todo tema errores
    }
    return imdb->currentYear->numSeries;
}

char *getMostVotedMovieTitle(imdbADT imdb)
{
    if (imdb->currentYear == NULL)
    {
        //todo tema errores
    }
    char *title = copy(imdb->currentYear->mostVotedMovie.title);
    if (title == NULL)
    { //todo ver el tema errores ERNO

        return NULL;
    }
    return title;
}

float getMostVotedMovieRating(imdbADT imdb)
{
    if (imdb->currentYear == NULL)
    {
        //todo tema errores
    }
    return imdb->currentYear->mostVotedMovie.rating;
}

size_t getMostVotedMovieVotes(imdbADT imdb)
{
    if (imdb->currentYear == NULL)
    {
        //todo tema errores
    }
    return imdb->currentYear->mostVotedMovie.votes;
}

char *getMostVotedSeriesTitle(imdbADT imdb)
{
    if (imdb->currentYear == NULL)
    {
        //todo tema errores
    }
    char *title = copy(imdb->currentYear->mostVotedSeries.title);
    if (title == NULL)
    { //todo ver el tema errores ERNO

        return NULL;
    }
    return title;
}

float getMostVotedSeriesRating(imdbADT imdb)
{
    if (imdb->currentYear == NULL)
    {
        //todo tema errores
    }
    return imdb->currentYear->mostVotedSeries.rating;
}

size_t getMostVotedSeriesVotes(imdbADT imdb)
{
    if (imdb->currentYear == NULL)
    {
        //todo tema errores
    }
    return imdb->currentYear->mostVotedSeries.votes;
}


static void copyTop100SeriesListToVecRec(tListMPS *vec,unsigned int size,tListMPS list)
{
    if ( list == NULL )
        return;
    vec[size - 1] = list;
    copyTop100SeriesListToVecRec(vec,size - 1,list->tail);
}

static void copyTop100SeriesListToVec(imdbADT imdb)
{
    imdb->limitedYear.vec = malloc(imdb->limitedYear.seriesSize * sizeof(tListMPS));
    copyTop100SeriesListToVecRec(imdb->limitedYear.vec,imdb->limitedYear.seriesSize,imdb->limitedYear.seriesFirst);
}


void toBeginTop100Series(imdbADT imdb)
{
    if ( imdb->limitedYear.vecInitialized == 0)
    {
        imdb->limitedYear.vecInitialized = 1;// pongo flag de copiado
        copyTop100SeriesListToVec(imdb);
    }
    imdb->limitedYear.iterativeVecIndex = 0;// seteo el primer index, itero sobre el vector de punteros

}

int hasNextTop100Series(imdbADT imdb)
{
    return imdb->limitedYear.iterativeVecIndex != imdb->limitedYear.seriesSize;
}

int NextTop100Series(imdbADT imdb)
{
    if ( !hasNext(imdb) )
    {
        //fprintf(stderr,"uso indebido del next");
        //exit(1);
    }
    // AVANZO EN LA POSICION DEL VECTOR
    imdb->limitedYear.iterativeVecIndex += 1;
    return OK;
}

// responsabilidad del usuario liberar
char * getTop100SeriesName(imdbADT imdb)
{
    char *copyToReturn = copy(imdb->limitedYear.vec[imdb->limitedYear.iterativeVecIndex]->head.title);
    //tema de errores
    return copyToReturn;
}


unsigned int getTop100SeriesStartYear(imdbADT imdb)
{
    return imdb->limitedYear.vec[imdb->limitedYear.iterativeVecIndex]->head.startYear;
}

unsigned int getTop100SeriesEndYear(imdbADT imdb)
{
    return imdb->limitedYear.vec[imdb->limitedYear.iterativeVecIndex]->head.endYear;
}

float getTop100SeriesRating(imdbADT imdb)
{
    return imdb->limitedYear.vec[imdb->limitedYear.iterativeVecIndex]->head.rating;
}

// parte analoga a lo de rana pero para zaka

static void copyTop100MoviesListToVecRec(unsigned int size,tListMPM *vec,tListMPM list)
{
    if (list == NULL)
        return;
    vec[size - 1] = list;
    copyTop100MoviesListToVecRec(size - 1,vec,list->tail);
}

static void copyTop100MoviesListToVec(imdbADT imdb)
{
    imdb->MPData.vec = malloc(sizeof(tListMPM) * imdb->MPData.movieSize);
    copyTop100MoviesListToVecRec(imdb->MPData.movieSize,imdb->MPData.vec,imdb->MPData.movieFirst);
}

void toBeginTop100Movies(imdbADT imdb)
{
    if ( imdb->MPData.vecinitialized == 0)
    {
        imdb->MPData.vecinitialized = 1;
        copyTop100MoviesListToVec(imdb);
    }
    imdb->MPData.veciterativeindex = 0;
}

int hasNextTop100Movies(imdbADT imdb)
{
    return imdb->MPData.veciterativeindex != imdb->MPData.movieSize;
}

int nextTop100Movies(imdbADT imdb)
{
    if ( !hasNextTop100Movies(imdb) )
    {
        // codigo de error
    }
    imdb->MPData.veciterativeindex += 1;
    return OK;
}

//que lo libere el front
char * getTop100MovieTitle(imdbADT imdb)
{
    char * toreturn = copy(imdb->MPData.vec[imdb->MPData.veciterativeindex]->head.title);
    return toreturn;
}

unsigned int getTop100MovieStartYear(imdbADT imdb){
    return imdb->MPData.vec[imdb->MPData.veciterativeindex]->head.startYear;
}


unsigned int getTop100MovieVotes(imdbADT imdb){
    return imdb->MPData.vec[imdb->MPData.veciterativeindex]->head.votes;
}

float getTop100MovieRating(imdbADT imdb){
    return imdb->MPData.vec[imdb->MPData.veciterativeindex]->head.rating;
}

static void freeRecGenre(tListGenre genre)
{
    if (genre == NULL)
        return;
    freeRecGenre(genre->tail);
    free(genre->genre);
    free(genre);
}

static void freeRecYear(tListYear year)
{
    if (year == NULL)
        return;
    freeRecYear(year->tail);
    freeRecGenre(year->firstGenre);
    free(year->mostVotedSeries.title);
    free(year->mostVotedMovie.title);
    free(year);
}

static void freeMP(tListMPM mostPopular)
{
    if (mostPopular == NULL)
        return;

    freeMP(mostPopular->tail);
    free(mostPopular->head.title);
    free(mostPopular);
}

static void freeGenreRating(tListGenreRating genreRating)
{
    if(genreRating==NULL)
        return;

    freeGenreRating(genreRating->tail);
    free(genreRating->genre);
    free(genreRating);
}

static void freeSeries(tListMPS list){
    if(list==NULL)
        return;
    freeSeries(list->tail);
    free(list->head.title);
    free(list);
}

void freeIMDB(imdbADT imdb)
{
    freeRecYear(imdb->first);
    freeMP(imdb->MPData.movieFirst);
    freeGenreRating(imdb->limitedYear.firstGenreRating);
    freeSeries(imdb->limitedYear.seriesFirst);
    free(imdb);
}
