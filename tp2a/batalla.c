#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "perfil.h"
#include "batalla.h"


const int VIDA_INMOVILES = 200;
const int VIDA_MOVILES = 100;
const int ATAQUE_INMOVILES = 10;
const int ATAQUE_MOVILES = 50;
const int ENERGIA_MAX = 10;
const int ENERGIA_INICIAL = 5;
const int ENERGIA_MOVILES = 3;
const int ENERGIA_INMOVILES = 8;
const int CANTIDAD_MAX_JUGADORES = 2;
const int COSTO_INMOVILES = 8;
const int COSTO_MOVILES = 3;
const int INTENSIDAD_MAX = 10;
const int FILA_HOMBRES = 9;

const char HOMBRES = 'H';
const char ELFOS = 'E';
const char URUKHAI = 'U';
const char ORCOS = 'O';
const char RESPUESTA_POSITIVA = 'S';
const char RESPUESTA_NEGATIVA = 'N';


//Inicializa el struct de personaje.
void limpiar_personajes(personaje_t *personaje){
  personaje->vida = 0;
  personaje->ataque = 0;
  personaje->fila = 0;
  personaje->columna = 0;
}

//Inicializa el struct de juego.
void limpiar_juego(juego_t *juego){
   juego->cantidad_rohan = 0;
	 juego->llegadas_rohan = 0;
	 juego->plus_rohan = 0;
   juego->cantidad_isengard = 0;
 	 juego->llegadas_isengard = 0;
 	 juego->plus_isengard = 0;
   juego->cantidad_jugadores = 0;
   juego->jugador1.energia = ENERGIA_INICIAL;
   juego->jugador2.energia = ENERGIA_INICIAL;
}

/*
 * Tomara la intensidad de perfil y le multiplicara un numero entre 0 y 5
 * para modificar la vida y el ataque
 */
void definir_plus_rohan(juego_t *juego, int intensidad){
    int plus;
    int numero = rand () % 6;
    plus = intensidad * numero;
    juego->plus_rohan = plus;

}

/*
 *Tomara la intensidad de perfil y le multiplicara un numero entre 0 y 5
 *para modificar la vida y el ataque.
 */
void definir_plus_isengard(juego_t *juego, int intensidad){
    int plus;
    int numero = rand () % 6;
    plus = intensidad * numero;
        juego->plus_isengard = plus;
}


void definir_pluses(juego_t* juego, int intensidad){
  int nivel_opuesto = INTENSIDAD_MAX - intensidad;
  if(juego->jugador1.tipo == BANDO_DEFENSIVO){
    definir_plus_rohan(juego, intensidad);
    definir_plus_isengard(juego, nivel_opuesto);
  }else{
    definir_plus_isengard(juego, intensidad);
    definir_plus_rohan(juego, nivel_opuesto);
  }
}


//Forma la matriz del terreno del juego.
void limpiar_terreno(juego_t* juego){
  for(int i = 0; i < MAX_TERRENO_FIL; i++){
      for(int j = 0; j < MAX_TERRENO_COL; j++){
          juego->terreno[i][j] = ' ';
      }
  }
}

//Imprime la matriz y sus bordes.
void imprimir_terreno(juego_t* juego){
  limpiar_terreno( juego);
  for(int i = 0; i < juego->cantidad_rohan; i++){
    juego->terreno[juego->rohan[i].fila][juego->rohan[i].columna] = juego->rohan[i].codigo;
  }
  for(int i = 0; i < juego->cantidad_isengard; i++){
    juego->terreno[juego->isengard[i].fila][juego->isengard[i].columna] = juego->isengard[i].codigo;
  }
  printf("+ ");
  for(int i = 1; i<MAX_TERRENO_COL+1; i++){
      printf("= ");
  }
  printf("+ \n");
  for(int i = 0; i < MAX_TERRENO_FIL; i++){
      printf("| ");
      for(int j = 0; j < MAX_TERRENO_COL; j++){
          printf(" %c", juego->terreno[i][j]);
      }
      printf("| \n");
  }
  printf("+ ");
  for(int i = 1; i<MAX_TERRENO_COL+1; i++){
      printf("= ");
  }
  printf("+ \n");
}


//Guarda los datos de los personajes inmoviles para cargarlos a su vector correspondiente.
personaje_t personaje_inmovil_nuevo(char codigo, int plus, int columna, int fila){
  personaje_t personaje;
  personaje.vida = VIDA_INMOVILES - plus;
  personaje.ataque = ATAQUE_INMOVILES + plus;
  personaje.codigo = codigo;
  personaje.fila = fila;
  personaje.columna = columna;
  return personaje;
}

//Guarda los datos de los personajes moviles para cargarlos a su vector correspondiente.
personaje_t personaje_movil_nuevo(char codigo, int plus, int columna, int fila){
  personaje_t personaje;
  personaje.vida = VIDA_MOVILES - plus;
  personaje.ataque = ATAQUE_MOVILES + plus;
  personaje.codigo = codigo;
  personaje.fila = fila;
  personaje.columna = columna;
  return personaje;
}

//Chequea que la poscion del terreno este vacia.
bool esta_vacio(char codigo){
  return (codigo == ' ');
}

//Asigna una posicion del terreno a los primeros 3 elfos del juego, para madarlo al vector de rohan.
void elfos_iniciales(juego_t* juego){
  int x;
  int y;
  for(int i=0; i < 3; i++){
    do{
      x = rand () % 10;
      y = rand () % 4 + 5;
    } while(!esta_vacio(juego->terreno[y][x]));
    juego->rohan[juego->cantidad_rohan] = personaje_inmovil_nuevo(ELFOS, juego->plus_rohan, x, y);
    juego->cantidad_rohan++;
  }
}

//Asigna una posicion del terreno a los primeros 3 urukhai del juego y lo al vector de isengard.
void urukhai_iniciales(juego_t* juego){
    int x;
    int y;
    for(int i=0; i < 3; i++){
      do{
        x = rand () % 10;
        y = rand () % 4 + 1;
      } while(!esta_vacio(juego->terreno[y][x]));
      juego->isengard[juego->cantidad_isengard] = personaje_inmovil_nuevo(URUKHAI, juego->plus_isengard, x, y);
      juego->cantidad_isengard++;
    }
}

/*
 * Le preguntara al usuario cuantos jugadores van a jugar
 */
void averiguar_cant_jugadores(int* cantidad){
  printf("Cuantos jugadores son? (1 o 2)\n");
  scanf("%i", cantidad);
  if(*cantidad > CANTIDAD_MAX_JUGADORES || *cantidad < 1){
    printf("Valorno valido. Ingrese 1 o 2.\n");
    averiguar_cant_jugadores( cantidad);
  }
}


void posicionar_personaje(juego_t* juego, personaje_t personaje){
  if(personaje.codigo == ELFOS || personaje.codigo == HOMBRES){
    juego->rohan[juego->cantidad_rohan] = personaje;
    juego->cantidad_rohan++;
  }else{
    juego->isengard[juego->cantidad_isengard] = personaje;
    juego->cantidad_isengard++;
  }
  juego->terreno[personaje.fila][personaje.columna] = personaje.codigo;
}

//Se fiajra que judaor es defensivo y le asignara la enregia del jugador a la variable
int energia_defensivo(juego_t* juego){
  int energia;
  if(juego->jugador1.tipo == BANDO_DEFENSIVO){
    energia = juego->jugador1.energia;
  }else{
    energia = juego->jugador2.energia;
  }
  return energia;
}


//Se fiajra que judaor es ofensivo y le asignara la enregia del jugador a la variable
int energia_ofensivo(juego_t* juego){
  int energia;
  if(juego->jugador1.tipo == BANDO_OFENSIVO){
    energia = juego->jugador1.energia;
  }else{
    energia = juego->jugador2.energia;
  }
  return energia;
}


/*
 * Se fijara que jugador es ofensivo y que personaje ingreso.
 * Y depende el personaje le quitara la energia correspondiente
 */
void sacar_energia_ofensivo(juego_t* juego, char codigo){
  if(codigo == ORCOS){
    if(juego->jugador1.tipo == BANDO_OFENSIVO){
      juego->jugador1.energia -= ENERGIA_MOVILES;
    }else{
      juego->jugador2.energia -= ENERGIA_MOVILES;
    }
  }else{
    if(juego->jugador1.tipo == BANDO_OFENSIVO){
      juego->jugador1.energia -= ENERGIA_INMOVILES;
    }else{
      juego->jugador2.energia -= ENERGIA_INMOVILES;
    }
  }
}


/*
 * Se fijara que jugador es defensivo y que personaje ingreso.
 * Y depende el personaje le quitara la energia correspondiente
 */
void sacar_energia_defensivo(juego_t* juego, char codigo){
  if(codigo == HOMBRES){
    if(juego->jugador1.tipo == BANDO_DEFENSIVO){
      juego->jugador1.energia -= ENERGIA_MOVILES;
    }else{
      juego->jugador2.energia -= ENERGIA_MOVILES;
    }
  }else{
    if(juego->jugador1.tipo == BANDO_DEFENSIVO){
      juego->jugador1.energia -= ENERGIA_INMOVILES;
    }else{
      juego->jugador2.energia -= ENERGIA_INMOVILES;
    }
  }
}

/*
 * Le preguntara al usurio si quiere ingresar un personaje
 */
 void preguntar_usuario(char* respuesta){
  printf("Desea posicionar un personaje? S/N\n");
  scanf(" %c", &(*respuesta));
  if(*respuesta != RESPUESTA_POSITIVA && *respuesta != RESPUESTA_NEGATIVA){
    printf("Respuesta invalida\n");
    preguntar_usuario( respuesta);
  }
}

/*
 * Le pregunta al usuario que personaje quiere ingresar.
 */
void preguntar_personaje(char* soldado, char soldado_inmovil, char soldado_movil){
  printf("Que personaje desea agregar?\n");
  scanf(" %c", &(*soldado));
  if(*soldado != soldado_inmovil && *soldado != soldado_movil){
    printf("Respuesta invalida\n");
    preguntar_personaje( soldado, soldado_inmovil, soldado_movil);
  }
}


/*
 * En el caso que el usurio quiera ingresar un hombre le pedira los datos,
 * y enviara los datos al vector de rohan.
 */
void agregar_hombres(juego_t* juego, personaje_t* personaje){
  printf("Donde lo deseas posicionar?\n");
  printf("Columna: ");
  scanf("%i", &(personaje->columna));
  personaje->fila = FILA_HOMBRES;
  if(personaje->columna <= 9 && personaje->columna >= 0){
    juego->rohan[juego->cantidad_rohan] = personaje_movil_nuevo(HOMBRES, juego->plus_rohan, personaje->columna, personaje->fila);
    juego->cantidad_rohan++;
    sacar_energia_defensivo( juego, HOMBRES);
  }else{
    while(personaje->columna < 0 || personaje->columna > 9) {
      printf("No se puede posicionar en ese lugar. Por favor ingrese otro valor.\n");
      printf("Donde lo deseas posicionar?\n");
      printf("Columna: ");
      scanf("%i", &(personaje->columna));
      personaje->fila = FILA_HOMBRES;
    }
    personaje_t hombre = personaje_movil_nuevo(HOMBRES, juego->plus_rohan, personaje->columna, personaje->fila);
    posicionar_personaje( juego, hombre);
    sacar_energia_defensivo( juego, HOMBRES);
  }
}


/*
 * En el caso que el usurio quiera ingresar un elfo le pedira los datos,
 * y enviara los datos al vector de rohan.
 */
void agregar_elfos(juego_t* juego, personaje_t* personaje){
  printf("Donde lo deseas posicionar?\n");
  printf("Columna: ");
  scanf("%i", &(personaje->columna));
  printf("Fila: ");
  scanf("%i", &(personaje->fila));
  while(!esta_vacio(juego->terreno[personaje->fila][personaje->columna]) || ((personaje->fila) < 5) || ((personaje->fila) > 8) || (personaje->columna < 0 || personaje->columna > 9)){
    printf("No se puede posicionar en ese lugar. Por favor ingrese otro valor.\n");
    printf("Donde lo deseas posicionar?\n");
    printf("Columna: ");
    scanf("%i", &(personaje->columna));
    printf("Fila: ");
    scanf("%i", &(personaje->fila));
  }
  personaje_t elfo = personaje_inmovil_nuevo(ELFOS, juego->plus_rohan, personaje->columna, personaje->fila);
  posicionar_personaje( juego, elfo);
  sacar_energia_defensivo( juego, ELFOS);
}


/*
 * En el caso que el usurio quiera ingresar un orco le pedira los datos,
 * y enviara los datos al vector de isengard.
 */
void agregar_orcos(juego_t* juego, personaje_t* personaje){
  printf("Donde lo deseas posicionar?\n");
  printf("Columna: ");
  scanf("%i", &(personaje->columna));
  personaje->fila = 0;
    while(personaje->columna < 0 || personaje->columna > 9) {
      printf("No se puede posicionar en ese lugar. Por favor ingrese otro valor.\n");
      printf("Donde lo deseas posicionar?\n");
      printf("Columna: ");
      scanf("%i", &(personaje->columna));
    }
    personaje->fila = 0;
    personaje_t orcos = personaje_movil_nuevo(ORCOS, juego->plus_rohan, personaje->columna, personaje->fila);
    posicionar_personaje( juego, orcos);
    sacar_energia_ofensivo( juego, ORCOS);
}


/*
 * En el caso que el usurio quiera ingresar un urukhai le pedira los datos,
 * y enviara los datos al vector de isengard.
 */
void agregar_urukhai(juego_t* juego, personaje_t* personaje){
  printf("Donde lo deseas posicionar?\n");
  printf("Columna: ");
  scanf("%i", &(personaje->columna));
  printf("Fila: ");
  scanf("%i", &(personaje->fila));
  while(!esta_vacio(juego->terreno[personaje->fila][personaje->columna]) || ((personaje->fila) < 1) || ((personaje->fila) > 4) || (personaje->columna < 0 || personaje->columna > 9)){
    printf("No se puede posicionar en ese lugar. Por favor ingrese otro valor.\n");
    printf("Donde lo deseas posicionar?\n");
    printf("Columna: ");
    scanf("%i", &(personaje->columna));
    printf("Fila: ");
    scanf("%i", &(personaje->fila));
  }
  personaje_t urukhai = personaje_inmovil_nuevo(URUKHAI, juego->plus_rohan, personaje->columna, personaje->fila);
  posicionar_personaje( juego, urukhai);
  sacar_energia_ofensivo( juego, URUKHAI);
}


/*
 * Le preguntara al usurio si quiere ingresar un personaje y en el caso que quiera,
 * le preguntara que personaje quiere ingresar.
 */
void agregar_personajes(juego_t* juego, personaje_t* personaje, char bando){
  char respuesta;
  int energia = energia_defensivo(juego);
  if(bando == 'R'){
    preguntar_usuario(&respuesta);
    if(respuesta == RESPUESTA_POSITIVA){
      char soldado;
      preguntar_personaje( &soldado, ELFOS, HOMBRES);
      if(soldado == ELFOS){
        if((energia >= ENERGIA_INMOVILES)){
          agregar_elfos(juego, personaje);
        }else{
          if((energia < ENERGIA_INMOVILES)){
            printf("Energia insuficiente.\n");
            agregar_personajes(juego, personaje, bando);
          }
        }
      }else if(soldado == HOMBRES ){
        if((energia >= ENERGIA_MOVILES)){
          agregar_hombres(juego, personaje);
        }else{
          if((energia < ENERGIA_MOVILES)){
            printf("Energia insuficiente.\n");
            agregar_personajes(juego, personaje, bando);
          }
        }
      }
    }
  }else{
    int energia = energia_ofensivo(juego);
    preguntar_usuario(&respuesta);
    if(respuesta == RESPUESTA_POSITIVA){
      char soldado;
      preguntar_personaje( &soldado, URUKHAI, ORCOS);
      if(soldado == URUKHAI){
        if((energia >= ENERGIA_INMOVILES)){
          agregar_urukhai(juego, personaje);
        }else{
          if((energia < ENERGIA_INMOVILES)){
            printf("Energia insuficiente.\n");
            agregar_personajes(juego, personaje, bando);
          }
        }
      }else if(soldado == ORCOS){
        if((energia >= ENERGIA_MOVILES)){
          agregar_orcos(juego, personaje);
        }else{
          if((energia < ENERGIA_MOVILES)){
            printf("Energia insuficiente.\n");
            agregar_personajes(juego, personaje, bando);
          }
        }
      }
    }
  }
}


/*
 * En el caso de que haya un solo jugador y su bando sea ofensivo, posicionara hombres y
 * cargara los datos al vector de rohan
 */
void jugador_maquina_defensivo(juego_t* juego, personaje_t* personaje){
  int numero = rand () % 10;
  if(juego->jugador2.energia >= ENERGIA_MOVILES){
      personaje->fila = FILA_HOMBRES;
      personaje->columna = numero;
      personaje_t hombre = personaje_movil_nuevo(HOMBRES, juego->plus_rohan, personaje->columna, personaje->fila);
      posicionar_personaje( juego, hombre);
      sacar_energia_defensivo( juego, HOMBRES);
  }
}

/*
 * En el caso de que haya un solo jugador y su bando sea defensivo, posicionara orcos y
 * cargara los datos al vector de isengard
 */
void jugador_maquina_ofensivo(juego_t* juego, personaje_t* personaje){
  int numero = rand () % 10;
  if(juego->jugador2.energia >= ENERGIA_MOVILES){
      personaje->fila = 0;
      personaje->columna = numero;
      personaje_t orcos = personaje_movil_nuevo(ORCOS, juego->plus_rohan, personaje->columna, personaje->fila);
      posicionar_personaje( juego, orcos);
      sacar_energia_ofensivo( juego, ORCOS);
  }
}


//Le asigna el bando opuesto del jugador 1 al jugador 2.
char tipo_opuesto(char tipo){
  if (tipo == BANDO_OFENSIVO)
    return BANDO_DEFENSIVO;
  return BANDO_OFENSIVO;
}

//Aumenta en 1 la energia de jugador 1.
void energia_j1(juego_t* juego){
  if(juego->jugador1.energia < ENERGIA_MAX){
    juego->jugador1.energia++;
  }
}

//Aumenta en 1 la energia de jugador 2.
void energia_j2(juego_t* juego){
  if(juego->jugador2.energia < ENERGIA_MAX){
    juego->jugador2.energia++;
  }
}


//Recarga la energia de ambos jugadores
void recargar_energia(juego_t* juego){
  energia_j1( juego);
  energia_j2( juego);
}


/*
 * Revisara los vectores de cada bando y revisara si algun personaje no sigue vivo,
 * y en tal caso lo eliminara del vector y de la matriz
*/
void eliminar_personajes(personaje_t personajes[MAX_PERSONAJES], int *cantidad_personajes, int indice){
  personajes[indice] = personajes[*cantidad_personajes - 1];
  (*cantidad_personajes)--;
}


/*
 * Revisara la posicion del personje, y en el caso de que esta en la posicion
 * esperada le sumara 1 a la cantidad de llegadas de rohan.
 */
void puntos_rohan(juego_t* juego, int pos){
  if(juego->rohan[pos].fila == 0){
    juego->llegadas_rohan++;
    eliminar_personajes( juego->rohan, &(juego->cantidad_rohan), pos);
  }
}


/*
 * Revisara la posicion del personje, y en el caso de que esta en la posicion
 * esperada le sumara 1 a la cantidad de llegadas de isengard.
 */
void puntos_isengard(juego_t* juego, int pos){
  if(juego->isengard[pos].fila == FILA_HOMBRES){
    juego->llegadas_isengard++;
    eliminar_personajes( juego->isengard, &(juego->cantidad_isengard), pos);
  }
}


/*
*Se fijara la poscion de los personajes inmoviles, y revisara el vector del bando
* contrario y en el caso que este en rango atacara al personaje.
*/
void accion_inmoviles(juego_t* juego, char bando, int posicion_personaje){
  int distancia = 0;
  if(bando == BANDO_DEFENSIVO){
    for(int j = 0; j < juego->cantidad_isengard; j++){
      distancia = (abs(juego->rohan[posicion_personaje].fila - juego->isengard[j].fila) + abs(juego->rohan[posicion_personaje].columna - juego->isengard[j].columna));
      if(distancia <= 3){
        juego->isengard[j].vida -= (juego->rohan[posicion_personaje].ataque);
        if(juego->isengard[j].vida <= 0){
          eliminar_personajes( juego->isengard, &(juego->cantidad_isengard), j);
        }
      }
    }
  }else{
    for(int i = 0; i < juego->cantidad_rohan; i++){
      distancia = (abs(juego->rohan[i].fila - juego->isengard[posicion_personaje].fila) + abs(juego->rohan[i].columna - juego->isengard[posicion_personaje].columna));
      if(distancia <= 3){
        juego->rohan[i].vida -= (juego->isengard[posicion_personaje].ataque);
        if(juego->rohan[i].vida <= 0){
          eliminar_personajes( juego->rohan, &(juego->cantidad_rohan), i);
        }
      }
    }
  }
}


/*
 * Recibe las posciones de ambos vectores y ve si hay algun personaje
 * del bando contrario en el rango
 */
 bool esta_en_rango(juego_t* juego, char tipo, int fila, int columna, int pos){
   bool esta = false;
   if(tipo == BANDO_DEFENSIVO){
    for(int i = (fila - 1); i < (fila + 2); i++){
      for(int j = (columna - 1); j < (columna + 2); j++){
         if(juego->terreno[i][j] == juego->isengard[pos].codigo){
           esta = true;
         }
       }
     }
   }else{
     for(int i = (fila - 1); i < (fila + 2); i++){
       for(int j = (columna - 1); j < (columna + 2); j++){
         if(juego->terreno[i][j] == juego->rohan[pos].codigo){
           esta = true;
         }
       }
     }
   }
   return esta;
}


/*
* Se fijara la poscion de los personajes moviles, y revisara el vector del bando
* contrario y en el caso que este en rango atacara al personaje.
*/
void accion_moviles(juego_t* juego, char bando, int posicion_personaje){
  bool ataco = false;
  if(bando == BANDO_DEFENSIVO){
    int fila = juego->rohan[posicion_personaje].fila;
    int columna = juego->rohan[posicion_personaje].columna;
    int j = 0;
    while(j < juego->cantidad_isengard && ataco == false){
      if(esta_en_rango( juego, bando, fila, columna, j) == true){
        juego->isengard[j].vida -= juego->rohan[posicion_personaje].ataque;
        ataco = true;
        if(juego->isengard[j].vida <= 0){
          eliminar_personajes( juego->isengard, &(juego->cantidad_isengard), j);
        }
      }else{
        j++;
      }
    }
    if(ataco == false){
      juego->rohan[posicion_personaje].fila--;
      puntos_rohan( juego, posicion_personaje);
    }
  }else{
    int fila = juego->isengard[posicion_personaje].fila;
    int columna = juego->isengard[posicion_personaje].columna;
    int i = 0;
    while(i < juego->cantidad_rohan  && ataco == false){
      if(esta_en_rango( juego, bando, fila, columna, i) == true){
        juego->rohan[i].vida -= juego->isengard[posicion_personaje].ataque;
        ataco = true;
        if(juego->rohan[i].vida <= 0){
          eliminar_personajes( juego->rohan, &(juego->cantidad_rohan), i);
        }
      }else{
        i++;
      }
    }
    if(ataco == false){
      juego->isengard[posicion_personaje].fila++;
      puntos_isengard( juego, posicion_personaje);
    }
  }
}


/*
 * Cuando el juego termine mostrara un mensaje en pantalla felicitando
 * al jugador.
 */
void mensaje_ganador(juego_t* juego){
  if(juego->llegadas_rohan == CANTIDAD_DE_LLEGADAS){
    printf("Rohan ha ganado. Felicidades!\n");
  }else{
    printf("Isengard ha ganado. Felicidades!\n");
  }
}


/*
 * Revisara la cantidad de llegadas para saber cuando gano un jugador
 * y terminar el juego.
 */
void gano(juego_t* juego, bool* ganador){
  if((juego->llegadas_rohan == CANTIDAD_DE_LLEGADAS )|| (juego->llegadas_isengard == CANTIDAD_DE_LLEGADAS)){
    *ganador = true;
    mensaje_ganador(juego);
  }
}


void jugar(juego_t* juego, char bando, int posicion_personaje){
  if(bando == BANDO_DEFENSIVO){
    if(juego->rohan[posicion_personaje].codigo == ELFOS){
      accion_inmoviles( juego, bando, posicion_personaje);
    }else{
      accion_moviles( juego, bando, posicion_personaje);
    }
  }else{
    if(juego->isengard[posicion_personaje].codigo == URUKHAI){
      accion_inmoviles( juego, bando, posicion_personaje);
    }else{
      accion_moviles( juego, bando, posicion_personaje);
    }
  }
}

/*
 * Recorre los vectores de cada jugador, para ver la posicion del personaje
 * en el vector y sus valores para ejecutar su proxima accion.
 */
void recorrer_atacantes(juego_t* juego){
  for(int i = 0; i < juego->cantidad_rohan; i++){
    jugar( juego, BANDO_DEFENSIVO, i);
  }
  for(int j = 0; j < juego->cantidad_isengard; j++){
    jugar( juego, BANDO_OFENSIVO, j);
  }
}

/*
 * En el caso que haya un solo jugador y su bando sea defensivo,
 * jugara contra una maquina del bando ofensivo, y ejecutara
 * las funciones necesarias.
 */
void juego_individual_rohan(juego_t* juego, personaje_t* personaje){
  bool ganador = false;
  while(ganador == false){
    agregar_personajes(juego, personaje, BANDO_DEFENSIVO);
    jugador_maquina_ofensivo(juego, personaje);
    recorrer_atacantes( juego);
    recargar_energia( juego);
    imprimir_terreno(juego);
    printf("Rohan: %i\n", juego->llegadas_rohan);
    printf("Isengard: %i\n", juego->llegadas_isengard);
    gano( juego, &ganador);
  }
}


/*
 * En el caso que haya un solo jugador y su bando sea ofensivo,
 * jugara contra una maquina del bando defensivo, y ejecutara
 * las funciones necesarias.
 */
void juego_individual_isengard(juego_t* juego, personaje_t* personaje){
  bool ganador = false;
  while(ganador == false){
    jugador_maquina_defensivo(juego, personaje);
    agregar_personajes(juego, personaje, BANDO_OFENSIVO);
    recorrer_atacantes( juego);
    recargar_energia( juego);
    imprimir_terreno(juego);
    printf("Rohan: %i\n", juego->llegadas_rohan);
    printf("Isengard: %i\n", juego->llegadas_isengard);
    gano( juego, &ganador);
  }
}

/*
 * En el caso que haya dos jugadores se les preguntara los personajes,
 * se posicionran en sus vectores corresponsientes y en la matriz, y
 * ejecutara sus acciones.
 */
void multijugador(juego_t* juego, personaje_t* personaje){
  bool ganador = false;
  while (ganador == false){
    printf("Jugador Rohan:\n");
    agregar_personajes(juego, personaje, BANDO_DEFENSIVO);
    printf("Jugador Isengard:\n");
    agregar_personajes(juego, personaje, BANDO_OFENSIVO);
    recorrer_atacantes( juego);
    recargar_energia( juego);
    imprimir_terreno(juego);
    printf("Rohan: %i\n", juego->llegadas_rohan);
    printf("Isengard: %i\n", juego->llegadas_isengard);
    gano( juego, &ganador);
  }
}


void inicializar_juego(juego_t* juego){
    int intensidad;
    int cantidad;
    char tipo;
    srand (( unsigned ) time ( NULL ));
    limpiar_juego( juego);
    limpiar_terreno(juego);
    perfil(&tipo, &intensidad);
    (juego->jugador1.tipo) = tipo;
    (juego->jugador2.tipo) = tipo_opuesto(tipo);
    averiguar_cant_jugadores( &cantidad);
    juego->cantidad_jugadores = cantidad;
    definir_pluses( juego, intensidad);
    elfos_iniciales( juego);
    urukhai_iniciales( juego);
    imprimir_terreno( juego);
}


void ejecutar_juego(juego_t* juego, personaje_t* personaje){
  if(juego->cantidad_jugadores == 1){
    if(juego->jugador1.tipo == BANDO_DEFENSIVO){
        juego_individual_rohan( juego, personaje);
    }else{
      juego_individual_isengard( juego, personaje);
    }
  }else{
    multijugador(juego, personaje);
  }
}








//gcc  batalla.c  -o batalla -std=c99 -Wall -Wconversion -Werror -lm
