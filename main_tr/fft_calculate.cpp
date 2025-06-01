#include "fft_calculate.h"
#define ABS(x) sqrt((x)[0]*(x)[0] + (x)[1]*(x)[1])
fft_calculate::fft_calculate(int Points, QObject *parent)
    : QObject{parent}
{
    fftPoints = Points;
    // in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftPoints);
    // out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fftPoints);
    // plan = fftw_plan_dft_1d(fftPoints, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    // output = new float[fftPoints];
}

void fft_calculate::calculate()
{
    // fftw_execute(plan);
    // for(int i=0;i<fftPoints;i++)
    //     output[i] = ABS(out[i]);

}
fft_calculate::~fft_calculate()
{
    // fftw_destroy_plan(plan);
    // delete output;
    // fftw_free(in);
    // fftw_free(out);
}
