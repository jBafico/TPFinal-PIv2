#ifndef TPFINAL2_IMDBTAD_H
#define TPFINAL2_IMDBTAD_H

#define OK 1
#define NOTOK !OK
#define ERROR_CODE (-1)
#define NO_RESTRICTION (-1)
#define TYPESERIES "tvSeries"
#define TYPEMOVIE "movie"
#define EPOS (-2)
#define NOTINITIALIZED (-3)

#define ITERATIVE_ERROR (-2)

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
// en el cual esta parado el iterador de años
int getYear(imdbADT imdb);

//Devuelve el numero de peliculas del año en el cual esta parado el iterado de años
//(SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
int getYearNumMovies(imdbADT imdb);

//Devuelve el numero de series del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
int getYearNumSeries(imdbADT imdb);

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
int getGenreCant(imdbADT imdb);

//Devuelve el genero de donde este parado el iterador de generos con restricciones
char * getLimitedGenre(imdbADT imdb);

//Devuelve el rating minimo de las series del genero de donde este parado el iterado de generos con restricciones
float getLimitedGenreMin(imdbADT imdb);

//Devuelve el rating maximo de las series del genero de donde este parado el iterado de generos con restricciones
float getLimitedGenreMax(imdbADT imdb);

//Libera todos los recursos reservados por el TAD
void freeIMDB(imdbADT imdb);

// actualiza los datos del Top 100 series en caso de que se hayan cargado nuevos datos
void prepareTop100Series(imdbADT imdb);

// devuelve la cantidad de elementos guardados en el top 100, (en caso de que los datos cargados no lleguen a 100 devuelve cuantos se guardaron)
int getsizeTop100Series(imdbADT imdb);

/*
 * devuelve en parametros de salida los datos requeridos, si no se desea obtener algun dato, se le debe pasar NULL como parametro
 * en caso de que sea la primera carga  no es necesario llamar a prepareTop100Series, si se hace una carga adicional,
 * es necesario llamar a dicha funcion para * que se actualicen los datos
 */
int getDataFromPositionOfTop100Series(imdbADT imdb, int position, unsigned int *startYear, unsigned int *endYear, float *rating, unsigned int * cantVotes, char **title)



#endif //TPFINAL2_IMDBTAD_H

