#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  /* el tipo de contador a utilizar para los datos de entrada. */
  counter_type_t counter_type;

  /* Path del archivo con los datos de entrada */
  const char *path;

  /* Boolean ndica si se usa stdin */
  bool is_stdin;
};

/** Estructuras de datos */

/** Estructura que uitliza getopt_log para parsear los argumentos de linea de
 * comandos. */
static const struct option _long_opts[] = {
    {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
    {.name = "version", .has_arg = no_argument, .flag = NULL, .val = 'V'},
    {.name = "bytes", .has_arg = no_argument, .flag = NULL, .val = 'b'},
    {.name = "chars", .has_arg = no_argument, .flag = NULL, .val = 'c'},
    {.name = "words", .has_arg = no_argument, .flag = NULL, .val = 'w'},
    {.name = "lines", .has_arg = no_argument, .flag = NULL, .val = 'l'},
    {.name = "input", .has_arg = required_argument, .flag = NULL, .val = 'i'},
    {0},
};

/** Funciones */

/**
 * @brief Imprime un mensaje de ayuda y termina el programa.
 *
 * @param bin_name argv[0].
 */
static void _print_help(const char *bin_name) {
  printf("Uso: %s [OPCIONES]\n", bin_name);
  printf("Las opciones aceptadas son:\n");
  printf("  -h, --help        Imprime este mensaje y termina.\n");
  printf("  -V, --version     Imprime la versión y termina.\n");
  printf("  -b, --bytes       Cuenta los bytes en el archivo de entrada.\n");
  printf("  -c, --chars       Cuenta los caracteres en el archivo de entrada.\n");
  printf("  -w, --words       Cuenta las palabras en el archivo de entrada.\n");
  printf("  -l, --lines       Cuenta las líneas en el archivo de entrada.\n");
  printf(
      "  -i, --input [ARCHIVO] Indica que se usara la ruta que siga al -i, si "
      "no existe se usara stdin.\n");
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
  counter_type_t type = counter_type_invalid;
  args->is_stdin = true;
  int ch = -1;

  while ((ch = getopt_long(argc, (char **)argv, "hVbcwli:", _long_opts, NULL)) != -1) {
    switch (ch) {
      case 'h':
        _print_help(argv[0]);
        exit(0);
        break;

      case 'V':
        _print_version(argv[0]);
        exit(0);
        break;

      case 'b':
        type = counter_type_byte;
        break;

      case 'c':
        type = counter_type_char;
        break;

      case 'w':
        type = counter_type_word;
        break;

      case 'l':
        type = counter_type_line;
        break;

      case 'i':
        args->path = argv[optind - 1];
        args->is_stdin = (strcmp("-", args->path) == 0);
        break;

      /* this is returned when a required argument was not provided */
      case ':':
      case '?':
        exit(1);
    }
  }

  if (type == counter_type_invalid) {
    printf("No se especificó tipo de contador.\n");
    exit(1);
  }

  /* llena la estructura de salida */
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
  unsigned int counter = 0;
  char buffer[2048];
  size_t buffer_len = 0;
  char new_byte, prev_byte = 0;

  while (buffer_len = fread(buffer, 1, sizeof(buffer), input), buffer_len > 0) {
    for (size_t i = 0; i < buffer_len; i++) {
      new_byte = buffer[i];
      switch (counter_type) {
        case counter_type_byte:
          counter++;
          break;

        case counter_type_char:
          counter += (new_byte & 0xc0) != 0x80;
          break;

        case counter_type_word:
          if (prev_byte == 0 || (isspace(prev_byte) && !isspace(new_byte))) {
            counter++;
          }
          break;

        case counter_type_line:
          if (new_byte == '\n') {
            counter++;
          }
          break;

        case counter_type_invalid:
          return 0;
      }

      prev_byte = new_byte;
    }
  }

  return counter;
}

int main(int argc, const char *argv[]) {
  struct args args = {0};

  /* parsea la línea de comandos */
  _arg_parse(&args, argc, argv);

  /* Si es STDin, pone el archivo como stdin. Si no abrimos con la ruta */
  FILE *file;

  if (args.is_stdin) {
    file = stdin;
  } else {
    file = fopen(args.path, "r");
    if (file == 0) {
      perror("Error");
      exit(0);
    }
  }

  /* procesa la entrada */
  uint64_t count = _process_input(file, args.counter_type);
  printf("%" PRIu64 "\n", count);

  fclose(file);
  return EXIT_SUCCESS;
}
