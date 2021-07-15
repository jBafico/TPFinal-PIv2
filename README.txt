Autores:
- Agustin Zakalik 62068 (TheSpackJarrow)
- Franco Bosetti 61654 (francobosetti)
- Juan Cruz Bafico 62070 (jBafico)
- Santino Augusto Ranucci 62092 (sranucci)


Hola buenas, en este .txt se detallaran los pasos a seguir
para poder generar los ejecutables y luego ejecutarlos
de la forma correcta. Es de vital importancia que todos
los archivos del comprimido se mantengan juntos en el
mismo directorio a todo momento

1) Abrir la terminal y estar ubicado en el mismo directorio
en el cual se encuentran los archivos que estaban dentro
de este comprimido.

2) Ingresar a la terminal lo siguiente: make front.o
(Esto generara el .o del front)

3) Ingresar a la terminal lo siguiente: make back.o
(Esto generara el .o del back)

4) Una vez generados front.o y back.o ingresar a la
terminal lo siguiente: make all
(Esto generara el ejecutable completo llamado imdb)

5) Al ya tener el ejecutable imdb, ya puede usarlo
con su .csv, para eso ingrese lo siguiente a la
terminal: ./imdb (NOMBRE DEL .csv) (Año inicial) (Año final)
Es importante que el .csv ingresado este en el
mismo directorio que el ejecutable
Los campos (Año inicial) y (Año final) son restricciones para las Queries 5 y 6

Usted puede:    1-No ingresar años por lo cual no hay restricciones
                2-Ingresar solo el Año inicial por lo cual hay restricciones
                  solo para el año inicial pero no final
                3-Ingresar ambos años por lo cual hay doble restriccion

Si usted decide ingresar cualquier opcion que no fue mencionada previamente
el programa abortara y le avisara que ingreso mal los argumentos.

Al hacer el paso anterior ya se le crearon las
6 queries que genera el programa

OPCIONAL:
6) Si usted decide querer remover los .o
y el ejecutable imdb que genero el archivo puede hacerlo
escribiendo en la terminal lo siguiente: make clean
