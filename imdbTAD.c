#include "imdbTAD.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ADDED 1
#define NOT_ADDED !ADDED
#define BLOCK 30
#define MINVOTES 100000 //minima cantidad de votos para entrar en mostPopularList
#define MP_MAX 100

typedef struct tNodeYear *tListYear;
typedef struct tNodeMP * tListMP;
typedef struct tNodeGenre * tListGenre;
typedef struct tNodeGenreRating * tListGenreRating;
typedef struct tNodeMPSeries * tListMPSeries;

//Q6 RELATED STRUCT
typedef struct tNodeGenreRating{
    char * genre;
    float minRating;
    float maxRating;
    struct tNodeGenreRating * tail;
}tNodeGenreRating;

//Q5 Y Q6 RELATED STRUCT
typedef struct tNodeLimitedYear{
    int minYear;
    int maxYear;
    tListGenreRating firstGenreRating;
    tListGenreRating currentGenre;
}tNodeLimitedYear;

//Q2 RELATED STRUCT
typedef struct tNodeGenre{
    char * genre;
    unsigned int cantMovies;
    struct tNodeGenre * tail;
}tNodeGenre;

//Q3 RELATED STRUCT AND INFO SAVER
typedef struct tNode
{
    char *title;
    float rating;
    size_t votes;
}tNode;

//Q1 Q2 Q3 RELATED STRUCT
typedef struct tNodeYear
{
    unsigned int year;
    unsigned int numMovies;
    unsigned int numSeries;
    tListYear tail;
    tNode mostVotedSeries;
    tNode mostVotedMovie;
    tListGenre firstGenre;
}tNodeYear;

//Q4 y Q5 RELATED STRUCT
typedef struct tElemMP{
    char *title;
    int startYear;
    int endYear; //va a valer -1 si se trata de una pelicula
    float rating;
    unsigned long votes;
}tElemMP;

//Q4 Y Q5 RELATED STRUCT
typedef struct tNodeMP{
    tElemMP head;
    tListMP tail;
}tNodeMP;

//Q4 y Q5 RELATED STRUCT
typedef struct tMPData{
    unsigned int movieSize; //el tamano de la lista de mostPopular, maximo 100
    tListMP movieFirst;
    unsigned int seriesSize;
    tListMP seriesFirst;
}tMPData;

//MAIN STRUCT
typedef struct imdbCDT
{
    tListYear first;
    //iterador por años
    tListYear currentYear;
    tMPData MPData;
    tNodeLimitedYear limitedYear;
    tListGenre currentGenre;
} imdbCDT;

static char *
copy(const char * copyFrom){
    unsigned int i=0, j=0;
    char * copyTo = NULL;
    for(; copyFrom[j]!='\0'; i++, j++){
        if(i%BLOCK == 0){
            copyTo=realloc(copyTo, sizeof(char) * (BLOCK + i));
            //Checkear ERRORES
        }
        copyTo[i]=copyFrom[j];
    }
    copyTo=realloc(copyTo, sizeof(char) * (i+1));
    //Checkear ERRORES
    copyTo[i]='\0';
    return copyTo;
}

//Q2 related function) En esta funcion vemos si en la lista existe el genero aumenta el contador.
// Si no existia el genero lo agrega
static tListGenre addRecGenre(tListGenre first, char * genre){
    int c;
    if(first ==NULL || (c=strcmp(genre, first->genre))<0){
        tListGenre newGenre=malloc(sizeof(tNodeGenre));
        //todo Checkear ERRORES
        newGenre->genre=copy(genre);
        //todo Checkear ERRORES
        newGenre->tail=first;
        newGenre->cantMovies=1;
        return newGenre;
    }
    if(c==0){
        (first->cantMovies)++;
        return first;
    }
    first->tail=addRecGenre(first->tail, genre);
    return first;
}

//Q6 related function) En esta funcion por cada genero que tenga la pelicula agarramos su rating
//y revisamos si en cada genero supera el rating max o min ya establecidos y si asi es, lo updateamos.
//si el genero no existia, lo agregamos
static tListGenreRating addRecGenreRating(tListGenreRating first, char * genre, float rating){
    int c;
    if(first==NULL || (c=strcmp(genre, first->genre))<0){
        tListGenreRating newNode=malloc(sizeof(tNodeGenreRating));
        //todo check ERRORES
        newNode->genre=copy(genre);
        //todo check ERRORES
        newNode->tail=first;
        newNode->maxRating=rating;
        newNode->minRating=rating;
        return newNode;
    }
    if(c==0){
        if(rating>first->maxRating)
            first->maxRating=rating;
        else if(rating<first->minRating)
            first->minRating=rating;
        return first;
    }
    first->tail=addRecGenreRating(first->tail, genre, rating);
    return first;
}

static tListMP addRecMP(tListMP list,tElemMP elem/*, int * controlFlag*/) //va a estar ordenada de men a may rating
{
    if ( list == NULL || list->head.rating > elem.rating || ((list->head.rating == elem.rating) && (elem.votes <= list->head.votes)) ){
        tListMP newnode = malloc(sizeof(tNodeMP));
        /**controlFlag=checkNULL(newnode);
        if(*controlFlag==ERROR_CODE)
            return NULL;*/
            //todo fijarse que las pelis van a tener endYear tmb
        newnode->head = elem;
        newnode->tail = list;
        return newnode;
    }
    list->tail = addRecMP(list->tail,elem);
    return list;
}

static tListMP deletefirst(tListMP first)
{
    tListMP aux = first->tail;
    free(first->head.title);
    free(first);
    return aux;
}


void MP(imdbADT imdb, tNode newNode, int startYear, int endYear){ //meto int * controlflag?
    if (newNode.votes < MINVOTES)
        return;

    if (endYear == -1) {//si es una pelicula todo ESTO LO HAGO FUNCION O LO DEJO ASI?
        if (imdb->MPData.movieSize == MP_MAX && ( (imdb->MPData.movieFirst->head.rating > newNode.rating) || ((imdb->MPData.movieFirst->head.rating == newNode.rating) && imdb->MPData.movieFirst->head.votes >= newNode.votes)) )
            return;
    }
    else{ //si es una serie
        if (imdb->MPData.seriesSize == MP_MAX && ( (imdb->MPData.seriesFirst->head.rating > newNode.rating) || ((imdb->MPData.seriesFirst->head.rating == newNode.rating) && imdb->MPData.seriesFirst->head.votes >= newNode.votes)) )
            return;
    }
    tNodeMP newNodeMP;
    newNodeMP.head.votes = newNode.votes;
    newNodeMP.head.rating = newNode.rating;
    newNodeMP.head.startYear = startYear;
    newNodeMP.head.endYear = endYear;
    newNodeMP.head.title = copy(newNode.title/*, controlFlag*/);
    if (endYear == -1){ //si es movie todo LO HAGO FUNCION O LO DEJO
        if ( imdb->MPData.movieSize == MP_MAX)
            imdb->MPData.movieFirst = deletefirst(imdb->MPData.movieFirst);
        else
            imdb->MPData.movieSize++;
        imdb->MPData.movieFirst = addRecMP(imdb->MPData.movieFirst, newNodeMP.head/*, controlFlag*/);
    }
    else{ //si es serie todo fijarse que cumpla el rango
        if ( imdb->MPData.seriesSize == MP_MAX)
            imdb->MPData.seriesFirst = deletefirst(imdb->MPData.seriesFirst);
        else
            imdb->MPData.seriesSize++;
        imdb->MPData.seriesFirst = addRecMP(imdb->MPData.seriesFirst, newNodeMP.head/*, controlFlag*/);
    }
}

static tNode addType(tNode first, tNode node /*int * controlFlag*/)
{
    if (node.votes > first.votes) {
        free(first.title);
        //TODO CAMBIAR FLAG A ADDED
        return node; //cambio el nodo, lo supero en votos
    }
    //todo *controlFlag=NOT_ADDED;
    return first; //no lo supero en votos, devuelvo el que ya estaba
}

static tListYear addRec(tListYear first, unsigned int year, char *type, tNode node, char ** genres, int dim)
{
    if (first == NULL || year > first->year)
    {
        tListYear newYear = calloc(1, sizeof(tNodeYear));
        //TODO check ERROR

        if(!strcmp(type, "movie")) {
            newYear->mostVotedMovie = node;
            newYear->numMovies++;
            for (int i = 0; i < dim; i++) {
                newYear->firstGenre = addRecGenre(newYear->firstGenre, genres[i]);
                //todo check ERROR
            }
        }
        else if(!strcmp(type, "tvSeries")) {
            newYear->mostVotedSeries = node;
            newYear->numSeries++;
        }
        else {
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
        if (strcmp(type, "movie") == 0){ // se agrega una pelicula
            first->numMovies++;
            for (int i = 0; i < dim; ++i) {
                first->firstGenre = addRecGenre(first->firstGenre, genres[i]);
                //todo check ERROR
            }
            first->mostVotedMovie = addType(first->mostVotedMovie, node);
            //todo check ERROR
        }
        else if (strcmp(type,"tvSeries") == 0){ // se agrega una serie
            first->mostVotedSeries = addType(first->mostVotedSeries, node);
            first->numSeries++;
        }
        return first; // si no era ni pelicula ni serie, simplemente devuelvo lo que estaba pues no hay nada a insertar
    }
    first->tail = addRec(first->tail, year, type, node, genres, dim);
    return first;
}

// donde dim es la dimension del vector de generos
void add(imdbADT imdb,char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear)
{
    tNode newNode;
    newNode.votes=votes;
    newNode.rating=rating;
    newNode.title=copy(title);
    imdb->first=addRec(imdb->first, startYear, type, newNode, genres, dim);
    //todo add Q4
    if(!strcmp(type,"tvSeries") && ((imdb->limitedYear.minYear==NO_RESTRICTION || startYear > imdb->limitedYear.minYear) && (imdb->limitedYear.maxYear==NO_RESTRICTION || startYear < imdb->limitedYear.maxYear))){
        for (int i = 0; i < dim; ++i) {
            imdb->limitedYear.firstGenreRating=addRecGenreRating(imdb->limitedYear.firstGenreRating, genres[i], rating);
        }
        //todo add Q5
    }
}

//HACER FUNCION RECIEVE YEARS Q5 Y Q6 related function
void receiveYears(imdbADT imdb, int startYear, int endYear){
    imdb->limitedYear.minYear=startYear;
    imdb->limitedYear.maxYear=endYear;
}

void toBegin(imdbADT imdb)
{
    imdb->currentYear = imdb->first;
}

int hasNext(imdbADT imdb)
{
    return imdb->currentYear != NULL;
}

static tListYear next(imdbADT imdb)
{
    tListYear toReturn = imdb->currentYear;
    imdb->currentYear = imdb->currentYear->tail;
    return toReturn;
}

static tListYear searchYear(tListYear first, int year){
    if (first == NULL || first->year>year)
        return NULL;
    if (first->year== year)
        return first;
    return searchYear(first->tail, year);
}

void toBeginGenre(imdbADT imdb, int year){
    tListYear aux=searchYear(imdb->first, year);
    if(aux!=NULL){
        imdb->currentGenre=aux->firstGenre;
    }
}

int hasNextGenre(imdbADT imdb){
    return imdb->currentGenre != NULL;
}

static tListGenre nextGenre(imdbADT imdb)
{
    tListGenre toReturn = imdb->currentGenre;
    imdb->currentGenre = imdb->currentGenre->tail;
    return toReturn;
}

unsigned int getYear(imdbADT imdb){
    tListYear aux=next(imdb);
    return aux->year;
}

char * getGenre(imdbADT imdb, unsigned int * cant){
    tListGenre aux=nextGenre(imdb);
    char * string=copy(aux->genre);
    *cant=aux->cantMovies;
    return string;
}


void toBeginLimitedGenres(imdbADT imdb){
    imdb->limitedYear.currentGenre= imdb->limitedYear.firstGenreRating;
}

int hasNextLimitedGenres(imdbADT imdb){
    return imdb->limitedYear.currentGenre!=NULL;
}

static tListGenreRating nextLimitedGenres(imdbADT imdb){
    tListGenreRating aux=imdb->limitedYear.currentGenre;
    imdb->limitedYear.currentGenre=imdb->limitedYear.currentGenre->tail;
    return aux;
}

char * getLimitedGenre(imdbADT imdb, float * min, float * max){
    tListGenreRating aux= nextLimitedGenres(imdb);
    char * string=copy(aux->genre);
    *min=aux->minRating;
    *max=aux->maxRating;
    return string;
}

