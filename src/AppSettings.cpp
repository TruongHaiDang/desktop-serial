#include "AppSettings.h"

namespace
{
const auto kOrganization = "truonghaidang.com";
const auto kApplication = "desktop_serial";

const auto kSerialPortNameKey = "serial/portName";
const auto kSerialBaudRateKey = "serial/baudRate";
const auto kSerialDataBitsKey = "serial/dataBits";
const auto kSerialParityKey = "serial/parity";
const auto kSerialStopBitsKey = "serial/stopBits";
} // namespace

AppSettings::AppSettings()
    : m_settings(QSettings::IniFormat, QSettings::UserScope, kOrganization, kApplication)
{
    setDefaultValues();
}

AppSettings::~AppSettings() = default;

void AppSettings::setDefaultValues()
{
    if (!m_settings.contains(kSerialPortNameKey)) {
        m_settings.setValue(kSerialPortNameKey, QString());
    }

    if (!m_settings.contains(kSerialBaudRateKey)) {
        m_settings.setValue(kSerialBaudRateKey, 115200);
    }

    if (!m_settings.contains(kSerialDataBitsKey)) {
        m_settings.setValue(kSerialDataBitsKey, 8);
    }

    if (!m_settings.contains(kSerialParityKey)) {
        m_settings.setValue(kSerialParityKey, 0);
    }

    if (!m_settings.contains(kSerialStopBitsKey)) {
        m_settings.setValue(kSerialStopBitsKey, 1);
    }

    m_settings.sync();
}

SerialSettings AppSettings::readAll()
{
    setDefaultValues();

    SerialSettings settings;
    settings.portName = m_settings.value(kSerialPortNameKey, QString()).toString();
    settings.baudRate = m_settings.value(kSerialBaudRateKey, 115200).toInt();
    settings.dataBits = m_settings.value(kSerialDataBitsKey, 8).toInt();
    settings.parity = m_settings.value(kSerialParityKey, 0).toInt();
    settings.stopBits = m_settings.value(kSerialStopBitsKey, 1).toInt();
    return settings;
}

QVariant AppSettings::read(const QString &key, const QVariant &defaultValue)
{
    return m_settings.value(key, defaultValue);
}

void AppSettings::writeAll(const SerialSettings &settings)
{
    m_settings.setValue(kSerialPortNameKey, settings.portName);
    m_settings.setValue(kSerialBaudRateKey, settings.baudRate);
    m_settings.setValue(kSerialDataBitsKey, settings.dataBits);
    m_settings.setValue(kSerialParityKey, settings.parity);
    m_settings.setValue(kSerialStopBitsKey, settings.stopBits);
    m_settings.sync();
}

void AppSettings::write(const QString &key, const QVariant &value)
{
    m_settings.setValue(key, value);
    m_settings.sync();
}

void AppSettings::clear()
{
    m_settings.clear();
    setDefaultValues();
}
