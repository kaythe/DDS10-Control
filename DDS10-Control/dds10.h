#ifndef DDS10_H
#define DDS10_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVariant>

class DDS10 : public QObject
{
    Q_OBJECT
public:
    explicit DDS10(const QString &portName = "", QObject *parent = nullptr);
    ~DDS10();

    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(double frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)

    Q_INVOKABLE bool connectToDevice();

    bool connected();

    double frequency();
    bool setFrequency(double frequency);

signals:
    void connectedChanged(bool connected);
    void frequencyChanged(double frequency);

private:
    QSerialPort serial_port;
    bool connected_;

    double frequency_;

    static constexpr uint8_t start_symbol = 0x02;
    static constexpr uint8_t stop_symbol = 0x03;
    static constexpr uint8_t escape_symbol = 0x05;
    static constexpr uint8_t escape_symbol_mask = 0x80;

    static constexpr uint8_t GET_VERSION = 0x56;
    static constexpr uint8_t GET_FREQUENCY = 0x46;
    static constexpr uint8_t SET_FREQUENCY = 0x66;

    QByteArray encode_packet(QByteArray &data);
    bool send_command(uint8_t command, const QList<QVariant> &parameters);
};

#endif // DDS10_H
