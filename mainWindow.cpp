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
    m_stepCtrl = new StepController(this);
    connect(m_stepCtrl, &StepController::experimentFinished, this, [this]() {
        QMessageBox::information(this, "恭喜", "实验已完成！\n点击第一步可重新开始实验。");
    });

    initUI();
    setupExperimentContext(expId);
}

MainWindow::~MainWindow() {}

void MainWindow::initUI()
{
    setupTitleBar();
    setupWindowBase();
    setCustomMainWindowTitle(":/icons/resources/icons/title.png", "气象雷达测量原理实验平台");

    setupLeftSidebar();
    setupCenterArea();
    setupRightPanel();

    QWidget *bodyContainer = new QWidget;
    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyContainer);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(15);
    bodyLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    bodyLayout->addWidget(m_leftSidebarContainer);
    bodyLayout->addWidget(m_CenterContainer);
    bodyLayout->addWidget(m_rightPanelContainer);

    m_rootLayout->addWidget(bodyContainer);
    resize(1920, 1033);

    connect(m_sidebar, &ExperimentSidebar::componentSelected,
            this, &MainWindow::onComponentSelected);
}

void MainWindow::setupExperimentContext(int expId)
{
    m_sidebar->switchExperiment(expId);

    QString title = ExperimentManager::instance().getExperimentName(expId);
    if (m_experimentTitle)
        m_experimentTitle->setText(title);

    m_mainCenterStack->setCurrentIndex(0);

    m_isTestMode = ExperimentManager::instance().isTestMode(expId);
    int txCount  = ExperimentManager::instance().getTxCount(expId);

    m_instrumentPanel->setTargetLightCount(txCount);
    m_instrumentPanel->resetAllTargetLights();

    m_unlockedStepIndex = 0;
    m_stepItems = m_sidebar->getStepItems();

    m_stepCtrl->reset(m_stepItems.size());
    m_currentStepId = "";

    if (m_isTestMode)
        initSerial();
    else
        closeSerial();
}

void MainWindow::setupWindowBase()
{
    QWidget *mainContainer = this->centralWidget;
    if (!mainContainer) {
        mainContainer = new QWidget(this);
        setCentralWidget(mainContainer);
    }
    mainContainer->setObjectName("mainContainer");
    mainContainer->setStyleSheet("#mainContainer { background-image: none; background-color: #E9EEF8; }");
    m_rootContainer = mainContainer;

    QVBoxLayout *mainLayout = new QVBoxLayout(mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    m_rootLayout = mainLayout;

    if (this->titleBar) {
        mainLayout->addWidget(this->titleBar);
        setupWindowStyle();
    }
    setupMenu();
}

void MainWindow::setupTitleBar()
{
    setupCustomTitleBar();
}

void MainWindow::setupLeftSidebar()
{
    m_leftSidebarContainer = new QWidget(this);
    m_leftSidebarContainer->setFixedSize(270, 963);
    m_leftSidebarContainer->setObjectName("leftSidebarContainer");
    m_leftSidebarContainer->setStyleSheet("background-color: #FFFFFF;");

    m_leftSidebarContentLayout = new QVBoxLayout(m_leftSidebarContainer);
    m_leftSidebarContentLayout->setContentsMargins(0, 0, 0, 0);
    m_leftSidebarContentLayout->setSpacing(0);

    {
        QWidget *titleArea = new QWidget(m_leftSidebarContainer);
        titleArea->setFixedSize(270, 130);
        titleArea->setObjectName("titleArea");
        titleArea->setStyleSheet(R"(
        QWidget#titleArea {
            background-image: url(:/mainicons/resources/mainIcons/titleBG.png);
            background-repeat: no-repeat;
            background-position: center;
        })");

        QVBoxLayout *titleLayout = new QVBoxLayout(titleArea);
        titleLayout->setContentsMargins(0, 0, 0, 0);
        titleLayout->setSpacing(0);
        titleLayout->setAlignment(Qt::AlignCenter);

        m_experimentTitle = new QLabel("气象雷达原理实验平台", titleArea);
        m_experimentTitle->setFixedSize(239, 61);
        m_experimentTitle->setAlignment(Qt::AlignCenter);
        m_experimentTitle->setWordWrap(true);
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

    m_sidebar = new ExperimentSidebar(this);
    m_leftSidebarContentLayout->addWidget(m_sidebar);
}

void MainWindow::setupCenterArea()
{
    QWidget *centerContainer = new QWidget(this);
    centerContainer->setObjectName("centerContainer");
    centerContainer->setFixedSize(1419, 963);
    m_CenterContainer = centerContainer;

    QVBoxLayout *mainVLayout = new QVBoxLayout(centerContainer);
    mainVLayout->setContentsMargins(0, 10, 0, 10);
    mainVLayout->setSpacing(0);

    m_mainCenterStack = new QStackedWidget(centerContainer);
    m_mainCenterStack->setObjectName("mainCenterStack");

    QWidget *welcomePage = createWelcomePage();
    m_experimentPage     = createExperimentWorkPage();

    m_mainCenterStack->addWidget(welcomePage);      // index 0
    m_mainCenterStack->addWidget(m_experimentPage); // index 1
    m_mainCenterStack->setCurrentIndex(0);

    mainVLayout->addWidget(m_mainCenterStack);
}

void MainWindow::setupRightPanel()
{
    m_rightPanelContainer = new QWidget(this);
    m_rightPanelContainer->setFixedSize(200, 963);
    m_rightPanelContainer->setObjectName("rightPanelContainer");
    m_rightPanelContainer->setStyleSheet("QWidget#rightPanelContainer { background-color: #FFFFFF; }");

    QVBoxLayout *rightPanelLayout = new QVBoxLayout(m_rightPanelContainer);
    rightPanelLayout->setContentsMargins(0, 0, 0, 0);
    rightPanelLayout->setSpacing(0);
    rightPanelLayout->setAlignment(Qt::AlignTop);

    m_instrumentPanel = new InstrumentPanel(this);
    rightPanelLayout->addWidget(m_instrumentPanel);
}

QWidget* MainWindow::createWelcomePage()
{
    QWidget *page = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(page);
    mainLayout->setSpacing(15);

    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    QFrame *welcomeBox = new QFrame();
    welcomeBox->setObjectName("welcomeBox");
    welcomeBox->setStyleSheet(R"(
        QFrame#welcomeBox {
            background-color: #FFFFFF;
            border-radius: 10px;
            background-image: url(:/mainicons/resources/mainIcons/welcome.png);
            background-position: bottom right;
            background-repeat: no-repeat;
        }
    )");

    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeBox);
    welcomeLayout->setAlignment(Qt::AlignCenter);

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

    QFrame *tipsBox = new QFrame();
    tipsBox->setObjectName("tipsBox");
    tipsBox->setStyleSheet("QFrame#tipsBox { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *tipsLayout = new QVBoxLayout(tipsBox);
    tipsLayout->setContentsMargins(85, 36, 85, 64);
    tipsLayout->setSpacing(20);

    QLabel *tipsHeader = new QLabel();
    tipsHeader->setPixmap(QPixmap(":/mainicons/resources/mainIcons/hint.png").scaled(523, 93, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    tipsLayout->addWidget(tipsHeader);

    QStringList tipTexts = {"按照左侧步骤顺序进行操作", "每个步骤都有相应的操作界面",
                            "按照左侧步骤顺序进行操作", "每个步骤都有相应的操作界面"};
    for (const QString &t : tipTexts) {
        QLabel *stepLabel = new QLabel(t);
        stepLabel->setFixedHeight(51);
        stepLabel->setAlignment(Qt::AlignCenter);
        stepLabel->setStyleSheet(R"(
            QLabel {
                color:#2D2F4F;
                font-size: 22px;
                border-image: url(:/mainicons/resources/mainIcons/tishiwenzikuang.png);
            }
        )");
        tipsLayout->addWidget(stepLabel);
    }
    tipsLayout->addStretch();

    topLayout->addWidget(welcomeBox, 5);
    topLayout->addWidget(tipsBox, 5);

    QFrame *bottomBox = new QFrame();
    bottomBox->setObjectName("bottomBox");
    bottomBox->setStyleSheet("QFrame#bottomBox { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomBox);
    bottomLayout->setAlignment(Qt::AlignCenter);

    QLabel *placeholderLabel = new QLabel("实验原理将在这里显示");
    placeholderLabel->setFixedSize(430, 60);
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

    mainLayout->addWidget(topArea, 5);
    mainLayout->addWidget(bottomBox, 5);

    return page;
}

QWidget* MainWindow::createExperimentWorkPage()
{
    QWidget *experimentPage = new QWidget();
    QVBoxLayout *mainVLayout = new QVBoxLayout(experimentPage);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(15);

    m_centerStack = new QStackedWidget;

    m_componentDetailWidget = setupComponentDetailWidget();
    m_centerStack->addWidget(m_componentDetailWidget); // index 0

    m_stepDetailWidget = setupStepDetailWidget();
    m_centerStack->addWidget(m_stepDetailWidget);      // index 1

    mainVLayout->addWidget(m_centerStack);
    return experimentPage;
}

QWidget* MainWindow::setupComponentDetailWidget()
{
    QWidget *componentDetailWidget = new QWidget();
    componentDetailWidget->setStyleSheet("background-color: transparent;");

    QVBoxLayout *mainVLayout = new QVBoxLayout(componentDetailWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(15);

    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    QFrame *textFrame = new QFrame();
    textFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *textLayout = new QVBoxLayout(textFrame);
    textLayout->setContentsMargins(20, 20, 20, 20);
    textLayout->setSpacing(10);

    m_detailTitleLabel = new QLabel();
    m_detailTitleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2D2F4F;");
    m_detailTitleLabel->setAlignment(Qt::AlignCenter);

    m_detailDescLabel = new QLabel();
    m_detailDescLabel->setWordWrap(true);
    m_detailDescLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_detailDescLabel->setStyleSheet("font-size: 16px; color: #2D2F4F;");

    textLayout->addWidget(m_detailTitleLabel);
    textLayout->addWidget(m_detailDescLabel, 1);

    QFrame *imageFrame = new QFrame();
    imageFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *imageLayout = new QVBoxLayout(imageFrame);

    m_detailTopImage = new QLabel();
    m_detailTopImage->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
    m_detailTopImage->setAlignment(Qt::AlignCenter);
    imageLayout->addWidget(m_detailTopImage);

    topLayout->addWidget(textFrame, 5);
    topLayout->addWidget(imageFrame, 5);

    QFrame *bottomFrame = new QFrame();
    bottomFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomFrame);

    m_detailBottomImage = new QLabel();
    m_detailBottomImage->setStyleSheet("font-size: 22px; font-weight: bold; color: #333;");
    m_detailBottomImage->setAlignment(Qt::AlignCenter);
    bottomLayout->addWidget(m_detailBottomImage);

    // 新增动画组件，距离退模糊
    m_rangeDeblurAnim = new RangeDeblurAnimation;
    m_rangeDeblurAnim->setVisible(false);
    bottomLayout->addWidget(m_rangeDeblurAnim);

    mainVLayout->addWidget(topArea, 5);
    mainVLayout->addWidget(bottomFrame, 5);

    return componentDetailWidget;
}

QWidget* MainWindow::setupStepDetailWidget()
{
    QWidget *stepDetailWidget = new QWidget();
    stepDetailWidget->setStyleSheet("background-color: transparent;");

    QVBoxLayout *mainVLayout = new QVBoxLayout(stepDetailWidget);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(15);

    QWidget *topArea = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topArea);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->setSpacing(15);

    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->setContentsMargins(0, 0, 0, 0);
    leftPanelLayout->setSpacing(15);

    QFrame *promptFrame = new QFrame();
    promptFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *promptLayout = new QVBoxLayout(promptFrame);

    QLabel *fixedTitle = new QLabel("操作提示: ");
    fixedTitle->setAlignment(Qt::AlignTop);
    fixedTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #70A9FF;");
    promptLayout->addWidget(fixedTitle);

    m_stepPromptLabel = new QLabel();
    m_stepPromptLabel->setWordWrap(true);
    m_stepPromptLabel->setAlignment(Qt::AlignTop);
    m_stepPromptLabel->setStyleSheet("font-size: 16px; color: #2D2F4F;");
    promptLayout->addWidget(m_stepPromptLabel, 1);

    QFrame *actionImageFrame = new QFrame();
    actionImageFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *actionImageLayout = new QVBoxLayout(actionImageFrame);

    m_stepActionImage = new QLabel();
    m_stepActionImage->setAlignment(Qt::AlignCenter);

    m_radarRangingDisply = new RadarRangingDisplay;
    m_radarRangingDisply->setVisible(false);

    m_radarDistanceWidget = new RadarDistanceWidget;
    m_radarDistanceWidget->setVisible(false);

    actionImageLayout->addWidget(m_stepActionImage);
    actionImageLayout->addWidget(m_radarRangingDisply);
    actionImageLayout->addWidget(m_radarDistanceWidget);

    leftPanelLayout->addWidget(promptFrame, 5);
    leftPanelLayout->addWidget(actionImageFrame, 5);

    QFrame *chartFrame = new QFrame();
    chartFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *chartLayout = new QVBoxLayout(chartFrame);
    m_oscilloscope = new OscilloscopeWidget();
    chartLayout->addWidget(m_oscilloscope);

    topLayout->addWidget(leftPanel, 5);
    topLayout->addWidget(chartFrame, 5);

    QFrame *bottomFrame = new QFrame();
    bottomFrame->setStyleSheet("QFrame { background-color: #FFFFFF; border-radius: 10px; }");
    QVBoxLayout *bottomLayout = new QVBoxLayout(bottomFrame);
    m_stepBottomImage = new QLabel();
    m_stepBottomImage->setAlignment(Qt::AlignCenter);
    bottomLayout->addWidget(m_stepBottomImage);

    m_rangeDeblurAnimStep = new RangeDeblurAnimation;
    m_rangeDeblurAnimStep->setVisible(false);
    bottomLayout->addWidget(m_rangeDeblurAnimStep);

    mainVLayout->addWidget(topArea, 5);
    mainVLayout->addWidget(bottomFrame, 5);

    connect(m_radarRangingDisply, &RadarRangingDisplay::animationFinished,
            m_oscilloscope, &OscilloscopeWidget::onRadarAnimationFinished);

    connect(m_radarDistanceWidget, &RadarDistanceWidget::waveformRequested,
            this, [=](double timeUs) {
        if (timeUs < 0)
            m_oscilloscope->setData("");
        else
            m_oscilloscope->onDistanceWaveformRequested(timeUs);
    });

    connect(m_radarDistanceWidget, &RadarDistanceWidget::waveformRequested,
            this, [this](double timeUs) {
        if (timeUs < 0) return;
        m_stepCtrl->completeStep(m_stepCtrl->totalSteps() - 1);
    });

    return stepDetailWidget;
}

void MainWindow::onMenuExperimentSelected(int expId)
{
    if (this->m_currentExpId == expId) {
        qDebug() << "检测到重复点击相同实验，已拦截渲染请求。";
        return;
    }
    this->m_currentExpId = expId;

    if (m_sidebar) {
        m_leftSidebarContentLayout->removeWidget(m_sidebar);
        m_sidebar->deleteLater();
        m_sidebar = nullptr;
    }
    m_sidebar = new ExperimentSidebar(this);
    m_leftSidebarContentLayout->addWidget(m_sidebar);
    connect(m_sidebar, &ExperimentSidebar::componentSelected,
            this, &MainWindow::onComponentSelected);

    this->setupExperimentContext(expId);
}

void MainWindow::onComponentSelected(const ExperimentContentItem &item, const QString &bottomImgPath)
{
    if (item.id == "home") {
        emit returnToHome();
        return;
    }

    if (m_mainCenterStack->currentIndex() != 1)
        m_mainCenterStack->setCurrentIndex(1);

    // ===== Step 分支 =====
    if (item.moduleType == "Step") {
        //qDebug() << "[Step] id=" << item.id << "special=" << item.special << "waveform=" << item.waveform;
        if (m_isTestMode) {
            int clickedIndex = -1;

            for (int i = 0; i < m_stepItems.size(); ++i) {
                if (m_stepItems[i].id == item.id) {
                    clickedIndex = i;
                    break;
                }
            }

            if (clickedIndex == -1) return;

            if (m_stepCtrl->isFinished() && clickedIndex == 0) {
                m_stepCtrl->reset(m_stepItems.size());
                m_instrumentPanel->resetAllTargetLights();
            } else {
                QString errMsg;
                if (!m_stepCtrl->canGoTo(clickedIndex, errMsg)) {
                    QMessageBox::warning(this, "提示", errMsg);
                    return;
                }
                if (clickedIndex == 0)
                    m_stepCtrl->setCurrentStepIndex(0);
                else if (clickedIndex == m_stepCtrl->currentStepIndex() + 1)
                    m_stepCtrl->setCurrentStepIndex(clickedIndex);
            }
        }

        m_currentStepId = item.id;
        m_centerStack->setCurrentWidget(m_stepDetailWidget);

        m_stepPromptLabel->setText(item.description);
        QPixmap bottomPix(bottomImgPath);
        if (!bottomPix.isNull())
            m_stepBottomImage->setPixmap(
                bottomPix.scaled(m_stepBottomImage->size(),
                                 Qt::KeepAspectRatio, Qt::SmoothTransformation));

        // ===== special == "noLight"：不受灯控制，直接显示 =====
        // if (item.special == "noLight") {
        //     if (item.imagePath.isEmpty()) {
        //         m_stepActionImage->setVisible(false);
        //     } else {
        //         m_stepActionImage->setVisible(true);
        //     }
        //     //m_stepActionImage->setVisible(true);
        //     m_radarRangingDisply->setVisible(false);
        //     m_radarDistanceWidget->setVisible(false);
        //     QPixmap pix(item.imagePath);
        //     if (!pix.isNull())
        //         m_stepActionImage->setPixmap(pix.scaled(m_stepActionImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        //     m_oscilloscope->setData(item.waveform);
        //     return;
        // }
        if (item.special == "noLight") {
            m_radarRangingDisply->setVisible(false);
            m_radarDistanceWidget->setVisible(false);

            if (item.imagePath.isEmpty()) {
                m_stepActionImage->setVisible(false);
            } else {
                QPixmap pix(item.imagePath);
                if (pix.isNull()) {
                    m_stepActionImage->setVisible(false);
                } else {
                    m_stepActionImage->setVisible(true);
                    m_stepActionImage->setPixmap(
                        pix.scaled(m_stepActionImage->size(),
                                   Qt::KeepAspectRatio, Qt::SmoothTransformation));
                }
            }

            m_oscilloscope->setData(item.waveform);
            return;
        }

        // ===== special == "distanceMeasure"：显示测距控件 =====
        if (item.special == "distanceMeasure") {
            //qDebug() << "[s14] special=distanceMeasure 分支触发, isTestMode=" << m_isTestMode;
            m_stepActionImage->setVisible(false);
            if (m_isTestMode) {
                m_radarRangingDisply->setVisible(false);
                m_radarDistanceWidget->setVisible(true);
                m_oscilloscope->setData("");
            } else {
                m_radarDistanceWidget->setVisible(false);
                m_radarRangingDisply->setVisible(true);
                m_oscilloscope->setData(item.waveform);
            }
            return;
        }

        // ===== 普通步骤：根据灯状态显示 =====
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
            lightIsOn = true;
        }

        m_stepActionImage->setVisible(true);
        m_radarRangingDisply->setVisible(false);
        m_radarDistanceWidget->setVisible(false);

        if (lightIsOn) {
            QPixmap pix(item.imagePath);
            if (!pix.isNull())
                m_stepActionImage->setPixmap(
                    pix.scaled(m_stepActionImage->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation));
            else
                m_stepActionImage->setText("暂无图片");
            m_oscilloscope->setData(item.waveform); // ← waveform 字段
        } else {
            resetStepDisplay();
        }

        if (bottomImgPath == "deblurAnimation") {
            m_stepBottomImage->setVisible(false);
            m_rangeDeblurAnimStep->setVisible(true);
            m_rangeDeblurAnimStep->reset();
        } else {
            m_rangeDeblurAnimStep->setVisible(false);
            m_stepBottomImage->setVisible(true);
            QPixmap bottomPix(bottomImgPath);
            if (!bottomPix.isNull())
                m_stepBottomImage->setPixmap(
                    bottomPix.scaled(m_stepBottomImage->size(),
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
                topPix.scaled(m_detailTopImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else
            m_detailTopImage->setText("暂无图片");

        if (bottomImgPath == "deblurAnimation") {
            qDebug()<<bottomImgPath;
            m_detailBottomImage->setVisible(false);
            m_rangeDeblurAnim->setVisible(true);
            m_rangeDeblurAnim->reset();
        } else {
            m_rangeDeblurAnim->setVisible(false);
            m_detailBottomImage->setVisible(true);
            QPixmap bottomPix(bottomImgPath);
            if (!bottomPix.isNull())
                m_detailBottomImage->setPixmap(
                    bottomPix.scaled(m_detailBottomImage->size(),
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void MainWindow::resetStepDisplay()
{
    QPixmap defaultPix("resources/assets/MCFJLCL/26.png");
    if (!defaultPix.isNull())
        m_stepActionImage->setPixmap(
            defaultPix.scaled(m_stepActionImage->size(),
                              Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else
        m_stepActionImage->setText("等待信号接入...");

    // 用第一个步骤的 waveform 作为空坐标基准
    // （s1 的 waveform 就是该实验对应的空坐标类型）
    QString emptyWaveform = m_stepItems.isEmpty() ? "" : m_stepItems[0].waveform;
    m_oscilloscope->setData(emptyWaveform);
}

void MainWindow::initSerial()
{
    if (m_serial) return;
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
    m_lastRadarData = data;
    m_radarDistanceWidget->feedDistance(data.distance);

    if (!m_isTestMode || m_stepItems.isEmpty()) return;

    for (int i = 0; i < m_stepItems.size(); ++i) {
        const QString &txBit = m_stepItems[i].txBit;
        if (txBit.isEmpty()) continue;

        QStringList parts = txBit.split(':');
        if (parts.size() != 2) continue;

        quint16 mask = parts[1].toUShort();
        quint16 sVal = 0;
        if      (parts[0] == "s1") sVal = data.s1;
        else if (parts[0] == "s2") sVal = data.s2;
        else if (parts[0] == "s3") sVal = data.s3;

        bool isOn = (sVal & mask) != 0;

        // int tIndex = 0;
        // quint16 tmp = mask;
        // while (tmp > 1) { tmp >>= 1; tIndex++; }
        // tIndex += 1;

        // 改为：按步骤顺序，跳过 txBit 为空的步骤
        int tIndex = 0;
        for (int j = 0; j <= i; ++j) {
            if (!m_stepItems[j].txBit.isEmpty())
                tIndex++;
        }
        m_instrumentPanel->setLightColor(QString("T%1").arg(tIndex), isOn ? "green" : "gray");

        bool isOnMainWorkPage = (m_mainCenterStack->currentIndex() == 1);
        bool isOnStepPage     = (m_centerStack->currentWidget() == m_stepDetailWidget);
        bool isCurrentStep    = (m_stepItems[i].id == m_currentStepId);

        if (!isOnMainWorkPage || !isOnStepPage || !isCurrentStep) continue;

        if (isOn) {
            autoRefreshStep(m_stepItems[i]);
            // distanceMeasure 步骤不靠灯完成，由 waveformRequested 信号处理
            if (m_stepItems[i].special != "distanceMeasure") {
                m_stepCtrl->completeStep(i);
            }
        } else {
            resetStepDisplay();
        }
    }
}

// void MainWindow::autoRefreshStep(const ExperimentContentItem &item)
// {
//     m_stepPromptLabel->setText(item.description);

//     // distanceMeasure 步骤不走图片/波形刷新
//     if (item.special == "distanceMeasure") {
//         m_stepActionImage->setVisible(false);
//         m_radarRangingDisply->setVisible(false);
//         return;
//     }

//     m_stepActionImage->setVisible(true);
//     m_radarRangingDisply->setVisible(false);

//     QPixmap pix(item.imagePath);
//     if (!pix.isNull()) {
//         m_stepActionImage->setPixmap(
//             pix.scaled(m_stepActionImage->size(),
//                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     } else {
//         m_stepActionImage->setText("暂无图片");
//         m_stepActionImage->setAlignment(Qt::AlignCenter);
//     }
//     m_oscilloscope->setData(item.waveform); // ← waveform 字段
// }

void MainWindow::autoRefreshStep(const ExperimentContentItem &item)
{
    m_stepPromptLabel->setText(item.description);

    if (item.special == "distanceMeasure") {
        m_stepActionImage->setVisible(false);
        m_radarRangingDisply->setVisible(false);
        return;
    }

    m_radarRangingDisply->setVisible(false);

    if (item.imagePath.isEmpty()) {
        m_stepActionImage->setVisible(false);
    } else {
        QPixmap pix(item.imagePath);
        if (pix.isNull()) {
            m_stepActionImage->setVisible(false);
        } else {
            m_stepActionImage->setVisible(true);
            m_stepActionImage->setPixmap(
                pix.scaled(m_stepActionImage->size(),
                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    m_oscilloscope->setData(item.waveform);
}
