#ifndef FFT_CALCULATE_H
#define FFT_CALCULATE_H

#include <QObject>
// #include "fftw3.h"

class fft_calculate : public QObject
{
    Q_OBJECT
public:
    explicit fft_calculate(int Points, QObject *parent = nullptr);
    ~fft_calculate();
    // fftw_complex *in, *out;
    // fftw_plan plan;
    int fftPoints;
    float *output;
signals:
    void calculated();
public slots:
    void calculate();
};

#endif // FFT_CALCULATE_H
