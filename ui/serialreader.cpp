#include "serialreader.h"
#include <QByteArray>

/**
 * @brief Tạo mới đối tượng SerialReader.
 * @param port QSerialPort* cổng serial cần lắng nghe.
 * @param parent QObject* đối tượng cha.
 */
SerialReader::SerialReader(QSerialPort *port, QObject *parent)
    : QThread(parent), m_port(port), m_running(false) {}

/**
 * @brief Yêu cầu dừng thread.
 */
void SerialReader::stop()
{
    m_running = false;
}

/**
 * @brief Hàm thực thi chính của thread, đọc dữ liệu liên tục.
 */
void SerialReader::run()
{
    m_running = true;
    while (m_running)
    {
        if (!m_port || !m_port->isOpen())
        {
            msleep(100);
            continue;
        }
        if (m_port->waitForReadyRead(100))
        {
            QByteArray bytes = m_port->readAll();
            if (!bytes.isEmpty())
            {
                QString data = QString::fromUtf8(bytes);
                emit dataReceived(data);
            }
        }
    }
}
