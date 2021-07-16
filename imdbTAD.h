#ifndef TPFINAL2_IMDBTAD_H
#define TPFINAL2_IMDBTAD_H

#include <stdio.h>
// R: dejar en mayor o igual a cero!!!!! ( usamos unsigned int)
#define TYPESERIES "tvSeries"
#define TYPEMOVIE "movie"

#define NO_RESTRICTION (0)

#define OK 1
#define NOTOK !OK
#define ERROR_NULL (-1)
#define ERROR_ARG (-2)
#define ERROR_YEAR (-3)
#define ERROR_POS (-4)
#define ERROR_ITERATION (-5)
#define NOT_INITIALIZED (-6)

typedef struct imdbCDT * imdbADT;

//Crea un nuevo imdb vacio
imdbADT new();

//Guarda los anios de restricciones en el struct
void receiveYears(imdbADT imdb, int startYear, int endYear);

//Agrega la informacion de una linea del csv al struct
//Setea errno en ENOMEM si se quedo sin memoria para guardar datos
//Setea errno en ERROR_NULL si se le paso un ADT en NULL
void add(imdbADT imdb,char *type, char *title, char **genres, int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear);

//Setea el iterador de años al primero
void toBegin(imdbADT imdb);

//Chequea si hay un año siguiente
int hasNext(imdbADT imdb);

//UNICAMENTE avanza el iterador de los años
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
int next(imdbADT imdb);

//Setea el iterador de limitedGenres al primero
void toBeginLimitedGenres(imdbADT imdb);

//Chequea si hay un limitedGenre siguiente en la lista
int hasNextLimitedGenres(imdbADT imdb);

//UNICAMENTE avanza el iterador de los limitedGenres
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
int nextLimitedGenres(imdbADT imdb);

//Setea el iterador de Genres al primero del año que se le pase
void toBeginGenre(imdbADT imdb, unsigned int year);

//Chequea si hay un Genre siguiente en la lista
int hasNextGenre(imdbADT imdb);

//UNICAMENTE avanza el iterador de los Genres
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
int nextGenre(imdbADT imdb);

//Devuelve el año (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// en el cual esta parado el iterador de años
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getYear(imdbADT imdb);

//Devuelve el numero de peliculas del año en el cual esta parado el iterado de años
//(SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getYearNumMovies(imdbADT imdb);

//Devuelve el numero de series del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getYearNumSeries(imdbADT imdb);

//Devuelve el titulo de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
//devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getMostVotedMovieTitle(imdbADT imdb);

//Devuelve el Rating de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getMostVotedMovieRating(imdbADT imdb);

//Devuelve el votes de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve NOTOK y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
size_t getMostVotedMovieVotes(imdbADT imdb);

//Devuelve el titulo de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
//devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getMostVotedSeriesTitle(imdbADT imdb);

//Devuelve el rating de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getMostVotedSeriesRating(imdbADT imdb);

//Devuelve el votes de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
//devuelve NOTOK y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
size_t getMostVotedSeriesVotes(imdbADT imdb);

//Devuelve el genero de donde este parado el iterador de generos
//devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
//devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getGenre(imdbADT imdb);

//Devuelve la cant de pelis que tienen el genero de donde este parado el iterador de generos
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getGenreCant(imdbADT imdb);

//Devuelve el genero de donde este parado el iterador de generos con restricciones
//devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
//devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getLimitedGenre(imdbADT imdb);

//Devuelve el rating minimo de las series del genero de donde este parado el iterado de generos con restricciones
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getLimitedGenreMin(imdbADT imdb);

//Devuelve el rating maximo de las series del genero de donde este parado el iterado de generos con restricciones
//devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getLimitedGenreMax(imdbADT imdb);

//Libera todos los recursos reservados por el TAD
void freeIMDB(imdbADT imdb);

// actualiza los datos del Top 100 series en caso de que se hayan cargado nuevos datos
void prepareTop100Series(imdbADT imdb);

// devuelve la cantidad de elementos guardados en el top 100, (en caso de que los datos cargados no lleguen a 100 devuelve cuantos se guardaron)
// si se le pasa NULL en el puntero a la estructura devuelve NOTOK
unsigned int getSizeTop100Series(imdbADT imdb);

//actualiza los datos del Top 100 Movies en caso de que se hayan cargado nuevos datos
void prepareTop100Movies(imdbADT imdb);

//devuelve la cantidad de elementos guardados en el top100 de peliculas (un entero de 0 a 100)
// si se le pasa NULL en el puntero a la estructura devuelve IMDB
unsigned int getsizeTop100Movies(imdbADT imdb);

/*
 * las siguientes funciones devuelven el dato solicitado en su nombre, y piden como argumento el puntero a la estructura
 * y la posicion en el top 100 de la cual se desea extraer informacion, es necesario para obtener datos del top 100 de series llamar
 * a prepareTop100Series antes de que las funciones a continuacion sean utilizadas, ocurre lo mismo con movies.
 * ademas, es necesario llamar a getsize para saber hasta que posicion se guardaron peliculas, en caso de que no lleguen a 100, estas
 *funciones devuelven cuantas peliculas/series se llegaron a almacenar que cumplian los criterios para entrar en el top 100
 * las funciones devuelven NOTOK (NULL en los casos que retorna char *) si se les pasa una posicion invalida ( es decir, supera a la dada en getsize o es menor que 1), si se pasa puntero a NULL
 * o, si hay un problema al reservar memoria. en tal caso, setea a errno en el error correspondiente:
 * 1-ENOMEM si no se pudo reservar memoria
 * 2-ERROR_POS si se pasa una posicion invalida
 * 3-ERROR_NULL si se pasa un puntero a NULL
 * 4-NOT_INITIALIZED si no se llamo a las funciones de preparacion correspondientes (prepareTop100Movies y prepareTop100Series)
 */

unsigned int getMPSStartYear(imdbADT imdb,int position);

unsigned int getMPSEndYear(imdbADT imdb,int position);

// devuelve una COPIA del titulo, es necesario que el frontend se encargue de hacer el free correspondiente
char *getMPSTitle(imdbADT imdb,int position);

size_t getMPSVotes(imdbADT imdb,int position);

float getMPSRating(imdbADT imdb,int position);

unsigned int getMPMYear(imdbADT imdb, int position);

float getMPMRating(imdbADT imdb, int position);

// devuelve una COPIA del titulo, es necesario que el frontend se encargue de hacer el free correspondiente
char * getMPMTitle(imdbADT imdb, int position);

size_t getMPMVotes(imdbADT imdb, int position);

#endif //TPFINAL2_IMDBTAD_H
