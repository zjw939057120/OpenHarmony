#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev4.0"

int spi_fd = -1;
uint32_t spi_speed = 100000;

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

/*
 * 写入寄存器。
 * SGM51242 输入移位寄存器格式（Table 2）:
 *   D15    = 0 (写命令)
 *   D[14:11] = 寄存器地址 (4 bit)
 *   D[10:9]  = 0
 *   D[8:0]   = 9 bit 寄存器数据
 *
 * 修复点：原先用 0x07FF 掩码（11 bit），会把 D10/D9 也写进去。
 * 这里严格按规格书使用 9 bit 数据位 D[8:0]。
 */
void write_reg(uint16_t addr, uint16_t val) {
    spi_xfer(((addr & 0x0F) << 11) | (val & 0x01FF));
}

/*
 * 读寄存器。
 * 步骤：
 *   1) 向 0x07 (readback mode) 写入 EN=1 + REG_READBACK[3:0]=addr
 *   2) 下一帧 (NOP) 时，从 SDO 读出指定寄存器内容
 *   3) EN 在读完后自动清零，无需手动再写
 */
uint16_t read_reg(uint16_t addr) {
    // 1) 写 readback 寄存器：D6=1 (EN), D[5:2] = addr
    spi_xfer((0x07 << 11) | (1u << 6) | ((addr & 0x0F) << 2));
    // 2) 下一帧读出数据
    return spi_xfer(0x0000);
}

/*
 * 切换 ADC 通道。
 * 关键修复：每次切换通道都重新写一次下拉配置寄存器 0x06 = 0x00，
 * 防止软件复位或异常路径后内部 85kΩ 下拉重新打开导致输入信号被分压。
 */
void adc_switch_channel(int ch) {
    if (ch < 0 || ch > 7) return;
    write_reg(0x06, 0x0000);              // 关闭全部输入引脚内部 85kΩ 下拉（关键）
    write_reg(0x04, 1u << ch);            // INx 配置为 ADC 输入
    write_reg(0x02, 0x0200 | (1u << ch)); // REP=1 连续转换 + 选择通道 x
    usleep(20000);                        // 20ms 等待 MUX/采样电容稳定
}

// 读取当前通道 ADC 值 (12 bit)
uint16_t adc_read(void) {
    return spi_xfer(0x0000) & 0x0FFF;
}

// 芯片初始化
int sgm51242_init(void) {
    if (spi_init() < 0) return -1;

    // 软件复位：所有寄存器恢复默认（含 Pull-Down = 0x0FF = 全开）
    spi_xfer(0x7DAC);
    usleep(300000);

    // 关键：复位后立即关闭所有输入引脚的 85kΩ 下拉电阻
    write_reg(0x06, 0x0000);

    // ADC 控制寄存器：清零（默认：buffer off, range 0~VREF, lock off）
    write_reg(0x03, 0x0000);

    // 电源/参考控制寄存器：PD_ALL=0, EN_REF=0 → 使用外部 VREF
    write_reg(0x0B, 0x0000);
    printf("外部 4.5V 参考电压已配置（EN_REF=0, PD_ALL=0）\n");
    return 0;
}

// 多次采样取平均值
uint16_t adc_read_avg(int samples) {
    uint32_t sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += adc_read();
        usleep(5000); // 5ms 间隔
    }
    return (uint16_t)(sum / samples);
}

int main(int argc, char *argv[]) {
    if (sgm51242_init() < 0) return -1;

    if (argc == 1) {
        // 无参数：扫描全部 8 通道
        printf("\n============================================\n");
        printf("  通道   ADC值      电压(V)    状态\n");
        printf("============================================\n");
        for (int ch = 0; ch < 8; ch++) {
            adc_switch_channel(ch);
            uint16_t val = adc_read_avg(5);
            float v = (float)val / 4095.0f * 4.5f;
            const char *status = (val < 10) ? "接地/低电平" :
                                 (val > 4085) ? "接近VREF" : "正常";
            printf("  IN%d    %4d       %.3f      %s\n", ch, val, v, status);
        }
        printf("============================================\n");
    } else if (argc == 2) {
        // 一个参数：持续监控指定通道
        int ch = atoi(argv[1]);
        if (ch < 0 || ch > 7) {
            printf("通道号范围: 0~7\n");
            return -1;
        }
        adc_switch_channel(ch);
        printf("持续监控 IN%d (Ctrl+C 退出)\n", ch);
        while (1) {
            uint16_t val = adc_read_avg(5);
            float v = (float)val / 4095.0f * 4.5f;
            printf("\r  IN%d: ADC=%4d  电压=%.3fV   ", ch, val, v);
            fflush(stdout);
            usleep(200000);
        }
    } else if (argc == 3 && strcmp(argv[1], "cal") == 0) {
        // cal <通道> : 校准模式
        int ch = atoi(argv[2]);
        adc_switch_channel(ch);
        printf("校准模式 IN%d (请接入已知电压)\n", ch);
        for (int i = 0; i < 10; i++) {
            uint16_t val = adc_read();
            float v = (float)val / 4095.0f * 4.5f;
            printf("  RAW=0x%04X  ADC=%4d  V=%.3fV\n", val, val, v);
            usleep(100000);
        }
    }

    close(spi_fd);
    return 0;
}
