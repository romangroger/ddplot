#include <qmessagebox.h>
#include <qstring.h>


void msgCritical(const char *msg)
{
  QMessageBox::critical(0, "Critical Error", msg);
}


void msgError(const char *msg)
{
  QMessageBox::critical(0, "Error", msg);
}


int msgQuestion(const char *title, const char *msg)
{
  return QMessageBox::warning(0, title, msg, "&Yes", "&No", "&Cancel", 0, 1);
}


int msgWarning(const char *msg, char *button1, char *button2, char *button3, int par1, int par2)
{
  return QMessageBox::warning( 0, "Warning", msg, button1, button2, button3, par1, par2 );
}


void msgInfo(QString msg)
{
  QMessageBox::information( 0, "Information", msg.toLatin1().data() );
}
