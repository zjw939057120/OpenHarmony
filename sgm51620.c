///> @file sgm51620.c
///> @brief SGM51620 16位ADC SPI接口驱动示例
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// SPI设备节点
#define SPI_DEVICE "/dev/spidev4.1"
// SPI时钟频率 (1MHz，对于20SPS的ADC来说足够了，可根据实际硬件能力调高)
uint32_t spi_speed = 1000000;

// 四通道配置宏定义（单端输入，增益1）
#define CHANNEL_AIN0 0x80  // MUX=1000，GAIN=000，PGA_BYPASS=0（自动旁路）
#define CHANNEL_AIN1 0x90  // MUX=1001，GAIN=000，PGA_BYPASS=0
#define CHANNEL_AIN2 0xA0  // MUX=1010，GAIN=000，PGA_BYPASS=0
#define CHANNEL_AIN3 0xB0  // MUX=1011，GAIN=000，PGA_BYPASS=0

// 通道名称数组（用于打印）
const char* channel_names[] = {"AIN0", "AIN1", "AIN2", "AIN3"};
// 通道配置数组
const uint8_t channel_configs[] = {CHANNEL_AIN0, CHANNEL_AIN1, CHANNEL_AIN2,
                                   CHANNEL_AIN3};
#define CHANNEL_COUNT 4  // 通道总数

// 全局SPI文件描述符
static int spi_fd = -1;

// 函数声明
int spi_init(void);
int spi_transfer(uint8_t* tx_buf, uint8_t* rx_buf, int len);
void adc_write_reg(uint8_t reg_addr, uint8_t data);
uint8_t adc_read_reg(uint8_t reg_addr);
void adc_set_channel(uint8_t channel_cfg);
void adc_start_conversion(void);
int16_t adc_read_data(void);
float adc_convert_to_temp(int16_t raw_data);

// SPI初始化（打开设备并配置参数）
int spi_init(void) {
  uint8_t mode = SPI_MODE_1;  // 对应原代码 CPOL=0, CPHA=1
  uint8_t bits = 8;           // 8位数据位

  spi_fd = open(SPI_DEVICE, O_RDWR);
  if (spi_fd < 0) {
    perror("Failed to open SPI device");
    return -1;
  }

  // 配置SPI模式
  if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0) {
    perror("Failed to set SPI mode");
    close(spi_fd);
    return -1;
  }

  // 配置数据位
  if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
    perror("Failed to set SPI bits per word");
    close(spi_fd);
    return -1;
  }

  // 配置时钟频率
  if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0) {
    perror("Failed to set SPI max speed");
    close(spi_fd);
    return -1;
  }

  printf("SPI initialized successfully (Device: %s, Speed: %d Hz, Mode: 1)\n",
         SPI_DEVICE, spi_speed);
  return 0;
}

// SPI传输函数（全双工）
int spi_transfer(uint8_t* tx_buf, uint8_t* rx_buf, int len) {
  struct spi_ioc_transfer tr = {
      .tx_buf = (unsigned long)tx_buf,
      .rx_buf = (unsigned long)rx_buf,
      .len = len,
      .delay_usecs = 0,  // 片选间隔时间，如遇时序问题可适当增加(如50)
      .speed_hz = spi_speed,
      .bits_per_word = 8,
  };

  // 如果rx_buf为NULL，则说明是纯写操作
  if (rx_buf == NULL) {
    uint8_t dummy_rx[len];
    tr.rx_buf = (unsigned long)dummy_rx;
  }

  int ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1) {
    perror("SPI transfer failed");
  }
  return ret;
}

// 写入ADC配置寄存器（WREG命令：0100 rrnn）
void adc_write_reg(uint8_t reg_addr, uint8_t data) {
  uint8_t tx[2];
  tx[0] = 0x40 | ((reg_addr & 0x03) << 2) | 0x00;  // WREG命令
  tx[1] = data;                                    // 寄存器数据
  spi_transfer(tx, NULL, 2);
  usleep(1000);
}

// 读取ADC配置寄存器（RREG命令：0010 rrnn）
uint8_t adc_read_reg(uint8_t reg_addr) {
  uint8_t tx[2] = {0};
  uint8_t rx[2] = {0};

  tx[0] = 0x20 | ((reg_addr & 0x03) << 2) | 0x00;  // RREG命令
  tx[1] = 0x00;                                    // 发送空字节获取数据

  spi_transfer(tx, rx, 2);
  usleep(1000);
  return rx[1];
}

// 设置ADC输入通道（通过配置REG0x00的MUX位）
void adc_set_channel(uint8_t channel_cfg) {
  adc_write_reg(0x00, channel_cfg);
  // 发送START/SYNC命令重启转换
  adc_start_conversion();
}

// 发送START/SYNC命令启动转换
void adc_start_conversion(void) {
  uint8_t tx[1] = {0x08};  // START/SYNC命令：0000100x
  spi_transfer(tx, NULL, 1);
  usleep(1000);
}

// 读取ADC转换结果（16位）
int16_t adc_read_data(void) {
  uint8_t tx[3] = {0};
  uint8_t rx[3] = {0};

  tx[0] = 0x10;  // RDATA命令：0001xxxx
  tx[1] = 0x00;  // 空字节
  tx[2] = 0x00;  // 空字节

  spi_transfer(tx, rx, 3);

  // 组合2字节数据（MSB先）
  return (int16_t)((rx[1] << 8) | rx[2]);
}

// 将温度传感器原始数据转换为温度值（℃）
float adc_convert_to_temp(int16_t raw_data) {
  // 温度数据为14位，左对齐在16位结果中
  int16_t temp_data = raw_data >> 2;
  // 1LSB = 0.03125℃，负数为二进制补码
  if (temp_data & 0x2000) {  // 最高位为1（负数）
    temp_data = -(~temp_data + 1);
  }
  return temp_data * 0.03125f;
}

int main(void) {
  printf("SGM51620R ADC 四通道读取 Demo (Hardware SPI via /dev/spidev)\n");
  printf("=============================================================\n");

  // 1. 初始化SPI
  if (spi_init() < 0) {
    return -1;
  }

  // 2. 全局配置ADC（正常模式，20SPS，连续转换，内部参考）
  adc_write_reg(
      0x01,
      0x04);  // REG1: DR=000（20SPS），MODE=00（正常模式），CM=1（连续转换）
  adc_write_reg(0x02, 0x40);  // REG2: 内部参考2.048V，无50/60Hz滤波，IDAC关闭
  adc_write_reg(0x03, 0x00);  // REG3: IDAC路由禁用，DRDYM=0

  // 3. 启动转换
  adc_start_conversion();

  // 4. 循环读取四通道数据 + 温度
  while (1) {
    // 循环读取四路ADC通道
    for (int i = 0; i < CHANNEL_COUNT; i++) {
      // 设置当前通道
      adc_set_channel(channel_configs[i]);
      usleep(50000);  // 20SPS对应50ms转换间隔

      // 读取当前通道ADC数据
      int16_t adc_raw = adc_read_data();

      // 计算实际电压（VREF=2.048V，Gain=1，1LSB=2*2.048/65536=62.5uV）
      float voltage = adc_raw * (8.9f / 65536.0f);
      printf("[%s] ADC Raw: 0x%04X, Voltage: %.6fV\n", channel_names[i],
             adc_raw, voltage);
    }

    // 读取温度传感器数据
    adc_write_reg(0x01, 0x06);  // REG1: TS=1（启用温度传感器）
    adc_start_conversion();
    usleep(50000);
    int16_t temp_raw = adc_read_data();
    float temperature = adc_convert_to_temp(temp_raw);
    printf("[TEMP] Temperature: %.2f℃\n", temperature);
    printf("--------------------------------------------\n");

    // 切换回ADC连续转换模式
    adc_write_reg(0x01, 0x04);
    usleep(1000000);  // 暂停1秒后继续循环
  }

  close(spi_fd);
  return 0;
}