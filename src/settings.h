#pragma once
#include "io/print.h"

#define MODE_CLI 0 
#define MODE_UCI 1

struct Settings{
  printSettings ps;
  char mode;
};
typedef struct Settings Settings;
extern Settings settings;

void loadSettings();
