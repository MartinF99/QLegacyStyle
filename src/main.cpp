#include "q5legacystyle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q5LegacyStyle w;
    w.show();

    return app.exec();
}

