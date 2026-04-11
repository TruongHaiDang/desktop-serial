#pragma once

#include <QtGui/QColor>
#include <QtWidgets/QMainWindow>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QFont>
#include <QtGui/QPalette>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QDebug>

#include "SerialManager.h"
#include "AppSettings.h"

class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;
class QWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    SerialManager m_serial;
    AppSettings m_appSettings;

    QWidget *createSerialPanel();
    QWidget *createModemLinesPanel();
    QWidget *createSendPanel();
    QWidget *createIndicator(const QString &text, const QColor &color);
    QGroupBox *createSendRow(const QString &placeholder);

    QTextEdit *m_receiveView = nullptr;
};
