#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
//外部参考电压
#define VREF_EXT 4.5f
#define SPI_DEVICE "/dev/spidev4.0"

int spi_fd = -1;
uint32_t spi_speed = 100000;

int spi_init(void) {
    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) { perror("无法打开SPI设备"); return -1; }
    uint8_t mode = SPI_MODE_2;
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
    spi_xfer((0x07 << 11) | (1 << 6) | (addr << 2));
    uint16_t val = spi_xfer(0x0000);
    // 【关键】读完立即关闭读回模式，否则ADC数据会错乱！
    spi_xfer((0x07 << 11) | 0x0000);
    spi_xfer(0x0000);
    return val;
}

// 切换ADC通道，等待内部MUX和采样电容稳定
void adc_switch_channel(int ch) {
    write_reg(0x04, 1 << ch);              // INx 配置为ADC输入
    write_reg(0x06, 0x00FF & ~(1 << ch));   // 关闭 INx 内部下拉
    write_reg(0x02, 0x0200 | (1 << ch));   // 连续转换模式 + 选择通道x
    usleep(20000); // 20ms 等待稳定（通道切换必须等！）
    sleep(1);
}

// 读取当前通道ADC值 (12位)
uint16_t adc_read(void) {
    uint16_t val = spi_xfer(0x0000) & 0x0FFF;
    printf("RAW=%d\n", val);
    return val;
}

// 芯片初始化
int sgm51242_init(void) {
    if (spi_init() < 0) return -1;

    // 软件复位
    spi_xfer(0x7DAC);
    usleep(300000);


    // 禁用内部2.5V参考电压
    write_reg(0x0B, 0x0000);
    // 启用内部2.5V参考电压
    // write_reg(0x0B, 0x0200);
    // uint16_t vref = read_reg(0x0B);
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
    for (int i = 0; i < samples; i++) {
        sum += adc_read();
        usleep(5000); // 5ms间隔
    }
    return (uint16_t)(sum / samples);
}

int main(int argc, char *argv[]) {
    if (sgm51242_init() < 0) return -1;

    if (argc == 1) {
        // 无参数：扫描全部8通道
        printf("\n============================================\n");
        printf("  通道   ADC值      电压(V)    状态\n");
        printf("============================================\n");
        for (int ch = 0; ch < 8; ch++) {
            adc_switch_channel(ch);
            uint16_t val = adc_read_avg(5); // 5次平均
            float v = (float)val / 4095.0f * VREF_EXT;
            const char *status = (val < 10) ? "接地/低电平" :
                                 (val > 4085) ? "接近VREF_EXT" : "正常";
            printf("  IN%d    %4d       %.3f      %s\n", ch, val, v, status);
            fflush(stdout);
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
            float v = (float)val / 4095.0f * VREF_EXT;
            printf("IN%d: ADC=%4d  电压=%.3fV   \n", ch, val, v);
            fflush(stdout);
            usleep(200000);
        }
    } else if (argc == 3 && strcmp(argv[1], "cal") == 0) {
        // cal <通道> : 校准模式，将INx接已知电压
        int ch = atoi(argv[2]);
        adc_switch_channel(ch);
        printf("校准模式 IN%d (请接入已知电压)\n", ch);
        for (int i = 0; i < 10; i++) {
            uint16_t val = adc_read();
            float v = (float)val / 4095.0f * VREF_EXT;
            printf("  RAW=0x%04X  ADC=%4d  V=%.3fV\n", val, val, v);
            fflush(stdout);
            usleep(100000);
        }
    }

    close(spi_fd);
    return 0;
}

