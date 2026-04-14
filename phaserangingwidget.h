#ifndef PHASERANGINGWIDGET_H
#define PHASERANGINGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtMath>

class PhaseRangingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PhaseRangingWidget(QWidget *parent = nullptr);
    void feedDistance(int rawDistance);
    void reset();
    void setManualInputEnabled(bool enabled) {
        m_spinDistance->setEnabled(enabled);
        m_btnStart->setEnabled(enabled);
    }

signals:
    void waveformRequested(double phase1, double phase2);

private slots:
    void onStartClicked();
    void onResetClicked();

private:
    void setupUI();
    void calculate(double R_km);

    QDoubleSpinBox *m_spinDistance;
    QPushButton    *m_btnStart;
    QPushButton    *m_btnReset;

    QLabel *m_lblR;
    QLabel *m_lblRmax1;
    QLabel *m_lblRmax2;
    QLabel *m_lblPhi1;
    QLabel *m_lblPhi2;
    QLabel *m_lblDeltaPhi;
    QLabel *m_lblRsolved;

    static constexpr double T1_US   = 650.0;
    static constexpr double T2_US   = 700.0;
    static constexpr double C_KM_US = 0.3;
    double m_Rmax1;
    double m_Rmax2;
};

#endif
