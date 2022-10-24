#ifndef ATOMSTYLE_H
#define ATOMSTYLE_H

#include <qcolor.h>

#include "darray.h"
#include "pltwin.h"

#include "ui_datomstyle.h"


class AtomStyle : public QDialog, public Ui::DAtomStyle
{
  Q_OBJECT
 
public:
  DArray<QColor,2> zColor;
  PltWin *pw;

  AtomStyle(PltWin *parent = 0);
  void setData();
  bool verify();

public slots:
  void setColor(int row, int col);
};

#endif
