#ifndef CLINE_H
#define CLINE_H

#include <qstring.h>

#include "pltwin.h"


void CLineHelp();
bool InterpretArgs(int argc, char *argv[]);
void InterpretCommand(QString cmd);

#endif
