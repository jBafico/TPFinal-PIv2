#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "imdbTAD.h"

#define ARG_AMOUNT 2
#define ARGWITHYEAR 4
#define QUERY_AMOUNT 6
#define CANT_FILES ((QUERY_AMOUNT) + 1) //Porque tengo un archivo por cada query + el archivo de donde se saca la informacion
#define FILE_POSITION 1
#define FIRSTYEARPOSITION 2
#define SECONDYEARPOSITION 3

#define MINVOTES 100000     //Minima cantidad de votos para entrar en mostPopularList
#define MP_MAX 100          //Tamano maximo de las listas de las query 4 y 5

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
#define LOADYEAR(argc, argv, year, pos, errId) { if ((argc) >= ((pos) + 1)) {         \
                                                    *(year) = atoi((argv)[(pos)]);  \
                                                    if (*(year) <= 0)               \
                                                        return (errId); }           \
                                                }

enum filePosition {ARCH=0, Q1,Q2,Q3,Q4, Q5, Q6};
enum data {TYPE=0,TITLE,STARTING_YEAR,ENDING_YEAR,GENRES,RATING,VOTES};

// Revisa si los argumentos ingresados son correctos, retorna OK si son validos, NOTOK si no
int checkArg(int argc,char *argv[],int *year1,int *year2);

// Devuelve un string con una linea del csv para poder usarla
// Retorna NULL si ya llego a EOF
// Retorna NULL y setea errno en ENOMEM si no hay mas memoria
char *getLineNoLimitFile(FILE *arch);

// Revisa si los archivos se pudieron abrir correctamente, cierra el programa si no
void checkopen(FILE **filesVec,int queryamount, char * filenames[]);

// Transforma un string con varios generos a un vector con strings dentro y lo devuelve
// Retorna NULL y setea errno en ENOMEM si no hay mas memoria
char ** copyGenres(char * genresToDivide, unsigned int * dim);

// Funcion que escribe dentro del archivo de la Q1
// Retorna OK si no hubo ningun error
// Retorna NOTOK si hubo un error de cualquier tipo (validad con errno)
int query1(FILE *arch, imdbADT imdb, unsigned int year);

// Funcion que escribe dentro del archivo de la Q2
// Retorna OK si no hubo ningun error
// Retorna NOTOK si hubo un error de cualquier tipo (validad con errno)
int query2(FILE * arch, imdbADT imdb, unsigned int year);

// Funcion que escribe dentro del archivo de la Q3
// Retorna OK si no hubo ningun error
// Retorna NOTOK si hubo un error de cualquier tipo (validad con errno)
int query3(FILE *arch, imdbADT imdb, unsigned int year);

// Funcion que escribe dentro del archivo de la Q4
// Retorna OK si no hubo ningun error
// Retorna NOTOK si hubo un error de cualquier tipo (validad con errno)
int query4(FILE *arch, imdbADT imdb);

// Funcion que escribe dentro del archivo de la Q5
// Retorna OK si no hubo ningun error
// Retorna NOTOK si hubo un error de cualquier tipo (validad con errno)
int query5(FILE *arch,imdbADT imdb);

// Funcion que escribe dentro del archivo de la Q6
// Retorna OK si no hubo ningun error
// Retorna NOTOK si hubo un error de cualquier tipo (validad con errno)
int query6(FILE* arch, imdbADT imdb);

//Cierra todos los archivos que fueron abiertos por el programa
void closeFiles(FILE **filesVec,int fileAmount);

//Saltea una linea del csv, por si no quieres usar la linea de los titulos por ejemplo
void skipline(FILE *arch);

//Printea un mensaje de error y cierra el programa
void errorMessage(const char * errMessage, int exitValue);

//Ademas de lo que hace errorMessage, tambien libera toda la memoria usada hasta ese momento
//y cierra los archivos que se habian abierto
void errorAbort(imdbADT imdb, FILE * filesVec[], const char * errMessage, int exitValue);

/*
 * Comentario (1): Luego del primer chequeo de si imdb==NULL despues del new, no se vuelve a revisar esto con errno
 */

int main(int argc, char *argv[])
{
    int year1 = NO_RESTRICTION, year2 = NO_RESTRICTION;
    if (checkArg(argc,argv,&year1,&year2) != OK){
        //No usamos errorMessage ya que en la consigna especifica lo siguiente:
        // "[...] se informa en salida estandar que los parametros son incorrectos y no se procesa la informacion"
        puts("Argumentos invalidos");
        exit(ERROR_ARG);
    }

    FILE *arch;
    char * fileNames[] = {argv[FILE_POSITION], FIRSTQUERY,SECONDQUERY,THIRDQUERY,FOURTHQUERY,FIFTHQUERY,SIXTHQUERY};
    arch = fopen(fileNames[ARCH],"r");
    FILE *q1 = fopen(fileNames[Q1],"w");
    FILE *q2 = fopen(fileNames[Q2],"w");
    FILE *q3 = fopen(fileNames[Q3],"w");
    FILE *q4 = fopen(fileNames[Q4],"w");
    FILE *q5 = fopen(fileNames[Q5],"w");
    FILE *q6 = fopen(fileNames[Q6],"w");
    FILE *filesVec[] = {arch,q1,q2,q3,q4,q5,q6};
    checkopen(filesVec,CANT_FILES, fileNames);

    skipline(arch);

    imdbADT imdb = new(MINVOTES, MP_MAX);
    if(imdb==NULL || errno == ENOMEM){
        closeFiles(filesVec, CANT_FILES);
        errorMessage("No se pude crear el imdb por falta de memoria", errno);
    }
    receiveYears(imdb, year1, year2); // Ver Comentario (1)
    char * line;
    while ((line = getLineNoLimitFile(arch)) != NULL) {
        unsigned int startingYear,endingYear, dim=0;
        size_t votes;
        float rating;
        char ** genres;
        char * title, * type, *genresToDivide, *token = strtok(line, VARIABLE_LIM);
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
            if(genres==NULL || errno==ENOMEM) {
                free(line); //Porque si no hay memoria, aborta y nunca libera line
                errorAbort(imdb, filesVec, "No hay memoria sufiencte", ENOMEM);
            }
            add(imdb, type, title, genres, dim, rating, votes, startingYear, endingYear);
            free(genres);
        }
        free(line);
        if (errno == ENOMEM || errno==ERROR_NULL){
            errorAbort(imdb, filesVec, "No hay memoria suficiente para guardar mas datos", errno);
        }
    }
    //Esta validacion es del getLineNoLimit del while
    if(errno==ENOMEM)
        errorAbort(imdb, filesVec, "No hay memoria suficiente para hacer una copia de la linea del csv", errno);


    //Codigo para correr las 6 queries

    toBegin(imdb);//Ver comentario (1)
    fprintf(filesVec[Q1], "year;films;series\n");
    fprintf(filesVec[Q2], "%s;%s;%s\n", "year", "genre", "films");
    fprintf(filesVec[Q3], "startYear;film;votesFilm;ratingFilm;serie;votesSerie;ratingSerie\n");

    while(hasNext(imdb)){
        unsigned int year=getYear(imdb);
        if(year==NOTOK)
            errorAbort(imdb, filesVec, "Hubo un error en el uso de iteradores de año", errno);
        if(!query1(filesVec[Q1], imdb, year))
            errorAbort(imdb, filesVec, "Hubo un error en el uso de iteradores dentro de la Q1", errno);
        if(!query2(filesVec[Q2], imdb, year))
            errorAbort(imdb, filesVec, "Hubo un error en el uso de iteradores dentro de la Q2", errno);
        if(!query3(filesVec[Q3], imdb, year))
            errorAbort(imdb, filesVec, "Hubo un error en el uso de iteradores dentro de la Q3", errno);
        next(imdb);
        if(errno==ERROR_ITERATION){
            errorAbort(imdb, filesVec, "Hubo un error en al trata de avanzar el iterador de año", errno);
        }
    }
    if(!query4(filesVec[Q4], imdb))
        errorAbort(imdb, filesVec, "Hubo un error en el uso de la Q4", errno);
    if(!query5(filesVec[Q5], imdb))
        errorAbort(imdb, filesVec, "Hubo un error en el uso de la Q5", errno);
    if(!query6(filesVec[Q6], imdb))
        errorAbort(imdb, filesVec, "Hubo un error en el uso de iteradores dentro de la Q6", errno);

    closeFiles(filesVec, CANT_FILES);
    freeIMDB(imdb);
    return 0;
}

void skipline(FILE *arch)
{
    while(fgetc(arch) != '\n')
        ;
}

char ** copyGenres(char * genresToDivide, unsigned int * dim){
    int i = 0;
    char ** genres = NULL;
    for(char * aux = strtok(genresToDivide, GENRE_LIM); aux != NULL; aux = strtok(NULL, GENRE_LIM), i++){
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

void closeFiles(FILE **filesVec,int fileAmount){
    for (int i = 0; i < fileAmount; ++i) {
        if(filesVec[i]!=NULL)
            fclose(filesVec[i]);
    }
}

void checkopen(FILE **filesVec,int queryamount, char * filenames[]){
    for (int i = 0; i < queryamount ;i++) {
        if (filesVec[i] == NULL) {
            fprintf(stderr,"Problemas al abrir el archivo %s\n",filenames[i]);
            closeFiles(filesVec, i);
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

int checkArg(int argc,char *argv[],int *year1,int *year2){

    if (!VALIDARGUMENT(argc))
        return NOTOK;

    LOADYEAR(argc, argv, year1, FIRSTYEARPOSITION, ERROR_YEAR)
    LOADYEAR(argc, argv, year2, SECONDYEARPOSITION, ERROR_YEAR)
    if (argc == ARGWITHYEAR && (*year1 > *year2))
        return NOTOK;

    return OK;
}

void errorMessage(const char * errMessage, int exitValue) {
    fprintf(stderr, "Error message: %s\nExit Value: %d\n", errMessage, exitValue);
    exit(exitValue);
}

void errorAbort(imdbADT imdb, FILE * filesVec[CANT_FILES], const char * errMessage, int exitValue){
    freeIMDB(imdb);
    closeFiles(filesVec, CANT_FILES);
    errorMessage(errMessage, exitValue);
}

int query1(FILE *arch, imdbADT imdb, unsigned int year)
{
    unsigned int numMovies, numSeries;
    numMovies= getYearNumMovies(imdb);//Ver comentario (1)
    numSeries= getYearNumSeries(imdb);//Ver comentario (1)
    if(errno==ERROR_ITERATION)
        return NOTOK;
    fprintf(arch, "%u;%u;%u\n", year, numMovies, numSeries);
    return OK;
}

int query2(FILE * arch, imdbADT imdb, unsigned int year) {
    toBeginGenre(imdb, year);//Ver comentario (1)
    unsigned int cantMovies;
    while(hasNextGenre(imdb)){
        char * genre=getGenre(imdb);//Ver comentario (1)
        cantMovies= getGenreCant(imdb);//Ver comentario (1)
        if(errno==ERROR_ITERATION || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u;%s;%u\n", year, genre, cantMovies);
        free(genre);
        nextGenre(imdb);
        if(errno==ERROR_ITERATION){
            return NOTOK;
        }
    }
    return OK;
}

int query3(FILE *arch, imdbADT imdb, unsigned int year){
    unsigned int numMovies, numSeries;
    numMovies= getYearNumMovies(imdb);//Ver comentario (1)
    numSeries= getYearNumSeries(imdb);//Ver comentario (1)
    if(errno==ERROR_ITERATION)
        return NOTOK;
    //El llamado a las funciones que vienen ahora esta hecho de esta manera porque si por ejemplo
    //no existen peliculas en ese año specifico las funciones que agarran los parametros de las
    //peliculas estarian accediendo a NULL. Al hacerlo de la forma que nosotros implementamos
    //esas situaciones no se presentan.
    if(numMovies == 0) {
        //Ver comentario (1)
        size_t serieVotes= getMostVotedSeriesVotes(imdb);
        float serieRating= getMostVotedSeriesRating(imdb);
        char * serieTitle= getMostVotedSeriesTitle(imdb);
        if(errno==ERROR_ITERATION || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u; ; ; ;%s;%lu;%.1f\n", year, serieTitle, serieVotes, serieRating);
        free(serieTitle);
    }
    else if(numSeries == 0) {
        //Ver comentario (1)
        size_t movieVotes= getMostVotedMovieVotes(imdb);
        float movieRating= getMostVotedMovieRating(imdb);
        char * movieTitle= getMostVotedMovieTitle(imdb);
        if(errno==ERROR_ITERATION || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u;%s;%lu;%.1f; ; ; \n", year, movieTitle, movieVotes, movieRating);
        free(movieTitle);
    }
    else {
        //Ver comentario (1)
        size_t serieVotes= getMostVotedSeriesVotes(imdb);
        float serieRating= getMostVotedSeriesRating(imdb);
        size_t movieVotes= getMostVotedMovieVotes(imdb);
        float movieRating= getMostVotedMovieRating(imdb);
        char * serieTitle= getMostVotedSeriesTitle(imdb);
        char * movieTitle= getMostVotedMovieTitle(imdb);
        if(errno==ERROR_ITERATION || errno==ENOMEM)
            return NOTOK;
        fprintf(arch, "%u;%s;%lu;%.1f;%s;%lu;%.1f\n", year, movieTitle, movieVotes, movieRating, serieTitle, serieVotes, serieRating);
        free(serieTitle);
        free(movieTitle);
    }
    return OK;
}

int query4(FILE *arch, imdbADT imdb)
{
    prepareTopMovies(imdb);//Ver comentario (1)
    if(errno==ENOMEM)
        return NOTOK;
    unsigned int size = getSizeTopMovies(imdb);//Ver comentario (1)
    fprintf(arch, "startyear;primaryTitle;numVotes;averageRating\n");
    for ( int i = 1; i <= size; i++)
    {
        unsigned int startYear;
        size_t cantVotes;
        float rating;
        char * string;
        //Ver comentario (1)
        startYear = getMPMYear(imdb,i);
        cantVotes = getMPMVotes(imdb,i);
        rating = getMPMRating(imdb,i);
        string = getMPMTitle(imdb,i);
        if(errno==ENOMEM || errno==ERROR_POS || errno==ERROR_NULL || errno==NOT_INITIALIZED)
            return NOTOK;
        fprintf(arch, "%u;%s;%lu;%.1f\n", startYear, string, cantVotes, rating);
        free(string);
    }
    return OK;
}

int query5(FILE *arch,imdbADT imdb)
{
    prepareTopSeries(imdb);//Ver comentario (1)
    if(errno==ENOMEM)
        return NOTOK;
    unsigned int size = getSizeTopSeries(imdb);//Ver comentario (1)
    fprintf(arch,"startYear;endYear;primaryTitle;numVotes;averageRating\n");
    for ( int i = 1; i <= size;i++)
    {
        unsigned int startYear, endYear;
        size_t cantVotes;
        float rating;
        char *string;
        //Ver comentario (1)
        startYear = getMPSStartYear(imdb,i);
        endYear = getMPSEndYear(imdb,i);
        cantVotes = getMPSVotes(imdb,i);
        rating = getMPSRating(imdb,i);
        string = getMPSTitle(imdb,i);
        if(errno==ENOMEM || errno==ERROR_POS || errno==ERROR_NULL || errno==NOT_INITIALIZED)
            return NOTOK;
        //En la impresion de esta query tomamos como criterio que si no tenia endYear este sera = 0
        fprintf(arch,"%u;%u;%s;%lu;%.1f\n",startYear,endYear,string,cantVotes,rating);
        free(string);
    }
    return OK;
}

int query6(FILE* arch, imdbADT imdb){
    toBeginLimitedGenres(imdb);//Ver comentario (1)
    fprintf(arch, "%s;%s;%s\n", "genre", "min", "max");
    float min, max;
    while(hasNextLimitedGenres(imdb)){
        //Ver comentario (1)
        char * genre=getLimitedGenre(imdb);
        min= getLimitedGenreMin(imdb);
        max= getLimitedGenreMax(imdb);
        if(errno==ERROR_ITERATION || errno==ENOMEM)
            return NOTOK;

        fprintf(arch, "%s;%.1f;%.1f\n", genre, min, max);
        free(genre);
        nextLimitedGenres(imdb);
        if(errno==ERROR_ITERATION){
            return NOTOK;
        }
    }
    return OK;
}
