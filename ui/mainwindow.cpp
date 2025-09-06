#include "mainwindow.h"

/**
 * @brief Khởi tạo MainWindow và thiết lập các kết nối tín hiệu.
 * @param parent QWidget* cha của cửa sổ.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), settings("configs.ini", QSettings::IniFormat), readerThread(nullptr)
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
    connect(ui->goToYoutube, &QPushButton::clicked, this, [](){
        QUrl url("https://www.youtube.com/@truonghaiang7670");
        QDesktopServices::openUrl(url);
    });

    ui->input->setFocus();
    statusBar()->showMessage("Ready");
}

/**
 * @brief Giải phóng tài nguyên của MainWindow.
 */
MainWindow::~MainWindow()
{
    if (readerThread)
    {
        readerThread->stop();
        readerThread->wait();
        delete readerThread;
        readerThread = nullptr;
    }
    delete ui;
}

/**
 * @brief Khởi tạo các thành phần giao diện.
 */
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
    
    // Thiết lập QTextEdit để hiển thị HTML và menu chuột phải
    ui->dataTransRecv->setAcceptRichText(true);
    ui->dataTransRecv->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->dataTransRecv, &QWidget::customContextMenuRequested, this, &MainWindow::showDataTransRecvContextMenu);
}

/**
 * @brief Tải các thiết lập đã lưu.
 */
void MainWindow::loadSettings()
{
    this->comPort = settings.value("COM_PORT", "/dev/ttyUSB0").toString();
    this->baudrate = settings.value("Baudrate", 9600).toInt();
    this->parity = settings.value("Parity", 0).toInt();
    this->dataSize = settings.value("Data_Size", 8).toInt();
    this->inputType = settings.value("Input_Type", "Text").toString();
    this->newLine = settings.value("New_Line", "LF (\\n)").toString();

    // Sync UI with loaded settings
    int comIndex = ui->comPort->findText(this->comPort);
    if (comIndex >= 0)
        ui->comPort->setCurrentIndex(comIndex);

    int baudIndex = ui->baudrate->findText(QString::number(this->baudrate));
    if (baudIndex >= 0)
        ui->baudrate->setCurrentIndex(baudIndex);

    int parityIndex = ui->parityBit->findText(QString::number(this->parity));
    if (parityIndex >= 0)
        ui->parityBit->setCurrentIndex(parityIndex);

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

/**
 * @brief Khi chọn COM port mới.
 * @param index int chỉ số trong combo box.
 */
void MainWindow::onComportChanged(int /*index*/)
{
    QString currComPort = ui->comPort->currentText();
    settings.setValue("COM_PORT", currComPort);
    statusBar()->showMessage(QString("COM Port: %1").arg(currComPort), 2000);
}

/**
 * @brief Khi chọn baudrate mới.
 * @param index int chỉ số trong combo box.
 */
void MainWindow::onBaudrateChanged(int /*index*/)
{
    QString currBaud = ui->baudrate->currentText();
    int baudValue = currBaud.toInt();
    this->baudrate = baudValue;
    settings.setValue("Baudrate", baudValue);
    statusBar()->showMessage(QString("Baudrate: %1").arg(baudValue), 2000);
}

/**
 * @brief Khi chọn parity mới.
 * @param index int chỉ số trong combo box.
 */
void MainWindow::onParityChanged(int /*index*/)
{
    QString currParity = ui->parityBit->currentText();
    int parityValue = currParity.toInt();
    this->parity = parityValue;
    settings.setValue("Parity", parityValue);
    statusBar()->showMessage(QString("Parity: %1").arg(parityValue), 2000);
}

/**
 * @brief Khi chọn kích thước dữ liệu mới.
 * @param index int chỉ số trong combo box.
 */
void MainWindow::onDataSizeChanged(int /*index*/)
{
    QString currDataSize = ui->dataSize->currentText();
    int dataSizeValue = currDataSize.toInt();
    this->dataSize = dataSizeValue;
    settings.setValue("Data_Size", dataSizeValue);
    statusBar()->showMessage(QString("Data Size: %1").arg(dataSizeValue), 2000);
}

/**
 * @brief Bật chế độ nhập Text.
 * @param checked bool trạng thái lựa chọn.
 */
void MainWindow::onTextModeToggled(bool checked)
{
    if (!checked)
        return;
    this->inputType = "Text";
    settings.setValue("Input_Type", this->inputType);
    statusBar()->showMessage("Mode: Text", 2000);
}

/**
 * @brief Bật chế độ nhập Hex.
 * @param checked bool trạng thái lựa chọn.
 */
void MainWindow::onHexModeToggled(bool checked)
{
    if (!checked)
        return;
    this->inputType = "Hex";
    settings.setValue("Input_Type", this->inputType);
    statusBar()->showMessage("Mode: Hex", 2000);
}

/**
 * @brief Bật chế độ nhập Bin.
 * @param checked bool trạng thái lựa chọn.
 */
void MainWindow::onBinaryModeToggled(bool checked)
{
    if (!checked)
        return;
    this->inputType = "Bin";
    settings.setValue("Input_Type", this->inputType);
    statusBar()->showMessage("Mode: Bin", 2000);
}

/**
 * @brief Khi chọn ký tự xuống dòng mới.
 * @param index int chỉ số trong combo box.
 */
void MainWindow::onNewLineChanged(int /*index*/)
{
    QString text = ui->newLine->currentText();
    this->newLine = text;
    settings.setValue("New_Line", text);
    statusBar()->showMessage(QString("New Line: %1").arg(text), 2000);
    updateSerialReaderNewLine();
}

/**
 * @brief Gửi dữ liệu ra cổng serial và hiển thị lên màn hình.
 */
void MainWindow::sendData()
{
    QString newline;
    if (this->newLine.startsWith("LF"))
        newline = "\n";
    else if (this->newLine.startsWith("CRLF"))
        newline = "\r\n";
    else if (this->newLine.startsWith("CR"))
        newline = "\r";
    else
        newline = "";

    QString data = ui->input->text();
    QString sendStr = data + newline;
    if (readerThread)
        readerThread->writeData(sendStr.toUtf8());
    appendMessage(">>", data, "#FFFFFF");
    ui->input->setText("");
}

/**
 * @brief Xử lý tín hiệu nhận dữ liệu từ thread.
 * @param data const QString& dữ liệu nhận được.
 */
void MainWindow::handleReceivedData(const QString &data)
{
    appendMessage("<<", data, "#008000");
}

/**
 * @brief Hiển thị dữ liệu với thời gian và màu sắc.
 * @param prefix const QString& dấu phân cách (<< hoặc >>).
 * @param data const QString& nội dung dữ liệu.
 * @param color const QString& màu dạng mã HEX.
 */
void MainWindow::appendMessage(const QString &prefix, const QString &data, const QString &color)
{
    const QString timeStr = QDateTime::currentDateTime().toString("hh:mm:ss dd/MM/yyyy");
    const QString html = QString("<span style=\"color:%1;\">%2 %3 %4</span>")
        .arg(color,
             timeStr.toHtmlEscaped(),
             prefix.toHtmlEscaped(),   // << sẽ thành &lt;&lt;
             data.toHtmlEscaped());    // bảo vệ mọi dấu <, &, "
    ui->dataTransRecv->append(html);
    QTextCursor cursor = ui->dataTransRecv->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->dataTransRecv->setTextCursor(cursor);
    ui->dataTransRecv->ensureCursorVisible();
}

/**
 * @brief Hiển thị menu chuột phải cho vùng nhận dữ liệu.
 * @param pos const QPoint& vị trí con trỏ chuột.
 */
void MainWindow::showDataTransRecvContextMenu(const QPoint &pos)
{
    QMenu *menu = ui->dataTransRecv->createStandardContextMenu();
    menu->addSeparator();
    QAction *clearAction = menu->addAction("Clear");
    connect(clearAction, &QAction::triggered, this, &MainWindow::clearDataTransRecv);
    menu->exec(ui->dataTransRecv->mapToGlobal(pos));
    delete menu;
}

/**
 * @brief Xóa toàn bộ nội dung trong ô hiển thị dữ liệu.
 */
void MainWindow::clearDataTransRecv()
{
    ui->dataTransRecv->clear();
}

/**
 * @brief Kết nối hoặc ngắt kết nối với cổng serial.
 */
void MainWindow::connectOrDisconnect()
{
    if (readerThread)
    {
        // Disconnect
        readerThread->stop();
        readerThread->wait();
        serial.moveToThread(this->thread());
        delete readerThread;
        readerThread = nullptr;
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

            readerThread = new SerialReader(&serial, this);
            serial.moveToThread(readerThread);
            connect(readerThread, &SerialReader::dataReceived, this, &MainWindow::handleReceivedData, Qt::QueuedConnection);
            readerThread->start();
            updateSerialReaderNewLine();
        }
        else
        {
            statusBar()->showMessage(QString("Failed to connect: %1").arg(serial.errorString()), 3000);
        }
    }
}

/**
 * @brief Cập nhật newline cho SerialReader.
 */
void MainWindow::updateSerialReaderNewLine()
{
    if (readerThread)
    {
        QString newline;
        if (this->newLine.startsWith("LF"))
            newline = "\n";
        else if (this->newLine.startsWith("CRLF"))
            newline = "\r\n";
        else if (this->newLine.startsWith("CR"))
            newline = "\r";
        else
            newline = "\r\n"; // Mặc định
        
        // Cập nhật newline trực tiếp với bảo vệ mutex
        readerThread->setNewLine(newline);
    }
}
