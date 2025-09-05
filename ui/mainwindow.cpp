#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), settings("configs.ini", QSettings::IniFormat)
{
    ui->setupUi(this);
    this->initializeInterface();
    this->loadSettings();

    connect(ui->comPort, &QComboBox::currentIndexChanged, this, &MainWindow::onComportChanged);
    connect(ui->baudrate, &QComboBox::currentIndexChanged, this, &MainWindow::onBaudrateChanged);
    connect(ui->parityBit, &QComboBox::currentIndexChanged, this, &MainWindow::onParityChanged);
    connect(ui->dataSize, &QComboBox::currentIndexChanged, this, &MainWindow::onDataSizeChanged);
    connect(ui->textMode, &QRadioButton::toggled, this, &MainWindow::onTextModeToggled);
    connect(ui->hexMode, &QRadioButton::toggled, this, &MainWindow::onHexModeToggled);
    connect(ui->binaryMode, &QRadioButton::toggled, this, &MainWindow::onBinaryModeToggled);
    connect(ui->newLine, &QComboBox::currentIndexChanged, this, &MainWindow::onNewLineChanged);
    connect(ui->input, &QLineEdit::returnPressed, this, &MainWindow::sendData);
    connect(ui->connDisconnBtn, &QPushButton::clicked, this, &MainWindow::connectOrDisconnect);

    ui->input->setFocus();
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeInterface()
{
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto &p : ports)
        if (p.portName().contains("USB"))
            ui->comPort->addItem(p.systemLocation());

    QList<QString> popularBaudrate = {
        "110",
        "300",
        "600",
        "1200",
        "2400",
        "4800",
        "9600",
        "14400",
        "19200",
        "38400",
        "57600",
        "115200",
        "128000",
        "256000",
        "460800",
        "921600"};
    ui->baudrate->addItems(popularBaudrate);

    QList<QString> popularParityBit = {"0", "1"};
    ui->parityBit->addItems(popularParityBit);

    QList<QString> popularDataSize = {"5", "6", "7", "8"};
    ui->dataSize->addItems(popularDataSize);

    QList<QString> popularNewLines = {"LF (\\n)", "CRLF (\\r\\n)", "CR (\\r)", "None"};
    ui->newLine->addItems(popularNewLines);
}

void MainWindow::loadSettings()
{
    this->comPort = settings.value("COM_PORT", "/dev/ttyUSB0").toString();
    this->baudrate = settings.value("Baudrate", 9600).toInt();
    this->parity = settings.value("Parity", 0).toInt();
    this->dataSize = settings.value("Data_Size", 8).toInt();
    this->inputType = settings.value("Input_Type", "Text").toString();
    this->newLine = settings.value("New_Line", "LF (\\n)").toString();

    // Sync UI with loaded settings
    // comPort: exact text match
    int comIndex = ui->comPort->findText(this->comPort);
    if (comIndex >= 0)
        ui->comPort->setCurrentIndex(comIndex);

    // baudrate: compare as string
    int baudIndex = ui->baudrate->findText(QString::number(this->baudrate));
    if (baudIndex >= 0)
        ui->baudrate->setCurrentIndex(baudIndex);

    // parityBit: compare as string ("0" or "1")
    int parityIndex = ui->parityBit->findText(QString::number(this->parity));
    if (parityIndex >= 0)
        ui->parityBit->setCurrentIndex(parityIndex);

    // dataSize: compare as string (e.g., "8", "9")
    int dataSizeIndex = ui->dataSize->findText(QString::number(this->dataSize));
    if (dataSizeIndex >= 0)
        ui->dataSize->setCurrentIndex(dataSizeIndex);

    int newLineIndex = ui->newLine->findText(this->newLine);
    if (newLineIndex >= 0)
        ui->newLine->setCurrentIndex(newLineIndex);

    if (this->inputType == "Text")
    {
        ui->textMode->setChecked(true);
    }
    else if (this->inputType == "Hex")
    {
        ui->hexMode->setChecked(true);
    }
    else if (this->inputType == "Bin")
    {
        ui->binaryMode->setChecked(true);
    }
    else
    {
        ui->textMode->setChecked(true);
    }

    statusBar()->showMessage(QString("COM: %1 | Baud: %2 | Parity: %3 | Data: %4 | Mode: %5")
                                 .arg(this->comPort)
                                 .arg(this->baudrate)
                                 .arg(this->parity)
                                 .arg(this->dataSize)
                                 .arg(this->inputType),
                             3000);
}

void MainWindow::onComportChanged(int index)
{
    QString currComPort = ui->comPort->currentText();
    settings.setValue("COM_PORT", currComPort);
    statusBar()->showMessage(QString("COM Port: %1").arg(currComPort), 2000);
}

void MainWindow::onBaudrateChanged(int index)
{
    QString currBaud = ui->baudrate->currentText();
    int baudValue = currBaud.toInt();
    this->baudrate = baudValue;
    settings.setValue("Baudrate", baudValue);
    statusBar()->showMessage(QString("Baudrate: %1").arg(baudValue), 2000);
}

void MainWindow::onParityChanged(int index)
{
    QString currParity = ui->parityBit->currentText();
    int parityValue = currParity.toInt();
    this->parity = parityValue;
    settings.setValue("Parity", parityValue);
    statusBar()->showMessage(QString("Parity: %1").arg(parityValue), 2000);
}

void MainWindow::onDataSizeChanged(int index)
{
    QString currDataSize = ui->dataSize->currentText();
    int dataSizeValue = currDataSize.toInt();
    this->dataSize = dataSizeValue;
    settings.setValue("Data_Size", dataSizeValue);
    statusBar()->showMessage(QString("Data Size: %1").arg(dataSizeValue), 2000);
}

void MainWindow::onTextModeToggled(bool checked)
{
    if (!checked)
        return;
    this->inputType = "Text";
    settings.setValue("Input_Type", this->inputType);
    statusBar()->showMessage("Mode: Text", 2000);
}

void MainWindow::onHexModeToggled(bool checked)
{
    if (!checked)
        return;
    this->inputType = "Hex";
    settings.setValue("Input_Type", this->inputType);
    statusBar()->showMessage("Mode: Hex", 2000);
}

void MainWindow::onBinaryModeToggled(bool checked)
{
    if (!checked)
        return;
    this->inputType = "Bin";
    settings.setValue("Input_Type", this->inputType);
    statusBar()->showMessage("Mode: Bin", 2000);
}

void MainWindow::onNewLineChanged(int index)
{
    QString text = ui->newLine->currentText();
    this->newLine = text;
    settings.setValue("New_Line", text);
    statusBar()->showMessage(QString("New Line: %1").arg(text), 2000);
}

void MainWindow::sendData()
{
    QString data = ui->input->text();
    data += this->newLine;
    ui->dataTransRecv->append(data);
    ui->input->setText("");
}

void MainWindow::connectOrDisconnect()
{
    if (serial.isOpen())
    {
        // Disconnect
        serial.close();
        ui->connDisconnBtn->setText("Connect");
        ui->comPort->setEnabled(true);
        ui->baudrate->setEnabled(true);
        ui->parityBit->setEnabled(true);
        ui->dataSize->setEnabled(true);
        ui->input->setEnabled(false);
        statusBar()->showMessage("Disconnected", 2000);
    }
    else
    {
        // Connect
        serial.setPortName(ui->comPort->currentText());
        serial.setBaudRate(ui->baudrate->currentText().toInt());
        serial.setParity(static_cast<QSerialPort::Parity>(ui->parityBit->currentText().toInt()));
        serial.setDataBits(static_cast<QSerialPort::DataBits>(ui->dataSize->currentText().toInt()));
        serial.setStopBits(QSerialPort::OneStop);
        serial.setFlowControl(QSerialPort::NoFlowControl);

        if (serial.open(QIODevice::ReadWrite))
        {
            ui->connDisconnBtn->setText("Disconnect");
            ui->comPort->setEnabled(false);
            ui->baudrate->setEnabled(false);
            ui->parityBit->setEnabled(false);
            ui->dataSize->setEnabled(false);
            ui->input->setEnabled(true);
            statusBar()->showMessage(QString("Connected to %1").arg(serial.portName()), 2000);
        }
        else
        {
            statusBar()->showMessage(QString("Failed to connect: %1").arg(serial.errorString()), 3000);
        }
    }
}
