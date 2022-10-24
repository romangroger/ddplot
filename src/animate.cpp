#include <QDialog>

#include "animate.h"


Animate::Animate()
{
  setupUi(this);
  connect(btSelAll, SIGNAL(released()), this, SLOT(selectAll()));
  connect(btUnselAll, SIGNAL(released()), this, SLOT(unselectAll()));
}


void Animate::selectAll()
{
  int i;
  
  for (i=0; i<plotFiles->count(); i++)
    plotFiles->item(i)->setSelected(true);
}


void Animate::unselectAll()
{
  int i;
  
  for (i=0; i<plotFiles->count(); i++)
    plotFiles->item(i)->setSelected(false);
}
