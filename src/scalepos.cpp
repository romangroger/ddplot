#include "scalepos.h"


ScalePos::ScalePos(PltWin *parent) : QDialog(parent)
{
  pw = parent;
  setupUi(this);
  connect(btAuto, SIGNAL(released()), this, SLOT(autoScale()));
}


void ScalePos::autoScale()
{
  double xfact, yfact, factor;

  xfact = (pw->width()-2*pw->xBorder) / pw->blSize(1);
  yfact = (pw->height()-2*pw->yBorder) / pw->blSize(2);
  factor = (xfact<yfact) ? xfact : yfact;

  magRatio->setText(QString::number(factor));
  magRatio->selectAll();
}
