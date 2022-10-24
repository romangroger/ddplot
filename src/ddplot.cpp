/*--------------------------------------------------------------------------------------------------

                                              DDPLOT

  Program for visualization of relative displacements between atoms in crystalline lattices using
  the dislocation displacement maps. When started, the program reads the configuration file .ddplot
  from (1) the given directory if the program is called as "ddplot -f <given directory>/.ddplot",
  (2) the current directory, (3) the home directory ($HOME in Linux/Mac).

  Developed by Roman Gröger
  Distributed under the GNU General Public License
    (2002-2007) University of Pennsylvania, Department of Materials Science and Engineering
    (2007-2009) Los Alamos National Laboratory, Theoretical Division
    (2009-now)  Academy of Sciences of the Czech Republic, Institute of Physics of Materials

--------------------------------------------------------------------------------------------------*/

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QDir>
#include <QStyleFactory>

#include "appwin.h"
#include "cline.h"
#include "colormap.h"
#include "darray.h"
#include "matinv.h"

char const *ATOMPOS_NAME[3] = {"UNRELAXED", "RELAXED", "COMPOSITE"};
int ATOM_DIAM[10] = {12, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int ATOM_THICKNESS[10] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
QColor ATOM_FGCOLOR[10];   // defined below
QColor ATOM_BGCOLOR[10];   // defined below
QColor BACKGROUND_COLOR = Qt::transparent;
QString ATOM_NAME[MAX_LAYERS];
int ARR_NEIGHNUM = 1;
int ARR_NEIGHBORS[MAX_NEIGHBORS];
double CSYS[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
int DISP_COMPONENT = SCREW;
double DSCALE_FACT = 1;
int NUM_FGCOLORS = 1;
int NUM_BGCOLORS = 8;
int NUM_DIAM = 1;
int NUM_THICKNESS = 1;
double PROJ_VECTOR[3] = {1,1,1};
int ARROW_SHORTEST = 0;
int ARROW_THICKNESS = 1;
int PLOT_TYPE = PLOT_ATOM_LAYERS;
int ATOMPOS = UNRELAXED;
double ATOMDISP_SCALE = 1.0;
int WINWIDTH=1024, WINHEIGHT=768;
bool PRINT_MAG_DISP = false;
bool PRINT_NUM_ATOM = false;
double ZOOM_FACT = 1.0;
bool ATOM_NUMBERS = false;
bool SHOW_DCENTER = false;
bool PTRACES = false;
int PTRACES_ANGLE = 0.0;
bool SHOWGB = true;
bool SHOW_ZLAYERS = false;
bool ALL_ZLAYERS = true;
bool VERBOSE = false;
double NEIGHRCUT = -1;  // if no rcut has been defined by user
bool SHOW_CSYS = false;
bool SHOW_NEIGHCELLS = false;
bool CORR_EDGECOMP_BCC = false;
double ZTOLERANCE = 0.0;
double RTOLDISORDER = 1e-3;
bool ANTIALIASE = false;
bool ATOM_3DSPHERE = false;

QImage GRAY_SPHERE;

QString SETTINGS_FILE;
QStringList PLOT_FILE;

QVector<QRgb> cmap;

#ifdef DEBUG
FILE *fdbg;
#endif


/*  The main function of the ddplot code */

int main(int argc, char *argv[])
{
  QString home_dir;
  char const *fgcolors[1] = {"black"};
  char const *bgcolors[8] = {"white", "yellow", "green", "cyan", "red", "blue", "gray", "black"};
  int i;

#ifdef DEBUG
  printf("Running ddplot in the DEBUG mode...\n");
  printf("The output will be saved in file ddplot.dbg\n");
  fdbg = fopen("ddplot.dbg","w+");
#endif

  home_dir = QDir::toNativeSeparators(QDir::homePath());
#ifdef Q_OS_WIN
  SETTINGS_FILE = home_dir + QString("\\.ddplot");
#else
  SETTINGS_FILE = home_dir + QString("/.ddplot");
#endif
  printf("user settings read from %s\n", SETTINGS_FILE.toLatin1().data());

  if (argc > 1 && !InterpretArgs(argc, argv)) {
#ifdef DEBUG
    fclose(fdbg);
#endif
    exit(EXIT_FAILURE);
  }

  for (i=0; i<NUM_FGCOLORS; i++)  ATOM_FGCOLOR[i] = QColor(fgcolors[i]);
  for (i=0; i<NUM_BGCOLORS; i++)  ATOM_BGCOLOR[i] = QColor(bgcolors[i]);

  for (i=0; i<MAX_LAYERS; i++)
    ATOM_NAME[i] = "X";

  ARR_NEIGHBORS[0] = 1;

  //  GRAY_SPHERE.load(":sphere.png");
  QApplication App(argc, argv);
  AppWin *MainWin = new AppWin();
  if (PLOT_FILE.count() > 0) MainWin->loadFile(PLOT_FILE);

  QString title = QString("ddplot ") + QString(VERSION);
  MainWin->setWindowTitle(title);
  //#ifdef Q_OS_WIN
  //  MainWin->setWindowIcon(QIcon(QString("ddplot.ico")));
  //#endif
  MainWin->setWindowIcon(QIcon("ddplot.png"));
  MainWin->show();

  App.exec();

#ifdef DEBUG
  fclose(fdbg);
#endif
  exit(EXIT_SUCCESS);
}
