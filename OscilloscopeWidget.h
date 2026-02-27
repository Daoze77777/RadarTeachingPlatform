#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

// 必须使用命名空间
//QT_CHARTS_USE_NAMESPACE

class OscilloscopeWidget : public QChartView {
public:
    OscilloscopeWidget(QWidget *parent = nullptr) : QChartView(parent) {
        m_chart = new QChart();
        m_series = new QLineSeries();
        m_chart->addSeries(m_series);

        // 设置 Y 轴：电压 (V)
        m_axisY = new QValueAxis();
        m_axisY->setRange(0, 5.0);
        m_axisY->setTitleText("电压 (V)");
        m_axisY->setLabelFormat("%.1f");
        m_axisY->setTickCount(11);

        // 设置 X 轴：时间 (us)
        m_axisX = new QValueAxis();
        m_axisX->setRange(0, 20);
        m_axisX->setTitleText("时间 (μs)");
        m_axisX->setLabelFormat("%d");

        m_chart->addAxis(m_axisX, Qt::AlignBottom);
        m_chart->addAxis(m_axisY, Qt::AlignLeft);
        m_series->attachAxis(m_axisX);
        m_series->attachAxis(m_axisY);

        m_chart->legend()->hide(); // 隐藏图例
        this->setChart(m_chart);
        this->setRenderHint(QPainter::Antialiasing); // 抗锯齿
    }

    // 根据步骤 ID 更新波形
    void setData(const QString& stepId) {
        m_series->clear();
        if (stepId == "s1") { // 设置目标位置
            m_series->append(0, 0); m_series->append(20, 0); // 初始直线
        } else if (stepId == "s2") { // 触发脉冲信号
            m_series->append(0, 0); m_series->append(5, 0);
            m_series->append(5, 4); m_series->append(7, 4);
            m_series->append(7, 0); m_series->append(20, 0);
        }
    }

private:
    QChart *m_chart;
    QLineSeries *m_series;
    QValueAxis *m_axisX, *m_axisY;
};
