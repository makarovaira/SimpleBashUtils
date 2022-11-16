#include "s21_grep.h"

#include <ctype.h>
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initFlags(t_allFlags *choice);
void processing(int argc, char *argv[], char *pattern, t_allFlags *choice);
void fun_grep(char *pattern, int countFile, char *fileName, t_allFlags *choice);
void fun_f(char *fileName, char *pattern, t_allFlags *choice);
void fun_e(int *wordCount, char *optarg, char *pattern);

int main(int argc, char *argv[]) {
  t_allFlags choice;
  initFlags(&choice);
  int countFile;
  char pattern[10000] = {0};
  if (argc > 2) {
    processing(argc, argv, pattern, &choice);
    if (!choice.flag_f && !choice.flag_e) {
      strcat(pattern, argv[optind]);
      optind++;
    }
    countFile = argc - optind;
    while (optind < argc) {
      fun_grep(pattern, countFile, argv[optind], &choice);
      optind++;
    }
  }
  return 0;
}

void fun_f(char *fileName, char *pattern, t_allFlags *choice) {
  FILE *file;
  file = fopen(fileName, "r");
  int countWord = 0;
  char arr_str[10000] = {0};
  if (file != NULL) {
    while (fgets(arr_str, sizeof(arr_str), file) != NULL) {
      strtok(arr_str, "\n");
      if (*arr_str != '\0') {
        countWord += 1;
      }
      if (countWord > 1) {
        strcat(pattern, "|");
      }
      strcat(pattern, arr_str);
    }
    fclose(file);

  } else if (!choice->flag_s) {
    fprintf(stderr, "grep: %s: No such file or directory\n", fileName);
  }
}

void fun_e(int *wordCount, char *optarg, char *pattern) {
  if (*wordCount > 0) {
    strcat(pattern, "|");
  }
  *wordCount += 1;
  if (*optarg == '\0') {
    strcat(pattern, ".");
  } else {
    strcat(pattern, optarg);
  }
}

void initFlags(t_allFlags *choice) {
  choice->flag_e = 0;
  choice->flag_i = 0;
  choice->flag_v = 0;
  choice->flag_c = 0;
  choice->flag_l = 0;
  choice->flag_n = 0;
  choice->flag_h = 0;
  choice->flag_s = 0;
  choice->flag_f = 0;
  choice->flag_o = 0;
}

void processing(int argc, char *argv[], char *pattern, t_allFlags *choice) {
  int x, wordCount = 0;
  ;
  while ((x = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) != -1)
    switch (x) {
      case 'e':
        choice->flag_e = 1;
        fun_e(&wordCount, optarg, pattern);
        break;
      case 'i':
        choice->flag_i = 1;
        break;
      case 'v':
        choice->flag_v = 1;
        break;
      case 'c':
        choice->flag_c = 1;
        break;
      case 'l':
        choice->flag_l = 1;
        break;
      case 'n':
        choice->flag_n = 1;
        break;
      case 'h':
        choice->flag_h = 1;
        break;
      case 's':
        choice->flag_s = 1;
        break;
      case 'f':
        choice->flag_f = 1;
        fun_f(optarg, pattern, choice);
        break;
      case 'o':
        choice->flag_o = 1;
        break;
      default:
        printf("grep");
    }
}

void fun_grep(char *pattern, int countFile, char *fileName,
              t_allFlags *choice) {
  char arr_str[10000] = {0};
  int countLine = 1;
  int countStrc = 0;
  int tf_l = 0;
  FILE *file;
  file = fopen(fileName, "r");
  if (file) {
    regex_t reg;
    if (choice->flag_i) {
      regcomp(&reg, pattern, REG_ICASE);
    } else {
      regcomp(&reg, pattern, REG_EXTENDED);
    }
    regmatch_t pmatch[1];
    while (fgets(arr_str, sizeof(arr_str), file) != NULL) {
      if (strchr(arr_str, '\n') == NULL) {
        strcat(arr_str, "\n");
      }
      int success = regexec(&reg, arr_str, 1, pmatch, 0);
      if ((success == 0 && !choice->flag_v) ||
          (choice->flag_v && success == REG_NOMATCH)) {
        if (countFile > 1 && !choice->flag_h && !choice->flag_c &&
            !choice->flag_l) {
          printf("%s:", fileName);
        }
        tf_l = 1;
        if (choice->flag_n && !choice->flag_l && !choice->flag_c) {
          printf("%d:", countLine);
        }
        if (!choice->flag_l && !choice->flag_c &&
            (!choice->flag_o || (choice->flag_v && choice->flag_o))) {
          printf("%s", arr_str);
        }
        if (choice->flag_o && !choice->flag_c && !choice->flag_l) {
          char *line = arr_str;
          while (success == 0) {
            for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
              printf("%c", line[i]);
            }
            printf("\n");
            while (pmatch[0].rm_eo != 0) {
              pmatch[0].rm_eo--;
              line++;
            }
            success = regexec(&reg, line, 1, pmatch, 0);
          }
        }
        countStrc += 1;
      }
      countLine += 1;
    }
    if (choice->flag_c && choice->flag_l && countStrc) {
      countStrc = 1;
    }
    if (choice->flag_c) {
      if (countFile > 1 && !choice->flag_h) {
        printf("%s:", fileName);
      }
      printf("%d\n", countStrc);
    }
    if (choice->flag_l && tf_l == 1) {
      printf("%s\n", fileName);
    }
    regfree(&reg);
  } else if (!choice->flag_s) {
    fprintf(stderr, "grep: %s: No such file or directory\n", fileName);
  }
  fclose(file);
}
