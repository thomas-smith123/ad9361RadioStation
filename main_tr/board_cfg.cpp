#include "board_cfg.h"

#ifdef simulate
#include "random"
#endif

board_cfg::board_cfg(QObject *parent, double buffer_size)
    : QObject{parent}
{
    // sharedresources = sharedresource;
    // ring_buffer = ringHandle;
    board_cfg::buffer_size = buffer_size;
    start_flag = false;
    // emit_signal_to_process = true;
    config_flag = 1;
    this->chunk_size = 1024*10;
    I0 = (int16_t*)malloc(sizeof(int16_t) * buffer_size);
    Q0 = (int16_t*)malloc(sizeof(int16_t) * buffer_size);
    I1 = (int16_t*)malloc(sizeof(int16_t) * buffer_size);
    Q1 = (int16_t*)malloc(sizeof(int16_t) * buffer_size);
}
void board_cfg::stop()
{
    iio_channel_disable(tx0_i);
    iio_channel_disable(tx0_q);
}
void board_cfg::config(float bw=5, float fs=10, float rx_lo=437, float tx_lo=433)
{
    iq_lock = new QMutex;
    stop_ = true;
    start_flag = 0;
    ctx = NULL;
    rx0_i = NULL;
    rx0_q = NULL;
    tx0_i = NULL;
    tx0_q = NULL;
    rxbuf = NULL;
    txbuf = NULL;

    // === RX 配置 ===
    rxcfg.bw_hz = MHZ(bw);
    rxcfg.fs_hz = MHZ(fs);
    rxcfg.lo_hz = MHZ(rx_lo);  // RX频率
    rxcfg.rfport = "A_BALANCED";

    // === TX 配置 ===
    txcfg.bw_hz = MHZ(bw);
    txcfg.fs_hz = MHZ(fs);
    txcfg.lo_hz = MHZ(tx_lo);  // TX频率
    txcfg.rfport = "A";

    if(this->config_flag)
        IIO_ENSURE((ctx = iio_create_context_from_uri(ip)) && "No context", "No context");

    if(this->config_flag)
        IIO_ENSURE(iio_context_get_devices_count(ctx) > 0 && "No devices", "No devices");

    if(this->config_flag)
    {
        // IIO_ENSURE(get_ad9361_stream_dev(ctx, RX, &rx) && "No rx dev found", "No rx dev found");
        IIO_ENSURE(get_ad9361_stream_dev(ctx, TX, &tx) && "No tx dev found", "No tx dev found");
    }

    if(this->config_flag)
    {
        // IIO_ENSURE(cfg_ad9361_streaming_ch(ctx, &rxcfg, RX, 0) && "RX port 0 not found", "RX port 0 not found");
        IIO_ENSURE(cfg_ad9361_streaming_ch(ctx, &txcfg, TX, 0) && "TX port 0 not found", "TX port 0 not found");
        printf("* Initializing AD9361 IIO streaming channels\n");
    }

    if(this->config_flag)
    {
        // IIO_ENSURE(get_ad9361_stream_ch(ctx, RX, rx, 0, &rx0_i) && "RX chan i not found", "RX chan i not found");
        // IIO_ENSURE(get_ad9361_stream_ch(ctx, RX, rx, 1, &rx0_q) && "RX chan q not found", "RX chan q not found");

        IIO_ENSURE(get_ad9361_stream_ch(ctx, TX, tx, 0, &tx0_i) && "TX chan i not found", "TX chan i not found");
        IIO_ENSURE(get_ad9361_stream_ch(ctx, TX, tx, 1, &tx0_q) && "TX chan q not found", "TX chan q not found");

        // iio_channel_enable(rx0_i);
        // iio_channel_enable(rx0_q);
        iio_channel_enable(tx0_i);
        iio_channel_enable(tx0_q);


        // rxbuf = iio_device_create_buffer(rx, board_cfg::buffer_size, false);
        txbuf = iio_device_create_buffer(tx, board_cfg::buffer_size, false);
    }

    if(this->config_flag)
    {
        // if (!rxbuf) {
        //     perror("Could not create RX buffer");
        //     shutdownDevice();
        // }
        // p_inc_rx = iio_buffer_step(rxbuf);
        // p_end_rx = (char*)iio_buffer_end(rxbuf);
        if (!txbuf) {
            perror("Could not create TX buffer");
            shutdownDevice();
        }
        p_inc_tx = iio_buffer_step(txbuf);
        p_end_tx = (char*)iio_buffer_end(txbuf);

    }
    // if (this->config_flag) {
    //     // Disable TX channels after config
    //     IIO_ENSURE(cfg_ad9361_disable_ch(ctx, TX, 0) && "TX chan fail to disable", "TX chan fail to disable");


    // }

}
void board_cfg::start_read()
{
    if(start_flag)
    {
        // ssize_t nbytes_rx;
        // char *p_dat, *p_end;
        // ptrdiff_t p_inc;

        while (!this->stop_)
        {
            // QMutexLocker locker(&sharedresources->mutex);
            // while (sharedresources->isProcessing) {
            //     sharedresources->condition.wait(&sharedresources->mutex);
            //     // if(this->stop_)break;
            // }
            long int cnt = 0;
            // if (sharedresources->emit_signal_to_process)
            {
                // Refill RX buffer
                nbytes_rx = iio_buffer_refill(rxbuf);
                if (nbytes_rx < 0) { printf("Error refilling buf %d\n", (int)nbytes_rx); shutdownDevice(); }
                // READ: Get pointers to RX buf and read IQ from RX buf port 0
                // p_inc = iio_buffer_step(rxbuf);
                // p_end = (char*)iio_sbuffer_end(rxbuf);
                for (p_dat_rx = (char*)iio_buffer_first(rxbuf, rx0_i); p_dat_rx < p_end_rx; p_dat_rx += p_inc_rx) {
                    // Example: swap I and Q
                    I0[cnt] = ((int16_t*)p_dat_rx)[0];// Real (I)
                    Q0[cnt] = ((int16_t*)p_dat_rx)[1];// Imag (Q)
                    // I1[cnt] = ((int16_t*)p_dat)[2];// Real (I)
                    // Q1[cnt] = ((int16_t*)p_dat)[3];// Imag (Q)
                    cnt++;
                }
                // sharedresources->emit_signal_to_process = false;
                // ring_buffer->receiveDataSlot(I0,Q0,board_cfg::buffer_size,rxcfg.fs_hz);
                emit read_onece_done(I0,Q0,board_cfg::buffer_size,rxcfg.fs_hz);
            }
        }
        qDebug()<<"exit reading";
    }

}
void board_cfg::start_send(const std::vector<std::complex<int16_t>>& iq_data)
{
    // this->cfg_ad9361_enable_ch(this->ctx, TX, 0);
    // size_t total_samples = iq_data.size();
    size_t sent = 0;
    ssize_t nbytes_tx;
    while (1)
    {

        // char* p_dat;
        // char* p_end;
        // ptrdiff_t p_inc;
        p_dat_tx = (char*)iio_buffer_first(txbuf, tx0_i);
        nbytes_tx = iio_buffer_push(txbuf);
        if (nbytes_tx < 0) {
            fprintf(stderr, "Error pushing TX buffer: %zd\n", (int) nbytes_tx);
            iio_buffer_destroy(txbuf);
            return;
        }
        // 当前要发送多少个样本（不超过chunk_size）
        // size_t this_chunk = std::min(this->chunk_size, total_samples - sent);

        // 创建临时 buffer（非循环发送，false）
        // struct iio_buffer* txbuf_chunk = iio_device_create_buffer(tx, this_chunk, false);
        // if (!txbuf_chunk) {
        //     fprintf(stderr, "Could not create TX buffer\n");
        //     return;
        // }

        // 填充 TX buffer（写入 IQ 数据）


        for (p_dat_tx = (char *)iio_buffer_first(txbuf, tx0_i); p_dat_tx < p_end_tx; p_dat_tx += p_inc_tx)
        {
            // 左对齐：12-bit 对应 AD9361 要求（<< 4）
            // int16_t i_val = iq_data[sent + i].real() << 4;
            // int16_t q_val = iq_data[sent + i].imag() << 4;
            // ((int16_t*)p_dat_rx)[0] = i_val;
            // ((int16_t*)p_dat_rx)[1] = q_val;
            ((int16_t*)p_dat_tx)[0] = 0 << 4;
            ((int16_t*)p_dat_tx)[1] = 0 << 4;
        }

        // 推送 TX buffer


        // 销毁临时 TX buffer
        // iio_buffer_destroy(txbuf);
        // sent += this_chunk;
    }
    // this->cfg_ad9361_disable_ch(this->ctx, TX, 0);
}
void board_cfg::reset_to_emmit()
{
    emit_signal_to_process = true;
}
board_cfg::~board_cfg()
{

    free(I0); free(Q0);
    free(I1); free(Q1);
    shutdownDevice();
}
void board_cfg::shutdownDevice()
{
    printf("* Destroying buffers\n");
    if (rxbuf) { iio_buffer_destroy(rxbuf); }
    if (txbuf) { iio_buffer_destroy(txbuf); }

    printf("* Disabling streaming channels\n");
    if (board_cfg::rx0_i) { iio_channel_disable(board_cfg::rx0_i); }
    if (board_cfg::rx0_q) { iio_channel_disable(board_cfg::rx0_q); }
    // if (board_cfg::rx1_i) { iio_channel_disable(board_cfg::rx1_i); }
    // if (board_cfg::rx1_q) { iio_channel_disable(board_cfg::rx1_q); }
    if (board_cfg::tx0_i) { iio_channel_disable(board_cfg::tx0_i); }
    if (board_cfg::tx0_q) { iio_channel_disable(board_cfg::tx0_q); }

    printf("* Destroying context\n");
    if (ctx) { iio_context_destroy(ctx); }
    exit(0);
}

/* check return value of attr_write function */
void board_cfg::errchk(int v, const char* what) {
    if (v < 0) { fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); shutdownDevice(); }
}

/* write attribute: long long int */
void board_cfg::wr_ch_lli(struct iio_channel* chn, const char* what, long long val)
{
    errchk(iio_channel_attr_write_longlong(chn, what, val), what);
}

/* write attribute: string */
void board_cfg::wr_ch_str(struct iio_channel* chn, const char* what, const char* str)
{
    errchk(iio_channel_attr_write(chn, what, str), what);
}

/* helper function generating channel names */
char* board_cfg::get_ch_name(const char* type, int id)
{
    snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
    return board_cfg::tmpstr;
}

/* returns ad9361 phy device */
struct iio_device* board_cfg::get_ad9361_phy(struct iio_context* ctx)
{
    struct iio_device* dev = iio_context_find_device(ctx, "ad9361-phy");
    IIO_ENSURE(dev && "No ad9361-phy found","No ad9361-phy found");
    return dev;
}

/* finds AD9361 streaming IIO devices */
bool board_cfg::get_ad9361_stream_dev(struct iio_context* ctx, enum iodev d, struct iio_device** dev)
{
    switch (d) {
    case TX: *dev = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc");
        return *dev != NULL;
    case RX: *dev = iio_context_find_device(ctx, "cf-ad9361-lpc");
        return *dev != NULL;
    default: IIO_ENSURE(0,"get_ad9361_stream_dev false");
        return false;
    }
}
bool board_cfg::get_ad9361_stream_ch(__notused struct iio_context* ctx, enum iodev d, struct iio_device* dev, int chid, struct iio_channel** chn)
{
    *chn = iio_device_find_channel(dev, get_ch_name("voltage", chid), d == TX);
    if (!*chn)
        *chn = iio_device_find_channel(dev, get_ch_name("altvoltage", chid), d == TX);
    return *chn != NULL;
}

/* finds AD9361 phy IIO configuration channel with id chid */
bool board_cfg::get_phy_chan(struct iio_context* ctx, enum iodev d, int chid, struct iio_channel** chn)
{
    switch (d) {
    case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), false); return *chn != NULL;
    case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("voltage", chid), true);  return *chn != NULL;
    default: IIO_ENSURE(0,"get_phy_chan false"); return false;
    }
}

/* finds AD9361 local oscillator IIO configuration channels */
bool board_cfg::get_lo_chan(struct iio_context* ctx, enum iodev d, struct iio_channel** chn)
{
    switch (d) {
        // LO chan is always output, i.e. true
    case RX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 0), true); return *chn != NULL;
    case TX: *chn = iio_device_find_channel(get_ad9361_phy(ctx), get_ch_name("altvoltage", 1), true); return *chn != NULL;
    default: IIO_ENSURE(0,"get_lo_chan false"); return false;
    }
}

/* applies streaming configuration through IIO */
bool board_cfg::cfg_ad9361_streaming_ch(struct iio_context* ctx, struct stream_cfg* cfg, enum iodev type, int chid)
{
    struct iio_channel* chn = NULL;

    // Configure phy and lo channels
    printf("* Acquiring AD9361 phy channel %d\n", chid);
    if (!get_phy_chan(ctx, type, chid, &chn)) { return false; }
    wr_ch_str(chn, "rf_port_select", cfg->rfport);
    wr_ch_lli(chn, "rf_bandwidth", cfg->bw_hz);
    wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);

    // Configure LO channel
    printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
    if (!get_lo_chan(ctx, type, &chn)) { return false; }
    wr_ch_lli(chn, "frequency", cfg->lo_hz);
    // wr_ch_str(chn, "powerdown", "1");
    return true;
}
bool board_cfg::cfg_ad9361_enable_ch(struct iio_context* ctx, enum iodev type, int chid)
{
    struct iio_channel* chn = NULL;

    // Configure phy and lo channels
    printf("* Acquiring AD9361 phy channel %d\n", chid);
    if (!get_lo_chan(ctx, type, &chn)) { return false; }
    wr_ch_str(chn, "powerdown", "0");
    return true;
}
bool board_cfg::cfg_ad9361_disable_ch(struct iio_context* ctx, enum iodev type, int chid)
{
    struct iio_channel* chn = NULL;

    // Configure phy and lo channels
    printf("* Acquiring AD9361 phy channel %d\n", chid);
    if (!get_lo_chan(ctx, type, &chn)) { return false; }
    wr_ch_str(chn, "powerdown", "1");
    return true;
}
void board_cfg::IIO_ENSURE(int expr, QString reason)
{
    if (!(expr)) { \
            QMessageBox::warning(NULL, "warning", reason, QMessageBox::Ok);\
            board_cfg::config_flag = 0;
    }
}
