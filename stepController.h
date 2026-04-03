#ifndef STEPCONTROLLER_H
#define STEPCONTROLLER_H

#include <QObject>
#include <QSet>
#include <QString>

class StepController : public QObject
{
    Q_OBJECT
public:
    explicit StepController(QObject *parent = nullptr);

    // 初始化/重置，传入总步骤数
    void reset(int totalSteps);

    // 外部告知某步已完成（串口灯亮、用户操作等）
    void completeStep(int index);

    // 询问是否可以跳转到目标步骤
    // 返回 true 表示允许，返回 false 时 outErrorMsg 携带提示文字
    bool canGoTo(int targetIndex, QString &outErrorMsg) const;

    // 手动标记实验完成（用于 s14 这类不靠串口灯的最后一步）
    void setFinished();

    // ===== 状态查询 =====
    bool isFinished()          const { return m_isFinished; }
    int  currentStepIndex()    const { return m_currentStepIndex; }
    bool isCompleted(int index)const { return m_completedSteps.contains(index); }
    int  totalSteps()          const { return m_totalSteps; }

    // 直接设置当前步骤索引（放行后由外部调用）
    void setCurrentStepIndex(int index) { m_currentStepIndex = index; }

signals:
    void experimentFinished(); // 最后一步完成时发出

private:
    int       m_totalSteps       = 0;
    int       m_currentStepIndex = 0;
    bool      m_isFinished       = false;
    QSet<int> m_completedSteps;
};

#endif // STEPCONTROLLER_H
