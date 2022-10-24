#include <qdir.h>

#include "appwin.h"
#include "cline.h"
#include "fformat.h"
#include "messages.h"


void AppWin::repaintStatusBar()
{
  if (tabWidget==NULL)
    return;

  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  const char *dcomp;
  char msg[128];
  int NPlots = tabWidget->count();

  switch(pw->DispComponent) {
    case EDGE:
      dcomp = "EDGE";
      break;

    case SCREW:
      dcomp = "SCREW";
      break;

    case NONE:
      dcomp = "NONE";
      break;

    case DIFF_EDGE:
    case DIFF_SCREW:
      dcomp = "DIFF";
      break;

    default:
      dcomp = "PROJ";
  }

  sprintf(msg, "%d plot(s) | MAG=%0.2lf | DSCALE=%0.2lf | PANX=%0.2lf, PANY=%0.2lf" \
    " | PROJVECT=[%0.1lf,%0.1lf,%0.1lf] | DCOMP=%s | ATOMS=%s", 
    NPlots, 
    pw->ZFact,
    pw->DScaleFact,
    pw->xPan,
    pw->yPan,
    pw->ProjVector(1),
    pw->ProjVector(2),
    pw->ProjVector(3),
    dcomp,
    ATOMPOS_NAME[pw->AtomPos]);

  statusLabel->setText(msg);
}



















