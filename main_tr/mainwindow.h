#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QGridLayout"
#include "QHBoxLayout"
#include "QGroupBox"
#include "QTabWidget"
#include "QLabel"
#include "QLineEdit"
#include "QPushButton"
#include "QTableWidget"
#include "QTextBrowser"
#include "QSpinBox"
#include "board_cfg.h"

#include "QTimer"
#include "QList"
#include "waveformdisplay/waveformdisplay.h"
#include "configWidget/modulationselect.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QWidget *centralWidget, *MapWidget, *TableWidget, *FrameLogWidget, *TXWidget;
    waveFormDisplay *waveformDisplay;
    //layout
    QGridLayout *gridLayout, *MapLayout, *TableLayout, *FrameLogLayout, *TXLayout;
    QHBoxLayout *ConfigLayout, *StaticticLayout;
    QGroupBox *statistic;
    QTabWidget *tab;
    //widget
    QLabel *url_label, *fs_label, *fc1_label, *fc2_label, *BW_label, *real_label, *imag_label, *moulde_label, *numthread_label;
    QLabel *FrameCount, *Count;
    QLineEdit *url, *fs, *fc1, *fc2, *BW;
    QSpinBox *numthread;
    QPushButton *select, *clear;
    QTableWidget *table;
    QTextBrowser *log;
    //tab
    QWidget *tableWidget;
    QMap<int,QString> adsb_frame_log_map;
    QMenuBar *menuBar();
    // QMap<std::string, struct ADSBFrame> buff;

    bool ad9361_started_flag;
    board_cfg *ad9361;
    QTimer *refreshSpectrum;


    // QCPSelectionRect *selectRect;

    // FUNCTIONS


private:

    Ui::MainWindow *ui;

public slots:

    void onPushselect();
};

enum adsb_header{
    ICAO=0,
    DF,
    OPERATOR,
    OPERATORCallsign,
    model,
    survive_time,
    Altitude,
    Longitude,
    Latitude,
    Velocity,
    // Time,
    VS,
    CA,
    Message,
    CALLSIGN,
    CNT,
};

std::vector<std::complex<int16_t>> generate_sine_wave(
    float tone_freq_hz,       // 正弦波频率，例如 100kHz
    float sample_rate_hz,     // 采样率，例如 2.5MHz
    float duration_sec,       // 生成数据的时长（秒）
    float amplitude  // 最大幅度，int16范围：-32768~32767
    );
#endif // MAINWINDOW_H
