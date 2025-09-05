#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QThread>
#include <QSerialPort>
#include <QString>
#include <QMutex>

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
};

#endif // SERIALREADER_H