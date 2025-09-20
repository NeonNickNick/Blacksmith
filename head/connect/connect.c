#include "connect.h"

#include <stdio.h>

char connection[100] = {"Connection from "};
char connected[100] = {"Connected to "};
char port[10] = {"\0"};
int playercommand = -1;
int robotcommand = -1;
bool if_guide = false;
int portsite = 0;
bool prequit = true;