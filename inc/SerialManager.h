#pragma once

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <QByteArray>
#include <QList>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

struct SerialConfig {
  QString portName;
  qint32 baudRate = QSerialPort::Baud115200;
  QSerialPort::DataBits dataBits = QSerialPort::Data8;
  QSerialPort::Parity parity = QSerialPort::NoParity;
  QSerialPort::StopBits stopBits = QSerialPort::OneStop;
  QSerialPort::FlowControl flowControl = QSerialPort::NoFlowControl;
};

class SerialManager {
    private:
        QSerialPort m_serial;
        SerialConfig m_config;

    public:
        SerialManager();
        ~SerialManager();

        QList<QSerialPortInfo> getAvailablePorts();

        bool connectPort();
        bool connectPort(const QString &portName);
        bool connectPort(const SerialConfig &config);

        void disconnectPort();
        bool isConnected() const;

        qint64 sendText(const QString &text);
        qint64 sendHex(const QString &hexText);
        qint64 sendBytes(const QByteArray &data);

        bool applyConfig(const SerialConfig &config);
        SerialConfig getConfig() const;
};

#endif
