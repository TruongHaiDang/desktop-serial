#include <QApplication>
#include <QGuiApplication>
#include <QScreen>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.adjustSize();

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = screen->availableGeometry();
        const QRect windowGeometry = window.frameGeometry();
        const QPoint centeredTopLeft(
            screenGeometry.x() + (screenGeometry.width() - windowGeometry.width()) / 2,
            screenGeometry.y() + (screenGeometry.height() - windowGeometry.height()) / 2);
        window.move(centeredTopLeft);
    }

    window.show();

    return app.exec();
}
