#include "SerialManager.h"

#include <QIODevice>
#include <QObject>

#include <utility>

namespace
{
QByteArray normalizeHexString(const QString &hexText)
{
    QByteArray normalized;
    normalized.reserve(hexText.size());

    for (int i = 0; i < hexText.size(); ++i) {
        const QChar ch = hexText.at(i);

        if (ch.isSpace()) {
            continue;
        }

        if (ch == '0' && i + 1 < hexText.size()) {
            const QChar next = hexText.at(i + 1);
            if (next == 'x' || next == 'X') {
                ++i;
                continue;
            }
        }

        normalized.append(ch.toLatin1());
    }

    return normalized;
}
} // namespace

SerialManager::SerialManager()
{
    QObject::connect(&m_serial, &QSerialPort::readyRead, [this]() {
        handleReadyRead();
    });
}

SerialManager::~SerialManager()
{
    disconnectPort();
}

QList<QSerialPortInfo> SerialManager::getAvailablePorts()
{
    return QSerialPortInfo::availablePorts();
}

bool SerialManager::connectPort()
{
    return connectPort(m_config);
}

bool SerialManager::connectPort(const QString &portName)
{
    SerialConfig config = m_config;
    config.portName = portName;
    return connectPort(config);
}

bool SerialManager::connectPort(const SerialConfig &config)
{
    if (config.portName.trimmed().isEmpty()) {
        return false;
    }

    disconnectPort();
    m_serial.setPortName(config.portName.trimmed());

    if (!applyConfig(config)) {
        return false;
    }

    return m_serial.open(QIODevice::ReadWrite);
}

void SerialManager::disconnectPort()
{
    if (m_serial.isOpen()) {
        m_serial.close();
    }
}

bool SerialManager::isConnected() const
{
    return m_serial.isOpen();
}

qint64 SerialManager::sendText(const QString &text)
{
    return sendBytes(text.toUtf8());
}

qint64 SerialManager::sendHex(const QString &hexText)
{
    const QByteArray normalized = normalizeHexString(hexText);
    if (normalized.isEmpty() && !hexText.trimmed().isEmpty()) {
        return -1;
    }

    if (normalized.size() % 2 != 0) {
        return -1;
    }

    return sendBytes(QByteArray::fromHex(normalized));
}

qint64 SerialManager::sendBytes(const QByteArray &data)
{
    if (!isConnected()) {
        return -1;
    }

    return m_serial.write(data);
}

void SerialManager::setReceiveCallback(ReceiveCallback callback)
{
    m_receiveCallback = std::move(callback);
}

void SerialManager::handleReadyRead()
{
    const QByteArray data = m_serial.readAll();
    if (data.isEmpty() || !m_receiveCallback) {
        return;
    }

    m_receiveCallback(data);
}

bool SerialManager::applyConfig(const SerialConfig &config)
{
    m_config = config;

    if (!config.portName.trimmed().isEmpty()) {
        m_serial.setPortName(config.portName.trimmed());
    }

    return m_serial.setBaudRate(config.baudRate)
        && m_serial.setDataBits(config.dataBits)
        && m_serial.setParity(config.parity)
        && m_serial.setStopBits(config.stopBits)
        && m_serial.setFlowControl(config.flowControl);
}

SerialConfig SerialManager::getConfig() const
{
    return m_config;
}
