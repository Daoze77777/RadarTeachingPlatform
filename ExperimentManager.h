#ifndef EXPERIMENTMANAGER_H
#define EXPERIMENTMANAGER_H

#include <QString>
#include <QMap>
#include <QDebug>
#include <QObject>


// 1. 单个实验的数据结构
struct ExperimentInfo {
    int expId;              // 全局唯一ID (建议：测距101-119, 测角201-299，避免冲突)
    QString name;           // 实验名称 (例如："脉冲法距离测量-原理演示实验")
    QString xmlFileName;    // 对应的 XML 配置文件名 (例如："dist_exp1.xml")
    int txCount;      // 新增：Tx灯数量
    bool isTestMode;  // 新增：是否测试验证模式
};

// 2. 大系统平台的数据结构
struct SystemPlatformInfo {
    int systemId;           // 平台ID (例如：1=测距, 2=测角, 3=测速)
    QString systemName;     // 平台名称 (例如："气象雷达距离测量实验平台")
    QList<ExperimentInfo> experiments; // 该平台下包含的所有实验列表
};

class ExperimentManager : public QObject {
    Q_OBJECT

public:
    static ExperimentManager& instance() {
        static ExperimentManager inst;
        return inst;
    }

    // 获取当前激活的平台信息（名称、包含的实验等）
    SystemPlatformInfo getCurrentPlatform() const
    {
        return m_platforms.value(m_currentSystemId);
    }

    // 根据实验全局 ID 获取它的 XML 路径
    QString getXmlPathByExpId(int expId) const
    {
        // 遍历寻找，或者维护一个专门的 expId -> xmlPath 的哈希表以提高查找速度
        return m_baseXmlPath + m_expIdToXmlMap.value(expId);
    }

    QString getExperimentName(int expId) const
    {
        QString fullName;
        // 1. 先获取完整名称 (逻辑保持不变)
        SystemPlatformInfo currentSys = getCurrentPlatform();
        bool found = false;
        // 1. 优先从当前平台查找（效率最高）
        for (const auto& exp : currentSys.experiments) {
            if (exp.expId == expId) {
                fullName = exp.name;
                found = true;
                break;
            }
        }
        // 2. 如果没找到（例如从文件菜单切换到了其他平台的实验），则全局查找
        if (!found) {
            for (const auto& platform : m_platforms) {
                for (const auto& exp : platform.experiments) {
                    if (exp.expId == expId) {
                        fullName = exp.name;
                        found = true;
                        break;
                    }
                }
                if(found) break;
            }
        }
        // 3. 兜底方案：返回默认平台名称
        if (fullName.isEmpty()) return "气象雷达原理实验平台";

        // 2. ★ 核心过滤逻辑：去除 "实验科目X. " 前缀 ★
        // 假设你的格式固定为 "实验科目1. 脉冲法..."
        // 我们寻找第一个点 "." 出现的位置，并取其后的字符串
        int dotIndex = fullName.indexOf(".");
        if (dotIndex != -1) {
            // 截取点之后的全部内容，并使用 trimmed() 去除可能存在的空格
            return fullName.mid(dotIndex + 1).trimmed();
        }
        return fullName;
    }

    // ★ 预留给未来的核心功能：切换大系统平台
    void switchSystemPlatform(int newSystemId)
    {
        if (m_platforms.contains(newSystemId) && m_currentSystemId != newSystemId)
        {
            m_currentSystemId = newSystemId;
            emit systemPlatformChanged(); // 告诉所有 UI：平台换了，快重新渲染！
        }
    }

    // 新增查询接口
    bool isTestMode(int expId) const {
        for (const auto& sys : m_platforms)
            for (const auto& exp : sys.experiments)
                if (exp.expId == expId) return exp.isTestMode;
        return false;
    }

    int getTxCount(int expId) const {
        for (const auto& sys : m_platforms)
            for (const auto& exp : sys.experiments)
                if (exp.expId == expId) return exp.txCount;
        return 0;
    }
signals:
    void systemPlatformChanged(); // 切换平台信号

private:
    ExperimentManager()
    {
        m_baseXmlPath = "resources/xml/";
        m_currentSystemId = 1; // 默认加载：1=距离测量平台
        initMockData();        // 初始化数据
    }

    void initMockData()
    {
        // --- 1. 构造距离测量平台 (System ID: 1) ---
        SystemPlatformInfo distSystem;
        distSystem.systemId = 1;
        distSystem.systemName = "距离测量实验平台";
        // 赋予 10 个实验
        distSystem.experiments = {
            {101, "实验科目1. 脉冲法距离测量-原理演示实验", "mcfjlcl.xml",      12, false},
            {102, "实验科目2. 脉冲法距离测量-测试验证实验", "mcfjlcl_test.xml", 12, true},
            {103, "实验科目3. 调频法距离测量-原理演示实验", "tpfjlcl.xml",       4, false},
            {104, "实验科目4. 调频法距离测量-测试验证实验", "tpfjlcl_test.xml",       4, true},
            {105, "实验科目5. 相位法距离测量-原理演示实验", "xwfjlcl.xml",       7, false},
            {106, "实验科目6. 相位法距离测量-测试验证实验", "xwfjlcl_test.xml",       7, true},
            {107, "实验科目7. 距离退模糊-原理演示实验",     "jltmh.xml",       6, false},
            {108, "实验科目8. 距离退模糊-自主设计实验",     "jltmh_test.xml",       6, true},
            {109, "实验科目9. 距离跟踪-原理演示实验",       "jlgz.xml",       9, false},
            {110, "实验科目10. 距离跟踪-测试验证实验",      "jlgz_test.xml",       9, true},
        };
        m_platforms.insert(1, distSystem);

        // --- 2. 构造角度测量平台 (System ID: 2) ---
        // SystemPlatformInfo angleSystem;
        // angleSystem.systemId = 2;
        // angleSystem.systemName = "角度测量实验平台";
        // // 赋予 8 个实验
        // angleSystem.experiments = {
        //     {201, "实验科目1. 相位法测角-原理演示", "angle_01.xml"},
        //     // ... 一直到 208
        // };
        // m_platforms.insert(2, angleSystem);

        // --- 3. 构造速度测量平台 (System ID: 3) ---
        // 同理...

        // 建立一个快捷查找映射，方便通过 expId 直接拿到 XML 文件名
        for(const auto& sys : m_platforms) {
            for(const auto& exp : sys.experiments) {
                m_expIdToXmlMap.insert(exp.expId, exp.xmlFileName);
            }
        }
    }

    int m_currentSystemId; // 当前激活的系统ID
    QString m_baseXmlPath;
    QMap<int, SystemPlatformInfo> m_platforms; // 所有的平台数据
    QMap<int, QString> m_expIdToXmlMap;        // 快捷查找表：实验ID -> XML文件名
};
#endif // EXPERIMENTMANAGER_H
