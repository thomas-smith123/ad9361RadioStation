#ifndef WAVEFORMDISPLAY_H
#define WAVEFORMDISPLAY_H

#include <QObject>
#include <QWidget>
#include <QWidget>
#include "QVBoxLayout"
#include "qcustomplot/qcustomplot.h"
#include "fftwithcursor.h"

class waveformdisplay : public QWidget
{
    Q_OBJECT
public:
    explicit waveformdisplay(QWidget *parent = nullptr);
    QWidget *PlotWidget, *fftplotWidget;
    QGridLayout *fftLayout;
    QVBoxLayout *PlotLayout;
    QCustomPlot *spectrumplot, *fftplot;
    QCPColorMap *spectrum;
    QCPAxis *xAxis_spectrum,*yAxis_spectrum, *xAxis_fft,*yAxis_fft;
    QList<QVector<double>> value_spectrum;
    int fftPoints=1024;
    int spectrumPoints = fftPoints;
    double *spectrumData = new double[spectrumPoints];
    QVector<double> *x;
    fftwithcursor *cursor1, *cursor2;
#ifdef simulate
    std::default_random_engine e;
#endif
signals:

public slots:
    void spectrum_update();
};

#endif // WAVEFORMDISPLAY_H
