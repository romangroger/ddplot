#include "selectplots.h"


SelectPlots::SelectPlots(QDialog *parent) : QDialog(parent)
{
  setupUi(this);
  connect(btSelAll, SIGNAL(released()), this, SLOT(selectAll()));
  connect(btUnselAll, SIGNAL(released()), this, SLOT(unselectAll()));
}


void SelectPlots::selectAll()
{
  int i;
  
  for (i=0; i<plotFiles->count(); i++)
    plotFiles->item(i)->setSelected(true);
}


void SelectPlots::unselectAll()
{
  int i;
  
  for (i=0; i<plotFiles->count(); i++)
    plotFiles->item(i)->setSelected(false);
}
