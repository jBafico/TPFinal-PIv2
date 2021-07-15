#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "imdbTAD.h"

#define ARG_AMOUNT 2
#define ARGWITHYEAR 4
#define QUERY_AMOUNT 6
#define CANT_FILES ((QUERY_AMOUNT) + 1) //Porque tengo un archivo por cada query + el archivo de donde se saca la informacion
#define FILE_POSITION 1
#define FIRSTYEARPOSITION 2
#define SECONDYEARPOSITION 3

#define BLOCK 30

#define GENRE_LIM ","
#define VARIABLE_LIM ";"

#define FIRSTQUERY "query1.csv"
#define SECONDQUERY "query2.csv"
#define THIRDQUERY "query3.csv"
#define FOURTHQUERY "query4.csv"
#define FIFTHQUERY "query5.csv"
#define SIXTHQUERY "query6.csv"

#define VALIDARGUMENT(x) (((x) >= (ARG_AMOUNT)) && ((x) <= (ARGWITHYEAR)))
// #define LOADYEAR(argc, argv, year, pos, errId) { if ((argc) == (pos) + 1) { *(year) = atoi((argv)[(pos)]); if (*(year) <= 0) return (errId); } }
#define LOADYEAR(argc, argv, year, pos, errId) { if ((argc) >= (pos) + 1) {         \
                                                    *(year) = atoi((argv)[(pos)]);  \
                                                    if (*(year) <= 0)               \
                                                        return (errId); }           \
                                                }

enum filePosition {arch=0, Q1,Q2,Q3,Q4, Q5, Q6};
enum data {TYPE=0,TITLE,STARTING_YEAR,ENDING_YEAR,GENRES,RATING,VOTES};

//revisa si los argumentos ingresados son correctos
int checkarg(int argc,char *argv[],int *year1,int *year2);

//devuelve un string con una linea del csv para poder usarla
char *getLineNoLimitFile(FILE *arch);

//revisa si los archivos s epudieron abrir correctamente
void checkopen(FILE **filevec,int queryamount, char * filenames[]);

//transforma un string con varios generos a un vector con strings dentro
char ** copyGenres(char * genresToDivide, unsigned int * dim);

//Funcion que escribe dentro del archivo de la Q1
int query1(FILE *arch, imdbADT imdb, unsigned int year);

//Funcion que escribe dentro del archivo de la Q2
int query2(FILE * arch, imdbADT imdb, unsigned int year);

//Funcion que escribe dentro del archivo de la Q3
int query3(FILE *arch, imdbADT imdb, unsigned int year);

//Funcion que escribe dentro del archivo de la Q4
int query4(FILE *arch, imdbADT imdb);

//Funcion que escribe dentro del archivo de la Q5
int query5(FILE *arch,imdbADT imdb);

//Funcion que escribe dentro del archivo de la Q6
int query6(FILE* arch, imdbADT imdb);

//Cierra todos los archivos que fueron abiertos por el programa
void closeFiles(FILE **filevec,int queryAmount);

//Saltea una linea del csv, por si no quieres usar la linea de los titulos por ejemplo
void skipline(FILE *arch);

//Printea un mensaje de error y cierra el programa
void errorMessage(const char * errMessage, int exitValue);

//Ademas de lo que hace errorMessage, tambien libera toda la memoria usada hasta ese momento
//y cierra los archivos que se habian abierto
void errorAbort(imdbADT imdb, FILE * fileVec[], const char * errMessage, int exitValue);


int main(int argc,char *argv[])
{
    int year1 = NO_RESTRICTION,year2 = NO_RESTRICTION;
    if (!checkarg(argc,argv,&year1,&year2)){
        errorMessage("Argumentos invalidos", ERROR_CODE);
    }

    FILE *arch;
    arch = fopen(argv[FILE_POSITION],"r");
    FILE *q1 = fopen(FIRSTQUERY,"w");
    FILE *q2 = fopen(SECONDQUERY,"w");
    FILE *q3 = fopen(THIRDQUERY,"w");
    FILE *q4 = fopen(FOURTHQUERY,"w");
    FILE *q5 = fopen(FIFTHQUERY,"w");
    FILE *q6 = fopen(SIXTHQUERY,"w");
    FILE *filevec[] = {arch,q1,q2,q3,q4,q5,q6};
    char * vecfilenames[] = {argv[FILE_POSITION], FIRSTQUERY,SECONDQUERY,THIRDQUERY,FOURTHQUERY,FIFTHQUERY,SIXTHQUERY};
    checkopen(filevec,CANT_FILES, vecfilenames);
    skipline(arch);
    imdbADT imdb = new();
    if(imdb==NULL || errno == ENOMEM){
        closeFiles(filevec, CANT_FILES);
        errorMessage("No se pude crear el imdb por falta de memoria", errno);
    }
    receiveYears(imdb, year1, year2);
    char * line;
    while ((line = getLineNoLimitFile(arch)) != NULL) {
        unsigned int startingYear,endingYear, dim=0;
        size_t votes;
        float rating;
        char * title, * type, *genresToDivide;
        char ** genres;
        char *token = strtok(line, VARIABLE_LIM);
        // Solo leemos datos hasta los votos, salteamos runtimeMinutes
        for (int i = 0; i <= VOTES; i++, token=strtok(NULL, VARIABLE_LIM)) {
            switch (i) {
                case TYPE:
                    type=token;
                    break;
                case TITLE:
                    title = token;
                    break;
                case STARTING_YEAR:
                    startingYear = atoi(token);
                    break;
                case ENDING_YEAR:
                    endingYear = atoi(token);
                    break;
                case GENRES:
                    genresToDivide=token;
                    break;
                case RATING:
                    rating = atof(token);
                    break;
                case VOTES:
                    votes = atoi(token);
                    break;
            }
        }

        if(startingYear > 0) { //Porque si el atoi encuentra un "\N" en el .csv le va a asignar un 0 a startingYear
            genres=copyGenres(genresToDivide,&dim);
            if(genres==NULL || errno ==ENOMEM)
                errorAbort(imdb, filevec, "No hay memoria sufiencte", ENOMEM);
            add(imdb, type, title, genres, dim, rating, votes, startingYear, endingYear);
            free(genres);
        }
        free(line);
        if (errno == ENOMEM) {
            errorAbort(imdb, filevec, "No hay memoria suficiente para guardar mas datos", errno);
        }
    }
    //esta validacion es del getLineNoLimit del while
    if(errno==ENOMEM){
        errorAbort(imdb, filevec, "No hay memoria suficiente para hacer una copia de la linea del csv", errno);
    }

    // codigo para correr las 6 queries
    toBegin(imdb);
    fprintf(filevec[Q1], "year;films;series\n");
    fprintf(filevec[Q2], "%s;%s;%s\n", "year", "genre", "films");
    fprintf(filevec[Q3], "startYear;film;votesFilm;ratingFilm;serie;votesSerie;ratingSerie\n");
    unsigned int year; //declaramos year para usar dentro del while
    while(hasNext(imdb)){
        year=getYear(imdb);
        if(year==ITERATIVE_ERROR)
            errorAbort(imdb, filevec, "Hubo un error en el uso de iteradores de año", ITERATIVE_ERROR);
        if(!query1(filevec[Q1], imdb, year))
            errorAbort(imdb, filevec, "Hubo un error en el uso de iteradores dentro de la Q1", errno);
        if(!query2(filevec[Q2], imdb, year))
            errorAbort(imdb, filevec, "Hubo un error en el uso de iteradores dentro de la Q2", errno);
        if(!query3(filevec[Q3], imdb, year))
            errorAbort(imdb, filevec, "Hubo un error en el uso de iteradores dentro de la Q3", errno);
        next(imdb);
        if(errno==ITERATIVE_ERROR){
            errorAbort(imdb, filevec, "Hubo un error en al trata de avanzar el iterador de año", errno);
        }
    }
    if(!query6(filevec[Q6], imdb))
        errorAbort(imdb, filevec, "Hubo un error en el uso de iteradores dentro de la Q6", errno);
    if(!query4(filevec[Q4], imdb))
        errorAbort(imdb, filevec, "Hubo un error en el uso de la Q4", errno);
    if(!query5(filevec[Q5], imdb))
        errorAbort(imdb, filevec, "Hubo un error en el uso de la Q5", errno);
    closeFiles(filevec, CANT_FILES);
    freeIMDB(imdb);
    return 0;
}

void skipline(FILE *arch)
{
    while( fgetc(arch) != '\n' )
        ;
}

int query4(FILE *arch, imdbADT imdb)
{
    prepareTop100Movies(imdb);
    int size = getsizeTop100Movies(imdb);
    fprintf(arch, "startyear;primaryTitle;numVotes;averageRating\n");
    for ( int i = 1; i <= size; i++)
    {
        unsigned int startyear, cantvotes;
        float rating;
        char * string;
        getDataFromPositionOfTop100Movies(imdb, i, &string, &startyear, &rating, &cantvotes);
        if(errno==ENOMEM || errno==EPOS)
            return NOTOK;
        fprintf(arch, "%u;%s;%u;%.1f\n", startyear, string, cantvotes, rating);
        free(string);
    }
    return OK;
}

int query5(FILE *arch,imdbADT imdb)
{
    prepareTop100Series(imdb);
    int size = getsizeTop100Series(imdb);
    fprintf(arch,"startYear;endYear;primaryTitle;numVotes;averageRating\n");
    for ( int i = 1; i <= size;i++)
    {
        unsigned int startyear, endyear, cantvotes;
        float rating;
        char *string;
        getDataFromPositionOfTop100Series(imdb,i,&startyear,&endyear,&rating,&cantvotes,&string);
        if(errno==ENOMEM || errno==EPOS || errno==ERROR_CODE)
            return NOTOK;
        fprintf(arch,"%u;%u;%s;%u;%.1f\n",startyear,endyear,string,cantvotes,rating);
        free(string);
    }
    return OK;
}


int query1(FILE *arch, imdbADT imdb, unsigned int year)
{
    unsigned int numMovies, numSeries;
    numMovies= getYearNumMovies(imdb);
    numSeries= getYearNumSeries(imdb);
    if(errno==ITERATIVE_ERROR)
        return NOTOK;
    fprintf(arch, "%u;%u;%u\n", year, numMovies, numSeries);
    return OK;
}

int query2(FILE * arch, imdbADT imdb, unsigned int year) {
    toBeginGenre(imdb, year);
    unsigned int cantMovies;
    while(hasNextGenre(imdb)){
        char * genre=getGenre(imdb);
        cantMovies= getGenreCant(imdb);
        if(errno==ITERATIVE_ERROR || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%d;%s;%d\n", year, genre, cantMovies);
        free(genre);
        nextGenre(imdb);
        if(errno==ITERATIVE_ERROR){
            return NOTOK;
        }
    }
    return OK;
}

int query3(FILE *arch, imdbADT imdb, unsigned int year){
    unsigned int numMovies, numSeries;
    numMovies= getYearNumMovies(imdb);
    numSeries= getYearNumSeries(imdb);
    if(errno==ITERATIVE_ERROR)
        return NOTOK;
    //el llamado a las funciones que vienen ahora esta hecho de esta manera porque si por ejemplo
    //no existen peliculas en ese año specifico las funciones que agarran los parametros de las
    //peliculas estarian accediendo a NULL. Al hacerlo de la forma que nosotros implementamos
    //esas situaciones no se presentan.
    if(numMovies == 0) {
        char * serieTitle= getMostVotedSeriesTitle(imdb);
        size_t serieVotes= getMostVotedSeriesVotes(imdb);
        float serieRating= getMostVotedSeriesRating(imdb);
        if(errno==ITERATIVE_ERROR || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u; ; ; ;%s;%lu;%.1f\n", year, serieTitle, serieVotes, serieRating);
        free(serieTitle);
    }
    else if(numSeries == 0) {
        char * movieTitle= getMostVotedMovieTitle(imdb);
        size_t movieVotes= getMostVotedMovieVotes(imdb);
        float movieRating= getMostVotedMovieRating(imdb);
        if(errno==ITERATIVE_ERROR || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u;%s;%lu;%.1f; ; ; \n", year, movieTitle, movieVotes, movieRating);
        free(movieTitle);
    }
    else {
        char * serieTitle= getMostVotedSeriesTitle(imdb);
        size_t serieVotes= getMostVotedSeriesVotes(imdb);
        float serieRating= getMostVotedSeriesRating(imdb);
        char * movieTitle= getMostVotedMovieTitle(imdb);
        size_t movieVotes= getMostVotedMovieVotes(imdb);
        float movieRating= getMostVotedMovieRating(imdb);
        if(errno==ITERATIVE_ERROR || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u;%s;%lu;%.1f;%s;%lu;%.1f\n", year, movieTitle, movieVotes, movieRating, serieTitle, serieVotes, serieRating);
        free(serieTitle);
        free(movieTitle);
    }
    return OK;
}


int query6(FILE* arch, imdbADT imdb){
    toBeginLimitedGenres(imdb);
    fprintf(arch, "%s;%s;%s\n", "genre", "min", "max");
    float min, max;
    while(hasNextLimitedGenres(imdb)){
        char * genre=getLimitedGenre(imdb);
        min= getLimitedGenreMin(imdb);
        max= getLimitedGenreMax(imdb);
        if(errno==ITERATIVE_ERROR || errno==ENOMEM){
            return NOTOK;
        }
        fprintf(arch, "%s;%.1f;%.1f\n", genre, min, max);
        free(genre);
        nextLimitedGenres(imdb);
        if(errno==ITERATIVE_ERROR){
            return NOTOK;
        }
    }
    return OK;
}

char ** copyGenres(char * genresToDivide, unsigned int * dim){
    int i = 0;
    char ** genres = NULL;
    for(char * aux = strtok(genresToDivide, GENRE_LIM); aux != NULL; aux = strtok(NULL, GENRE_LIM), i++) {
        genres = realloc(genres, sizeof(char*)*(i+1));
        if(genres==NULL || errno == ENOMEM){
            errno=ENOMEM;
            return NULL;
        }
        genres[i] = aux;
    }
    *dim = i;
    return genres;
}

void closeFiles(FILE **filevec,int fileAmount){
    for (int i = 0; i < fileAmount; ++i) {
        if(filevec[i]!=NULL)
            fclose(filevec[i]);
    }
}

void checkopen(FILE **filevec,int queryamount, char * filenames[]){
    for (int i = 0; i < queryamount ;i++) {
        if (filevec[i] == NULL) {
            fprintf(stderr,"problemas al abrir el archivo %s",filenames[i]);
            closeFiles(filevec, i);
            exit(1);
        }
    }
}

char *getLineNoLimitFile(FILE *arch){
    int c, i = 0;
    char *s = NULL; // para que el primer realloc funcione como malloc
    while ((c = fgetc(arch)) != '\n'){
        if(c==EOF)
            return NULL;
        if (i % BLOCK == 0){
            s = realloc(s, i + BLOCK); // multiplicar por sizeof(char) no es necesario
            if(s==NULL || errno==ENOMEM){
                errno=ENOMEM;
                return NULL;
            }

        }
        s[i++] = c;
    }
    s = realloc(s, i + 1); // Liberamos lo que sobra del ultimo bloque
    if(s==NULL || errno==ENOMEM){
        errno=ENOMEM;
        return NULL;
    }
    s[i] = 0;
    return s;
}


// ojo esta funcion
int checkarg(int argc,char *argv[],int *year1,int *year2){

    if (!VALIDARGUMENT(argc)) {
        return NOTOK;
      }
    /*
      if(argc >= FIRSTYEARPOSITION + 1) {
           *year1 = atoi(argv[FIRSTYEARPOSITION]);
           if (*year1 <= 0) {
               return NOTOK;
           }
      }*/
      LOADYEAR(argc, argv, year1, FIRSTYEARPOSITION, EYEAR)
      /*if(argc == SECONDYEARPOSITION + 1) {
           *year2 = atoi(argv[SECONDYEARPOSITION]);
           if (*year2 <= 0) {
               return NOTOK;
           }
      }*/
        LOADYEAR(argc, argv, year2, SECONDYEARPOSITION, EYEAR)
      if (argc == ARGWITHYEAR && *year1 > *year2) {
           return NOTOK;
      }
     return OK;
}

void errorMessage(const char * errMessage, int exitValue) {
    fprintf(stderr, "Error message: %s\nExit Value: %d", errMessage, exitValue);
    exit(exitValue);
}

void errorAbort(imdbADT imdb, FILE * fileVec[CANT_FILES], const char * errMessage, int exitValue){
    freeIMDB(imdb);
    closeFiles(fileVec, CANT_FILES);
    errorMessage(errMessage, exitValue);
}

