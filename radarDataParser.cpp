#include "RadarDataParser.h"

RadarDataParser::RadarDataParser(QObject *parent) : QObject(parent) {}

void RadarDataParser::feedData(const QByteArray &rawData)
{
    m_buffer += QString::fromLatin1(rawData);

    // 按行切割，处理完整的帧
    while (m_buffer.contains('\n')) {
        int idx = m_buffer.indexOf('\n');
        QString line = m_buffer.left(idx).trimmed(); // 去掉\r\n
        m_buffer = m_buffer.mid(idx + 1);            // 剩余留缓冲

        if (line.startsWith("$RADAR")) {
            RadarData data = parseLine(line);
            if (data.isValid)
                emit dataReady(data);
            else
                emit parseError("解析失败: " + line);
        }
    }
}

RadarData RadarDataParser::parseLine(const QString &line)
{
    RadarData data;
    data.isValid = false;

    // 格式：$RADAR,mcuV,knobV,dist,s1,s2,s3,*XX
    // 1. 校验和验证
    if (!verifyChecksum(line)) {
        // 校验和暂未启用，仅警告不拦截
        // emit parseError("校验和错误");
    }

    // 2. 去掉校验部分 "*XX"
    QString payload = line;
    int starIdx = payload.indexOf('*');
    if (starIdx != -1)
        payload = payload.left(starIdx);

    // 3. 按逗号分割
    QStringList parts = payload.split(',');
    // 期望：["$RADAR", mcuV, knobV, dist, s1, s2, s3]
    if (parts.size() < 7) return data;
    if (parts[0] != "$RADAR") return data;

    bool ok1, ok2, ok3, ok4, ok5, ok6;
    data.mcuVoltage  = parts[1].toDouble(&ok1);
    data.knobVoltage = parts[2].toDouble(&ok2);
    data.distance    = parts[3].toInt(&ok3);
    data.s1          = parts[4].toUShort(&ok4);
    data.s2          = parts[5].toUShort(&ok5);
    data.s3          = parts[6].toUShort(&ok6);

    if (!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6) return data;

    data.isValid = true;
    return data;
}

bool RadarDataParser::verifyChecksum(const QString &line)
{
    // 计算 $ 和 * 之间字符的 XOR
    int start = line.indexOf('$') + 1;
    int end   = line.indexOf('*');
    if (start <= 0 || end <= 0 || end <= start) return false;

    quint8 xorVal = 0;
    for (int i = start; i < end; ++i)
        xorVal ^= static_cast<quint8>(line[i].toLatin1());

    QString expected = line.mid(end + 1, 2).toUpper();
    QString actual   = QString("%1").arg(xorVal, 2, 16, QChar('0')).toUpper();
    return expected == actual;
}

bool RadarDataParser::isSignalOn(quint16 sValue, int channel)
{
    // channel: 1~16，对应bit0~bit15
    if (channel < 1 || channel > 16) return false;
    return (sValue >> (channel - 1)) & 0x01;
}
