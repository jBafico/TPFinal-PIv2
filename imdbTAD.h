#ifndef TPFINAL2_IMDBTAD_H
#define TPFINAL2_IMDBTAD_H

#include <stdio.h>

#define TYPESERIES "tvSeries"
#define TYPEMOVIE "movie"

#define NO_RESTRICTION (0)      // Para cuando no hay restriccion de anios
#define OK 1
#define NOTOK !OK
#define ERROR_NULL (-1)         // Error si el imdb pasado es NULL
#define ERROR_ARG (-2)          // Error en los argumentos ingresados
#define ERROR_YEAR (-3)         // Error en los años ingresados
#define ERROR_POS (-4)          // Error cuando se pasa una posicion inválida
#define ERROR_ITERATION (-5)    // Error en los iteradores de listas
#define NOT_INITIALIZED (-6)    // Error cuando el vector de las queries 4 y 5 no esta inicializado

typedef struct imdbCDT * imdbADT;

// ACLARACION:
// MPM: Most popular movies
// MPS: Most popular series

// Crea un nuevo imdb vacio
// Recibe la minima cantidad de votos para entrar al top y la cantidad de posiciones en el top
imdbADT new (unsigned int minVotes, unsigned int topInserted);

// Guarda los anios de restricciones en el struct
void receiveYears(imdbADT imdb, int startYear, int endYear);

// Agrega la informacion de una linea del csv al struct
// Setea errno en ENOMEM si se quedo sin memoria para guardar datos
// Setea errno en ERROR_NULL si se le paso un ADT en NULL
void add(imdbADT imdb,char *type, char *title, char **genres, unsigned int dim, float rating, size_t votes, unsigned int startYear, unsigned int endYear);

// Setea el iterador de años al primero
void toBegin(imdbADT imdb);

// Chequea si hay un año siguiente
int hasNext(imdbADT imdb);

// UNICAMENTE avanza el iterador de los años
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
int next(imdbADT imdb);

// Setea el iterador de limitedGenres al primero
void toBeginLimitedGenres(imdbADT imdb);

// Chequea si hay un limitedGenre siguiente en la lista
int hasNextLimitedGenres(imdbADT imdb);

// UNICAMENTE avanza el iterador de los limitedGenres
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
int nextLimitedGenres(imdbADT imdb);

// Setea el iterador de Genres al primero del año que se le pase
void toBeginGenre(imdbADT imdb, unsigned int year);

// Chequea si hay un Genre siguiente en la lista
int hasNextGenre(imdbADT imdb);

// UNICAMENTE avanza el iterador de los Genres
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
int nextGenre(imdbADT imdb);

// Devuelve el año (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// en el cual esta parado el iterador de años
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getYear(imdbADT imdb);

// Devuelve el numero de peliculas del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getYearNumMovies(imdbADT imdb);

// Devuelve el numero de series del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getYearNumSeries(imdbADT imdb);

// Devuelve el titulo de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
// Devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getMostVotedMovieTitle(imdbADT imdb);

// Devuelve el Rating de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getMostVotedMovieRating(imdbADT imdb);

// Devuelve el votes de la pelicula mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve NOTOK y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
size_t getMostVotedMovieVotes(imdbADT imdb);

// Devuelve el titulo de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
// Devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getMostVotedSeriesTitle(imdbADT imdb);

// Devuelve el rating de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getMostVotedSeriesRating(imdbADT imdb);

// Devuelve el votes de la serie mas votada del año en el cual esta parado el iterado de años
// (SIN TENER EN CUENTA LAS RESTRICCIONES PASADAS POR ARGUMENTOS)
// Devuelve NOTOK y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
size_t getMostVotedSeriesVotes(imdbADT imdb);

// Devuelve el genero de donde este parado el iterador de generos
// Devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
// Devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getGenre(imdbADT imdb);

// Devuelve la cant de pelis que tienen el genero de donde este parado el iterador de generos
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
unsigned int getGenreCant(imdbADT imdb);

// Devuelve el genero de donde este parado el iterador de generos con restricciones
// Devuelve NULL y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
// Devuelve NULL y setea el errno en ENOMEM si hay un error en el uso del copy al haber quedado sin memoria
char * getLimitedGenre(imdbADT imdb);

// Devuelve el rating minimo de las series del genero de donde este parado el iterado de generos con restricciones
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getLimitedGenreMin(imdbADT imdb);

// Devuelve el rating maximo de las series del genero de donde este parado el iterado de generos con restricciones
// Devuelve y setea el errno en ERROR_ITERATION si hay un error en el uso del iterador
float getLimitedGenreMax(imdbADT imdb);

// Libera todos los recursos reservados por el TAD
void freeIMDB(imdbADT imdb);

/*
 *  Justificacion de uso de las dos funciones siguientes:
 *  Se decidio hacer un vector dinamico de punteros porque el desafio principal era sacar los datos al reves, para ello, se crearon
 *  las dos funciones debajo de este texto, estas funciones preparan el vector copiando de la lista iterativamente, luego se
 *  puede acceder a los campos de manera directa, si bien comprendemos que hay formas mas eficientes de hacer esto,
 *  nos parecio que era lo mas comodo para el usuario que usa el tad, ya que puede preguntar directamente por una pelicula
 *  en una posicion dada.
 */

// Actualiza los datos del Top series en caso de que se hayan cargado nuevos datos
void prepareTopSeries(imdbADT imdb);

// Actualiza los datos del Top 100 Movies en caso de que se hayan cargado nuevos datos
void prepareTopMovies(imdbADT imdb);

// Devuelve la cantidad de elementos guardados en el top, (en caso de que los datos cargados no lleguen a 100 devuelve cuantos se guardaron)
// Si se le pasa NULL en el puntero a la estructura devuelve NOTOK
unsigned int getSizeTopSeries(imdbADT imdb);

// Devuelve la cantidad de elementos guardados en el top100 de peliculas (un entero de 0 a 100)
// Si se le pasa NULL en el puntero a la estructura devuelve IMDB
unsigned int getSizeTopMovies(imdbADT imdb);

/*
 * Las siguientes funciones devuelven el dato solicitado en su nombre, y piden como argumento el puntero a la estructura
 * y la posicion en el top 100 de la cual se desea extraer informacion, es necesario para obtener datos del top 100 de series llamar
 * a prepareTop100Series antes de que las funciones a continuacion sean utilizadas, ocurre lo mismo con movies.
 * ademas, es necesario llamar a getsize para saber hasta que posicion se guardaron peliculas, en caso de que no lleguen a 100, estas
 * funciones devuelven cuantas peliculas/series se llegaron a almacenar que cumplian los criterios para entrar en el top 100
 * las funciones devuelven NOTOK (NULL en los casos que retorna char *) si se les pasa una posicion invalida ( es decir, supera a la dada en getsize o es menor que 1), si se pasa puntero a NULL
 * o, si hay un problema al reservar memoria. en tal caso, setea a errno en el error correspondiente:
 * 1-ENOMEM si no se pudo reservar memoria
 * 2-ERROR_POS si se pasa una posicion invalida
 * 3-ERROR_NULL si se pasa un puntero a NULL
 * 4-NOT_INITIALIZED si no se llamo a las funciones de preparacion correspondientes (prepareTopMovies y prepareTop100Series)
 */

unsigned int getMPSStartYear(imdbADT imdb,int position);

unsigned int getMPSEndYear(imdbADT imdb,int position);

// devuelve una COPIA del titulo, es necesario que el frontend se encargue de hacer el free correspondiente
char *getMPSTitle(imdbADT imdb,int position);

size_t getMPSVotes(imdbADT imdb,int position);

float getMPSRating(imdbADT imdb,int position);

unsigned int getMPMYear(imdbADT imdb, int position);

float getMPMRating(imdbADT imdb, int position);

// Devuelve una COPIA del titulo
// Es necesario que el frontend se encargue de hacer el free correspondiente
char * getMPMTitle(imdbADT imdb, int position);

size_t getMPMVotes(imdbADT imdb, int position);

#endif //TPFINAL2_IMDBTAD_H
