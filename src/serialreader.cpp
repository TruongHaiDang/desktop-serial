#include "../inc/serialreader.h"
#include <QDebug>
#include <QMutexLocker>

SerialReader::SerialReader(QSerialPort *serial, QObject *parent)
    : QThread(parent), m_serial(serial), m_stop(false)
{
    // Thiết lập newline mặc định
    m_newLine = "\r\n";
}

void SerialReader::stop()
{
    m_stop = true;
}

void SerialReader::setNewLine(const QString &newLine)
{
    QMutexLocker locker(&m_mutex);
    m_newLine = newLine;
}

void SerialReader::writeData(const QByteArray &data)
{
    QMutexLocker locker(&m_writeMutex);
    m_writeQueue.enqueue(data);
}

void SerialReader::run()
{
    while (!m_stop && m_serial && m_serial->isOpen())
    {
        // Ghi dữ liệu nếu có trong hàng đợi
        {
            QMutexLocker locker(&m_writeMutex);
            while (!m_writeQueue.isEmpty())
            {
                m_serial->write(m_writeQueue.dequeue());
            }
        }

        if (m_serial->waitForReadyRead(100)) // Chờ 100ms
        {
            QByteArray data = m_serial->readAll();
            QString text = QString::fromUtf8(data);

            // Thêm vào buffer
            m_buffer += text;

            // Tách các dòng hoàn chỉnh
            QString currentNewLine;
            {
                QMutexLocker locker(&m_mutex);
                currentNewLine = m_newLine;
            }

            while (m_buffer.contains(currentNewLine))
            {
                int pos = m_buffer.indexOf(currentNewLine);
                QString line = m_buffer.left(pos);
                m_buffer = m_buffer.mid(pos + currentNewLine.length());

                // Chỉ emit nếu dòng không rỗng
                if (!line.isEmpty())
                {
                    emit dataReceived(line);
                }
            }
        }
    }

    if (m_serial && m_serial->isOpen())
    {
        m_serial->close();
    }
}