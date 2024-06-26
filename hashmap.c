#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

/*
struct Pair {
  char * key;
  void * value;
};
*/


Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}



long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}



int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}



void insertMap(HashMap * map, char * key, void * value) {
  //1) obtener posicion mediante funcion hash
  long pos = hash(key, map->capacity);

  //2) comprobar que posicion es valida
  while (map->buckets[pos] != NULL){
    
    //3) Como posicion no valida resolvemos colision
    pos = (pos + 1) % map->capacity;
  }
  
  //4) Agregar nuevo elemento
  Pair *nuevoDato = createPair(key, value);
  map->buckets[pos] = nuevoDato;
  
  //5) Modificar datos mapa
  map->size += 1;
  map->current = pos;
}




void enlarge(HashMap * map) {
  enlarge_called = 1; //no borrar (testing purposes)

  //1) Crea una referencia auxiliar para el antiguo arreglo
  long antiguaCapacidad = map->capacity;
  Pair **antiguoArreglo = map->buckets;

  //2) Aumenta la capacidad
  long nuevaCapacidad = 2 * antiguaCapacidad;
  map->capacity = nuevaCapacidad;

  //3) Reserva memoria y asigna el arreglo vacio
  map->buckets = (Pair**)calloc(nuevaCapacidad, sizeof(Pair*));
  map->size = 0;

  //4) Recorrer el antiguo arreglo para reinsertar elementos
  for (int i = 0; i < antiguaCapacidad; i++){
    //5) Comprobar que existen datos en esa posicion
    if (antiguoArreglo[i] != NULL)
    {
      //6) Insertar en el nuevo espacio
      insertMap(map, antiguoArreglo[i]->key, antiguoArreglo[i]->value);
      free(antiguoArreglo[i]);
    }
  }
  //7) Liberar memoria de arreglo antiguo
  free(antiguoArreglo);
}



HashMap * createMap(long capacity) {
  //1) Reservar memoria para el hashmap
  HashMap *nuevoHashMap = (HashMap *) malloc(sizeof(HashMap));
  if (nuevoHashMap == NULL) exit(EXIT_FAILURE);
  
  //2) Reservar memoria para cada bucket
  nuevoHashMap->buckets = (Pair**) malloc(capacity * sizeof(Pair*));
  
  //3) Recorrer cada bucket para NULL
  for (long i = 0 ; i < capacity ; i++)
    nuevoHashMap->buckets[i] = NULL;

  //4) Inicializar datos hashmap
  nuevoHashMap->size = 0;
  nuevoHashMap->capacity = capacity;
  nuevoHashMap->current = -1;

  return nuevoHashMap;
}




void eraseMap(HashMap * map,  char * key) {
  //1) Funcion para encontra posicion
  Pair *PairEliminar = searchMap(map, key);
  
  //2) comprobar que el elemento se encontro
  if (PairEliminar == NULL)
    return;
  
  //3) Volver nula esa posicion
  PairEliminar->key = NULL;
  
  //4) Disminuir tamaño
  (map->size)--; 
}




Pair * searchMap(HashMap * map,  char * key) {
  //1) Funcion hash para encontrar posicion
  long pos = hash(key, map->capacity);

  while (1){
    //2) Actualizar current
    map->current = pos;
    
    //3) Si la posicion es nula el dato no existe
    if (map->buckets[pos] == NULL)
      return NULL;
    //4) Si el dato esta en la posicion lo retorno
    else if (is_equal(map->buckets[pos]->key, key))
      return map->buckets[pos];
    //5) Si el datro no esta en la posicion (Colisdion) lo maneja
    else
      pos = (pos + 1) % map->capacity;
  }
}



Pair * firstMap(HashMap * map) {
  //1) Recorro todas las posiciones desde la primera
  for (long i = 0 ; i < map->capacity ; i++)
  {
    //2) compruebo que sea posicion valida
    if (map->buckets[i] != NULL && map->buckets[i]->key != NULL)
    {
      //Si es valida actualizo el current y reporno pair
      map->current = i;
      return map->buckets[i];
    }
  }
  //4) Si no encontro ningun elemento valido retorna null
  return NULL;
}



Pair * nextMap(HashMap * map) {
  //1) Recorro todas las posiciones desde el siguiente al current
  for (long i = map->current + 1 ; i < map->capacity ; i++)
  {
    //2) compruebo que sea posicion valida
    if (map->buckets[i] != NULL && map->buckets[i]->key != NULL)
    {
      //Si es valida actualizo el current y reporno pair
      map->current = i;
      return map->buckets[i];
    }
  }
  //4) Si no encontro ningun elemento valido retorna null
  return NULL;
}
