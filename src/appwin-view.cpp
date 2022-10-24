#include <qtabwidget.h>

#include "appwin.h"


void AppWin::fitWin()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  pw->xPan = 0;
  pw->yPan = 0;
  pw->ZFact = 1;
  pw->repaint();

  repaintStatusBar();
}


void AppWin::panRight()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(PAN_NORMAL,0);
  repaintStatusBar();
}


void AppWin::panLeft()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(-PAN_NORMAL,0);
  repaintStatusBar();
}


void AppWin::panUp()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(0,PAN_NORMAL);
  repaintStatusBar();
}


void AppWin::panDown()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(0,-PAN_NORMAL);
  repaintStatusBar();
}


void AppWin::panFastRight()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(PAN_FAST,0);
  repaintStatusBar();
}


void AppWin::panFastLeft()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(-PAN_FAST,0);
  repaintStatusBar();
}


void AppWin::panFastUp()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(0,PAN_FAST);
  repaintStatusBar();
}


void AppWin::panFastDown()
{
  plotWidget[ tabWidget->currentIndex() ]->Pan(0,-PAN_FAST);
  repaintStatusBar();
}


void AppWin::zoomIn()
{
  plotWidget[ tabWidget->currentIndex() ]->Zoom(2.0);
  repaintStatusBar();
}


void AppWin::zoomOut()
{
  plotWidget[ tabWidget->currentIndex() ]->Zoom(0.5); 
  repaintStatusBar();
}
