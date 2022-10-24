#include "appwin.h"
#include "ui_appwin.h"

AppWin::AppWin(QWidget *parent) : QMainWindow(parent)
{
  loadSettings(SETTINGS_FILE);

  tabWidget = NULL;

  ui = new Ui::AppWin;
  ui->setupUi(this);
  resize(WINWIDTH, WINHEIGHT);

  statusLabel = new QLabel(this);
  ui->statusBar->addPermanentWidget(statusLabel, 1);
  
  enableMenuItems(false);

  printer = new QPrinter( QPrinter::PrinterResolution );
  printer->setColorMode(QPrinter::Color);
  printer->setPageSize(QPrinter::Letter);
}

AppWin::~AppWin()
{
  delete ui;
  delete(printer);
}


void AppWin::closeEvent( QCloseEvent * )
{
  quit();
}


void AppWin::resizeEvent( QResizeEvent * )
{
  if (tabWidget==NULL)
    return;

  tabWidget->setGeometry(0, 23, width(), height()-46);
}
