#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include "ui_MainWindow.h"
#include <QMainWindow>
#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <string>
#include <QString>
#include <QSerialPortInfo>
#include <QList>

// Namespace and class placeholders generated in ui_<classname>.h follow the naming conventions of your project.
namespace Ui {
    class MainWindow;
}

class MainWindow: public QMainWindow {
    Q_OBJECT // Macro of Qt, this macro is used for enable signal/slot, introspection, other macro,...

    private:
        Ui::MainWindow *ui; // Receive the ui pointer in ui_<classname>.h
        QSettings settings;
        
        QString comPort;
        int baudrate;
        int parity;
        int dataSize;
        QString inputType;
        QString newLine;

        void onComportChanged(int index);
        void onBaudrateChanged(int index);
        void onParityChanged(int index);
        void onDataSizeChanged(int index);
        void onTextModeToggled(bool checked);
        void onHexModeToggled(bool checked);
        void onBinaryModeToggled(bool checked);
        void onNewLineChanged(int index);

        void initializeInterface();
        void loadSettings();
        void sendData();

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
};

#endif
