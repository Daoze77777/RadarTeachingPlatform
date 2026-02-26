#include "mainWindow.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QStatusBar>
#include <QPainter>
#include "ExperimentManager.h"
#include "experimentSidebar.h"
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
    rightPanelLayout->setContentsMargins(5, 10, 5, 10);
    rightPanelLayout->setSpacing(15);
    rightPanelLayout->setAlignment(Qt::AlignTop);
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

    // 副标题
    // QLabel *subTitleLabel = new QLabel("请点击左侧步骤开始实验");
    // subTitleLabel->setStyleSheet("font-size: 24px; color:#345196;");
    // subTitleLabel->setAlignment(Qt::AlignCenter);

    welcomeLayout->addWidget(hiWidget);
    //welcomeLayout->addWidget(subTitleLabel);

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
    m_componentDetailWidget = setupComponentDetailWidget();
    m_centerStack->addWidget(m_componentDetailWidget);

    mainVLayout->addWidget(m_centerStack);
    return experimentPage;
}
QWidget* MainWindow::setupComponentDetailWidget()
{
    QWidget* detailWidget = new QWidget();
    detailWidget->setStyleSheet("background-color: transparent;");

    // 总垂直布局
    QVBoxLayout *mainVLayout = new QVBoxLayout(detailWidget);
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
    m_detailTitleLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #2D2F4F;");
    m_detailTitleLabel->setAlignment(Qt::AlignCenter);

    // 描述 (必须开启自动换行)
    m_detailDescLabel = new QLabel();
    m_detailDescLabel->setWordWrap(true); // 允许文字换行
    m_detailDescLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_detailDescLabel->setStyleSheet("font-size: 24px; color: #2D2F4F;");

    textLayout->addWidget(m_detailTitleLabel);
    textLayout->addSpacing(30);
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

    return detailWidget;
}
void MainWindow::onComponentSelected(const ExperimentContentItem& item, const QString& bottomImgPath)
{
    // 如果是课程总目录，则触发返回主页
    if (item.id == "home") {
        emit returnToHome();
        return; // 直接返回，不再执行后续的右侧页面渲染逻辑
    }

    // 1. 填充文本数据
    m_detailTitleLabel->setText(item.title);
    m_detailDescLabel->setText(item.description);

    // 2. 填充右上角图片 (按比例缩放并保持平滑)
    QPixmap topPix(item.imagePath);
    if (!topPix.isNull()) {
        // 注意：尺寸可以根据你的实际需求调整
        m_detailTopImage->setPixmap(topPix.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_detailTopImage->setText("暂无图片"); // 容错处理
    }

    // 3. 填充底部固定图片
    QPixmap bottomPix(bottomImgPath);
    if (!bottomPix.isNull()) {
        m_detailBottomImage->setPixmap(bottomPix.scaled(800, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // 4. 确保外层大 Stack 也切到了工作区
     if (m_mainCenterStack->currentIndex() != 1) {
         m_mainCenterStack->setCurrentIndex(1);
     }

     // 控制内层小 Stack 显示具体的“详情页”
     m_centerStack->setCurrentWidget(m_componentDetailWidget);
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
    connect(m_sidebar, &ExperimentSidebar::componentSelected,
            this, &MainWindow::onComponentSelected);

    // 5. 刷新数据
    this->setupExperimentContext(expId);
}
