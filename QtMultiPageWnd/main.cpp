#include "QtMultiPageWnd.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	qRegisterMetaTypeStreamOperators<QtMimeType>("QtMimeType");

    QtMultiPageWnd w(0);
    w.show();
    return a.exec();
}
