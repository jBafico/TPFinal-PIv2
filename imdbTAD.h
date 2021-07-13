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

//Crea un nuevo imdb vacio
imdbADT new();

//Guarda los anios de restricciones en el struct
void receiveYears(imdbADT imdb, int startYear, int endYear);

//Agrega la informacion de una linea del csv al struct
void add(imdbADT imdb,char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear);

//Setea el iterador de años al primero
void toBegin(imdbADT imdb);

//Chequea si hay un año siguiente
int hasNext(imdbADT imdb);

//UNICAMENTE avanza el iterador de los años
int next(imdbADT imdb);

//Setea el iterador de limitedGenres al primero
void toBeginLimitedGenres(imdbADT imdb);

//Chequea si hay un limitedGenre siguiente en la lista
int hasNextLimitedGenres(imdbADT imdb);

//UNICAMENTE avanza el iterador de los limitedGenres
int nextLimitedGenres(imdbADT imdb);

//Setea el iterador de Genres al primero del año que se le pase
void toBeginGenre(imdbADT imdb, unsigned int year);

//Chequea si hay un Genre siguiente en la lista
int hasNextGenre(imdbADT imdb);

//UNICAMENTE avanza el iterador de los Genres
int nextGenre(imdbADT imdb);

//Devuelve el año (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// en el cual esta parado el iterado de años
unsigned int getYear(imdbADT imdb);

//Devuelve el numero de peliculas del año en el cual esta parado el iterado de años
//(SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
unsigned int getYearNumMovies(imdbADT imdb);

//Devuelve el numero de series del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
unsigned int getYearNumSeries(imdbADT imdb);

//Devuelve el titulo de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
char * getMostVotedMovieTitle(imdbADT imdb);

//Devuelve el Rating de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
float getMostVotedMovieRating(imdbADT imdb);

//Devuelve el votes de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
size_t getMostVotedMovieVotes(imdbADT imdb);

//Devuelve el titulo de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
char * getMostVotedSeriesTitle(imdbADT imdb);

//Devuelve el rating de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
float getMostVotedSeriesRating(imdbADT imdb);

//Devuelve el votes de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
size_t getMostVotedSeriesVotes(imdbADT imdb);

//Devuelve el genero de donde este parado el iterador de generos
char * getGenre(imdbADT imdb);

//Devuelve la cant de pelis que tienen el genero de donde este parado el iterador de generos
unsigned int getGenreCant(imdbADT imdb);

//Devuelve el genero de donde este parado el iterador de generos con restricciones
char * getLimitedGenre(imdbADT imdb);

//Devuelve el rating minimo de las series del genero de donde este parado el iterado de generos con restricciones
float getLimitedGenreMin(imdbADT imdb);

//Devuelve el rating maximo de las series del genero de donde este parado el iterado de generos con restricciones
float getLimitedGenreMax(imdbADT imdb);

//Libera todos los recursos reservados por el TAD
void freeIMDB(imdbADT imdb);

//Inicia iterador de series
void toBeginTop100Series(imdbADT imdb);

//Chequea si hay una serie siguiente
int hasNextTop100Series(imdbADT imdb);

//Avanza a la siguiente serie
int NextTop100Series(imdbADT imdb);

//Devuelve el nombre.
//ES NECESARIO LIBERARLO PORQUE SE DEVUELVE COMO COPIA
char *getTop100SeriesName(imdbADT imdb);

//Devuelve startYear, año de inicio de la serie
unsigned int getTop100SeriesStartYear(imdbADT imdb);

//Devuelve endYear, año final de la serie, cero si no tiene año final
unsigned int getTop100SeriesEndYear(imdbADT imdb);

//Devuelve el rating de la serie
float getTop100SeriesRating(imdbADT imdb);

//Inicializa el iterador sobre el top 100 de peliculas
// arranca por la que tiene rating mas alto
void toBeginTop100Movies(imdbADT imdb);

//Chequea si hay una pelicula siguiente
// Devuelve 1 si hay, cero si no hay
int hasNextTop100Movies(imdbADT imdb);

//Avanza el iterador a la siguiente pelicula
int nextTop100Movies(imdbADT imdb);

// devuelve como copia un string con el titulo.
// Es necesario que el front le haga free a esto
char * getTop100MovieTitle(imdbADT imdb);

// devuelve el año de inicio de la pelicula
unsigned int getTop100MovieStartYear(imdbADT imdb);

//Devuelve la cantidad de votos de una de las 100 peliculas mas votadas
unsigned int getTop100MovieVotes(imdbADT imdb);

//Devuelve el rating de de una de las 100 peliculas mas votadas
float getTop100MovieRating(imdbADT imdb);

#endif //TPFINAL2_IMDBTAD_H

