#ifndef DDPLOT_H
#define DDPLOT_H

#include <QImage>
#include <QColor>
#include <QString>
#include <QVector>

//------- version and release of ddplot --------
#ifndef EXPERIMENTAL
  const QString VERSION = "5.2";
#else
  const QString VERSION = "5.2 [EXPERIMENTAL]";
#endif

//----------------------------------------------


// global constants
const int UNDEFINED = 0;

const int END_OF_LIST = 0;
const int MAX_LAYERS = 10;        // max. layers for the assignment of the atomic names
const int MAX_NEIGHBORS = 5;      // number of nearest neighbors for which we will calculate the RDF
const int MAX_ELEMENTS = 10;       // maximum number of chemical elements in the plot

const int MAX_CELL = 50;          // maximum number of cells in either direction (for linked neigh. list)

const double PAN_RATIO = 0.01;    // multiplication constant for panning - PAN_RATIO*SIZEX(Y) is the step
const double PAN_NORMAL = 1;      // normal panning
const double PAN_FAST = 3;        // fast panning

const int LOAD_SETTINGS = 1;
const int SAVE_SETTINGS = 2;

const int PLOT_ATOM_LAYERS = 1;   // recognizing the atoms acoording to their layer
const int PLOT_ATOM_TYPES  = 2;   // recognizing the atoms according to their type
const int PLOT_ATOM_NEIGHBORS  = 3;   // recognizing the atoms according to their numbers of neighbors

const int AUTO = -1;              // automatic scaling of the displacements

const int ON = 1;
const int OFF = 0;

const int NONE = 0;               // no arrows plotted
const int SCREW = 1;              // plot screw component
const int EDGE = 2;               // plot edge component
const int PROJ = 3;               // relative displacements calculated in the given projection vector
const int DIFF_EDGE = 4;          // difference between displacements from two plots
const int DIFF_SCREW = 5;

const int UNRELAXED = 0;          // plot unrelaxed positions of atoms
const int RELAXED = 1;            // plot relaxed positions of atoms
const int COMPOSITE = 2;          // composite view - relaxed+unrelaxed configuration

const int FORMAT_UNKNOWN = 0;     // input file in unrecognized format
const int FORMAT_DD = 1;          // input file in the format of ddplot
const int FORMAT_BOP = 2;         // input file in the format of block file from BOP
const int FORMAT_PLT = 3;         // input file is the plot file from BOP
const int FORMAT_GB = 4;          // grain boundary data file (multiple structures)
const int FORMAT_XYZ = 5;         // input file in the JMol's .xyz format

const int DO_NOTHING = 0;         // no action to be done
const int CALC_BURG_VECTOR = 1;   // calculates the Burgers vector
const int CALC_DPOS_BURGCIRCUIT = 2;     // calculates the polygonal bounds for position of the dislocation
const int CALC_DPOS_PEIERLSNABARRO = 3;  // calculates the position of the dislocation using the P-N approach
const int CALC_ATOM_DISPL = 4;    // calculates the relative displacements of atoms along the chain
const int CALC_DPOS_DECIMATE = 5; // successful decimation to find the position of the dislocation

const double INF = 1e20;

// global variables

extern const char *ATOMPOS_NAME[3];
extern int ATOM_DIAM[10];
extern int ATOM_THICKNESS[10];
extern QColor ATOM_FGCOLOR[10];
extern QColor ATOM_BGCOLOR[10];
extern QColor BACKGROUND_COLOR;
extern QString ATOM_NAME[10];
extern int ARR_NEIGHNUM;
extern int ARR_NEIGHBORS[MAX_NEIGHBORS];
extern double CSYS[3][3];
extern int DISP_COMPONENT;
extern double DSCALE_FACT;
extern int NUM_FGCOLORS;
extern int NUM_BGCOLORS;
extern int NUM_DIAM;
extern int NUM_THICKNESS;
extern double PROJ_VECTOR[3];
extern int ARROW_SHORTEST;
extern int ARROW_THICKNESS;
extern int PLOT_TYPE;
extern int ATOMPOS;
extern double ATOMDISP_SCALE;
extern int WINWIDTH, WINHEIGHT;   // width and height of the application window [pixels]
extern bool PRINT_MAG_DISP;
extern bool PRINT_NUM_ATOM;
extern double ZOOM_FACT;
extern bool ATOM_NUMBERS;
extern bool SHOW_DCENTER;
extern bool PTRACES;
extern int PTRACES_ANGLE;
extern bool SHOWGB;
extern bool SHOW_ZLAYERS;
extern bool ALL_ZLAYERS;
extern bool VERBOSE;
extern double NEIGHRCUT;
extern bool SHOW_CSYS;
extern bool SHOW_NEIGHCELLS;
extern bool CORR_EDGECOMP_BCC;
extern double ZTOLERANCE;
extern double RTOLDISORDER;
extern bool ANTIALIASE;
extern bool ATOM_3DSPHERE;

extern QString SETTINGS_FILE;
extern QStringList PLOT_FILE;

extern QImage GRAY_SPHERE;

extern QVector<QRgb> cmap;

#ifdef DEBUG
  extern FILE *fdbg;
#endif

#endif // DDPLOT_H
