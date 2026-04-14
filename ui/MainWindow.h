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

    QComboBox *m_portCombo = nullptr;
    QComboBox *m_baudCombo = nullptr;
    QComboBox *m_dataBitsCombo = nullptr;
    QComboBox *m_parityCombo = nullptr;
    QComboBox *m_handshakeCombo = nullptr;
    QComboBox *m_modeCombo = nullptr;
    QPushButton *m_openButton = nullptr;
    QCheckBox *m_dtrCheck = nullptr;
    QCheckBox *m_rtsCheck = nullptr;
    QGroupBox *m_sendGroup = nullptr;

    QWidget *createSerialPanel();
    QWidget *createModemLinesPanel();
    QWidget *createSendPanel();
    QWidget *createIndicator(const QString &text, const QColor &color);
    QGroupBox *createSendRow(const QString &placeholder);
    void appendLogMessage(const QString &message);
    void updateConnectionControls();
    void syncSerialConfigFromUi();
    SerialConfig buildSerialConfigFromUi() const;
    void connectToDevice();

    QTextEdit *m_receiveView = nullptr;
};
