#include "MainWindow.h"

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
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Desktop Serial");
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
    QPalette receivePalette = m_receiveView->palette();
    receivePalette.setColor(QPalette::Base, Qt::white);
    m_receiveView->setPalette(receivePalette);
    receiveLayout->addWidget(m_receiveView);
    topRow->addWidget(receiveGroup, 1);

    topRow->addWidget(createSerialPanel());
    serialLayout->addLayout(topRow, 1);
    serialLayout->addWidget(createModemLinesPanel());
    serialLayout->addWidget(createSendPanel());

    rootLayout->setStretchFactor(serialRoot, 1);
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

    auto addLabeledCombo = [serialLayout](const QString &labelText, const QStringList &items) {
        auto *label = new QLabel(labelText);
        auto *combo = createComboBox(items);
        serialLayout->addWidget(label);
        serialLayout->addWidget(combo);
    };

    addLabeledCombo("Name", {""});
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
    });
    addLabeledCombo("Data size", {"8", "7", "6", "5"});
    addLabeledCombo("Parity", {"none", "even", "odd", "mark", "space"});
    addLabeledCombo("Handshake", {"OFF", "RTS/CTS", "XON/XOFF"});
    addLabeledCombo("Mode", {"Free", "RS485", "Loopback"});

    serialLayout->addSpacing(8);

    auto *openButton = new QPushButton("Open");
    openButton->setMinimumHeight(30);
    serialLayout->addWidget(openButton);

    auto *updateButton = new QPushButton("HWg FW update");
    updateButton->setMinimumHeight(30);
    serialLayout->addWidget(updateButton);

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

    auto *dtr = new QCheckBox("DTR");
    auto *rts = new QCheckBox("RTS");
    dtr->setEnabled(false);
    rts->setEnabled(false);
    layout->addWidget(dtr);
    layout->addWidget(rts);
    layout->addStretch();
    return group;
}

QWidget *MainWindow::createSendPanel()
{
    auto *group = new QGroupBox("Send");
    auto *layout = new QVBoxLayout(group);
    layout->setContentsMargins(8, 10, 8, 8);
    layout->setSpacing(8);

    layout->addWidget(createSendRow(""));
    layout->addWidget(createSendRow(""));
    layout->addWidget(createSendRow(""));

    return group;
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
    sendButton->setEnabled(false);

    layout->addWidget(lineEdit, 1);
    layout->addWidget(hexCheck);
    layout->addWidget(sendButton);
    return row;
}
