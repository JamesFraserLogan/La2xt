#pragma once
#define NWORDS (sizeof keywords / sizeof(struct words))
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> // For file i/o. Stat function.
struct words 
{
    char *word;
} keywords[] = 
{
    {"Abstract"},
    {"section"},
    {"subsection"},
    {"subsubsection"}
} ;
