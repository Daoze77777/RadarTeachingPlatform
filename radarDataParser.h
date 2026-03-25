// RadarDataParser.h
#ifndef RADARDATAPARSER_H
#define RADARDATAPARSER_H

#include <QObject>
#include <QString>
#include <QSerialPort>

// struct RadarData {
//     double  mcuVoltage;    // MCU电压(V)，正常4.5~5.5
//     double  knobVoltage;   // 手动跟踪旋钮电压(V)，0~5
//     int     distance;      // 测距数据(0.1mm)，100~5000，异常9999
//     quint16 s1;            // 测试信号组合S1（16位）
//     quint16 s2;            // 测试信号组合S2（16位）
//     quint16 s3;            // 测试信号组合S3（16位）
//     bool    isValid;       // 数据是否有效
// };

struct RadarData {
    double  mcuVoltage  = 0.0;
    double  knobVoltage = 0.0;
    int     distance    = 0;
    quint16 s1          = 0;
    quint16 s2          = 0;
    quint16 s3          = 0;
    bool    isValid     = false;
};

class RadarDataParser : public QObject
{
    Q_OBJECT
public:
    explicit RadarDataParser(QObject *parent = nullptr);

    // 喂入串口原始数据，内部自动拼包
    void feedData(const QByteArray &rawData);

    // 直接解析一行完整字符串（测试用）
    RadarData parseLine(const QString &line);

    // 判断某信号是否接通（通道1~16）
    static bool isSignalOn(quint16 sValue, int channel);

signals:
    void dataReady(const RadarData &data);  // 解析成功
    void parseError(const QString &reason); // 解析失败

private:
    QString m_buffer; // 串口数据缓冲
    bool verifyChecksum(const QString &line);
};

#endif // RADARDATAPARSER_H
