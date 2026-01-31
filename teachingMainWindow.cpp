#include "teachingMainWindow.h"
#include <QApplication>
#include <QScreen>
#include <qwidget.h>

#include <QGridLayout>
#include <QStyle>
#include <QVBoxLayout>

TeachingMainWindow::TeachingMainWindow(QWidget *parent)
{
    //加载窗口样式
    setupWindowStyle();
    //设置自定义标题栏
    setupCustomTitleBar();
    //设置基本布局
    setupBasicLayout();
    //设置窗口标题和图标
    setCustomMainWindowTitle(":/icons/resources/icons/title.png", "气象雷达测量原理实验平台");
    //设置菜单
    setupMenu();
    //设置窗口大小
    resize(1920, 1033);

    initUI();
}

TeachingMainWindow::~TeachingMainWindow() {}
QWidget *TeachingMainWindow::createHomePage()
{
    // 创建主页容器
    QWidget *homePage = new QWidget(this);
    homePage->setObjectName("homePage");
    homePage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(homePage);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建内容容器
    QWidget *contentWidget = new QWidget;
    //contentWidget->setObjectName("contentWidget");
    mainLayout->addWidget(contentWidget, /*stretch=*/1);

    // 内容容器的布局
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // 主容器的布局
    QVBoxLayout *containerLayout = contentLayout;
    containerLayout->setSpacing(60);

    // 标题区域 中+英
    QWidget *titleArea = new QWidget();
    titleArea->setObjectName("titleWidget");
    titleArea->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    QVBoxLayout *titleLayout = new QVBoxLayout(titleArea);
    titleLayout->setSpacing(8);                              // 中文与英文间距
    titleLayout->setContentsMargins(0, 0, 0, 0);             // 无内边距
    titleLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft); // 左上对齐

    m_cnTitleLabel = new QLabel("测距原理实验平台", titleArea);
    m_cnTitleLabel->setObjectName("mainTitle");

    m_enTitleLabel = new QLabel("Distance measurement experimental platform", titleArea);
    m_enTitleLabel->setObjectName("subTitle");

    titleLayout->addWidget(m_cnTitleLabel);
    titleLayout->addWidget(m_enTitleLabel);
    containerLayout->addWidget(titleArea);

    // 设置主页背景透明，显示父窗口的背景图片，并添加标题样式
    homePage->setStyleSheet(R"(
    QWidget#homePage {
        background-color: transparent;
        background-repeat: no-repeat;
    }
    QWidget#contentWidget {
        background-color: transparent;
    }
    QWidget#titleWidget {
        background-color: transparent;
    }
    QLabel#mainTitle {
        font-size: 75px;
        font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
        font-weight: 600;
        font-style: italic;
        color: #2D2F4F;
    }
    QLabel#subTitle {
        padding-left: 63px;
        font-size: 50px;
        font-family: 'PingFang SC', 'Microsoft YaHei', sans-serif;
        font-weight: 600;
        font-style: italic;
        color: #5E85E8;
    }
)");

    // 中间链接区域（替换原三列按钮，仅保留前 10 个测距实验）
    QWidget *centerWidget = new QWidget;
    centerWidget->setObjectName("centerWidget");

    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    //centerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    //containerLayout->addWidget(centerWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
    containerLayout->addWidget(centerWidget);
    // 设置对齐：整体水平居中、顶部对齐；不再强制拉伸，避免外侧漏边
    //containerLayout->setAlignment(centerWidget, Qt::AlignHCenter | Qt::AlignTop);

    m_linksWidget = new QWidget;
    m_linksWidget->setObjectName("linksWidget");
    m_linksWidget->setMinimumHeight(480);

    m_linksLayout = new QGridLayout(m_linksWidget);
    m_linksLayout->setContentsMargins(120, 40, 120, 45);

    QWidget *linksWidget = m_linksWidget;
    QGridLayout *linksLayout = m_linksLayout;

    // --- 布局设置 ---
    //linksLayout->setHorizontalSpacing(50); // 增大左右两列的间距
    //linksLayout->setVerticalSpacing(30);   // 增大上下行之间的间距

    //containerLayout->addWidget(linksWidget);
    //centerLayout->addWidget(linksWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
    centerLayout->addWidget(linksWidget);
    // 背景与按钮样式
    centerWidget->setStyleSheet(R"(
    QWidget#centerWidget {
        background-color: transparent;
    }
    QWidget#linksWidget {
        background-color: transparent;
        border-image: url(:/icons/resources/icons/links-widget.png) 60 50 60 50 stretch stretch;
    }
    QPushButton {
        background-image: url(:/icons/resources/icons/link-btn-circle.png);
        background-repeat: no-repeat;

        border: none;
        color: #2D2F4F;
        text-align:  left;
        padding-left: 80px;
        padding-right:30px;
        font-family: PingFangSC, 'Microsoft YaHei';
        font-weight: 400;
        font-size: 28px;
    }
    QPushButton:hover {
        background-image: url(:/icons/resources/icons/link-btn-hover.png);
        background-repeat: no-repeat;
        background-position: left center;
        color: #2D2F4F;
    }
    QPushButton:pressed {
        background-image: url(:/icons/resources/icons/link-btn-pressed.png);
        background-repeat: no-repeat;
        background-position: left center;
        color: #FFFFFF;
    }
)");
    // 10 个测距实验
    QStringList distanceLinks = {"实验科目1. 脉冲法距离测量-原理演示实验",
                                 "实验科目2. 脉冲法距离测量-测试验证实验",
                                 "实验科目3. 调频法距离测量-原理演示实验",
                                 "实验科目4. 调频法距离测量-测试验证实验",
                                 "实验科目5. 相位法距离测量-原理演示实验",
                                 "实验科目6. 相位法距离测量-测试验证实验",
                                 "实验科目7. 距离退模糊-原理演示实验",
                                 "实验科目8. 距离退模糊-自主设计实验",
                                 "实验科目9. 距离跟踪-原理演示实验",
                                 "实验科目10. 距离跟踪-测试验证实验"};

    // 创建并布局按钮（两列，每列最多5行）
    qDeleteAll(m_experimentButtons);
    m_experimentButtons.clear();
    //合并循环：创建、属性设置、布局一次性完成
    for (int i = 0; i < distanceLinks.size(); ++i) {
        QPushButton *btn = new QPushButton(distanceLinks[i], linksWidget);

        // 设置固定尺寸（建议考虑使用 setMinimumSize 配合布局拉伸）
        btn->setMinimumSize(680, 70);
        // 存储指针
        m_experimentButtons.append(btn);
        // 计算行列：i % 5 得到行 (0-4)，i / 5 得到列 (0-1)
        int row = i % 5;
        int col = i / 5;
        linksLayout->addWidget(btn, row, col);
    }

    if (m_experimentButtons.size() >= 10) {
        connect(m_experimentButtons[0],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onPulseMethodClicked);
        connect(m_experimentButtons[1],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onPulseMethodTestClicked);
        connect(m_experimentButtons[2],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onFrequencyModulationClicked);
        connect(m_experimentButtons[3],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onFrequencyModulationTestClicked);
        connect(m_experimentButtons[4],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onPhaseMethodClicked);
        connect(m_experimentButtons[5],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onPhaseMethodTestClicked);
        connect(m_experimentButtons[6],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onRangeAmbiguityClicked);
        connect(m_experimentButtons[7],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onRangeAmbiguityTestClicked);
        connect(m_experimentButtons[8],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onRangeTrackingClicked);
        connect(m_experimentButtons[9],
                &QPushButton::clicked,
                this,
                &TeachingMainWindow::onRangeTrackingTestClicked);
    }

    qDebug() << "主页创建完成";
    return homePage;
}
void TeachingMainWindow::setupMenu()
{
    //清空默认菜单
    fileMenu->clear();
    helpMenu->clear();

    // 设置菜单样式
    QString menuStyle = R"(
        QMenu {
            background-color:rgba(247, 249, 255, 0.5);
            border-radius: 10px;
            border: 2px solid #6796FE;
            padding: 0px;
        }

        QMenu::item {
            background-color: transparent ;
            border-radius: 8px;
            font-family: 'PingFang SC';
            font-weight: 400;
            font-size: 18px;
            color: #252E5B;
            padding: 5px 25px;
            margin: 2px 2px;
        }

        QMenu::item:selected {
            background-color: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #70A9FF, stop:1 #507CFF);
            color: #FFFFFF;
        }

        QMenu::item:pressed {
            background-color: qlineargradient(x1:1, y1:0, x2:0, y2:0, stop:0 #5A99EF, stop:1 #406CEF);

        }
    )";
    // 设置文件菜单窗口属性以支持圆角和透明
    fileMenu->setWindowFlags(fileMenu->windowFlags() | Qt::FramelessWindowHint
                             | Qt::NoDropShadowWindowHint);
    fileMenu->setAttribute(Qt::WA_TranslucentBackground);
    fileMenu->setStyleSheet(menuStyle);

    // 添加文件菜单项
    setupExperimentMenu();

    // 设置帮助菜单窗口属性以支持圆角和透明
    helpMenu->setWindowFlags(helpMenu->windowFlags() | Qt::FramelessWindowHint
                             | Qt::NoDropShadowWindowHint);
    helpMenu->setAttribute(Qt::WA_TranslucentBackground);
    helpMenu->setMinimumWidth(helpButton->width());
    helpMenu->setStyleSheet(menuStyle);

    // 添加帮助菜单项
    QAction *aboutAction = new QAction("关于(A)", this);
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &CustomMainWindow::about);
}

void TeachingMainWindow::setupExperimentMenu()
{
    // 将实验科目直接添加到文件菜单中

    // 实验科目1：脉冲法距离测量原理演示实验
    QAction *action1 = new QAction("实验科目1. 脉冲法距离测量-原理演示实验", this);
    connect(action1, &QAction::triggered, this, &TeachingMainWindow::onPulseMethodClicked);
    fileMenu->addAction(action1);

    // 实验科目2：脉冲法距离测量测试验证实验
    QAction *action2 = new QAction("实验科目2. 脉冲法距离测量-测试验证实验", this);
    connect(action2, &QAction::triggered, this, &TeachingMainWindow::onPulseMethodTestClicked);
    fileMenu->addAction(action2);

    // 实验科目3：调频法距离测量原理演示实验
    QAction *action3 = new QAction("实验科目3. 调频法距离测量-原理演示实验", this);
    connect(action3, &QAction::triggered, this, &TeachingMainWindow::onFrequencyModulationClicked);
    fileMenu->addAction(action3);

    // 实验科目4：调频法距离测量测试验证实验
    QAction *action4 = new QAction("实验科目4. 调频法距离测量-测试验证实验", this);
    connect(action4,
            &QAction::triggered,
            this,
            &TeachingMainWindow::onFrequencyModulationTestClicked);
    fileMenu->addAction(action4);
}
void TeachingMainWindow::onPulseMethodClicked()
{
    // 使用丝滑转场进入脉冲法实验
    //enterExperiment(EXP_PULSE_METHOD);
    qDebug() << "脉冲法距离测量-原理演示实验----进行中";
}
void TeachingMainWindow::onPulseMethodTestClicked()
{
    qDebug() << "脉冲法距离测量-测试验证实验----进行中";
}
void TeachingMainWindow::onFrequencyModulationClicked()
{
    qDebug() << "调频法距离测量-原理演示实验----进行中";
}
void TeachingMainWindow::onFrequencyModulationTestClicked()
{
    qDebug() << "调频法距离测量-测试验证实验----进行中";
}
void TeachingMainWindow::onPhaseMethodClicked() {}
void TeachingMainWindow::onPhaseMethodTestClicked() {}
void TeachingMainWindow::onRangeAmbiguityClicked() {}
void TeachingMainWindow::onRangeAmbiguityTestClicked() {}
void TeachingMainWindow::onRangeTrackingClicked() {}
void TeachingMainWindow::onRangeTrackingTestClicked() {}
void TeachingMainWindow::initUI()
{
    // 将主页面添加到contentWidget中
    m_homePage = createHomePage();
    contentWidget->addWidget(m_homePage);
    contentWidget->setCurrentWidget(m_homePage);
    qDebug() << "TeachingMainWindow::initUI - 已创建主页面";
    // // 创建堆叠窗口
    // m_stackedWidget = new QStackedWidget(this);
    // setCentralWidget(m_stackedWidget);
    // // 创建三个平台页面
    // QWidget *distancePage = createPlatformPage(Platform_Distance);
    // QWidget *anglePage = createPlatformPage(Platform_Angle);
    // QWidget *speedPage = createPlatformPage(Platform_Speed);
    // m_platformPages << distancePage << anglePage << speedPage;
    // // 添加到堆叠窗口
    // for (QWidget *page : m_platformPages) {
    //     m_stackedWidget->addWidget(page);
    // }
    // // 默认显示第一个平台
    // m_stackedWidget->setCurrentIndex(0);
    // // 创建隐藏切换按钮
    // ensureSwitchPlatformButton();
}
QWidget *TeachingMainWindow::createPlatformPage(PlatformType type)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("platformPage");
    page->setStyleSheet("QWidget#platformPage { background-color: transparent; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ====== 标题区域 ======
    QWidget *titleArea = new QWidget(page);
    titleArea->setObjectName("titleArea");
    titleArea->setStyleSheet("QWidget#titleArea { background-color: transparent; }");

    QVBoxLayout *titleLayout = new QVBoxLayout(titleArea);
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->setContentsMargins(0, 80, 0, 40);

    QLabel *cnTitle = new QLabel("", titleArea);
    cnTitle->setObjectName("mainTitle");
    cnTitle->setAlignment(Qt::AlignCenter);

    QLabel *enTitle = new QLabel("", titleArea);
    enTitle->setObjectName("subTitle");
    enTitle->setAlignment(Qt::AlignCenter);

    // 根据平台类型设置标题
    QString cnText, enText;
    switch (type) {
    case Platform_Distance:
        cnText = "测距原理实验平台";
        enText = "Distance measurement experimental platform";
        break;
    case Platform_Angle:
        cnText = "测角原理实验平台";
        enText = "Angle measurement experimental platform";
        break;
    case Platform_Speed:
        cnText = "测速原理实验平台";
        enText = "Speed measurement experimental platform";
        break;
    }

    cnTitle->setText(cnText);
    enTitle->setText(enText);

    titleLayout->addWidget(cnTitle);
    titleLayout->addWidget(enTitle);

    mainLayout->addWidget(titleArea, 0, Qt::AlignTop);

    // ====== 实验按钮区域 ======
    QWidget *linksWrapper = new QWidget(page);
    linksWrapper->setObjectName("linksWrapper");
    linksWrapper->setStyleSheet("QWidget#linksWrapper { background-color: transparent; }");

    QVBoxLayout *wrapperLayout = new QVBoxLayout(linksWrapper);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    // 半透明白色容器
    QWidget *linksBackground = new QWidget(linksWrapper);
    linksBackground->setObjectName("linksBackground");
    linksBackground->setMinimumSize(800, 400);
    linksBackground->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    linksBackground->setStyleSheet(R"(
        QWidget#linksBackground {
            background-color: rgba(255, 255, 255, 220);
            border-radius: 20px;
            padding: 20px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.05);
        }
    )");

    QGridLayout *gridLayout = new QGridLayout(linksBackground);
    gridLayout->setHorizontalSpacing(10);
    gridLayout->setVerticalSpacing(8);
    gridLayout->setContentsMargins(40, 30, 40, 30);

    // 根据平台类型获取实验列表（你可替换为你自己的数据）
    QStringList experiments;
    switch (type) {
    case Platform_Distance:
        experiments = {"实验科目1. 脉冲法距离测量-原理演示实验",
                       "实验科目2. 脉冲法距离测量-测试验证实验",
                       "实验科目3. 调频法距离测量-原理演示实验",
                       "实验科目4. 调频法距离测量-测试验证实验",
                       "实验科目5. 相位法距离测量-原理演示实验",
                       "实验科目6. 相位法距离测量-测试验证实验",
                       "实验科目7. 距离退模糊-原理演示实验",
                       "实验科目8. 距离退模糊-自主设计实验",
                       "实验科目9. 距离跟踪-原理演示实验",
                       "实验科目10. 距离跟踪-测试验证实验"};
        break;
    case Platform_Angle:
        experiments = {"实验科目1. 单脉冲法测角-原理演示实验",
                       "实验科目2. 单脉冲法测角-测试验证实验",
                       "实验科目3. 圆锥扫描法测角-原理演示实验",
                       "实验科目4. 圆锥扫描法测角-测试验证实验",
                       "实验科目5. 振幅比较法测角-原理演示实验",
                       "实验科目6. 振幅比较法测角-测试验证实验",
                       "实验科目7. 相位比较法测角-原理演示实验",
                       "实验科目8. 相位比较法测角-自主设计实验",
                       "实验科目9. 多普勒测角-原理演示实验",
                       "实验科目10. 多普勒测角-测试验证实验"};
        break;
    case Platform_Speed:
        experiments = {"实验科目1. 多普勒效应测速-原理演示实验",
                       "实验科目2. 多普勒效应测速-测试验证实验",
                       "实验科目3. 脉冲多普勒测速-原理演示实验",
                       "实验科目4. 脉冲多普勒测速-测试验证实验",
                       "实验科目5. 连续波多普勒测速-原理演示实验",
                       "实验科目6. 连续波多普勒测速-测试验证实验",
                       "实验科目7. 速度退模糊-原理演示实验",
                       "实验科目8. 速度退模糊-自主设计实验",
                       "实验科目9. 速度跟踪-原理演示实验",
                       "实验科目10. 速度跟踪-测试验证实验"};
        break;
    }

    // 创建按钮
    QList<QPushButton *> buttons;
    const int cols = 2;
    const int rows = (experiments.size() + cols - 1) / cols;

    for (int i = 0; i < experiments.size(); ++i) {
        QPushButton *btn = new QPushButton(experiments[i], linksBackground);
        btn->setObjectName("experimentButton");
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumHeight(50);

        int row = i % rows;
        int col = i / rows;
        gridLayout->addWidget(btn, row, col);

        // 第一个按钮设为选中状态
        if (i == 0) {
            btn->setProperty("selected", true);
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
        }

        buttons.append(btn);
    }

    wrapperLayout->addWidget(linksBackground);
    mainLayout->addWidget(linksWrapper, 1);

    // ====== 按钮样式 ======
    QString buttonStyle = QString(R"(
        QPushButton#experimentButton {
            background-color: white;
            border: 1px solid #D0D0D0;
            border-radius: 15px;
            padding: 12px 20px;
            text-align: left;
            color: #333;
            font-family: '%1', 'Microsoft YaHei', sans-serif;
            font-size: 16px;
            font-weight: normal;
        }
        QPushButton#experimentButton:hover {
            background-color: #E6F2FF;
            color: #1C5DA9;
        }
        QPushButton#experimentButton:pressed {
            background-color: #D0E5FF;
        }
        QPushButton#experimentButton[selected="true"] {
            background-color: #4A90E2;
            color: white;
            border: none;
        }
    )");

    page->setStyleSheet(page->styleSheet() + buttonStyle);

    // ====== 连接信号（示例：每个平台第一个按钮）=====
    if (!buttons.isEmpty()) {
        connect(buttons[0], &QPushButton::clicked, this, [this, type]() {
            qDebug() << "进入" << m_platformNames[type] << "的第一个实验";
            // 你可以在这里跳转到实验操作页
        });
    }

    return page;
}
void TeachingMainWindow::ensureSwitchPlatformButton()
{
    if (!m_switchPlatformBtn) {
        m_switchPlatformBtn = new QPushButton("切换到测角平台", this);
        m_switchPlatformBtn->setFixedSize(120, 30);
        m_switchPlatformBtn->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                color: transparent;
                border: none;
            }
        )");
        connect(m_switchPlatformBtn, &QPushButton::clicked, this, [this]() {
            if (!m_platformSwitchEnabled)
                return;
            int next = (m_currentPlatformIndex + 1) % 3;
            switchPlatform(next);
        });
        m_switchPlatformBtn->setEnabled(m_platformSwitchEnabled);
        m_switchPlatformBtn->raise();
    }
    updateSwitchButtonPosition();
}
void TeachingMainWindow::switchPlatform(int index)
{
    if (index >= 0 && index < m_platformPages.size()) {
        m_currentPlatformIndex = index;
        m_stackedWidget->setCurrentIndex(index);

        // 更新按钮文本
        int nextNext = (index + 1) % 3;
        m_switchPlatformBtn->setText("切换到" + m_platformNames[nextNext]);

        // 更新窗口标题
        setWindowTitle("气象雷达测量原理实验平台 - " + m_platformNames[index]);
    }
}
void TeachingMainWindow::updateSwitchButtonPosition()
{
    if (m_switchPlatformBtn && isVisible()) { // 👈 关键：检查指针和可见性
        int x = width() - 250;
        int y = 85;
        // 防止负坐标
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;
        m_switchPlatformBtn->move(x, y);
    }
}
void TeachingMainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateSwitchButtonPosition();
}
