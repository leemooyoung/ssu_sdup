#ifndef _SDUP_INPUTCNTL_H
#define _SDUP_INPUTCNTL_H

#include <stdio.h>

#define SBW_RES_ARR_DEFAULT 5

char *get_input(FILE *fp, char * buf, int size);

char **split_by_word(char *str);

#endif