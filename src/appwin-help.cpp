#include <qdialog.h>

#include "ui_dabout.h"

#include "appwin.h"


void AppWin::about()
{
  Ui::DAbout ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);
  dialog->exec();
  delete( dialog );
}
