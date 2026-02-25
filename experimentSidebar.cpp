
#include "experimentSidebar.h"
#include <QVBoxLayout>
#include "radarConfigLoader.h"
#include "ExperimentManager.h"//

ExperimentSidebar::ExperimentSidebar(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

void ExperimentSidebar::initUI()
{
    // 1. 设置主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0); // 紧密排列

    // 2. 定义配置数据 将变化的参数（标题、图标）提取出来，而不是硬编码在 new 语句里
    struct GroupConfig {
        QString title;
        QString iconPath;
        PanelStyle style; // 决定是普通复选还是带序号
    };

    // 配置列表：如果你想改名字或换图标，只用改这里
    GroupConfig configs[4] = {
        {"雷达组件", ":/mainicons/resources/mainIcons/leidazujian.png", PanelStyle::Standard},  // 对应 GroupType::RadarComponents (0)
        {"测量原理", ":/mainicons/resources/mainIcons/celiangyuanli.png", PanelStyle::Standard},  // 对应 GroupType::Principles (1)
        {"实验步骤", ":/mainicons/resources/mainIcons/shiyanbuzouICon.png", PanelStyle:: Numbered },  // 对应 GroupType::Steps (2)
        {"相关课程", ":/mainicons/resources/mainIcons/xaingguankechengICon.png", PanelStyle::Standard}   // 对应 GroupType::Courses (3)
    };

    // 3. 循环创建折叠组对象
    for (int i = 0; i < GroupCount; ++i) {
        // 调用构造函数,this 指针自动作为 parent 传入
        m_groups[i] = new CollapsibleGroup(configs[i].title, QIcon(configs[i].iconPath), this);

        // 创建内部通用面板 (根据配置传入样式)
        m_panels[i] = new SidebarListPanel(configs[i].style, this);

        // 放入主布局
        m_groups[i]->addWidget(m_panels[i]);

        // 加入布局
        mainLayout->addWidget(m_groups[i]);

        // 4. 绑定互斥逻辑（手风琴效果）
        connect(m_groups[i], &CollapsibleGroup::expandedChanged, [this, i](bool expanded) {
            if (expanded) {
                for (int j = 0; j < GroupCount; ++j) {
                    if (i != j) m_groups[j]->setExpanded(false);
                }
            }
        });
    }
    initConnections();
    // 5. 底部弹簧
    mainLayout->addStretch();
}
// 实现获取组的接口
CollapsibleGroup* ExperimentSidebar::getGroup(GroupType type)
{
    if (type >= 0 && type < 4)
    {
        return m_groups[type];
    }
    return nullptr;
}
// 实现重置接口
void ExperimentSidebar::resetAllGroups()
{
    for (int i = 0; i < 4; ++i)
    {
        // 这里调用你之前添加的 clearContent()
        if (m_groups[i]) m_groups[i]->clearContent();
    }
}
// 切换实验的接口
void ExperimentSidebar::switchExperiment(int expId)
{
    // 1. 检查缓存中是否有数据,如果没有，构建路径加载 XML
    if (!m_configCache.contains(expId)) {
        QString xmlPath = ExperimentManager::instance().getXmlPathByExpId(expId);
        if (xmlPath.isEmpty()) return;
       ExperimentConfig loadedConfig = RadarConfigLoader::loadConfig(xmlPath);
        // 简单校验加载是否成功
        if (loadedConfig.id == 0 && loadedConfig.name.isEmpty()) {
            qDebug() << "Failed to load experiment XML:" << xmlPath;
            return;
        }

        // 存入缓存
        m_configCache.insert(expId, loadedConfig);
    }

    // 2. 关键：在加载新实验前，强行重置所有折叠组的状态
    for (int i = 0; i < GroupCount; ++i) {
        if (m_groups[i]) {
            // 假设你的 CollapsibleGroup 有一个 setExpanded(bool) 方法
            // 强制设为 false，这会重置它们内部的高度逻辑
            m_groups[i]->setExpanded(false);
        }
    }

    // 2. ★ 关键修正：更新类成员 m_config ★确保整个 Sidebar 内部的状态变量指向当前选择的实验
    this->m_config = m_configCache[expId];

    // 3. 分别更新 4 个组
    updateGroupContent(RadarComponents, m_config.radarGroup);
    updateGroupContent(Principles, m_config.principleGroup);
    updateGroupContent(Steps, m_config.stepGroup);
    updateGroupContent(Courses, m_config.courseGroup);

    // 5. 核心：强制触发侧边栏总布局的刷新
    QCoreApplication::sendPostedEvents(this, QEvent::LayoutRequest);
    this->layout()->activate();
    this->adjustSize();


    // 4. (可选) 默认策略：如果雷达组件可见，自动展开第一个组
    // if (config.radarGroup.isVisible) {
    //     m_groups[RadarComponents]->setExpanded(true);
    // } else if (config.principleGroup.isVisible) {
    //     m_groups[Principles]->setExpanded(true);
    // }
}
// 辅助函数：统一处理 "是否显示组" 和 "更新数据"
void ExperimentSidebar::updateGroupContent(int groupIndex, const GroupData& data)
{
    if (groupIndex < 0 || groupIndex >= GroupCount) return;

    // 1. 设置组的可见性 (对应 XML 中的 visible 属性)
    m_groups[groupIndex]->setVisible(data.isVisible);

    // 2. 如果可见，则更新内部列表
    if (data.isVisible) {
        m_panels[groupIndex]->updateList(data.items);
    }
}
//初始化连接信号
void ExperimentSidebar::initConnections() {
    for (int i = 0; i < GroupCount; ++i) {
    // 防御性编程：防止空指针崩溃
        if (m_panels[i] == nullptr) continue;
    // 接力信号：当内部面板发出 itemClicked，侧边栏再发出 componentSelected
    connect(m_panels[i], &SidebarListPanel::itemClicked, this, [this](const ExperimentContentItem& item) {
            // ★ 关键 4：在这里把“底部原理图”的路径拿出来
            QString bottomPath = m_config.fixedBottomImage;
            emit componentSelected(item, bottomPath);
        });
    }
}
