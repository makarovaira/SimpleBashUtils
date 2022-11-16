#include "s21_cat.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

void file_print(FILE* file, int argc, char* argv[]);
int processing(int argc, char* argv[], t_allFlags* choice);
void fun_n(int argc, char* argv[]);
void fun_b(int argc, char* argv[]);
void fun_s(int argc, char* argv[]);
void fun_e(int argc, char* argv[]);
void fun_t(int argc, char* argv[]);
void fun_v(int argc, char* argv[]);
void initFlags(t_allFlags* choice);
void fun_cat(char* argv[], t_allFlags* choice);

int main(int argc, char* argv[]) {
  t_allFlags choice;
  int i = 1;
  initFlags(&choice);
  processing(argc, argv, &choice);
  while (i != argc) {
    if (argv[i][0] != '-') {
      fun_cat(&argv[i], &choice);
    }
    i += 1;
  }
}

void initFlags(t_allFlags* choice) {
  choice->flag_n = 0;
  choice->flag_b = 0;
  choice->flag_s = 0;
  choice->flag_e = 0;
  choice->flag_t = 0;
  choice->flag_v = 0;
}

int processing(int argc, char* argv[], t_allFlags* choice) {
  int tf = 0;
  size_t select;
  for (select = 1; (int)select < argc && argv[select][0] == '-'; select++) {
    switch (argv[select][1]) {
      case 'b':
        choice->flag_b = 1;
        tf = 1;
        break;
      case 'e':
        choice->flag_e = 1;
        tf = 1;
        choice->flag_v = 1;
        break;
      case 'n':
        choice->flag_n = 1;
        tf = 1;
        break;
      case 's':
        choice->flag_s = 1;
        tf = 1;
        break;
      case 't':
        choice->flag_t = 1;
        tf = 1;
        choice->flag_v = 1;
        break;
      case 'v':
        choice->flag_v = 1;
        tf = 1;
        break;
      case 'E':
        choice->flag_e = 1;
        tf = 1;
        break;
      case 'T':
        choice->flag_t = 1;
        tf = 1;
        break;

      default:
        fprintf(stderr, "%s usage: cat [-benstuv] [file ...]\n", argv[0]);
    }
  }
  argv += select;
  return tf;
}

void fun_cat(char* argv[], t_allFlags* choice) {
  char c;
  char beforec = '\n';
  int count = 1;
  int tf = 0;
  int flag_error = 0;
  FILE* file;
  file = fopen(*argv, "r");
  if (file == NULL || (c = fgetc(file)) == EOF) {
    fprintf(stderr, "cat: %s: No such file or directory", *argv);
    flag_error = 1;
  } else {
    ungetc(c, file);
  }
  if (flag_error == 0) {
    for (beforec = '\n'; (c = fgetc(file)) != EOF; beforec = c) {
      if (beforec == '\n') {
        if (choice->flag_s == 1) {
          if (c == '\n') {
            if (tf) {
              continue;
            }
            tf = 1;
          } else {
            tf = 0;
          }
        }
        if (choice->flag_n == 1 && (choice->flag_b == 0 || c != '\n')) {
          printf("%6d\t", count);
          count += 1;
        } else if (choice->flag_b == 1 && c != '\n') {
          printf("%6d\t", count);
          count += 1;
        }
      }
      if ((c == '\n') && choice->flag_e == 1) {
        printf("$");
      } else if (c == '\t' && choice->flag_t == 1) {
        if (putchar('^') == EOF || putchar('I') == EOF) break;
        continue;
      } else if (choice->flag_v == 1) {
        if ((!isascii(c)) && c != '\n' && c != '\t') {
          if (putchar('M') == EOF || putchar('-') == EOF) break;
          c = toascii(c);
        }
        if (iscntrl(c) && c != '\n' && c != '\t') {
          if (putchar('^') == EOF ||
              putchar(c == '\177' ? '?' : c | 0100) == EOF)
            break;
          continue;
        }
      }
      printf("%c", c);
    }
  }
  fclose(file);
}
