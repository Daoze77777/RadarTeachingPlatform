#ifndef INSTRUMENTPANEL_H
#define INSTRUMENTPANEL_H

#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>

class InstrumentPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InstrumentPanel(QWidget *parent = nullptr);

    // 对外提供的控制接口：传入组件ID（如 "power", "T1"）和颜色（"red", "green", "gray"）
    void setLightColor(const QString& id, const QString& color);

private:
    // 内部辅助函数：生成单行“标签+指示灯”
    QWidget* createPanelItem(const QString& text, const QString& id, const QString& initialColor);

    // 内部辅助函数：生成指示灯的QSS样式
    QString getLightStyleSheet(const QString& colorHex);

    // 存储所有指示灯的指针，键为ID，值为对应的QLabel对象
    QMap<QString, QLabel*> m_indicatorLights;
};

#endif // INSTRUMENTPANEL_H
