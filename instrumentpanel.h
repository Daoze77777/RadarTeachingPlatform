#ifndef INSTRUMENTPANEL_H
#define INSTRUMENTPANEL_H

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>

class InstrumentPanel : public QWidget
{
    Q_OBJECT

public:
    explicit InstrumentPanel(QWidget *parent = nullptr);

    // 根据实验配置的 txCount 动态创建 T 灯，初始全灰
    // 原理演示和测试验证都调用此接口，区别只在于后续是否联动串口
    void setTargetLightCount(int count);

    // 设置某个灯的颜色，id 如 "power"/"T1"/"T2"...
    // color: "green" / "red" / "gray"
    void setLightColor(const QString& id, const QString& color);

    // 将所有 T 灯重置为灰色（切换实验时调用）
    void resetAllTargetLights();

    // 获取当前 T 灯数量
    int targetLightCount() const { return m_txCount; }

private:
    QWidget* createPanelItem(const QString& text, const QString& id, const QString& initialColor);
    QString getLightStyleSheet(const QString& colorHex);
    QMap<QString, QLabel*> m_indicatorLights; // id → 指示灯Label
    QVBoxLayout* m_targetLayout;              // 专管 T 灯的布局
    int m_txCount = 0;                        // 当前 T 灯数量
};

#endif // INSTRUMENTPANEL_H
