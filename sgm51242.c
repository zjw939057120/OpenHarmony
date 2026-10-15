#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

/* ---------- 寄存器地址定义 ---------- */
#define REG_NOP             0x00
#define REG_ADC_SEQ         0x02
#define REG_ADC_CTRL        0x03
#define REG_ADC_PIN_CFG     0x04
#define REG_PULL_DOWN       0x06
#define REG_READBACK        0x07
#define REG_PD_REF_CTRL     0x0B

/* ---------- 软件复位字 ---------- */
#define SW_RESET_WORD       0x7DAC      /* 软件复位指令 */

//外部参考电压
#define VREF_EXT 4.066f
#define SPI_DEVICE "/dev/spidev4.0"

// ===== 0-10V 输入分压校准 =====
// 硬件: V_in(0-10V) --[R1]-- INx --[R2]-- GND
#define DIVIDER_R1     220.0f   // 串联电阻 (KΩ)
#define DIVIDER_R2     150.0f   // 并联电阻 (KΩ)
#define V_INPUT_FSR    10.0f    // 输入电压满量程 (V)
// 理论分压比 = R2 / (R1 + R2)
#define DIVIDER_RATIO_THEORY  (DIVIDER_R2 / (DIVIDER_R1 + DIVIDER_R2))

// 两点校准结构（生产时实测填入，精度高于单点校准）
typedef struct {
    float v_adc_at_0v;    // 输入 0V 时的 ADC 读数（去掉 ADC offset）
    float v_adc_at_10v;   // 输入 10V 时的 ADC 读数（确定分压比 + gain）
} divider_calib_t;

// 8 通道独立校准数据（先用占位值，测完后填实际值）
static const divider_calib_t g_divider_calib[8] = {
    [0] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN0 待测
    [1] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN1 待测
    [2] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN2 待测
    [3] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN3 待测
    [4] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN4 待测
    [5] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN5 待测
    [6] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN6 待测
    [7] = {.v_adc_at_0v = 0.000f, .v_adc_at_10v = 4.000f},  // IN7 待测
};

// V_adc → V_in(0-10V) 两点校准（精度高, 消掉电阻精度 + ADC offset）
// 温度补偿在应用层做, 这里只输出原始校准后的 V_in
static inline float adc_to_input_calib(float v_adc, const divider_calib_t* cal) {
    float span = cal->v_adc_at_10v - cal->v_adc_at_0v;
    if (span < 1e-6f) return 0.0f;  // 防止除零
    return (v_adc - cal->v_adc_at_0v) / span * V_INPUT_FSR;
}

int spi_fd = -1;
uint32_t spi_speed = 100000;

// GPIO组定义
int gpio_group[7][3] = {
    {284,285,286},
    {287,256,257},
    {258,259,260},
    {261,262,263},
    {264,265,266},
    {267,268,269},
    {270,271,52}
};
// GPIO组数量
int gpio_group_count = 7;
// GPIO数量
int gpio_count = 3;

/**
 * @brief 设置GPIO组模式
 * 
 * @param group GPIO组指针
 * 模式1：DI开关信号输入，GPIO配置：011，短接输入：电压为0V，断开输入：电压为：4.13V
 * 模式2：AI电流信号输入，GPIO配置：101，0mA输入：电压为0V，10mA输入：电压为2.7V，20mA输入：电压为：4.13V
 * 模式3：UI电压信号输入，GPIO配置：110，0V输入：电压为0V，5V输入：电压为2.7V，10V输入：电压为：4.13V
 * @param mode 模式，1：输入，2：输出，3：高电平输出
 * @return int 0：成功，-1：失败
 */
int gpio_group_mode(int *group,int mode){
    printf("GPIO=[%d,%d,%d],mode=[%d]\n", group[0], group[1] ,group[2],mode);
    char gpio_cmd[128] = {0};
        snprintf(gpio_cmd, sizeof(gpio_cmd), "echo %d > /sys/class/gpio/gpio%d/value", mode == 1 ? 0 : 1, group[0]);
        system(gpio_cmd);
        usleep(1000*10);
        snprintf(gpio_cmd, sizeof(gpio_cmd), "echo %d > /sys/class/gpio/gpio%d/value", mode == 2 ? 0 : 1, group[1]);
        system(gpio_cmd);
        usleep(1000*10);
        snprintf(gpio_cmd, sizeof(gpio_cmd), "echo %d > /sys/class/gpio/gpio%d/value", mode == 3 ? 0 : 1, group[2]);
        system(gpio_cmd);
        usleep(1000*10);
    return 0;
}

int gpio_init(void) {
    char gpio_cmd[128] = {0};
    for (int i = 0; i < gpio_group_count; i++) {
        printf("初始化GPIO=[%d,%d,%d]\n", gpio_group[i][0],gpio_group[i][1],gpio_group[i][2]);
        for (int j = 0; j < gpio_count; j++) {
        // 导出GPIO引脚
        snprintf(gpio_cmd, sizeof(gpio_cmd), "echo %d > /sys/class/gpio/export", gpio_group[i][j]);
        system(gpio_cmd);
        usleep(1000*10);
        // 设置GPIO方向为输出
        snprintf(gpio_cmd, sizeof(gpio_cmd), "echo out > /sys/class/gpio/gpio%d/direction", gpio_group[i][j]);
        system(gpio_cmd);
        usleep(1000*10);
        }
        //模式1：DI开关信号输入
        //模式2：AI电流信号输入
        //模式3：UI电压信号输入
        // 设置GPIO高电平输出
        gpio_group_mode(gpio_group[i],3);
    }
    return 0;
}

int spi_init(void) {
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) { perror("无法打开SPI设备"); return -1; }
    uint8_t mode = SPI_MODE_1;
    uint8_t bits = 8;
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    printf("SGM51242 初始化成功 (SPI MODE=%d, %dHz)\n", mode, spi_speed);
    return 0;
}

uint16_t spi_xfer(uint16_t tx) {
    uint8_t tx_buf[2] = { tx >> 8, tx & 0xFF };
    uint8_t rx_buf[2] = {0};
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 2, .delay_usecs = 20,
        .speed_hz = spi_speed, .bits_per_word = 8,
    };
    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    return (rx_buf[0] << 8) | rx_buf[1];
}

void write_reg(uint16_t addr, uint16_t val) {
    spi_xfer((addr << 11) | (val & 0x07FF));
}

uint16_t read_reg(uint16_t addr) {
    /* 设置回读模式 */
    write_reg(REG_READBACK, (uint16_t)(addr & 0x0F));
    /* 发送一次 NOP 读取数据 */
    uint16_t val = spi_xfer(0x0000) & 0x0FFF;
    /* 清除回读模式 */
    write_reg(REG_READBACK, 0x0000);
    /* 再发送一次 NOP 让芯片退出回读模式 */
    spi_xfer(0x0000);
    return val;
}

// 切换ADC通道，等待内部MUX和采样电容稳定
void adc_switch_channel(int ch) {
    /* 4) 引脚配置: 全部 8 个通道配置为 ADC 输入
     *    REG_ADC_PIN_CFG (0x04): D[7:0] 分别对应 IN7~IN0
     *    1 = 配置为 ADC 输入, 0 = 通用数字 IO
     */
    write_reg(REG_ADC_PIN_CFG, 0x00FF);
    /* 5) 关闭下拉电阻以减少干扰
     *    REG_PULL_DOWN (0x06): D[7:0] 对应 IN7~IN0, 1=启用下拉
     *    默认 0x0FF=全部下拉, 设置 0 全部关闭
     */
    write_reg(REG_PULL_DOWN, 0x0000);
    /* 6) 配置 Sequence 寄存器为自动扫描所有 8 通道
     *    REG_ADC_SEQ (0x02): D[7:0] 对应 IN7~IN0 的转换使能
     */
    write_reg(REG_ADC_SEQ, 0x0200 | (1 << ch));   // 连续转换模式 + 选择通道x
    usleep(20000); // 20ms 等待稳定（通道切换必须等！）
}

// 读取当前通道ADC值 (12位)
uint16_t adc_read(void) {
    uint16_t val = spi_xfer(0x0000) & 0x0FFF;
    printf("RAW=%d\n", val);
    return val;
}

// 芯片初始化
int sgm51242_init(void) {

    // 软件复位
    spi_xfer(0x7DAC);
    usleep(30000);// 等待30ms，确保复位完成


    /* 使用外部参考电压: REFON=0 */
    write_reg(REG_PD_REF_CTRL, 0x0000);
    // 启用内部2.5V参考电压
    // write_reg(REG_PD_REF_CTRL, 0x0200);
    // uint16_t vref = read_reg(REG_PD_REF_CTRL);
    // if (!(vref & 0x0200)) {
    //     printf("错误: 内部参考电压启用失败!\n");
    //     return -1;
    // }
    // printf("内部2.5V参考电压已启用\n");
    return 0;
}

// 多次采样取平均值 (消除噪声)
uint16_t adc_read_avg(int samples) {
    uint32_t sum = 0;
    adc_read();// 读取一次，避免初始值影响
    usleep(5000); // 5ms间隔
    for (int i = 0; i < samples; i++) {
        sum += adc_read();
        usleep(5000); // 5ms间隔
    }
    return (uint16_t)(sum / samples);
}

uint16_t adc_read_one() {
  adc_read();   // 读取一次，避免初始值影响
  usleep(50000); // 50ms间隔
  return adc_read();
}

int main(int argc, char *argv[]) {
    if (gpio_init() < 0) return -1;
    if (spi_init() < 0) return -1;

    if (argc == 1) {
        // 无参数：扫描全部8通道
        printf("\n============================================\n");
        printf("  通道   ADC值      电压(V)    V_in(0-10V)   状态\n");
        printf("============================================\n");
        while (1) {
        for (int ch = 0; ch < 7; ch++) {
            if (sgm51242_init() < 0) return -1;
            adc_switch_channel(ch);
            uint16_t val = adc_read_one();
            float v = (float)val / 4095.0f * VREF_EXT;
            float v_in = adc_to_input_calib(v, &g_divider_calib[ch]);
            const char *status = (val < 10) ? "接地/低电平" :
                                 (val > 4085) ? "接近VREF_EXT" : "正常";
            printf("  IN%d    %4d       %.3f       %.3f        %s\n", ch, val, v, v_in, status);
            fflush(stdout);
        }
        printf("============================================\n");
        usleep(1000000);  // 暂停1秒后继续循环
        }
    } else if (argc == 2) {
        if (sgm51242_init() < 0) return -1;
        // 一个参数：持续监控指定通道
        int ch = atoi(argv[1]);
        if (ch < 0 || ch > 7) {
            printf("通道号范围: 0~7\n");
            return -1;
        }
        adc_switch_channel(ch);
        printf("持续监控 IN%d (Ctrl+C 退出)\n", ch);
        while (1) {
            uint16_t val = adc_read_one();
            float v = (float)val / 4095.0f * VREF_EXT;
            float v_in = adc_to_input_calib(v, &g_divider_calib[ch]);
            printf("IN%d: ADC=%4d  电压=%.3fV  V_in=%.3fV   \n", ch, val, v, v_in);
            fflush(stdout);
            usleep(200000);
        }
    } else if (argc == 3 && strcmp(argv[1], "cal") == 0) {
        if (sgm51242_init() < 0) return -1;
        // cal <通道> : 校准模式，将INx接已知电压
        int ch = atoi(argv[2]);
        adc_switch_channel(ch);
        printf("校准模式 IN%d (请接入已知电压)\n", ch);
        for (int i = 0; i < 10; i++) {
            uint16_t val = adc_read();
            float v = (float)val / 4095.0f * VREF_EXT;
            float v_in = adc_to_input_calib(v, &g_divider_calib[ch]);
            printf("  RAW=0x%04X  ADC=%4d  V=%.3fV  V_in=%.3fV\n", val, val, v, v_in);
            fflush(stdout);
            usleep(100000);
        }
    }
    close(spi_fd);
    return 0;
}

