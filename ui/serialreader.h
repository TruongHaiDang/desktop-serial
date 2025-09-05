#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <QThread>
#include <QSerialPort>
#include <QString>
#include <atomic>

/**
 * @brief SerialReader đọc dữ liệu từ QSerialPort trong một thread riêng.
 */
class SerialReader : public QThread {
    Q_OBJECT

public:
    /**
     * @brief Khởi tạo SerialReader.
     * @param port QSerialPort* cổng serial cần lắng nghe.
     * @param parent QObject* đối tượng cha.
     */
    explicit SerialReader(QSerialPort *port, QObject *parent = nullptr);

    /**
     * @brief Dừng vòng lặp và kết thúc thread.
     */
    void stop();

signals:
    /**
     * @brief Phát tín hiệu khi nhận được dữ liệu mới.
     * @param data const QString& dữ liệu dạng chuỗi UTF-8.
     */
    void dataReceived(const QString &data);

protected:
    /**
     * @brief Hàm được chạy khi thread bắt đầu, thực hiện vòng lặp chờ dữ liệu.
     */
    void run() override;

private:
    QSerialPort *m_port;              ///< Con trỏ tới QSerialPort đang được sử dụng.
    std::atomic_bool m_running;       ///< Cờ điều khiển vòng lặp đọc dữ liệu.
};

#endif // SERIALREADER_H
