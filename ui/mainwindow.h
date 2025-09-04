#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "ui_MainWindow.h"
#include <QMainWindow>
#include <QWidget>

// Namespace and class placeholders generated in ui_<classname>.h follow the naming conventions of your project.
namespace Ui {
    class MainWindow;
}

class MainWindow: public QMainWindow {
    Q_OBJECT // Macro of Qt, this macro is used for enable signal/slot, introspection, other macro,...

    private:
        Ui::MainWindow *ui; // Receive the ui pointer in ui_<classname>.h

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
};

#endif
