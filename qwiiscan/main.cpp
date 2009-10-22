#include <QApplication>
#include "MainWindow.h"

int main( int argc, char **argv )
{
  QApplication app( argc, argv );

  MainWindow ww;
  ww.show();

  return app.exec();
}
