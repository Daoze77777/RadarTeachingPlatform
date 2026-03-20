// SerialTestHelper.h
#ifndef SERIALTESTHELPER_H
#define SERIALTESTHELPER_H

#include <QObject>
#include <QSerialPort>
#include "RadarDataParser.h"

class SerialTestHelper : public QObject
{
    Q_OBJECT
public:
    explicit SerialTestHelper(QObject *parent = nullptr);
    void start(const QString &portName = "COM3");
    void stop();

private:
    QSerialPort    *m_serial;
    RadarDataParser *m_parser;
};

#endif // SERIALTESTHELPER_H
