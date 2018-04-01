#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/** Tipos de datos */

/** Tipo de contador para los datos de entrada */
typedef enum {
  /** Cuenta los bytes */
  counter_type_byte,

  /** Cuenta los caracteres (teniendo en cuenta los multi-bytes) */
  counter_type_char,

  /** Cuenta las palabras (delimitadas por 'isspace') */
  counter_type_word,

  /** Cuenta las líneas (delimitadas po '\n') */
  counter_type_line,

  /** Valor cuando no se especificó nungún contador. */
  counter_type_invalid,

} counter_type_t;

/** Parámetros parseados de la línea de comandos. */
struct args {
  /* indica si el programa se debe ejecutar en modo verbose. */
  bool verbose;

  /* el tipo de contador a utilizar para los datos de entrada. */
  counter_type_t counter_type;

  /* handler del cual leer los datos de entrada */
  FILE *input;
};

/** Estructuras de datos */

/** Estructura que uitliza getopt_log para parsear los argumentos de linea de
 * comandos. */
static const struct option _long_opts[] = {
    {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
    {.name = "version", .has_arg = no_argument, .flag = NULL, .val = 'V'},
    {.name = "verbose", .has_arg = no_argument, .flag = NULL, .val = 'v'},
    {.name = "bytes", .has_arg = no_argument, .flag = NULL, .val = 'c'},
    {.name = "chars", .has_arg = no_argument, .flag = NULL, .val = 'm'},
    {.name = "words", .has_arg = no_argument, .flag = NULL, .val = 'w'},
    {.name = "lines", .has_arg = no_argument, .flag = NULL, .val = 'l'},
    {0},
};

/** Funciones */

/**
 * @brief Imprime un mensaje de ayuda y termina el programa.
 *
 * @param bin_name argv[0].
 */
static void _print_help(const char *bin_name) {
  printf("Uso: %s [OPCIONES] [ARCHIVO]\n", bin_name);
  printf("Las opciones aceptadas son:\n");
  printf("  -h, --help        Imprime este mensaje y termina.\n");
  printf("  -V, --version     Imprime la versión y termina.\n");
  printf("  -v, --verbose     Salida más detallada.\n");
  printf("  -c, --bytes       Cuenta los bytes en el archivo de entrada.\n");
  printf(
      "  -m, --chars       Cuenta los caracteres en el archivo de entrada.\n");
  printf("  -w, --words       Cuenta las palabras en el archivo de entrada.\n");
  printf("  -l, --lines       Cuenta las líneas en el archivo de entrada.\n");
  printf("\n");
  printf(
      "ARCHIVO es el nombre del archivo a leer, o '-' para leer desde "
      "STDIN.\n");
}

/**
 * @brief Imprime la versión del programa y termina.
 *
 * @param bin_name argv[0].
 */
static void _print_version(const char *bin_name) {
  printf("%s, versión 1.00\n", bin_name);
}

/**
 * @brief Realiza el parseo de los parámetros de línea de comandos.
 *
 * @param args Estructura que contiene los parámetros parseados.
 * @param argc
 * @param argv
 */
static void _arg_parse(struct args *args, int argc, const char **argv) {
  bool verbose = false;
  counter_type_t type = counter_type_invalid;

  int ch = -1;
  while ((ch = getopt_long(argc, (char **)argv, "hVvcmwlf:", _long_opts,
                           NULL)) != -1) {
    switch (ch) {
      case 'h':
        _print_help(argv[0]);
        exit(0);
        break;

      case 'V':
        _print_version(argv[0]);
        exit(0);
        break;

      case 'v':
        verbose = true;
        break;

      case 'c':
        type = counter_type_byte;
        break;

      case 'm':
        type = counter_type_char;
        break;

      case 'w':
        type = counter_type_word;
        break;

      case 'l':
        type = counter_type_line;
        break;
    }
  }

  if (type == counter_type_invalid) {
    printf("No se especificó tipo de contador.\n");
    exit(1);
  }

  /* llena la estructura de salida */
  args->verbose = verbose;
  args->input = stdin; // TODO: obtener desde los datos de entrada
  args->counter_type = type;
}

/**
 * @brief Lee del `input` hasta que no haya mas datos y aplica el contador
 * especificado.
 *
 * @param input Archivo de donde leer los datos.
 * @param counter_type Tipo de contador a utilizar.
 * @return Resultado del contador.
 */
static uint64_t _process_input(FILE *input, counter_type_t counter_type) {
  return 0;
}

int main(int argc, const char *argv[]) {
  struct args args = {0};

  /* parsea la línea de comandos */
  _arg_parse(&args, argc, argv);

  /* procesa la entrada */
  // TODO: manejar posibles casos de error en la lectura del archivo
  uint64_t count = _process_input(args.input, args.counter_type);
  printf("%" PRIu64 "\n", count);

  return EXIT_SUCCESS;
}
