#ifndef SCALEPOS_H
#define SCALEPOS_H

#include "pltwin.h"
#include "ui_dscalepos.h"


class ScalePos : public QDialog, public Ui::DScalePos
{
  Q_OBJECT
 
public:
  PltWin *pw;
  ScalePos(PltWin *parent = 0);

public slots:
  void autoScale();
};

#endif
