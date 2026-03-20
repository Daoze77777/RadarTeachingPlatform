// SerialTestHelper.cpp
#include "serialTestHelper.h"
#include <QDebug>

SerialTestHelper::SerialTestHelper(QObject *parent)
    : QObject(parent)
{
    m_serial = new QSerialPort(this);
    m_parser = new RadarDataParser(this);

    // 串口收到数据 → 打印原始数据 → 喂给解析器
    connect(m_serial, &QSerialPort::readyRead, this, [=]() {
        QByteArray raw = m_serial->readAll();
        qDebug() << "[原始数据]" << QString::fromLatin1(raw).trimmed();
        m_parser->feedData(raw);
    });

    // 解析成功 → 打印解析结果
    connect(m_parser, &RadarDataParser::dataReady, this, [=](const RadarData &d) {
        qDebug() << "========== 解析结果 ==========";
        qDebug() << "MCU电压  :" << d.mcuVoltage  << "V"
                 << (d.mcuVoltage >= 4.5 && d.mcuVoltage <= 5.5 ? "[正常]" : "[异常]");
        qDebug() << "旋钮电压 :" << d.knobVoltage << "V";
        qDebug() << "测距数据 :" << (d.distance == 9999 ? "异常" : QString("%1 × 0.1mm").arg(d.distance));
        qDebug() << "S1(16位) :" << QString("0x%1").arg(d.s1, 4, 16, QChar('0')).toUpper()
                 << "二进制:" << QString("%1").arg(d.s1, 16, 2, QChar('0'));
        qDebug() << "S2(16位) :" << QString("0x%1").arg(d.s2, 4, 16, QChar('0')).toUpper()
                 << "二进制:" << QString("%1").arg(d.s2, 16, 2, QChar('0'));
        qDebug() << "S3(16位) :" << QString("0x%1").arg(d.s3, 4, 16, QChar('0')).toUpper()
                 << "二进制:" << QString("%1").arg(d.s3, 16, 2, QChar('0'));

        // 打印S1各通道状态
        QStringList s1Names = {"触发脉冲","调制脉冲","中频","本振","射频发射","射频接收","基带","视频","门限","和支路","微分","差支路"};
        QStringList s1On;
        for (int i = 0; i < s1Names.size(); ++i) {
            if (RadarDataParser::isSignalOn(d.s1, i + 1))
                s1On << s1Names[i];
        }
        qDebug() << "S1接通通道:" << (s1On.isEmpty() ? "无" : s1On.join(", "));
        qDebug() << "==============================";
    });

    // 解析失败
    connect(m_parser, &RadarDataParser::parseError, this, [=](const QString &reason) {
        qDebug() << "[解析错误]" << reason;
    });
}

void SerialTestHelper::start(const QString &portName)
{
    m_serial->setPortName(portName);
    m_serial->setBaudRate(QSerialPort::Baud9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::ReadOnly)) {
        qDebug() << "[串口] 打开成功:" << portName << "9600 8N1";
    } else {
        qDebug() << "[串口] 打开失败:" << m_serial->errorString();
    }
}

void SerialTestHelper::stop()
{
    if (m_serial->isOpen()) {
        m_serial->close();
        qDebug() << "[串口] 已关闭";
    }
}
