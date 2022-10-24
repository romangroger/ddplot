#ifndef LOOKANDFEEL_H
#define LOOKANDFEEL_H

#include <qcolor.h>

#include "darray.h"
#include "pltwin.h"

#include "ui_dlookandfeel.h"


class LookAndFeel : public QDialog, public Ui::DLookAndFeel
{
  Q_OBJECT
 
public:
  QColor bgcol = Qt::white;
  
  LookAndFeel(PltWin *parent = 0);

public slots:
  void setColor(int col);
};

#endif
