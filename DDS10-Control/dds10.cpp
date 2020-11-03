#include "dds10.h"

DDS10::DDS10(const QString &portName, QObject *parent): QObject(parent), connected_(false), frequency_(10e6)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.portName() == portName)
        {
            serial_port.setPort(info);

            break;
        }
    }

    if (portName == "" && QSerialPortInfo::availablePorts().length() > 0)
    {
        serial_port.setPort(QSerialPortInfo::availablePorts().first());
    }
}

DDS10::~DDS10()
{
    if (serial_port.isOpen())
        serial_port.close();
}

bool DDS10::connectToDevice()
{
    serial_port.setBaudRate(9600);
    serial_port.setStopBits(QSerialPort::TwoStop);
    serial_port.setDataBits(QSerialPort::Data8);
    serial_port.setParity(QSerialPort::EvenParity);

    if (!serial_port.open(QIODevice::ReadWrite))
    {
        return false;
    }
    connected_ = true;
    emit connectedChanged(connected_);

    serial_port.write("abc");
    if (!serial_port.waitForBytesWritten())
    {
        return false;
    }

    return true;
}

bool DDS10::connected()
{
    return connected_;
}

double DDS10::frequency()
{
    return frequency_;
}

bool DDS10::setFrequency(double frequency)
{
    if (!qFuzzyCompare(frequency, frequency_))
    {
        frequency_ = frequency;

        send_command(DDS10::SET_FREQUENCY, QList<QVariant>{QVariant::fromValue(static_cast<uint16_t>(frequency_))});

        emit frequencyChanged(frequency_);
        return true;
    }

    return false;
}

QByteArray DDS10::encode_packet(QByteArray &data)
{
    QByteArray encoded_packet;

    for (int i = 0; i < data.length(); ++i)
    {
        if (data.at(i) == DDS10::start_symbol ||
            data.at(i) == DDS10::stop_symbol ||
            data.at(i) == DDS10::escape_symbol)
        {
            encoded_packet.append(DDS10::escape_symbol);
            encoded_packet.append(data.at(i) | DDS10::escape_symbol_mask);
        }
        else
        {
            encoded_packet.append(data.at(i));
        }
    }

    encoded_packet.push_front(DDS10::start_symbol);
    encoded_packet.push_back(DDS10::stop_symbol);

    return encoded_packet;
}

bool DDS10::send_command(uint8_t command, const QList<QVariant> &parameters)
{
    QByteArray command_bytes;

    switch (command)
    {
    case DDS10::SET_FREQUENCY:
        {
        command_bytes.append(DDS10::SET_FREQUENCY);
        uint32_t freq = parameters.first().toUInt();
        command_bytes.append((freq >> 24) & 0xFF);
        command_bytes.append((freq >> 16) & 0xFF);
        command_bytes.append((freq >> 8) & 0xFF);
        command_bytes.append(freq & 0xFF);

        break;
        }

    default:
        return false;
    }

    QByteArray encoded_bytes = encode_packet(command_bytes);

    return (serial_port.write(encoded_bytes) == encoded_bytes.length());
}
