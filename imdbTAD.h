#ifndef TPFINAL2_IMDBTAD_H
#define TPFINAL2_IMDBTAD_H

#define OK 1
#define NOTOK !OK
#define ERROR_CODE (-1)
#define NO_RESTRICTION (-1)
#define TYPESERIES "tvSeries"
#define TYPEMOVIE "movie"

#include <stdio.h>

typedef struct imdbCDT * imdbADT;

//Crea el puntero al struct
imdbADT new();

//Guarda los años de restricciones en el struct
void receiveYears(imdbADT imdb, int startYear, int endYear);

//Agrega la informacion de una linea del csv al struct
void add(imdbADT imdb,char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear);

//Setea el iterador de años al primero
void toBegin(imdbADT imdb);

//Checkea si hay un año siguiente
int hasNext(imdbADT imdb);

//UNICAMENTE avanza el iterador de los años
int next(imdbADT imdb);

//Setea el iterador de LimitedGenres al primero
void toBeginLimitedGenres(imdbADT imdb);

//Checkea si hay un LimitedGenre siguiente en la lista
int hasNextLimitedGenres(imdbADT imdb);

//UNICAMENTE avanza el iterador de los LimitedGenres
int nextLimitedGenres(imdbADT imdb);

//Setea el iterador de Genres al primero del año que se le pase
void toBeginGenre(imdbADT imdb, unsigned int year);

//Checkea si hay un Genre siguiente en la lista
int hasNextGenre(imdbADT imdb);

//UNICAMENTE avanza el iterador de los Genres
int nextGenre(imdbADT imdb);

//devuelve el año (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS) en el cual esta parado el iterado de años
unsigned int getYear(imdbADT imdb);

//devuelve el numero de peliculas del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
unsigned int getYearNumMovies(imdbADT imdb);

//devuelve el numero de series del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
unsigned int getYearNumSeries(imdbADT imdb);

//devuelve el titulo de la pelicula mas votada del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
char * getMostVotedMovieTitle(imdbADT imdb);

//devuelve el Rating de la pelicula mas votada del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
float getMostVotedMovieRating(imdbADT imdb);

//devuelve el votes de la pelicula mas votada del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
size_t getMostVotedMovieVotes(imdbADT imdb);

//devuelve el titulo de la serie mas votada del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
char * getMostVotedSeriesTitle(imdbADT imdb);

//devuelve el rating de la serie mas votada del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
float getMostVotedSeriesRating(imdbADT imdb);

//devuelve el votes de la serie mas votada del año en el cual esta parado el iterado de años (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
size_t getMostVotedSeriesVotes(imdbADT imdb);

//devuelve el genero de donde este parado el iterador de generos
char * getGenre(imdbADT imdb);

//devuelve la cant de pelis que tienen el genero de donde este parado el iterador de generos
unsigned int getGenreCant(imdbADT imdb);

//devuelve el genero de donde este parado el iterador de generos con restricciones
char * getLimitedGenre(imdbADT imdb);

//devuelve el rating minimo de las series del genero de donde este parado el iterado de generos con restricciones
float getLimitedGenreMin(imdbADT imdb);

//Devuelve el rating maximo de las series del genero de donde este parado el iterado de generos con restricciones
float getLimitedGenreMax(imdbADT imdb);

//Libera todos los recursos reservados por el TAD
void freeIMDB(imdbADT imdb);

#endif //TPFINAL2_IMDBTAD_H

