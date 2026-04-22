#include "MainWindow.h"
#include "config.h"
#include <QDateTime>
#include <qdebug.h>
#include <qhashfunctions.h>
#include <qlist.h>
#include <qpushbutton.h>
#include <qserialportinfo.h>

namespace
{
QComboBox *createComboBox(const QStringList &items)
{
    auto *combo = new QComboBox;
    combo->addItems(items);
    combo->setMinimumHeight(28);
    return combo;
}

QFrame *createSeparator()
{
    auto *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    return line;
}

QString formatReceivedData(const QByteArray &data)
{
    bool hasBinaryControl = false;
    for (const char rawByte : data) {
        const auto byte = static_cast<unsigned char>(rawByte);
        const bool isAllowedWhitespace = byte == '\r' || byte == '\n' || byte == '\t';
        if ((byte < 0x20 && !isAllowedWhitespace) || byte == 0x7f) {
            hasBinaryControl = true;
            break;
        }
    }

    if (hasBinaryControl) {
        return QString("HEX: %1").arg(QString::fromLatin1(data.toHex(' ').toUpper()));
    }

    QString text = QString::fromUtf8(data);
    text.replace("\r", "\\r");
    text.replace("\n", "\\n");
    text.replace("\t", "\\t");
    return text;
}
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral(APP_NAME " v" APP_VERSION));
    resize(840, 900);
    setMinimumSize(840, 900);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setContentsMargins(4, 4, 4, 4);
    rootLayout->setSpacing(6);

    auto *serialRoot = new QWidget;
    rootLayout->addWidget(serialRoot);

    auto *serialLayout = new QVBoxLayout(serialRoot);
    serialLayout->setContentsMargins(0, 0, 0, 0);
    serialLayout->setSpacing(6);

    auto *topRow = new QHBoxLayout;
    topRow->setSpacing(8);

    auto *receiveGroup = new QGroupBox("Received/Sent data");
    auto *receiveLayout = new QVBoxLayout(receiveGroup);
    receiveLayout->setContentsMargins(4, 6, 4, 4);

    m_receiveView = new QTextEdit;
    m_receiveView->setReadOnly(true);
    m_receiveView->setMinimumSize(470, 310);
    m_receiveView->setContextMenuPolicy(Qt::CustomContextMenu);
    QPalette receivePalette = m_receiveView->palette();
    receivePalette.setColor(QPalette::Base, Qt::white);
    receivePalette.setColor(QPalette::Text, Qt::black);
    m_receiveView->setPalette(receivePalette);
    receiveLayout->addWidget(m_receiveView);
    topRow->addWidget(receiveGroup, 1);
    connect(m_receiveView, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu menu(this);
    
        QAction *copyAct = menu.addAction("Copy");
        copyAct->setShortcut(QKeySequence::Copy);
    
        QAction *selectAllAct = menu.addAction("Select All");
        selectAllAct->setShortcut(QKeySequence::SelectAll);
    
        menu.addSeparator();
    
        QAction *clearAct = menu.addAction("Clear");
    
        QAction *selected = menu.exec(m_receiveView->mapToGlobal(pos));
    
        if (selected == copyAct) {
            m_receiveView->copy();
        } else if (selected == selectAllAct) {
            m_receiveView->selectAll();
        } else if (selected == clearAct) {
            m_receiveView->clear();
        }
    });

    topRow->addWidget(createSerialPanel());
    serialLayout->addLayout(topRow, 1);
    serialLayout->addWidget(createModemLinesPanel());
    serialLayout->addWidget(createSendPanel());

    rootLayout->setStretchFactor(serialRoot, 1);

    m_serial.setReceiveCallback([this](const QByteArray &data) {
        handleSerialDataReceived(data);
    });
}

QWidget *MainWindow::createSerialPanel()
{
    auto *panel = new QWidget;
    panel->setFixedWidth(165);

    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    auto *serialGroup = new QGroupBox("Serial");
    auto *serialLayout = new QVBoxLayout(serialGroup);
    serialLayout->setContentsMargins(8, 12, 8, 8);
    serialLayout->setSpacing(6);

    auto addLabeledCombo = [this, serialLayout](const QString &labelText,
                                                const QStringList &items,
                                                QComboBox *&targetCombo) {
        auto *label = new QLabel(labelText);
        targetCombo = createComboBox(items);
        serialLayout->addWidget(label);
        serialLayout->addWidget(targetCombo);

        connect(targetCombo,
                &QComboBox::currentIndexChanged,
                this,
                [this](int) { syncSerialConfigFromUi(); });
    };

    // Discover serial ports
    QList<QString> portNames = {};
    QList<QSerialPortInfo> portList = this->m_serial.getAvailablePorts();
    for (const auto &port : portList) {
        if (port.portName().contains("COM") || port.portName().contains("USB"))
            portNames.append(port.portName());
        // qDebug() << "name:" << port.portName()
        //          << "desc:" << port.description()
        //          << "manufacturer:" << port.manufacturer();
    };
    addLabeledCombo("Name", portNames, m_portCombo);
    addLabeledCombo("Baud", {
        "300",
        "600",
        "1200",
        "2400",
        "4800",
        "9600",
        "14400",
        "19200",
        "28800",
        "38400",
        "56000",
        "57600",
        "76800",
        "115200",
        "128000",
        "153600",
        "230400",
        "256000",
        "460800",
        "921600"
    }, m_baudCombo);
    m_baudCombo->setCurrentText("9600");
    addLabeledCombo("Data size", {"8", "7", "6", "5"}, m_dataBitsCombo);
    addLabeledCombo("Parity", {"none", "even", "odd", "mark", "space"}, m_parityCombo);
    addLabeledCombo("Handshake", {"OFF", "RTS/CTS", "XON/XOFF"}, m_handshakeCombo);
    addLabeledCombo("Mode", {"Free", "RS485", "Loopback"}, m_modeCombo);

    serialLayout->addSpacing(8);

    m_openButton = new QPushButton("Open");
    m_openButton->setMinimumHeight(30);
    serialLayout->addWidget(m_openButton);
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::connectToDevice);

    auto *logoFrame = new QFrame;
    logoFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    auto *logoLayout = new QVBoxLayout(logoFrame);
    logoLayout->setContentsMargins(8, 8, 8, 8);
    logoLayout->setSpacing(2);

    auto *brand = new QLabel("Hải Đăng");
    QFont brandFont = brand->font();
    brandFont.setPointSize(24);
    brandFont.setBold(true);
    brand->setFont(brandFont);
    brand->setAlignment(Qt::AlignCenter);

    auto *site = new QLabel("truonghaidang.com");
    site->setAlignment(Qt::AlignCenter);

    auto *product = new QLabel("Desktop Serial");
    product->setAlignment(Qt::AlignCenter);

    auto *version = new QLabel("Version 1.0.0");
    QFont versionFont = version->font();
    versionFont.setBold(true);
    version->setFont(versionFont);
    version->setAlignment(Qt::AlignCenter);

    logoLayout->addWidget(brand);
    logoLayout->addWidget(createSeparator());
    logoLayout->addWidget(site);
    logoLayout->addSpacing(4);
    logoLayout->addWidget(product);
    logoLayout->addWidget(version);

    serialLayout->addStretch(1);
    layout->addWidget(serialGroup, 1);
    layout->addWidget(logoFrame);

    syncSerialConfigFromUi();

    return panel;
}

QWidget *MainWindow::createModemLinesPanel()
{
    auto *group = new QGroupBox("Modem lines");
    auto *layout = new QHBoxLayout(group);
    layout->setContentsMargins(10, 8, 10, 10);
    layout->setSpacing(14);
    layout->addStretch();
    layout->addWidget(createIndicator("CD", QColor(0, 168, 76)));
    layout->addWidget(createIndicator("RI", QColor(0, 168, 76)));
    layout->addWidget(createIndicator("DSR", QColor(0, 168, 76)));
    layout->addWidget(createIndicator("CTS", QColor(0, 168, 76)));

    m_dtrCheck = new QCheckBox("DTR");
    m_rtsCheck = new QCheckBox("RTS");
    m_dtrCheck->setEnabled(false);
    m_rtsCheck->setEnabled(false);
    layout->addWidget(m_dtrCheck);
    layout->addWidget(m_rtsCheck);
    layout->addStretch();
    return group;
}

QWidget *MainWindow::createSendPanel()
{
    m_sendGroup = new QGroupBox("Send");
    auto *layout = new QVBoxLayout(m_sendGroup);
    layout->setContentsMargins(8, 10, 8, 8);
    layout->setSpacing(8);

    layout->addWidget(createSendRow(""));
    layout->addWidget(createSendRow(""));
    layout->addWidget(createSendRow(""));

    m_sendGroup->setEnabled(false);
    return m_sendGroup;
}

QWidget *MainWindow::createIndicator(const QString &text, const QColor &color)
{
    auto *widget = new QWidget;
    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    auto *led = new QLabel;
    led->setFixedSize(14, 14);
    led->setStyleSheet(QString(
        "background-color: %1;"
        "border: 1px solid black;"
        "border-radius: 7px;")
                           .arg(color.name()));

    auto *label = new QLabel(text);
    layout->addWidget(led);
    layout->addWidget(label);
    return widget;
}

QGroupBox *MainWindow::createSendRow(const QString &placeholder)
{
    auto *row = new QGroupBox;
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(4, 8, 4, 4);
    layout->setSpacing(8);

    auto *lineEdit = new QLineEdit;
    lineEdit->setPlaceholderText(placeholder);

    auto *hexCheck = new QCheckBox("HEX");

    auto *sendButton = new QPushButton("Send");

    layout->addWidget(lineEdit, 1);
    layout->addWidget(hexCheck);
    layout->addWidget(sendButton);

    connect(sendButton, &QPushButton::clicked, this, [this, lineEdit, hexCheck](){
        const QString rawText = lineEdit->text();
        qint64 written = -1;
    
        if (hexCheck->isChecked()) {
            written = this->m_serial.sendHex(rawText);
        } else {
            written = this->m_serial.sendText(rawText + "\n");
        }
    
        QString visible = rawText;
        visible.replace("\r", "\\r");
        visible.replace("\n", "\\n");
    
        if (written < 0) {
            appendLogMessage(QString("TX failed | HEX=%1 | data=%2")
                .arg(hexCheck->isChecked() ? "true" : "false")
                .arg(visible));
            return;
        }
    
        appendLogMessage(QString("TX %1 bytes | HEX=%2 | data=%3")
            .arg(written)
            .arg(hexCheck->isChecked() ? "true" : "false")
            .arg(visible));
    });

    return row;
}

void MainWindow::connectToDevice()
{
    const QString portName = m_serial.getConfig().portName.trimmed();
    const QString portLabel = portName.isEmpty() ? "serial port" : portName;

    if (m_serial.isConnected()) {
        flushPendingSerialData();
        m_serial.disconnectPort();
        updateConnectionControls();
        appendLogMessage(QString("Disconnected from %1").arg(portLabel));
        if (m_openButton != nullptr) {
            m_openButton->setText("Open");
        }
        return;
    }

    if (m_serial.connectPort()) {
        m_receiveBuffer.clear();
        updateConnectionControls();
        appendLogMessage(QString("Connected to %1").arg(portLabel));
        if (m_openButton != nullptr) {
            m_openButton->setText("Close");
        }
        return;
    }

    updateConnectionControls();
    appendLogMessage(QString("Failed to connect to %1").arg(portLabel));
}

void MainWindow::appendLogMessage(const QString &message)
{
    if (m_receiveView == nullptr) {
        return;
    }

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    m_receiveView->append(QString("[%1] %2").arg(timestamp, message));
}

void MainWindow::handleSerialDataReceived(const QByteArray &data)
{
    m_receiveBuffer.append(data);

    qsizetype newlineIndex = m_receiveBuffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = m_receiveBuffer.left(newlineIndex + 1);
        m_receiveBuffer.remove(0, newlineIndex + 1);
        appendReceivedDataLog(line);
        newlineIndex = m_receiveBuffer.indexOf('\n');
    }
}

void MainWindow::appendReceivedDataLog(const QByteArray &data, bool partial)
{
    const QString prefix = partial ? "RX partial" : "RX";
    appendLogMessage(QString("%1 (%2 bytes): %3")
                         .arg(prefix)
                         .arg(data.size())
                         .arg(formatReceivedData(data)));
}

void MainWindow::flushPendingSerialData()
{
    if (m_receiveBuffer.isEmpty()) {
        return;
    }

    appendReceivedDataLog(m_receiveBuffer, true);
    m_receiveBuffer.clear();
}

void MainWindow::updateConnectionControls()
{
    const bool isConnected = m_serial.isConnected();

    if (m_dtrCheck != nullptr) {
        m_dtrCheck->setEnabled(isConnected);
    }

    if (m_rtsCheck != nullptr) {
        m_rtsCheck->setEnabled(isConnected);
    }

    if (m_sendGroup != nullptr) {
        m_sendGroup->setEnabled(isConnected);
    }
}

SerialConfig MainWindow::buildSerialConfigFromUi() const
{
    SerialConfig config = m_serial.getConfig();

    if (m_portCombo != nullptr) {
        config.portName = m_portCombo->currentText().trimmed();
    }

    if (m_baudCombo != nullptr) {
        config.baudRate = m_baudCombo->currentText().toInt();
    }

    if (m_dataBitsCombo != nullptr) {
        const int dataBits = m_dataBitsCombo->currentText().toInt();
        switch (dataBits) {
        case 5:
            config.dataBits = QSerialPort::Data5;
            break;
        case 6:
            config.dataBits = QSerialPort::Data6;
            break;
        case 7:
            config.dataBits = QSerialPort::Data7;
            break;
        case 8:
        default:
            config.dataBits = QSerialPort::Data8;
            break;
        }
    }

    if (m_parityCombo != nullptr) {
        const QString parity = m_parityCombo->currentText().trimmed().toLower();
        if (parity == "even") {
            config.parity = QSerialPort::EvenParity;
        } else if (parity == "odd") {
            config.parity = QSerialPort::OddParity;
        } else if (parity == "mark") {
            config.parity = QSerialPort::MarkParity;
        } else if (parity == "space") {
            config.parity = QSerialPort::SpaceParity;
        } else {
            config.parity = QSerialPort::NoParity;
        }
    }

    if (m_handshakeCombo != nullptr) {
        const QString flowControl = m_handshakeCombo->currentText().trimmed().toUpper();
        if (flowControl == "RTS/CTS") {
            config.flowControl = QSerialPort::HardwareControl;
        } else if (flowControl == "XON/XOFF") {
            config.flowControl = QSerialPort::SoftwareControl;
        } else {
            config.flowControl = QSerialPort::NoFlowControl;
        }
    }

    return config;
}

void MainWindow::syncSerialConfigFromUi()
{
    m_serial.applyConfig(buildSerialConfigFromUi());
}
