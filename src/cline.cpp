#include <qstring.h>
#include <string.h>
#include <stdio.h>

#include "cline.h"
#include "ddplot.h"


/* Message that will be printed to STDOUT if ddplot is started as ddplot -h. */ 

void CLineHelp()
{
  printf("ddplot [options] [plot file]\n\n");
  printf("Options:\n");
  printf("  -f file    reads the custom settings from [file]. By default, the settings\n");
  printf("             is read from file .ddplot located in your HOME directory. If using\n");
  printf("             more different settings, use -f to specify which settings should \n");
  printf("             be adopted for the current session.\n"); 
  printf("  -h         this help\n");
  printf("  -v         verbose mode - print detailed information about what is currently\n");
  printf("             going on into the terminal window\n");
}


/* Interpretation of the command-line arguments */

bool InterpretArgs(int argc, char *argv[]) 
{
  QString arg, arg2;
  int i;

  for (i=1; i<argc; i++) {
    arg = argv[i];

    // this is needed for Mac OS X when running ddplot from the Finder - it gives as argv[1]
    // the process serial number
    /*   arg2 = arg;  arg2.remove(5,arg2.length()-5);
	 if (arg2 == "-psn_") continue; */

    // load settings file instead of .ddplot
    if (arg == "-f" && i+1 < argc) {
      SETTINGS_FILE = argv[++i];
      printf("User settings read from %s\n", SETTINGS_FILE.toLatin1().data());
      continue;
    }

    // command-line help
    if (arg == "-h") {
      CLineHelp();
      return false;
    }

    // verbose mode
    if (arg == "-v") {
      VERBOSE = true;
      continue;
    }

    // last arguments are the names of plot files
    PLOT_FILE << arg;
  }

  return true;
}


/* Interpretation of the commands given in the settings file .ddplot (or any other that is given to
   the code via ddplot -f). */

void InterpretCommand(QString cmd)
{
  QString params[100];
  int npar, idx, i, j, id;
  int angle;

  // remove comments, whitespaces, endlines and change to upper case

  idx = cmd.indexOf('#');   if (idx>=0) cmd.truncate(idx);
  idx = cmd.indexOf('\n');  if (idx>=0) cmd.truncate(idx);
  idx = cmd.indexOf('\r');  if (idx>=0) cmd.truncate(idx);  // Linux terminates with \r\n

  if (cmd.isEmpty())
    return;

  for (npar=0; npar<100; npar++) {
    params[npar] = cmd.section(',', npar, npar);
    if (params[npar].isEmpty())
      break;
    params[npar] = params[npar].trimmed();    // get rid of spaces
  }

  if (npar < 1)
    return;

  params[0] = params[0].toUpper();

  // interpretation of the commands read above

  if (params[0] == "ANTIALIASE") {
    ANTIALIASE = (params[1]=="on");
  }

  if (params[0] == "ATOM.3DSPHERE") {
    ATOM_3DSPHERE = (params[1] == "on");
  }

  if (params[0] == "ATOM.DIAMETER") {
    for (i=1; i<=npar-1; i++)
      ATOM_DIAM[i-1] = params[i].toInt();
    NUM_DIAM = npar-1;
  }

  if (params[0] == "ATOM.THICKNESS") {
    for (i=1; i<=npar-1; i++)
      ATOM_THICKNESS[i-1] = params[i].toInt();
    NUM_THICKNESS = npar-1;
  }

  if (params[0] == "ATOM.FGCOLOR") {
    for (i=1; i<=npar-1; i++)
      ATOM_FGCOLOR[i-1] = params[i];
    NUM_FGCOLORS = npar-1;
  }

  if (params[0] == "ATOM.BGCOLOR") {
    for (i=1; i<=npar-1; i++)
      ATOM_BGCOLOR[i-1] = params[i];
    NUM_BGCOLORS = npar-1;
  }

  if (params[0] == "ANAME") {
    id = params[1].toInt();
    ATOM_NAME[id] = params[2];
  }

  if (params[0] == "ATOMNUMBERS") {
    ATOM_NUMBERS = (params[1] == "on");
  }
      
  if (params[0] == "ARRNEIGHBORS") {
    ARR_NEIGHNUM = npar-1;
    for (i=1; i<=npar-1; i++)   
      ARR_NEIGHBORS[i-1] = params[i].toInt();

    if (ARR_NEIGHNUM == 1 && ARR_NEIGHBORS[0] == 0)
      ARR_NEIGHNUM = 0;
  }

  if (params[0] == "ADISTINGUISH") {
    if (params[1] == "layer")
      PLOT_TYPE = PLOT_ATOM_LAYERS;
    if (params[1] == "type")
      PLOT_TYPE = PLOT_ATOM_TYPES;
    if (params[1] == "neigh")
      PLOT_TYPE = PLOT_ATOM_NEIGHBORS;
  }

  if (params[0] == "ARROW.SHORTEST")          
    ARROW_SHORTEST = params[1].toInt();

  if (params[0] == "ARROW.THICKNESS")          
    ARROW_THICKNESS = params[1].toInt();

  if (params[0] == "ATOMPOS") {
    if (params[1] == "relaxed")
      ATOMPOS = RELAXED;

    if (params[1] == "unrelaxed")
      ATOMPOS = UNRELAXED;

    if (params[1] == "composite")
      ATOMPOS = COMPOSITE;

    if (params[1] == "relaxed" || params[1] == "composite") {
      if (npar > 2)
        ATOMDISP_SCALE = params[2].toInt();
      else
        ATOMDISP_SCALE = 1.0;
    }
  }

  if (params[0] == "BACKGROUNDCOLOR") {
    BACKGROUND_COLOR = params[1];
  }

  if (params[0] == "CORREDGECOMPBCC") {
    CORR_EDGECOMP_BCC = (params[1] == "on");
  }
  
  if (params[0] == "CSYS") {
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        idx = 3*i+j+1;
        CSYS[i][j] = params[idx].toDouble();
      }
    }
  }

  if (params[0] == "DISPCOMPONENT") {
    if (params[1] == "edge")  
      DISP_COMPONENT = EDGE;
    else if (params[1] == "screw")
      DISP_COMPONENT = SCREW;
    else
      DISP_COMPONENT = NONE;
  }

  if (params[0] == "DSCALEFACT") {
    if (params[1] == "auto")
      DSCALE_FACT = AUTO;
    else
      DSCALE_FACT = params[1].toDouble();
  }

  if (params[0] == "NEIGHRCUT") {
    NEIGHRCUT = params[1].toDouble();
  }

  if (params[0] == "PLANETRACES") {
    angle = params[1].toDouble(); 
    if (angle > 0) {
      PTRACES = true; 
      PTRACES_ANGLE = angle;
    }
  }

  if (params[0] == "PRINTMAGDISP") {
    if (params[1] == "on")
      PRINT_MAG_DISP = true;

    else
      PRINT_MAG_DISP = false;
  }

  if (params[0] == "PROJVECTOR") {
    PROJ_VECTOR[0] = params[1].toDouble();
    PROJ_VECTOR[1] = params[2].toDouble();
    PROJ_VECTOR[2] = params[3].toDouble(); 
  }

  if (params[0] == "WINSIZE") {
    WINWIDTH = params[1].toInt();
    WINHEIGHT = params[2].toInt();
  }

  if (params[0] == "ZOOMFACT") {
    ZOOM_FACT = params[1].toDouble();
  }

  if (params[0] == "ZTOLERANCE") {
    ZTOLERANCE = params[1].toDouble();
  }

  if (params[0] == "RTOLDISORDER") {
    RTOLDISORDER = params[1].toDouble();
  }
}
