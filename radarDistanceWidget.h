#ifndef RADARDISTANCEWIDGET_H
#define RADARDISTANCEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "qcustomplot.h"

// RadarDistanceWidget.h
class RadarDistanceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RadarDistanceWidget(QWidget *parent = nullptr);
    void feedDistance(int rawDistance);
    void reset();

signals:
    void waveformRequested(double timeNs); // 通知示波器画波形

private slots:
    void onCaptureClicked();
    void onStartClicked();
    void onResetClicked();

private:
    void setupUI();

    QLabel      *m_lblDistanceValue;
    QLabel      *m_lblTimeValue;
    QPushButton *m_btnCapture;
    QPushButton *m_btnStart;
    QPushButton *m_btnReset;

    int    m_rawDistance    = 0;
    double m_capturedMm     = 0.0;
    double m_capturedTimeNs = 0.0;
    bool   m_hasCaptured    = false;
};

#endif // RADARDISTANCEWIDGET_H
