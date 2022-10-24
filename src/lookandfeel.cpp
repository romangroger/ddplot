#include <qcolordialog.h>
#include <qpalette.h>

#include "lookandfeel.h"


LookAndFeel::LookAndFeel(PltWin *parent) : QDialog(parent)
{
  setupUi(this);
  connect(bgcolor, SIGNAL(cursorPositionChanged(int,int)), this, SLOT(setColor(int)));
}


void LookAndFeel::setColor(int col)
{
  QColor color = QColorDialog::getColor(bgcol, bgcolor);    
  if (color.isValid()) {
    bgcol = color;
    QPalette palette;
    palette.setColor(QPalette::Base, bgcol);
    bgcolor->setPalette(palette);
  }
}
