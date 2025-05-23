#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    centralWidget = new QWidget;
    gridLayout = new QGridLayout;
    ConfigLayout = new QHBoxLayout;
    statistic = new QGroupBox;
    tab = new QTabWidget;

    //control widget
    {
        url_label = new QLabel;
        url_label->setText("url:");
        url = new QLineEdit;
        url->setText("ip:193.168.1.5");
        BW = new QLineEdit;
        BW->setFixedWidth(50);
        BW->setText("5");
        BW_label = new QLabel;
        BW_label->setText("BW(MHz):");
        fs = new QLineEdit;
        fs->setFixedWidth(50);
        fs->setText("10");
        // fs->setDisabled(true);

        fs_label = new QLabel;
        fs_label->setText("fs(MHz):");
        fc1 = new QLineEdit;
        fc1->setFixedWidth(50);
        fc1->setText("439.460");
        fc1_label = new QLabel;
        fc1_label->setText("RXfc(MHz):");
        fc2 = new QLineEdit;
        fc2->setFixedWidth(50);
        fc2->setText("433");
        fc2_label = new QLabel;
        fc2_label->setText("TXfc(MHz):");
        numthread = new QSpinBox;
        numthread->setRange(1,45);
        numthread->setValue(15);
        numthread_label = new QLabel;
        numthread_label->setText("Threads:");
        // url->setEnabled(false);
        // fs->setEnabled(false);
        // fc->setEnabled(false);
        // BW->setEnabled(false);
        select = new QPushButton;
        select->setText("Connect");
        ConfigLayout->addWidget(url_label,0);
        ConfigLayout->addWidget(url,10);
        ConfigLayout->addWidget(fs_label,0);
        ConfigLayout->addWidget(fs,1);
        ConfigLayout->addWidget(fc1_label,1);
        ConfigLayout->addWidget(fc1,1);
        ConfigLayout->addWidget(fc2_label,1);
        ConfigLayout->addWidget(fc2,1);
        ConfigLayout->addWidget(BW_label,1);
        ConfigLayout->addWidget(BW,1);
        ConfigLayout->addWidget(numthread_label,1);
        ConfigLayout->addWidget(numthread,4);
        ConfigLayout->addWidget(select,1);
    }

    // tab
    {

        {//table
            TableWidget = new QWidget;
            TableLayout = new QGridLayout;
            table = new QTableWidget;
            // table->setSortingEnabled(true);

            {
                adsb_frame_log_map[adsb_header::ICAO] = "ICAO";
                adsb_frame_log_map[adsb_header::OPERATOR] = "Operator";
                adsb_frame_log_map[adsb_header::OPERATORCallsign] = "OperatorCallsign";
                adsb_frame_log_map[adsb_header::model] = "Model";
                adsb_frame_log_map[adsb_header::DF] = "DF";
                adsb_frame_log_map[adsb_header::survive_time] = tr("LastSeen");
                adsb_frame_log_map[adsb_header::Altitude] = tr("Altitude");
                adsb_frame_log_map[adsb_header::Longitude] = tr("Longitude");
                adsb_frame_log_map[adsb_header::Latitude] = tr("Latitude");
                adsb_frame_log_map[adsb_header::Velocity] = tr("Velocity");
                // adsb_frame_log_map[adsb_header::Time] = "Time";
                adsb_frame_log_map[adsb_header::VS] = "Vertical status";
                adsb_frame_log_map[adsb_header::CA] = "Plane";
                adsb_frame_log_map[adsb_header::Message] = "Message";
                adsb_frame_log_map[adsb_header::CALLSIGN] = "CallSign";
                adsb_frame_log_map[adsb_header::CNT] = "Count";
                table->setColumnCount(adsb_frame_log_map.count());
                QStringList headerText;
                for(char i=0;i<adsb_frame_log_map.count();i++)
                    headerText.append(adsb_frame_log_map[i]);
                table->setHorizontalHeaderLabels(headerText);
                table->setRowCount(0);
            }
            TableLayout->addWidget(table);
            TableWidget->setLayout(TableLayout);
            tab->addTab(TableWidget ,"Tabel");
        }


        tab->addTab(TableWidget,"LogTable");

        {//TX stimulate
            TXWidget = new QWidget;
            TXLayout = new QGridLayout;
            QLabel *velocity_TX_label = new QLabel("Velocity:");
            QLabel *lat_TX_label = new QLabel("Latitude:");
            QLabel *lon_TX_label = new QLabel("Longitude:");
            TXLayout->addWidget(velocity_TX_label);
            TXLayout->addWidget(lon_TX_label);
            TXLayout->addWidget(lat_TX_label);
            TXWidget->setLayout(TXLayout);
            tab->addTab(TXWidget,"TX");
        }

    }

    //statistic
    {
        StaticticLayout = new QHBoxLayout;
        FrameCount = new QLabel("Frame Count:");
        Count = new QLabel("0");
        clear = new QPushButton("Clear");
        StaticticLayout->addWidget(FrameCount,1);
        StaticticLayout->addWidget(Count,1);
        StaticticLayout->addWidget(clear,1);
        statistic->setLayout(StaticticLayout);
    }
    //layout
    gridLayout->addLayout(ConfigLayout,0,0,1,1);
    gridLayout->addWidget(tab,1,0,12,1);
    gridLayout->addWidget(statistic,13,0,1,1);
    centralWidget->setLayout(gridLayout);
    this->setCentralWidget(centralWidget);
    // ad9361 = new board_cfg;
    // ad9361->ip = "193.168.1.5";
    // ad9361->config(5,10,433);
    // ui->setupUi(this);
    ad9361_started_flag=false;
    connect(select, &QPushButton::pressed, this, &MainWindow::onPushselect);
}
void MainWindow::onPushselect()
{
    if(!ad9361_started_flag)
    {
        ad9361_started_flag=~ad9361_started_flag;
        select->setText("Disconnect");
        QString url_content;
        url_content = url->text();
        QByteArray ba = url_content.toUtf8();
        QString BW_content = BW->text();
        QString fs_content = fs->text();
        QString fc1_content = fc1->text();
        QString fc2_content = fc2->text();
        ad9361 = new board_cfg;
        ad9361->ip = ba.data();
        ad9361->bw = BW_content.toFloat();
        ad9361->fs = fs_content.toFloat();
        ad9361->lo_rx = fc1_content.toFloat();
        ad9361->lo_tx = fc2_content.toFloat();
        ad9361->config(ad9361->bw,ad9361->fs,ad9361->lo_rx,ad9361->lo_tx);
        std::vector<std::complex<int16_t>> sine = generate_sine_wave(
            100e3,       // 正弦波频率，例如 100kHz
            10e6,     // 采样率，例如 2.5MHz
            0.00001,       // 生成数据的时长（秒）
            2047  // 最大幅度，int16范围：-32768~32767
            );
        // // // while(1)
        ad9361->start_send(sine);
        // /*rx start*/

    }
    else
    {
        ad9361->stop();
        // delete(ad9361);
        ad9361_started_flag=~ad9361_started_flag;
        select->setText("Connect");
    }

}

std::vector<std::complex<int16_t>> generate_sine_wave(
    float tone_freq_hz,       // 正弦波频率，例如 100kHz
    float sample_rate_hz,     // 采样率，例如 2.5MHz
    float duration_sec,       // 生成数据的时长（秒）
    float amplitude   // 最大幅度，int16范围：-32768~32767
    ) {
    size_t total_samples = static_cast<size_t>(duration_sec * sample_rate_hz);
    std::vector<std::complex<int16_t>> iq_data(total_samples);

    for (size_t n = 0; n < total_samples; ++n) {
        float phase = 2.0f * M_PI * tone_freq_hz * n / sample_rate_hz;
        int16_t i = static_cast<int16_t>(amplitude * cos(phase));
        int16_t q = static_cast<int16_t>(amplitude * sin(phase));
        iq_data[n] = std::complex<int16_t>(i, q);
    }

    return iq_data;
}

MainWindow::~MainWindow()
{
    delete ui;
}
