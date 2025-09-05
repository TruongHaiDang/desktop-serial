#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QThread>
#include <QSerialPort>
#include <QString>
#include <QMutex>
#include <QByteArray>
#include <QQueue>

/**
 * @brief Thread đọc dữ liệu từ serial port.
 */
class SerialReader : public QThread
{
    Q_OBJECT

public:
    explicit SerialReader(QSerialPort *serial, QObject *parent = nullptr);
    void stop();

public slots:
    void setNewLine(const QString &newLine);

    /**
     * @brief Thêm dữ liệu cần ghi vào hàng đợi.
     * @param data const QByteArray& dữ liệu cần gửi.
     */
    void writeData(const QByteArray &data);

signals:
    void dataReceived(const QString &data);

protected:
    void run() override;

private:
    QSerialPort *m_serial;
    bool m_stop;
    QString m_buffer;
    QString m_newLine;
    QMutex m_mutex;
    QQueue<QByteArray> m_writeQueue;
    QMutex m_writeMutex;
};

#endif // SERIALREADER_H