#include "bcewindow.hpp"

int main(int argc, char ** args)
{
  QApplication app(argc,args);

  BCEWindow window;

  window.show();

  return app.exec();
}