#include "waveformdisplay.h"

waveformdisplay::waveformdisplay(QWidget *parent)
    : QWidget{parent}
{
#ifdef simulate
    e.seed(0);
#endif

    PlotLayout = new QVBoxLayout;
    fftLayout = new QGridLayout;
    PlotWidget = new QWidget;
    spectrumplot = new QCustomPlot;
    fftplot = new QCustomPlot;
    fftplotWidget = new QWidget;
    // fftwithcursorWidget = new QWidget;
    // x = QVector<double>(spectrumPoints);
    // y = QVector<double>(spectrumPoints);

    spectrumplot->addGraph(0);
    spectrumplot->setOpenGl(true);
    spectrumplot->graph(0)->setAntialiased(false);  // 关闭抗锯齿
    fftplot->addGraph(0);
    fftplot->graph(0)->setPen(QPen(Qt::blue));
    fftplot->setOpenGl(true);
    fftplot->graph(0)->setAntialiased(false);

    // fftplot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    // selectRect = new QCPSelectionRect(spectrumplot);
    xAxis_spectrum = spectrumplot->xAxis;
    yAxis_spectrum = spectrumplot->yAxis;
    xAxis_fft = fftplot->xAxis;
    yAxis_fft = fftplot->yAxis;
    xAxis_fft->setVisible(false);
    yAxis_fft->setVisible(false);
    xAxis_spectrum->setVisible(false);
    yAxis_spectrum->setVisible(false);
    xAxis_fft = fftplot->xAxis2;
    xAxis_fft->setVisible(true);
    spectrum = new QCPColorMap(xAxis_spectrum,yAxis_spectrum);
    spectrum->data()->setSize(spectrumPoints,61);
    spectrum->setGradient(QCPColorGradient::gpJet);

    // spectrumplot->setInteractions(QCP::iRangeZoom);
    // spectrumplot->setSelectionRectMode(QCP::srmZoom);

    spectrumplot->rescaleAxes();
    spectrumplot->replot();
    fftplot->rescaleAxes();
    fftplot->replot();
    cursor1 = new fftwithcursor(fftplot,this);
    cursor2 = new fftwithcursor(spectrumplot,this);
    // fftplotWidget->setLayout(fftLayout);
    PlotLayout->addWidget(fftplot,1);
    PlotLayout->addWidget(spectrumplot,3);

    // PlotLayout->addWidget(a);


    PlotWidget->setLayout(PlotLayout);

    setLayout(PlotLayout);
    connect(fftplot,&QCustomPlot::afterReplot,[this]() {
        cursor1->syncGeometry();
    });
    connect(spectrumplot,&QCustomPlot::afterReplot,[this]() {
        cursor2->syncGeometry();
    });
    connect(cursor1,&fftwithcursor::popCursorPosition,cursor2,&fftwithcursor::followCursorPosition);
    connect(cursor2,&fftwithcursor::popCursorPosition,cursor1,&fftwithcursor::followCursorPosition);
    connect(cursor1,&fftwithcursor::popCursorSize,cursor2,&fftwithcursor::followCursorSize);
    x = new QVector<double> (spectrumPoints);
    for(int i=0;i<spectrumPoints;i++)
    {
        x->data()[i]=i;
        spectrumData[i]=0;
    }
}
void waveformdisplay ::spectrum_update()
{

#ifdef simulate
    for(int i=0;i<spectrumPoints;i++)
    {
        spectrumData[i]=e();
    }
#endif
    if(value_spectrum.size()>60)
        value_spectrum.removeLast();
    QVector<double> y = QVector<double> (spectrumPoints);
    memcpy(y.data(),spectrumData,spectrumPoints*sizeof(double));
    value_spectrum.prepend(y);
    // fftplot
    // copy(spectrumData,spectrumData+spectrumPoints,begin(tmp));
    for(int i=0;i<value_spectrum.size();i++)
        for(int j=0;j<spectrumPoints;j++)
            spectrum->data()->setCell(j,i,value_spectrum[i][j]);
    fftplot->graph(0)->setData(*x,y);
    fftplot->graph(0)->rescaleAxes();
    fftplot->replot();
    spectrum->rescaleDataRange(true);
    spectrumplot->replot();


}
