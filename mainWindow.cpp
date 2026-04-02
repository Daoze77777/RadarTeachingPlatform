#include "mainWindow.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QStatusBar>
#include <QPainter>
#include "ExperimentManager.h"
#include "experimentSidebar.h"
#include "oscilloScopeWidget.h"
#include <QSerialPort>

MainWindow::MainWindow(int expId, QWidget *parent)
{
    initUI();
    setupExperimentContext(expId);
}
MainWindow::~MainWindow() {}
void MainWindow::initUI()
{
    //基础窗口设置
    setupTitleBar();
    setupWindowBase();

    // 设置自定义窗口标题和图标
    setCustomMainWindowTitle(":/icons/resources/icons/title.png", "气象雷达测量原理实验平台");

    //初始化各板块
    setupLeftSidebar();
    setupCenterArea();
    setupRightPanel();

    //创建Body内容容器
    QWidget *bodyContainer = new QWidget;

    //Body部分采用水平布局
    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyContainer);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(15);
    bodyLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop); // 左上对齐

    // 按顺序添加：左 -> 中 -> 右
    bodyLayout->addWidget(m_leftSidebarContainer);
    bodyLayout->addWidget(m_CenterContainer);
    bodyLayout->addWidget(m_rightPanelContainer);

    // 将Body加入主布局
    m_rootLayout->addWidget(bodyContainer);
    resize(1920, 1033);

    // 连接侧边栏发出的组合信号
    connect(m_sidebar, &ExperimentSidebar::componentSelected,
        this, &MainWindow::onComponentSelected);
}
void MainWindow::setupExperimentContext(int expId)
{
    // 1. 调用侧边栏的切换函数，并根据映射表加载对应的 XML
    m_sidebar->switchExperiment(expId);

    // 2. 获取标题并设置到 Label
    QString title = ExperimentManager::instance().getExperimentName(expId);
    if (m_experimentTitle) {
        m_experimentTitle->setText(title);
    }

    // 3. 页面重置：默认跳转到欢迎页
    m_mainCenterStack->setCurrentIndex(0);

    // ===== 新增 =====
    m_isTestMode = ExperimentManager::instance().isTestMode(expId);
    int txCount  = ExperimentManager::instance().getTxCount(expId);

    // 更新Tx灯数量（原理演示也更新，只是全灰）
    m_instrumentPanel->setTargetLightCount(txCount);
    m_instrumentPanel->resetAllTargetLights();

    // 重置步骤解锁状态
    m_unlockedStepIndex = 0;
    m_stepItems = m_sidebar->getStepItems(); // 新增

    m_totalSteps       = m_stepItems.size();
    m_completedSteps.clear();
    m_completedSteps.insert(0); // s1默认已完成，不需要灯
    m_currentStepIndex = 0;
    m_isFinished       = false;

    // 测试模式：初始化串口；原理演示：关闭串口
    if (m_isTestMode) {
        initSerial();
    } else {
        closeSerial();
    }
}
void MainWindow::setupWindowBase()
{
    //获取CentralWidget(父类构造函数中通常已经setCentralWidget了),如果父类没做，就自己new一个
    QWidget *mainContainer = this->centralWidget;
    if (!mainContainer)
    {
        mainContainer = new QWidget(this);
        setCentralWidget(mainContainer);
    }
    mainContainer->setObjectName("mainContainer");
    mainContainer->setStyleSheet("#mainContainer { background-image: none; background-color: #E9EEF8; }");
    m_rootContainer = mainContainer;

    //核心布局：顶层垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 彻底清空父类的巨大边距
    mainLayout->setSpacing(0);
    m_rootLayout = mainLayout;

    // 添加标题栏 (复用父类的成果)
    if (this->titleBar)
    {
        mainLayout->addWidget(this->titleBar);
        setupWindowStyle(); //设置样式表
    }
    setupMenu();
}
void MainWindow::setupTitleBar()
{
    setupCustomTitleBar();
}
void MainWindow::setupLeftSidebar()
{
    //创建左侧导航栏容器
    m_leftSidebarContainer = new QWidget(this);
    m_leftSidebarContainer->setFixedSize(270, 963);
    m_leftSidebarContainer->setObjectName("leftSidebarContainer");
    m_leftSidebarContainer->setStyleSheet("background-color: #FFFFFF;");

    //设置布局
    m_leftSidebarContentLayout = new QVBoxLayout(m_leftSidebarContainer);
    m_leftSidebarContentLayout->setContentsMargins(0, 0, 0, 0);
    m_leftSidebarContentLayout->setSpacing(0);

    // 标题区域
{   QWidget *titleArea = new QWidget(m_leftSidebarContainer);
    titleArea->setFixedSize(270,130);
    titleArea->setObjectName("titleArea");
    titleArea->setStyleSheet(R"(
    QWidget#titleArea {
        background-image: url(:/mainicons/resources/mainIcons/titleBG.png);
        background-repeat: no-repeat;
        background-position: center;
    })");

    // 标题区域的布局
    QVBoxLayout *titleLayout = new QVBoxLayout(titleArea);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(0);
    titleLayout->setAlignment(Qt::AlignCenter);

    // 添加主标题（垂直居中）- 根据实验类型动态设置
    //QString experimentTitle = getExperimentTitle(m_experimentType);
    m_experimentTitle = new QLabel("气象雷达原理实验平台", titleArea);
    m_experimentTitle->setFixedSize(239,61);
    m_experimentTitle->setAlignment(Qt::AlignCenter);
    m_experimentTitle->setWordWrap(true);  // 允许文字换行
    m_experimentTitle->setStyleSheet(R"(
    QLabel {
        color: #31314B;
        background: transparent;
        font-weight: 500;
        font-size: 24px;
    })");
    titleLayout->addWidget(m_experimentTitle);
    m_leftSidebarContentLayout->addWidget(titleArea);
}

    //创建折叠组
    m_sidebar = new ExperimentSidebar(this);
    m_leftSidebarContentLayout->addWidget(m_sidebar);
}
void MainWindow::setupCenterArea()
{
    // 1. 创建中间总容器
    QWidget *centerContainer = new QWidget(this);
    centerContainer->setObjectName("centerContainer");
    centerContainer->setFixedSize(1419, 963);
    //centerContainer->setStyleSheet("QWidget#centerContainer { background-color: orange; }");
    m_CenterContainer = centerContainer;

    // 总垂直布局：负责切分“上显示、下操作”
    QVBoxLayout *mainVLayout = new QVBoxLayout(centerContainer);
    mainVLayout->setContentsMargins(0, 10, 0, 10);
    mainVLayout->setSpacing(0);

    // 引入顶层堆叠容器，用于切换“欢迎页”和“工作页”
    m_mainCenterStack = new QStackedWidget(centerContainer);
    m_mainCenterStack->setObjectName("mainCenterStack");

    // 创建两个主要页面
    QWidget *welcomePage = createWelcomePage();       // 默认显示的欢迎页
    m_experimentPage = createExperimentWorkPage(); // 原来的雷达示波器页面

    // 加入堆叠容器
    m_mainCenterStack->addWidget(welcomePage);  //index 0
    m_mainCenterStack->addWidget(m_experimentPage);   //index 1

    // 默认显示欢迎页
    m_mainCenterStack->setCurrentIndex(0);
    mainVLayout->addWidget(m_mainCenterStack);
}
void MainWindow::setupRightPanel()
{
    // 1. 创建右侧固定宽度的容器
    m_rightPanelContainer = new QWidget(this);
    m_rightPanelContainer->setFixedSize(200, 963); // 根据你之前的 UI 设定宽度
    m_rightPanelContainer->setObjectName("rightPanelContainer");
    m_rightPanelContainer->setStyleSheet("QWidget#rightPanelContainer { background-color: #FFFFFF; }"); // 设置背景色，通常是稍微深一点的蓝色或灰色，以区分中央区域
    // 2. 创建垂直布局
    QVBoxLayout *rightPanelLayout = new QVBoxLayout(m_rightPanelContainer);
    //rightPanelLayout->setContentsMargins(5, 10, 5, 10);
    rightPanelLayout->setContentsMargins(0, 0, 0, 0);
    rightPanelLayout->setSpacing(0);
    rightPanelLayout->setAlignment(Qt::AlignTop);

    // 直接实例化我们封装好的面板
    m_instrumentPanel = new InstrumentPanel(this);
    rightPanelLayout->addWidget(m_instrumentPanel);
}
QWidget* MainWindow::createWelcomePage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    //mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // ================== 上半部分：欢迎区 + 提示区 ==================
    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    // --- 左侧：欢迎卡片 ---
    QFrame *welcomeBox = new QFrame();
    welcomeBox->setObjectName("welcomeBox");
    // 使用 QSS 设置背景白底圆角，并把 "welcome" 大字作为背景图片铺在右下角
    welcomeBox->setStyleSheet(R"(
        QFrame#welcomeBox {
            background-color: #FFFFFF;
            border-radius: 10px;
            /* 假设你有那个浅蓝色的 welcome 水印图片 */
            background-image: url(:/mainicons/resources/mainIcons/welcome.png);
            background-position: bottom right;
            background-repeat: no-repeat;
        }
    )");

    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeBox);
    welcomeLayout->setAlignment(Qt::AlignCenter); // 内容居中

    // Hi 图标和主标题
    QWidget *hiWidget = new QWidget();
    QHBoxLayout *hiLayout = new QHBoxLayout(hiWidget);
    hiLayout->setAlignment(Qt::AlignCenter);

    QLabel *hiIconLabel = new QLabel();
    hiIconLabel->setPixmap(QPixmap(":/mainicons/resources/mainIcons/Hi.png").scaled(88, 89, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel *hiTitleLabel = new QLabel("欢迎使用实验系统");
    hiTitleLabel->setStyleSheet("font-size: 34px; font-weight: bold; color:#345196; margin-left: 10px;");

    hiLayout->addWidget(hiIconLabel);
    hiLayout->addWidget(hiTitleLabel);
    welcomeLayout->addWidget(hiWidget);

    // --- 右侧：操作提示卡片 ---
    QFrame *tipsBox = new QFrame();
    tipsBox->setObjectName("tipsBox");
    tipsBox->setStyleSheet("QFrame#tipsBox { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *tipsLayout = new QVBoxLayout(tipsBox);
    tipsLayout->setContentsMargins(85, 36, 85, 64);
    tipsLayout->setSpacing(20);

    // 提示区头部 (机器人图标 + 标题)
    QLabel *tipsHeader = new QLabel();
    // 假设你把机器人和带有渐变背景的标题做成了一张整图，直接贴图最完美
    tipsHeader->setPixmap(QPixmap(":/mainicons/resources/mainIcons/hint.png").scaled(523, 93, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    tipsLayout->addWidget(tipsHeader);

    // 提示步骤条 (循环生成4个)
    QStringList tipTexts = {"按照左侧步骤顺序进行操作", "每个步骤都有相应的操作界面", "按照左侧步骤顺序进行操作", "每个步骤都有相应的操作界面"};
    for (int i = 0; i < tipTexts.size(); ++i) {
        QLabel *stepLabel = new QLabel(tipTexts[i]);
        stepLabel->setFixedHeight(51);
        stepLabel->setAlignment(Qt::AlignCenter);
        // 使用 QSS 设置你提供的步骤条 PNG 背景
        stepLabel->setStyleSheet(R"(
            QLabel {
                color:#2D2F4F ;
                font-size: 22px;
                /* 使用 border-image 可以让图片随 label 自动拉伸缩放 */
                border-image: url(:/mainicons/resources/mainIcons/tishiwenzikuang.png);

            }
        )");
        tipsLayout->addWidget(stepLabel);
    }
    tipsLayout->addStretch();

    // 将左右卡片加入上半部布局
    topLayout->addWidget(welcomeBox, 5);
    topLayout->addWidget(tipsBox, 5);

    // ================== 下半部分：操作区占位符 ==================
    QFrame *bottomBox = new QFrame();
    bottomBox->setObjectName("bottomBox");
    bottomBox->setStyleSheet("QFrame#bottomBox { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomBox);
    bottomLayout->setAlignment(Qt::AlignCenter);

    QLabel *placeholderLabel = new QLabel("实验原理将在这里显示");
    placeholderLabel->setFixedSize(430, 60); // 根据图片中那个胶囊状的背景设置大小
    placeholderLabel->setAlignment(Qt::AlignCenter);
    placeholderLabel->setStyleSheet(R"(
        QLabel {
            color: #2D2F4F;
            font-size: 24px;
            background-color:#CBD4EE;
            border-radius: 30px;

        }
    )");
    bottomLayout->addWidget(placeholderLabel);

    // 将上下两部分加入主布局
    mainLayout->addWidget(topArea, 5);
    mainLayout->addWidget(bottomBox, 5);

    return page;
}
QWidget* MainWindow::createExperimentWorkPage()
{
    QWidget *experimentPage = new QWidget();
    QVBoxLayout *mainVLayout = new QVBoxLayout(experimentPage);
    // 这里设为0，因为外层的 centerContainer 已经有边距了（或者按需调整）
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(15);

    // 创建内层 Stack
    m_centerStack = new QStackedWidget;

    //雷达组件、实验原理页面(index=0)
    m_componentDetailWidget = setupComponentDetailWidget();
    m_centerStack->addWidget(m_componentDetailWidget);

    //实验步骤页面(index=1)
    m_stepDetailWidget = setupStepDetailWidget();
    m_centerStack->addWidget(m_stepDetailWidget);

    mainVLayout->addWidget(m_centerStack);
    return experimentPage;
}
QWidget* MainWindow::setupComponentDetailWidget()
{
    QWidget* componentDetailWidget = new QWidget();
    componentDetailWidget->setStyleSheet("background-color: transparent;");

    // 总垂直布局
    QVBoxLayout *mainVLayout = new QVBoxLayout(componentDetailWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(15);

    // ================== 上半部分：左右分栏 ==================
    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    // --- 左侧：文字说明区 ---
    QFrame *textFrame = new QFrame();
    textFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *textLayout = new QVBoxLayout(textFrame);
    textLayout->setContentsMargins(20, 20, 20, 20);
    textLayout->setSpacing(10);

    // 标题
    m_detailTitleLabel = new QLabel();
    m_detailTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2D2F4F;");
    m_detailTitleLabel->setAlignment(Qt::AlignCenter);

    // 描述 (必须开启自动换行)
    m_detailDescLabel = new QLabel();
    m_detailDescLabel->setWordWrap(true); // 允许文字换行
    m_detailDescLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_detailDescLabel->setStyleSheet("font-size: 16px; color: #2D2F4F;");

    textLayout->addWidget(m_detailTitleLabel);
    textLayout->addWidget(m_detailDescLabel, 1); // 给描述部分更多拉伸空间

    // --- 右侧：对应图片区 ---
    QFrame *imageFrame = new QFrame();
    imageFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *imageLayout = new QVBoxLayout(imageFrame);

    m_detailTopImage = new QLabel();
    m_detailTopImage->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
    m_detailTopImage->setAlignment(Qt::AlignCenter);
    imageLayout->addWidget(m_detailTopImage);

    // 加入上半部布局 (5:5 等宽切分，可根据 UI 调整)
    topLayout->addWidget(textFrame, 5);
    topLayout->addWidget(imageFrame, 5);

    // ================== 下半部分：底部固定原理图 ==================
    QFrame *bottomFrame = new QFrame();
    bottomFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomFrame);

    m_detailBottomImage = new QLabel();
    m_detailBottomImage->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
    m_detailBottomImage->setAlignment(Qt::AlignCenter);
    bottomLayout->addWidget(m_detailBottomImage);

    // 把上下两部分加入总布局
    mainVLayout->addWidget(topArea, 5);     // 上面占一半
    mainVLayout->addWidget(bottomFrame, 5); // 下面占一半

    return componentDetailWidget;
}
QWidget* MainWindow::setupStepDetailWidget()
{
    QWidget* stepDetailWidget = new QWidget();
    stepDetailWidget->setStyleSheet("background-color: transparent;");

    // 总垂直布局 (上部+下部原理图)
    QVBoxLayout *mainVLayout = new QVBoxLayout(stepDetailWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(15);

    // 上半部分：左右分栏
    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    // 左侧：操作提示 + 操作图片 (上下结构)
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->setContentsMargins(0, 0, 0, 0);
    leftPanelLayout->setSpacing(15);

    // 1. 操作提示框
    QFrame *promptFrame = new QFrame();
    promptFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *promptLayout = new QVBoxLayout(promptFrame);

    // 操作提示 Label
    QLabel *fixedTitle = new QLabel("操作提示: ");
    fixedTitle->setAlignment(Qt::AlignTop);
    fixedTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #70A9FF;");
    promptLayout->addWidget(fixedTitle);

    //描述 Label
    m_stepPromptLabel = new QLabel();
    m_stepPromptLabel->setWordWrap(true);
    m_stepPromptLabel->setAlignment(Qt::AlignTop);
    m_stepPromptLabel->setStyleSheet("font-size: 16px; color: #2D2F4F;");
    promptLayout->addWidget(m_stepPromptLabel,1);


    // 2. 操作图片框
    QFrame *actionImageFrame = new QFrame();
    actionImageFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *actionImageLayout = new QVBoxLayout(actionImageFrame);
    m_stepActionImage = new QLabel();
    m_stepActionImage->setAlignment(Qt::AlignCenter);

    m_radarRangingDisply = new RadarRangingDisplay;
    m_radarRangingDisply->setVisible(false); // 初始状态隐藏

    m_radarDistanceWidget = new RadarDistanceWidget;
    m_radarDistanceWidget->setVisible(false);

    actionImageLayout->addWidget(m_stepActionImage);
    actionImageLayout->addWidget(m_radarRangingDisply);
    actionImageLayout->addWidget(m_radarDistanceWidget);
    leftPanelLayout->addWidget(promptFrame, 5);      // 提示占小部分高度
    leftPanelLayout->addWidget(actionImageFrame, 5); // 图片占大部分高度

    // --- 右侧：波形图表区 ---
    QFrame *chartFrame = new QFrame();
    chartFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *chartLayout = new QVBoxLayout(chartFrame);
    m_oscilloscope = new OscilloscopeWidget();
    chartLayout->addWidget(m_oscilloscope);

    topLayout->addWidget(leftPanel, 5);
    topLayout->addWidget(chartFrame, 5);

    // ================== 下半部分：底部固定原理图 ==================
    QFrame *bottomFrame = new QFrame();
    bottomFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomFrame);
    m_stepBottomImage = new QLabel();
    m_stepBottomImage->setAlignment(Qt::AlignCenter);
    bottomLayout->addWidget(m_stepBottomImage);

    mainVLayout->addWidget(topArea, 5);     // 上部高度占比
    mainVLayout->addWidget(bottomFrame, 5); // 下部高度占比

    connect(m_radarRangingDisply, &RadarRangingDisplay::animationFinished, m_oscilloscope,  &OscilloscopeWidget::onRadarAnimationFinished);
    connect(m_radarDistanceWidget, &RadarDistanceWidget::waveformRequested, this, [=](double timeNs) {
                if (timeNs < 0) {
                    m_oscilloscope->setData(""); // 重置时清空
                } else {
                    m_oscilloscope->onDistanceWaveformRequested(timeNs);
                }
            });
    connect(m_radarDistanceWidget, &RadarDistanceWidget::waveformRequested,
            this, [this](double timeNs) {
                if (timeNs < 0) return; // 重置操作，忽略

                // 标记最后一步完成
                int lastIndex = m_totalSteps - 1;
                m_completedSteps.insert(lastIndex);
                m_currentStepIndex = lastIndex;

                if (!m_isFinished) {
                    m_isFinished = true;
                    QMessageBox::information(this, "恭喜", "实验已完成！\n点击第一步可重新开始实验。");
                }
            });

    return stepDetailWidget;
}
void MainWindow::onMenuExperimentSelected(int expId)
{
    //  1. 增加拦截：防止重复加载相同实验
    if (this->m_currentExpId == expId) {
        qDebug() << "检测到重复点击相同实验，已拦截渲染请求。";
        return;
    }
    this->m_currentExpId = expId; // 更新当前 ID

    // 2. 彻底销毁旧的侧边栏
    if (m_sidebar) {
        m_leftSidebarContentLayout->removeWidget(m_sidebar);
        m_sidebar->deleteLater();
        m_sidebar = nullptr;
    }
    // 3. 重新创建侧边栏实例 (冷启动)
    m_sidebar = new ExperimentSidebar(this);
    m_leftSidebarContentLayout->addWidget(m_sidebar);

    // 4. 重新连接侧边栏发出的组合信号
    connect(m_sidebar, &ExperimentSidebar::componentSelected, this, &MainWindow::onComponentSelected);

    // 5. 刷新数据
    this->setupExperimentContext(expId);
}
void MainWindow::onComponentSelected(const ExperimentContentItem& item, const QString& bottomImgPath)
{
    // 课程总目录：返回主页
    if (item.id == "home") {
        emit returnToHome();
        return;
    }

    // 确保切到工作区
    if (m_mainCenterStack->currentIndex() != 1){
        m_mainCenterStack->setCurrentIndex(1);
    }

    // ===== Step 分支 =====
    if (item.moduleType == "Step") {
        //m_currentStepId = item.id;
        //m_centerStack->setCurrentWidget(m_stepDetailWidget);

        // ===== 测试模式跳步检测 =====
        if (m_isTestMode) {
            int clickedIndex = -1;
            for (int i = 0; i < m_stepItems.size(); ++i) {
                if (m_stepItems[i].id == item.id) {
                    clickedIndex = i;
                    break;
                }
            }
            if (clickedIndex == -1) return;

            // 已完成最后一步：允许点击第一步重新开始
            if (m_isFinished && clickedIndex == 0) {
                qDebug() << "[重置] 触发重置，清空completedSteps";
                m_currentStepIndex = 0;
                m_isFinished = false;
                m_completedSteps.clear();
                m_completedSteps.insert(0);
                m_instrumentPanel->resetAllTargetLights();
                // 继续正常渲染s1
            }
            // 已完成但点的不是第一步
            else if (m_isFinished && clickedIndex != 0) {
                QMessageBox::information(this, "提示",
                                         "实验已完成！\n点击第一步可重新开始实验。");
                return; // 注意：这里return，m_currentStepId还没更新，停留原步骤
            }
            // 未完成：检测是否允许点击
            else if (!m_isFinished) {
                // s1(index=0)不检测，直接放行
                if (clickedIndex == 0) {
                    // 允许
                    m_currentStepIndex = 0;          // ← 新增
                    m_completedSteps.insert(0);      // ← 新增，确保s1始终标记为已完成
                }
                // 点击当前步骤：允许
                else if (clickedIndex == m_currentStepIndex) {
                    // 允许
                }
                // 点击下一步：需要当前步骤已完成
                else if (clickedIndex == m_currentStepIndex + 1
                         && m_completedSteps.contains(m_currentStepIndex)) {
                     m_currentStepIndex = clickedIndex; // 新增：更新当前步骤索引
                    // 允许
                }
                // 其他情况：拦截，不更新m_currentStepId
                else {
                    QString msg;
                    if (!m_completedSteps.contains(m_currentStepIndex)) {
                        msg = QString("请先完成当前步骤「%1」！")
                                  .arg(m_stepItems[m_currentStepIndex].title);
                    } else {
                        msg = QString("请按顺序进行实验！\n请先完成「%1」。")
                                  .arg(m_stepItems[m_currentStepIndex].title);
                    }
                    QMessageBox::warning(this, "提示", msg);
                    return; // return前m_currentStepId还没更新，停留原步骤
                }
            }
        }
        m_currentStepId = item.id;
        m_centerStack->setCurrentWidget(m_stepDetailWidget);
            // 已完成但点的不是第一步：提示可以重新开始
            // else if (m_isFinished && clickedIndex != 0) {
            //     QMessageBox::information(this, "提示",
            //                              "实验已完成！\n点击第一步可重新开始实验。");
            //     return;
            // }
        //}

        // 填充操作提示和底部原理图（所有步骤都填）
        m_stepPromptLabel->setText(item.description);
        QPixmap bottomPix(bottomImgPath);
        if (!bottomPix.isNull())
            m_stepBottomImage->setPixmap(
                bottomPix.scaled(m_stepBottomImage->size(),
                                 Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // ===== s1：写死，不受灯控制 =====
        if (item.id == "s1") {
            m_stepActionImage->setVisible(true);
            m_radarRangingDisply->setVisible(false);
            m_radarDistanceWidget->setVisible(false);
            QPixmap pix(item.imagePath);
            if (!pix.isNull())
                m_stepActionImage->setPixmap(
                    pix.scaled(m_stepActionImage->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation));
            m_oscilloscope->setData("");
            return;
        }

        // ===== s14：默认显示雷达动画，示波器等串口数据 =====
        if (item.id == "s14") {
            m_stepActionImage->setVisible(false);
            if (m_isTestMode) {
                // 测试验证：显示距离测量控件
                m_radarRangingDisply->setVisible(false);
                m_radarDistanceWidget->setVisible(true);
                m_oscilloscope->setData("");
            } else {
                // 原理演示：显示雷达动画
                m_radarDistanceWidget->setVisible(false);
                m_radarRangingDisply->setVisible(true);
                m_oscilloscope->setData("s14");
            }
            return;
        }

        // ===== s2~s13 =====
        // 判断对应灯是否已亮
        bool lightIsOn = false;
        if (m_isTestMode && !item.txBit.isEmpty()) {
            QStringList parts = item.txBit.split(':');
            if (parts.size() == 2) {
                quint16 mask = parts[1].toUShort();
                quint16 sVal = 0;
                if      (parts[0] == "s1") sVal = m_lastRadarData.s1;
                else if (parts[0] == "s2") sVal = m_lastRadarData.s2;
                else if (parts[0] == "s3") sVal = m_lastRadarData.s3;
                lightIsOn = (sVal & mask) != 0;
            }
        } else if (!m_isTestMode) {
            // 原理演示：直接显示真实内容
            lightIsOn = true;
        }

        m_stepActionImage->setVisible(true);
        m_radarRangingDisply->setVisible(false);
        m_radarDistanceWidget->setVisible(false);

        if (lightIsOn) {
            // 灯已亮：显示真实图片和波形
            QPixmap pix(item.imagePath);
            if (!pix.isNull())
                m_stepActionImage->setPixmap(
                    pix.scaled(m_stepActionImage->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation));
            else
                m_stepActionImage->setText("暂无图片");
            m_oscilloscope->setData(item.id);
        } else {
            // 灯未亮：显示默认图片和空坐标
            QPixmap defaultPix("resources/assets/MCFJLCL/26.png");
            if (!defaultPix.isNull())
                m_stepActionImage->setPixmap(
                    defaultPix.scaled(m_stepActionImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            else
                m_stepActionImage->setText("等待信号接入...");
            m_oscilloscope->setData("");
        }
    }
    // ===== 普通组件/原理 分支 =====
    else {
        m_centerStack->setCurrentWidget(m_componentDetailWidget);

        m_detailTitleLabel->setText(item.title);
        m_detailDescLabel->setText(item.description);

        QPixmap topPix(item.imagePath);
        if (!topPix.isNull())
            m_detailTopImage->setPixmap(
                topPix.scaled(m_detailTopImage->size(),
                              Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else
            m_detailTopImage->setText("暂无图片");

        QPixmap bottomPix(bottomImgPath);
        if (!bottomPix.isNull())
            m_detailBottomImage->setPixmap(
                bottomPix.scaled(m_detailBottomImage->size(),
                                 Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::initSerial()
{
    if (m_serial) return; // 防止重复初始化
    m_serial = new QSerialPort(this);
    m_parser = new RadarDataParser(this);

    m_serial->setPortName("COM7");
    m_serial->setBaudRate(QSerialPort::Baud9600);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);

    if (m_serial->open(QIODevice::ReadOnly)) {
        connect(m_serial, &QSerialPort::readyRead, this, [=]() {
            m_parser->feedData(m_serial->readAll());
        });
        connect(m_parser, &RadarDataParser::dataReady,
                this, &MainWindow::onRadarDataReceived);
    }
}

void MainWindow::closeSerial()
{
    if (m_serial && m_serial->isOpen()) {
        m_serial->close();
        qDebug() << "[串口] 已关闭";
    }
}

void MainWindow::onRadarDataReceived(const RadarData &data)
{
    m_lastRadarData = data; // 缓存最新数据，供点击步骤时查询
    m_radarDistanceWidget->feedDistance(data.distance); // 实时喂入距离

    if (!m_isTestMode || m_stepItems.isEmpty()) return;

    for (int i = 0; i < m_stepItems.size(); ++i) {
        const QString& txBit = m_stepItems[i].txBit;
        if (txBit.isEmpty()) continue;

        QStringList parts = txBit.split(':');
        if (parts.size() != 2) continue;

        quint16 mask = parts[1].toUShort();
        quint16 sVal = 0;
        if      (parts[0] == "s1") sVal = data.s1;
        else if (parts[0] == "s2") sVal = data.s2;
        else if (parts[0] == "s3") sVal = data.s3;

        bool isOn = (sVal & mask) != 0;

        // T灯编号计算
        int tIndex = 0;
        quint16 tmp = mask;
        while (tmp > 1) { tmp >>= 1; tIndex++; }
        tIndex += 1;
        QString tId = QString("T%1").arg(tIndex);

        // T灯始终更新
        m_instrumentPanel->setLightColor(tId, isOn ? "green" : "gray");

        // ===== 核心判断：灯亮 + 在步骤页 + 当前步骤和灯对应 =====
        if (isOn) {
            bool isOnMainWorkPage = (m_mainCenterStack->currentIndex() == 1);
            bool isOnStepPage     = (m_centerStack->currentWidget() == m_stepDetailWidget);
            bool isCurrentStep    = (m_stepItems[i].id == m_currentStepId);

            if (isOn && isOnMainWorkPage && isOnStepPage && isCurrentStep) {
                autoRefreshStep(m_stepItems[i]);

                // 更新当前步骤索引
                m_currentStepIndex = i;
                m_completedSteps.insert(i); // 标记该步骤已完成
                // 如果是最后一步，标记完成
                if (i == m_totalSteps - 1 && !m_isFinished  && m_stepItems[i].id != "s14") {
                    m_isFinished = true;
                    QMessageBox::information(this, "恭喜","实验已完成！\n点击第一步可重新开始实验。");
                }
            }
        }
    }
}

void MainWindow::autoRefreshStep(const ExperimentContentItem &item)
{
    m_stepPromptLabel->setText(item.description);

    if (item.id == "s14") {
        m_stepActionImage->setVisible(false);
        m_radarRangingDisply->setVisible(false);
        return; // 直接return，不调用setData
    }

    m_stepActionImage->setVisible(true);
    m_radarRangingDisply->setVisible(false);
    QPixmap pix(item.imagePath);
    if (!pix.isNull()) {
        m_stepActionImage->setPixmap(
            pix.scaled(m_stepActionImage->size(),
                       Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_stepActionImage->setText("暂无图片");
        m_stepActionImage->setAlignment(Qt::AlignCenter);
    }

    m_oscilloscope->setData(item.id); // s14不会走到这里
}
