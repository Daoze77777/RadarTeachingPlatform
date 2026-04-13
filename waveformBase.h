#ifndef WAVEFORMBASE_H
#define WAVEFORMBASE_H

#include <QVector>
#include <QString>
#include <QColor>
class WaveformBase
{
public:
    virtual ~WaveformBase() = default;

    // 生成主波形数据，子类必须实现
    virtual void generate(QVector<double> &x, QVector<double> &y) = 0;

    // 坐标轴配置，子类按需覆写
    virtual QString xLabel() const { return "时间 (µs)"; }
    virtual QString yLabel() const { return "电压 (V)"; }
    virtual double  xMax()   const { return 3050.0; }
    virtual double  xMin()   const { return 0.0; }
    virtual double  yMin()   const { return -0.1; }
    virtual double  yMax()   const { return 1.3; }
    // 额外 graph 的颜色，默认黄色，子类可覆写
    virtual QColor extraGraphColor() const { return QColor(255, 200, 0); }

    // X轴步长，0表示交给示波器自动决定
    virtual double  xStep()  const { return 0.0; }
    // Y轴步长，0表示交给示波器自动决定
    virtual double  yStep()  const { return 0.0; }

    // 是否需要额外的 graph（如双线波形），默认 false
    // 需要额外线的子类覆写为 true，并实现 generateExtra()
    virtual bool needsExtraGraph() const { return false; }
    virtual void generateExtra(QVector<double> &x, QVector<double> &y)
    {
        x.clear();
        y.clear();
    }
};

#endif // WAVEFORMBASE_H
