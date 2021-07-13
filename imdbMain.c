#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "imdbTAD.h"

#define ARG_AMOUNT 2
#define ARGWITHYEAR 4
#define QUERY_AMOUNT 6
#define FIRSTYEARPOSITION 2
#define SECONDYEARPOSITION 3

enum fileposition {Q1=0,Q2,Q3,Q4, Q5, Q6};

#define BLOCK 30

#define GENRE_LIM ","
#define VARIABLE_LIM ";"

#define FIRSTQUERY "query1.csv"
#define SECONDQUERY "query2.csv"
#define THIRDQUERY "query3.csv"
#define FOURTHQUERY "query4.csv"
#define FIFTHQUERY "query5.csv"
#define SIXTHQUERY "query6.csv"

enum data {TYPE,TITLE,STARTING_YEAR,ENDING_YEAR,GENRES,RATING,VOTES};

void noMemoryAbort(imdbADT imdb);
int checkarg(int argc,char *argv[],int *year1,int *year2);
char *getLineNoLimitFile(FILE *arch);
void checkopen(FILE **filevec,int queryamount);
char ** copyGenres(char * genresToDivide, unsigned int * dim);
void query1(FILE *arch, imdbADT imdb, unsigned int year);
void query2(FILE * arch, imdbADT imdb, unsigned int year);
void query3(FILE *arch, imdbADT imdb, unsigned int year);
void query6(FILE* arch, imdbADT imdb);
void closeFiles(FILE **filevec,int queryAmount);
void skipline(FILE *arch);


int main(int argc,char *argv[])
{
    int year1,year2;
    // mesaje a salida de error?
    if (!checkarg(argc,argv,&year1,&year2)){
        //todo hacer mesg de error
        return 1;
    }

    FILE *arch;
    arch = fopen(argv[1],"r");

    if (arch == NULL)
    {
        fprintf(stderr,"No se pudo abrir el archivo");
        exit(1);
    }
    skipline(arch);
    imdbADT imdb = new();
    //TODO VALIDAR IMDB CON NULL Y USAR ERRNO (imdb == NULL || errno == ENOMEM)
    //todo guardar los años potentes en el struct
    char * line;
    //  desconozco si es mas eficiente tener las variables afuera,
    // pero me hace rudio que se declaremos en todos los ciclos
    while ((line = getLineNoLimitFile(arch)) != NULL) {
        unsigned int startingYear,endingYear, dim=0;
        size_t votes;
        float rating;
        char * title, * type;
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
                    genres=copyGenres(token,&dim);
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
            add(imdb, type, title, genres, dim, rating, votes, startingYear, endingYear);
        }
        free(line);
        free(genres);
    }
    fclose(arch);
    // inicializo los archivos
    FILE *q1 = fopen(FIRSTQUERY,"w");
    FILE *q2 = fopen(SECONDQUERY,"w");
    FILE *q3 = fopen(THIRDQUERY,"w");
    FILE *q4 = fopen(FOURTHQUERY,"w");
    FILE *q5 = fopen(FIFTHQUERY,"w");
    FILE *q6 = fopen(SIXTHQUERY,"w");
    FILE *filevec[] = {q1,q2,q3,q4,q5,q6};
    checkopen(filevec,QUERY_AMOUNT);
    // codigo para correr las 6 queries,
    toBegin(imdb);
    fprintf(filevec[Q1], "year;films;series\n");
    fprintf(filevec[Q2], "%s;%s;%s\n", "year", "genre", "films");
    fprintf(filevec[Q3], "startYear;film;votesFilm;ratingFilm;serie;votesSerie;ratingSerie\n");
    unsigned int year; //declaramos year para usar dentro del while
    while(hasNext(imdb)){
        year=getYear(imdb);
        query1(filevec[Q1], imdb, year);
        query2(filevec[Q2], imdb, year);
        query3(filevec[Q3], imdb, year);
        next(imdb);
    }
    query6(filevec[Q6], imdb);

    closeFiles(filevec, QUERY_AMOUNT);

}

void skipline(FILE *arch)
{
    while( fgetc(arch) != '\n' )
        ;
}

void query1(FILE *arch, imdbADT imdb, unsigned int year)
{
    unsigned int numMovies, numSeries;
    numMovies= getYearNumMovies(imdb);
    numSeries= getYearNumSeries(imdb);
    fprintf(arch, "%u;%u;%u\n", year, numMovies, numSeries);
}

void query2(FILE * arch, imdbADT imdb, unsigned int year) {
    toBeginGenre(imdb, year);
    unsigned int cantMovies;
    while(hasNextGenre(imdb)){
        char * genre=getGenre(imdb);
        cantMovies= getGenreCant(imdb);
        fprintf(arch, "%d;%s;%d\n", year, genre, cantMovies);
        free(genre);
        nextGenre(imdb);
    }
}

void query3(FILE *arch, imdbADT imdb, unsigned int year){
    unsigned int numMovies, numSeries;
    numMovies= getYearNumMovies(imdb);
    numSeries= getYearNumSeries(imdb);
    //el llamado a las funciones que vienen ahora esta hecho de esta manera porque si por ejemplo
    //no existen peliculas en ese año specifico las funciones que agarran los parametros de las
    //peliculas estarian accediendo a NULL. Al hacerlo de la forma que nosotros implementamos
    //esas situaciones no se presentan.
    if(numMovies == 0) {
        char * serieTitle= getMostVotedSeriesTitle(imdb);
        size_t serieVotes= getMostVotedSeriesVotes(imdb);
        float serieRating= getMostVotedSeriesRating(imdb);
        fprintf(arch, "%u; ; ; ;%s;%lu;%.1f\n", year, serieTitle, serieVotes, serieRating);
    }
    else if(numSeries == 0) {
        char * movieTitle= getMostVotedMovieTitle(imdb);
        size_t movieVotes= getMostVotedMovieVotes(imdb);
        float movieRating= getMostVotedMovieRating(imdb);
        fprintf(arch, "%u;%s;%lu;%.1f; ; ; \n", year, movieTitle, movieVotes, movieRating);
    }
    else {
        char * serieTitle= getMostVotedSeriesTitle(imdb);
        size_t serieVotes= getMostVotedSeriesVotes(imdb);
        float serieRating= getMostVotedSeriesRating(imdb);
        char * movieTitle= getMostVotedMovieTitle(imdb);
        size_t movieVotes= getMostVotedMovieVotes(imdb);
        float movieRating= getMostVotedMovieRating(imdb);
        fprintf(arch, "%u;%s;%lu;%.1f;%s;%lu;%.1f\n", year, movieTitle, movieVotes, movieRating, serieTitle, serieVotes, serieRating);
    }
}

void query4(FILE* out,imdbADT imdb)
{
    fprintf(out,"startYear;primaryTitle;numVotes;averageRating\n");
    //printAndFreeMPrec(imdb->mostPopularData.first,out);
    /* ESTO ES EL CÓDIGO DE LA FUNCION DE ARRIBA
     * if ( list == NULL)
        return;
    printAndFreeMPrec(list->tail,out);
    fprintf(out,"%d;%s;%lu;%.1f\n",list->head.year,list->head.title,list->head.votes,list->head.rating);
     */
    fclose(out);
}

void query6(FILE* arch, imdbADT imdb){
    toBeginLimitedGenres(imdb);
    fprintf(arch, "%s;%s;%s\n", "genre", "min", "max");
    float min, max;
    while(hasNextLimitedGenres(imdb)){
        char * genre=getLimitedGenre(imdb);
        min= getLimitedGenreMin(imdb);
        max= getLimitedGenreMax(imdb);
        fprintf(arch, "%s;%.1f;%.1f", genre, min, max);
        free(genre);
        nextLimitedGenres(imdb);
    }
}

char ** copyGenres(char * genresToDivide, unsigned int * dim){
    int i = 0;
    char ** genres = NULL;
    for(char * aux = strtok(genresToDivide, GENRE_LIM); aux != NULL; aux = strtok(NULL, GENRE_LIM), i++) {
        genres = realloc(genres, sizeof(char*)*(i+1));
        genres[i] = aux;
    }
    *dim = i;
    return genres;
}
void closeFiles(FILE **filevec,int queryAmount){
    for (int i = 0; i < queryAmount; ++i) {
        fclose(filevec[i]);
    }
}

void checkopen(FILE **filevec,int queryamount)
{
    char *vecfilenames[] = {FIRSTQUERY,SECONDQUERY,THIRDQUERY,FOURTHQUERY,FIFTHQUERY,SIXTHQUERY};
    for (int i = 0; i < queryamount ;i++)
    {
        if ( filevec[i] == NULL)
        {
            fprintf(stderr,"problemas al abrir el archivo %s",vecfilenames[i]);
            for ( int j = 0; j < i; j++)
            {
                // cierro los archivos previamente abiertos
                fclose(filevec[j]);
            }
            exit(1);
        }
    }
}

char *getLineNoLimitFile(FILE *arch)
{
    int c, i = 0;
    char *s = NULL; // para que el primer realloc funcione como malloc
    while ((c = fgetc(arch)) != '\n')
    {
        if(c==EOF)
            return NULL;
        if (i % BLOCK == 0){
            s = realloc(s, i + BLOCK); // multiplicar por sizeof(char) no es necesario
            /**controlFlag=checkNULL(s);
            if(*controlFlag==ERROR_CODE)
                return NULL;*/
        }
        s[i++] = c;
    }
    s = realloc(s, i + 1); // Liberamos lo que sobra del ultimo bloque
    /**controlFlag=checkNULL(s);
    if(*controlFlag==ERROR_CODE)
        return NULL;*/
    s[i] = 0;
    return s;
}

// DEBE LIBERAR LA MEMORIA DEL TAD
void noMemoryAbort(imdbADT imdb)
{
    fprintf(stderr,"No se ha podido reservar memoria\n");
    freeIMDB(imdb); //TODO crear funcion free
    exit(1);
}

void checkYearFormat(char *strtocheck,int *errorflag)
{
    for (int i = 0; strtocheck[i] ; i++)
    {
        if ( !isdigit(strtocheck[i]) )
        {
            //si el string no es un digito propiamente formado, prendo el flag de error code
            *errorflag = ERROR_CODE;
            return;
        }
    }
}

// ojo esta funcion
int checkarg(int argc,char *argv[],int *year1,int *year2)
{
    if ( argc < ARG_AMOUNT || argc > ARGWITHYEAR)
    {
        // Como maximo 2 anios -> esto deberia ir a salida de error
        printf("Por favor ingrese un unico archivo a procesar y unicamente dos años\n");
        return NOTOK;
    }
    // valido que me hayan pasado años
    int error = 0;
    //todo ver si cambio esto de arriba
    if ( argc >= FIRSTYEARPOSITION + 1)
    {
        checkYearFormat(argv[FIRSTYEARPOSITION],&error);
    }
    if ( argc == SECONDYEARPOSITION + 1 && error != ERROR_CODE) {
        checkYearFormat(argv[SECONDYEARPOSITION],&error);
    }
    if ( error == ERROR_CODE ) {
        printf("por favor, ingrese años validos\n");
        return NOTOK;
    }
    int firstyear,secondyear;
    if ( argc >= FIRSTYEARPOSITION + 1)
        sscanf(argv[FIRSTYEARPOSITION],"%d",&firstyear);
    else
        firstyear = NO_RESTRICTION;
    if ( argc == SECONDYEARPOSITION + 1)
        sscanf(argv[SECONDYEARPOSITION],"%d",&secondyear);
    else
        secondyear = NO_RESTRICTION;
    if (firstyear != NO_RESTRICTION && secondyear != NO_RESTRICTION && secondyear < firstyear ) {
        printf("el año ingresado es incorrecto\n");
        return NOTOK;
    }

    // si los años son validos reemplazo
    *year1 = firstyear;
    *year2 = secondyear;
    return OK;
}
