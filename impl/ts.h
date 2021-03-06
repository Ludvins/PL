#ifndef __TS_H_
#define __TS_H_

// ----------------------------------------------------------------- //
// ---------------- Declaración de variables globales -------------- //
// ----------------------------------------------------------------- //

#define MAX_TS 500
#define MAX_ARGS 50

extern unsigned int tope;              // Tope de la pila
unsigned int sub_prog;              // Indicador de comienzo de bloque de un subprog
int ultima_funcion;   // Posición en la tabla de símbolos del último procedimiento
unsigned int bloques_anidados;  // Numero de bloques anidados
extern int yylineno;

// ----------------------------------------------------------------- //
// ------------------- Declaración de estructuras ------------------ //
// ----------------------------------------------------------------- //

/*
 * TipoEntrada
 *
 * Tipos de entradas presentes en la tabla de símbolos.
 */
typedef enum {
  marca,            // Indica que la entrada es una marca de principio de bloque
  funcion,          // La entrada describe una funcion
  variable,         // La entrada describe una variable local
  parametro_formal, // La entrada describe un parámetro formal de una función
  instr_control
} TipoEntrada;

/*
 * TipoDato
 *
 * Sólo aplicable cuando sea función, variable o parametro_formal.
 * En el caso de función corresponderá al tipo de dato que devuelve.
 */
typedef enum {
  entero,
  real,
  booleano,
  caracter,
  cadena,
  desconocido
} TipoDato;

/*
 * DescriptorDeInstrControl
 */
typedef struct {
  char* EtiquetaSalida;
  char* EtiquetaElse;
  #define EtiquetaEntrada EtiquetaElse
} DescriptorDeInstrControl;

/*
 * EntradaTS
 *
 * Descripción de una entrada en la tabla de símbolos.
 */
typedef struct {
  TipoEntrada               tipo_entrada;
  char*                     nombre;
  TipoDato                  tipo_dato;
  unsigned int              parametros;
  DescriptorDeInstrControl  etiquetas_control;
  unsigned int              t_dim1; //Tamaño de la primera dimension (Array)
  unsigned int              t_dim2; //Tamaño de la segunda dimension (Array)
} EntradaTS;

/*
 * Atributos
 */
typedef struct {
  char*             lexema;                   // Nombre del lexema
  TipoDato          tipo;                     // Tipo del símbolo
  unsigned          n_dims;                   // Número de dimensiones.
  unsigned          dim1;                     // Tamaño de la primera dimensión.
  unsigned          dim2;                     // Tamaño de la segunda dimensión.
  int               tope_listas;              // Tope de la lista de identificadores
  char*             lista_ids[MAX_ARGS];      // Lista de identificadores
  unsigned          lista_ndims[MAX_ARGS];    // Lista de dimensiones
  unsigned          lista_dims1[MAX_ARGS];    // Lista de tamaño de la primera dimensión
  unsigned          lista_dims2[MAX_ARGS];    // Lista de tamaño de la segunda dimensión
  TipoDato          lista_tipos[MAX_ARGS];    // Lista de tipos
} Atributos;

#define YYSTYPE Atributos            // Cada símbolo tiene una estructura de tipo atributos.
EntradaTS     TS[MAX_TS];      // Pila de la tabla de símbolos

// ----------------------------------------------------------------- //
// --- Lista de funciones y procedimientos para manejo de la TS  --- //
// ----------------------------------------------------------------- //

// -------------------------------------------------- //
// --------- Entrada y salida de bloques  ----------- //
// -------------------------------------------------- //

/*
 * Inserta los parámetros de la ultima funcion como variables en la TS.
 *
 * Estos parámetros se encuentran en la tabla de símbolos entre TS[ultima_funcion + 1]
 *  y TS[ultima_funcion + TS[ultima_funcion].parametros]
 *
 * Utilizamos la función ´insertaVarTipo´ para insertar los parametros como variables.
 */
void insertaParametrosComoVariables();

/*
 * Añade a la tabla de símbolos marca de comienzo.
 *
 * Aumenta el contador de bloques anidados.
 * En caso de ser un subprograma, llama a ´insertaParametrosComoVariables´
 */
void entraBloqueTS();

/*
 * Sal de bloque y elimina de la tabla de símbolos todos los símbolos hasta la última marca.
 *
 * Disminuye el contador de bloques anidados.
 */
void salBloqueTS();

/*
 * Sale del bloque y elimina en la tabla de símbolos todos los símbolos
 * hasta el último descriptor de una instrucción de control.
 */
void salEstructuraControl();

/*
 * Encuentra el nombre de la etiqueta de salida de la estructura de control actual
 */
char* encuentraGotoSalida();

/*
 * Encuentra el nombre de la etiqueta de ese de la estructura de control actual
 */
char* encuentraGotoElse();

#define encuentraGotoEntrada encuentraGotoElse

// -------------------------------------------------- //
// -------- Insercion en tabla de simbolos  --------- //
// -------------------------------------------------- //

/*
 * Inserta la entrada en la Tabla de Símbolos.
 * Incrementa el valor de ´tope´
 */
void insertaTS (EntradaTS entrada);

/*
 * Introduce una variable en la tabla de símbolos, dado su tipo como TipoDato
 * Comprueba si ya existe el identificador en la tabla llamando a ´esDuplicado´
 */
void insertaVarTipo(char* identificador, TipoDato tipo_dato, unsigned dimension1, unsigned dimension2);

/*
 * Inserta una variable en la tabla de símbolos, dado su tipo como string.
 *
 * Llama a ´leeTipoDato´ y luego a ´insertaVarTipo´
 */
void insertaVar(char* identificador, char* nombre_tipo, unsigned dimension1, unsigned dimension2);

/*
 * Inserta la función en la tabla de símbolos, requiere de el tipo y las dimensiones del valor que devuelve.
 *
 * Comprueba si el identificador ya existe en la tabla de símbolos llamando a ´esDuplicado´.
 * Guarda en ultima_funcion la posición de esta en la tabla de símbolos.
 * Indica que es necesario insertar los parámetros como variables (sub_prog = 1)
 */
void insertaFuncion(char* identificador, TipoDato tipo_ret, unsigned dim1_ret, unsigned dim2_ret);

/*
 * Inserta el parámetro formal en la tabla de símbolos.
 *
 * Si no existe una función devuelve un error.
 * Aumenta el número de parámetros de dicha función en la tabla.
 */
void insertaParametro(char* identificador, char* nombre_tipo, unsigned dim1, unsigned dim2);

/*
 * Inserta el descriptor de una instrucción if/else
 */
void insertaIf(char* etiqueta_salida, char* etiqueta_else);

/*
 * Inserta el descriptor de una instrucción de control while.
 */
void insertaWhile(char* etiqueta_entrada, char* etiqueta_salida);

/*
 * Inserta el descriptor de una instrucción de control de switch.
 */
void insertaSwitch(char* etiqueta_entrada, char* etiqueta_salida);

// -------------------------------------------------- //
// -------- Extracción de informacion de TS --------- //
// -------------------------------------------------- //

/*
 * Halla el índice de identificador de variable o función en TS.
 *
 * Devuelve la primera ocurrencia de abajo a arriba.
 *
 * En caso de no existir, devuelve -1.
 */
int encuentraTS(char* identificador);

/*
 * Encuentra el identificador de variable o función en la tabla de
 *  símbolos y devuelve su tipo.
 *
 * En caso de no existir, devuelve "no_asignado"
 */
TipoDato encuentraTipo(char* identificador);

/*
 * Comprueba si un identificador está duplicado en su ámbito.
 * 0 si no es duplicado, 1 si sí lo es.
 */
int esDuplicado(char* identificador);

/*
 * Devuelve el número de dimensiones de la entrada de la tabla de símbolos
 *  con identificador ´identificador´.
 */
unsigned nDimensiones(char* identificador);

// ---------------------------------------------------------------- //
// ---- Fin de funciones y procedimientos para manejo de la TS ---- //
// ---------------------------------------------------------------- //

// -------------------------------------------------- //
// ---------- Transformaciones de TipoDato ---------- //
// -------------------------------------------------- //

/*
 * Transforma el tipo de char* a TipoDato
 */
TipoDato strToTipodato(char* nombre_tipo);

/*
 * Transforma el tipo de tipoDato a string
 */
char* tipodatoToStr(TipoDato tipo);

/*
 * Transforma el tipo de tipoDato a string (nombre en C)
 */
char* tipodatoToStrC(TipoDato tipo);

// -------------------------------------------------- //
// --------------- Impresión DEBUG ------------------ //
// -------------------------------------------------- //

/*
 * Imprime el tipo de la entrada.
 */
char* imprimeTipoE(TipoEntrada tipo);

/*
 * Imprime el tipo de dato
 */
char* imprimeTipoD(TipoDato tipo);

/*
 * Imprime la tabla de símbolos
 */
void imprimeTS();

// -------------------------------------------------- //
// ------------ Funciones auxiliares ---------------- //
// -------------------------------------------------- //

/*
 * Comprueba si el tipoDato es un número.
 *
 * Devuelve 1 en caso afirmativo y 0 en caso negativo.
 *
 */
int esNumero(TipoDato tipo);

/*
 * Devuelve el tipo de dato de la constante.
 *
 * Comprueba el primer caracter de la constante:
 *  Si es ´v´ o ´f´, devuelve ´booleano´.
 *  Si es ´\´, devuelve caracter.
 *  En otro caso devuelve ´real´.
 */
TipoDato getTipoConstante(char* constante);

unsigned max (unsigned a, unsigned b);
unsigned min (unsigned a, unsigned b);

int esMain();

#endif // __TS_H_
