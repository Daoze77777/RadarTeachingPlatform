#include "stepController.h"

StepController::StepController(QObject *parent)
    : QObject(parent)
{}

void StepController::reset(int totalSteps)
{
    m_totalSteps       = totalSteps;
    m_currentStepIndex = 0;
    m_isFinished       = false;
    m_completedSteps.clear();
    m_completedSteps.insert(0); // s1 默认已完成，不需要灯
}

void StepController::completeStep(int index)
{
    m_completedSteps.insert(index);
    m_currentStepIndex = index;

    if (index == m_totalSteps - 1 && !m_isFinished) {
        m_isFinished = true;
        emit experimentFinished();
    }
}

void StepController::setFinished()
{
    if (!m_isFinished) {
        m_isFinished = true;
        emit experimentFinished();
    }
}

bool StepController::canGoTo(int targetIndex, QString &outErrorMsg) const
{
    // ===== 已完成状态 =====
    if (m_isFinished) {
        if (targetIndex == 0) return true; // 允许点第一步重来
        outErrorMsg = "实验已完成！\n点击第一步可重新开始实验。";
        return false;
    }

    // ===== 未完成状态 =====

    // s1（index=0）始终放行
    if (targetIndex == 0) return true;

    // 点击当前步骤：允许（重复点）
    if (targetIndex == m_currentStepIndex) return true;

    // 点击下一步：需要当前步骤已完成
    if (targetIndex == m_currentStepIndex + 1
        && m_completedSteps.contains(m_currentStepIndex)) {
        return true;
    }

    // 其他情况：拦截
    if (!m_completedSteps.contains(m_currentStepIndex)) {
        outErrorMsg = QString("请先完成当前步骤！");
    } else {
        outErrorMsg = QString("请按顺序进行实验！\n请先完成上一步。");
    }
    return false;
}
