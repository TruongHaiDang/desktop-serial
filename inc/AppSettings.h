#pragma once

#ifndef __APP_SETTINGS_H__
#define __APP_SETTINGS_H__

#include <QSettings>
#include <QString>
#include <QVariant>

// ===== Typed config =====
struct SerialSettings
{
    QString portName;
    int baudRate = 115200;
    int dataBits = 8;
    int parity = 0;
    int stopBits = 1;
};

class AppSettings
{
private:
    QSettings m_settings;

    void setDefaultValues(); // dùng nội bộ

public:
    AppSettings();
    ~AppSettings();

    SerialSettings readAll();
    QVariant read(const QString &key, const QVariant &defaultValue = QVariant());
    void writeAll(const SerialSettings &settings);
    void write(const QString &key, const QVariant &value);
    void clear();

};

#endif
