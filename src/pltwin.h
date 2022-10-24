#ifndef PLTWIN_H
#define PLTWIN_H

#include <QWidget>
#include <QPrinter>

#include "ddplot.h"
#include "darray.h"


class PltWin : public QWidget
{
  Q_OBJECT

public:
  QString FName;
  DArray<QColor,2> zColorLayer;   // colors of the atomic layers (1=foreground, 2=background)
  
  // coordinates of atoms and their relative displacements
  DMatrix xyzRel;          // coordinates of atoms in the relaxed configuration
  DMatrix xyzInit;         // coordinates of atoms in the initial (unrelaxed) configuration
  DMatrix xyzInert;        // coordinates of inert atoms
  DMatrix aDisp;           // atomic displacements (real values)
  IVector atomType;        // flag recognizing the types of atoms
  int NRel;                // number of atoms in the relaxed block
  int NInit;               // number of atoms in the initial block
  int NInert;              // number of inert atoms (block in the BOP format)

  // parameters of the atomic block
  DMatrix csys;            // coordinate system of the block
  DVector blSize;          // calculated size of the block in the x,y,z direction
  DVector xyzMin, xyzMax;
  DVector period;          // periodicity in the three directions
  double latpar=1.0;       // lattice parameter
  char *tail;              // the tail of the .plt file containing the relaxation info (text)
  int TailSize;            // size of the trailing info stored in <tail>

  // Z-layers
  DVector zCoordLayer;     // z-coordinates of layers (1 to NumZLayers)
  IVector zLayer;          // identifiers of the layers
  int NumZLayers;          // number of layers of atoms
  IVector zLineThickLayer; // line thickness of atoms in a specific layer
  IVector zDiamLayer;      // diameter of atoms in a specific layer in pixels
  BVector zLayerSel;       // true if a particular Z-layer is selected (considered for plotting arrows)
  bool showZLayers;        // true if the active Z-layers should be visualized as a small inset
  double zTolerance;       // tolerance for the Z-coordinates of atoms in the same Z-layer 

  // dislocations
  DVector Bvect;           // Burgers vector
  double xCore,yCore;
  bool showNyeTensor;      // show a contour map corresponding to a component of the Nye tensor
  bool knownNyeTensor;     // true if the Nye tensor was already calculated (initially false)

  // grain boundaries
  double gbYcoor;          // y-position of the grain boundary
  IVector gbLayer;    // grain boundary layer in which the atom lies(upper grain>0, lower grain<0)
  bool isGBfile;           // true if the file contains a grain boundary data
  bool showGB;             // true if the grain boundary is being plotted 

  // appearance of the plot
  DVector ProjVector;      // vector into whose direction the displacements are to be projected
                           // in the system [1-21], [-101], [111]
  double arrAngle;         // angle of arrows
  double ZFact;            // zoom factor
  double xPan,yPan;        // panning
  double DScaleFact;       // scaling factor for displacements (arrow lengths)
  double AtomDispScale;    // scaling of atomic displacements in the relaxed configuration
  double factor;
  IMatrix atomScreen;      // coordinates of atoms on the screen [pixels]
  int xBorder,yBorder,xOffset,yOffset;
  IVector xyzCen;
  IVector aorder;          // order in which the atoms are to be plotted
  int shortestArrow;       // shortest arrow to be plotted (in pixels)
  int thicknessArrow;      // thickness of an arrow (in pixels)
  int plotType;            // type of plot
  int DispComponent;       // component of the displacements plotted
  int AtomPos;             // plotting of atomic positions (RELAXED, UNRELAXED)  
  int PTAngle;             // angle between two neighboring traces of planes plotted
  bool PrintMagDisp;       // print magnitudes of displacements (value/a) ?
  bool AtomNumbers;        // print the numbers of atoms
  bool isSelected;         // true if this plot is selected
  bool DisloCenter;        // show the dislocation center ?
  bool PlaneTraces;        // show the traces of the planes ?
  bool InertAtoms;         // plot inert atoms ?
  bool showCSys;           // is the coordinate system plotted ?
  bool corrEdgeCompBCC;    // correction of the orientations of edge components for screw in BCC
  bool antialiase;         // should the antialiasing be applied ?

  FILE *feps;              // handle of the file into which we print postscript
  bool paintEPS;           // paint in the EPS file ?

  // radial distribution function
  DMatrix RdfDist;         // distances of the first, second, etc., nearest neighbors (ranges in rows)
  int arrNeighNum;         // number of different neighbor distances to be considered
  IVector arrNeighbors;    // which neighbor distances (1=first, 2=second, etc.)
  IVector RdfNum;          // numbers of 1st, 2nd, 3rd, ..., nearest neighbors
  int RdfNNeigh;           // how many nearest neighbors ? (3=first, second and third)

  // linked neighbor list
  double rcut;             // cut-off radius (minimum size of an edge in either direction)
  DVector cellsize;        // size of a cell's edge in x, y and z direction
  IVector ncell;           // number of cells in x, y and z direction
  IVector list;            // linked neighbor list
  IArray3 head;            // index of a head atom in each cell
  IMatrix NeighListInit, NeighListRel;
  IVector numNeighInit, numNeighRel;  // number of neighbors of each atom in the initial and relaxed config.
  DArray3 scaleArr;        // scaling factors for plotting the arrows
  int nnpairs;             // number of neighbor pairs of atomic neighbors
  bool showNeighCells;     // true if the division into cells is to be displayed

  // picking of atoms for the calculation of the Burgers vector
  IVector apicked;         // atoms picked
  DMatrix dpoly;           // (ndpoly_init,2) polygon with vertices of the polygon containing the dislocation center (initial conf.)
  DMatrix dpoly_rel;       // (ndpoly_rel,2) polygon with vertices of the polygon containing the dislocation center (relaxed conf.)
  DMatrix dpoly_init;      // (ndpoly_init,2) polygon with vertices of the polygon containing the dislocation center (initial conf.)
  int doAfterMouseClicks;  // the action to be done after double-click
  int napicked;            // number of atoms picked
  int ndpoly_rel;          // number of nodes defining the polygon that encompasses the dislocation center
  int ndpoly_init;         // number of nodes defining the polygon that encompasses the dislocation center
  int ncalcdpos;           // number of calculations of the dislocation position (when shuffling the cutting order)
  int ndpoly;              // number of nodes defining the dislocation polygon
  bool clickAtoms;         // true if we are allowed to select atoms by clicking
  bool interactive;        // interactive search for the position of the dislocation
  bool showDPoly;          // show the dislocation polygon in the main window
  bool shuffleIdx;         // shuffle the order of atoms before calculating the dislocation position

  // displacements of atoms in the direction of the Burgers vector
  DMatrix dposchain;       // positions of the dislocation in individual chains of atoms
  DMatrix ubchain;         // displacements of atoms || to the Burgers vector (ub)
  DMatrix dpath;           // path of the dislocation
  IMatrix achain;          // ids of atoms in each chain
  IVector nachain;         // numbers of atoms in each chain
  DVector ubchain_min;     // minimum ub along each chain
  DVector ubchain_max;     // maximum ub along each chain
  double xdavg, ydavg;     // approximate position of the dislocation
  int ndpath;              // number of nodes defining the dislocation pathway
  int nchain;              // number of chains of atoms
  int applyHow;            // calculate the dislocation position only for this plot (0) or for all (1)?

  QVector<QImage> sphere;  // rendered spheres with colors distinguishing atomic types
  
  PltWin(QWidget *parent=0, Qt::WindowFlags f=Qt::Widget);
  ~PltWin();
  PltWin &operator=(const PltWin &value);

  void paintEvent( QPaintEvent * );
  void plotEdgeComponent(QPainter *p);
  void plotScrewComponent(QPainter *p);
  void plotDifference(QPainter *p);
  void plotNyeTensor(QPainter *p);

protected:
  virtual void mouseDoubleClickEvent( QMouseEvent * event );
  virtual void mousePressEvent( QMouseEvent * event );
  virtual void resizeEvent( QResizeEvent * );

public:
  void drawIt(QPainter *p);
  void printIt(QPrinter *printer);

  bool DrawArrow(QPainter *p, double xw0, double yw0, double xw1, double yw1 );
  void DrawAtom(QPainter *p, int atom, double xw, double yw, int dia);
  void DrawLine(QPainter *p, double xw0, double yw0, double xw1, double yw1);
  void DrawPlaneTraces(QPainter *p, double xw0, double yw0, double xw1, double yw1);
  void DrawText(QPainter *p, double xw, double yw, QString txt, Qt::AlignmentFlag align=Qt::AlignLeft, float ang=0.0);
  void Pan(double xsteps, double ysteps);
  void SetGeometry();
  void ShowActiveZLayers(QPainter *p);
  void ShowColorMap(QPainter *p);
  void ShowCSys(QPainter *p);
  void Zoom(double zfact);

  void PrepareZColoredSpheres();
  
  void xyScreenToWorld(int xs, int ys, double &xw, double &yw);
  void xyWorldToScreen(double xw, double yw, int &xs, int &ys);

  // declared in pltwin-load.cpp
  bool LoadBOP(QString fname);
  bool LoadDD(QString fname);
  bool LoadGB(QString fname, long int *blstart, PltWin *initConf);
  bool LoadPLT(QString fname);
  bool LoadXYZ(QString fname);

public:
  void calcEdgeRelDisp();
  void calcScrewRelDisp();
  bool calcProjRelDisp();
  void calcNyeTensor();
  bool CompareDisp(PltWin *p2, int comp);
  int FindNearestAtom( int x, int y );
  bool InitBlockGeometry(bool toggle_inert);
  bool InitBlockANeighbors();

signals:
  void signalDPosPeierlsNabarro_interm();
  void signalDPosPeierlsNabarro_end();
  void signalDPosDecimate_end();
  void signalDefineBurgersCircuit_end();
  void signalDefineCalcAtomDispl_end();
};

#endif
