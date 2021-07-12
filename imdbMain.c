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

#define BLOCK 30

#define GENRE_LIM ","
#define VARIABLE_LIM ";"

#define FIRSTQUERY "query1.csv"
#define SECONDQUERY "query2.csv"
#define THIRDQUERY "query3.csv"
#define FOURTHQUERY "query4.csv"
#define FIFTHQUERY "query5.csv"
#define SIXTHQUERY "query6.csv"

enum data {TITLE,STARTING_YEAR,ENDING_YEAR,GENRES,RATING,VOTES};

void noMemoryAbort(void);
int checkarg(int argc,char *argv[],int *year1,int *year2);
char *getLineNoLimitFile(FILE *arch);
void checkopen(FILE **filevec,int queryamount);
char ** copyGenres(char * genresToDivide, unsigned int * dim);
void freeGenres(char **genres, unsigned int dim);


int main(int argc,char *argv[])
{
    int year1,year2;
    if (!checkarg(argc,argv,&year1,&year2))
        return 1;

    FILE *arch;
    arch = fopen(argv[1],"r");

    if (arch == NULL)
    {
        fprintf(stderr,"No se pudo abrir el archivo");
        exit(1);
    }
    imdbADT imdb = new();
    //TODO VALIDAR IMDB CON NULL O USAR ERRNO

    char * line;
    while ((line = getLineNoLimitFile(arch)) != NULL) {
        unsigned int startingYear,endingYear, dim=0;
        size_t votes;
        float rating;
        char * title;
        char ** genres;
        char *token = strtok(line, VARIABLE_LIM);
        char *type = token;
        // Solo leemos datos hasta los votos, salteamos runtimeMinutes
        for (int i = 0; i <= VOTES; i++) {
            switch (i) {
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
            token = strtok(NULL, ";");
        }
        free(line);
        if(startingYear > 0) {
            add(imdb, type, title, genres, dim, rating, votes, startingYear, endingYear);
        }
        free(title);
        freeGenres(genres,dim);
        // liberar el vector
    }

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

}

void query1(FILE *arch, imdbADT imdb)
{
    fprintf(arch, "year;films;series\n");
    toBegin(imdb);
    tListYear aux;

    while (hasNext(imdb))
    {
        aux = next(imdb);
        fprintf(arch, "%u;%u;%u\n", aux->year, aux->numMovies, aux->numSeries);
    }
    fclose(arch);
}

void query2(FILE * arch, imdbADT imdb) {
    toBegin(imdb);
    fprintf(arch, "%s;%s;%s\n", "year", "genre", "films");
    unsigned int year, cantMovies;
    while (hasNext(imdb)) {
        year=getYear(imdb);
        while(hasNextGenre(imdb)){
            char * genre=getGenre(imdb, &cantMovies);
            fprintf(arch, "%d;%s;%d\n", year, genre, cantMovies);
            free(genre);
        }

    }
    fclose(arch);
}

void query3(FILE *arch, imdbADT imdb){
    fprintf(arch, "startYear;film;votesFilm;ratingFilm;serie;votesSerie;ratingSerie\n");
    toBegin(imdb);
    tListYear aux;
    while(hasNext(imdb)) {
        aux=next(imdb);
        if(aux->numMovies == 0)
            fprintf(arch, "%u; ; ; ;%s;%lu;%.1f\n", aux->year, aux->mostVotedSeries.title, aux->mostVotedSeries.votes, aux->mostVotedSeries.rating);
        else if(aux->numSeries == 0)
            fprintf(arch, "%u;%s;%lu;%.1f; ; ; \n", aux->year, aux->mostVotedMovie.title, aux->mostVotedMovie.votes, aux->mostVotedMovie.rating);
        else
            fprintf(arch, "%u;%s;%lu;%.1f;%s;%lu;%.1f\n", aux->year, aux->mostVotedMovie.title, aux->mostVotedMovie.votes, aux->mostVotedMovie.rating, aux->mostVotedSeries.title, aux->mostVotedSeries.votes, aux->mostVotedSeries.rating);
    }
    fclose(arch);
}

void query4(FILE* out,imdbADT imdb)
{
    fprintf(out,"startYear;primaryTitle;numVotes;averageRating\n");
    //printAndFreeMPrec(imdb->mostPopularData.first,out);
    /* ESTO ES EL CÓDIGO DE LA FUNCION DE ARRIBA
     * if ( list == NULL)
        return;
    printAndFreeMPrec(list->tail,out);
    fprintf(out,"%d,%s,%lu,%.1f\n",list->head.year,list->head.title,list->head.votes,list->head.rating);
     */
    fclose(out);
}

void query6(FILE* arch, imdbADT imdb){
    toBeginLimitedGenres(imdb);
    fprintf(arch, "%s;%s;%s\n", "genre", "min", "max");
    float min, max;
    while(hasNextLimitedGenres(imdb)){
        char * genre=getLimitedGenre(imdb, &min, &max);
        fprintf(arch, "%s;%.2f;%.2f", genre, min, max);
        free(genre);
    }
}

char ** copyGenres(char * genresToDivide, unsigned int * dim){
    int i = 0;
    char ** genres;
    genres = malloc(sizeof(char *));
    genres[i++] = strtok(genresToDivide,GENRE_LIM);
    for(; genresToDivide != NULL; i++) {
        genres= realloc(genres, sizeof(char *) * (i+1));
        genres[i] = genresToDivide;
        genresToDivide = strtok(NULL, GENRE_LIM);
    }
    *dim = i;
    return genres;
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

void noMemoryAbort(void)
{
    fprintf(stderr,"No se ha podido reservar memoria\n");
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
        printf("Por favor ingrese un unico archivo a procesar y unicamente dos años\n");
        return NOTOK;
    }
    // valido que me hayan pasado años
    int error = 0;
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

void freeGenres(char **genres, unsigned int dim){
    for (int i = 0; i < dim; i++)
        free(genres[i]);
    free(genres);
}
