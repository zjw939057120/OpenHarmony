#ifndef _MODBUS_RTU_H_
#define _MODBUS_RTU_H_

#include <stdint.h>
#include <stdbool.h>

// Modbus 功能码定义
typedef enum {
    MB_FUNC_READ_COILS = 0x01,
    MB_FUNC_READ_DISCRETE_INPUTS = 0x02,
    MB_FUNC_READ_HOLDING_REGISTERS = 0x03,
    MB_FUNC_READ_INPUT_REGISTERS = 0x04,
    MB_FUNC_WRITE_SINGLE_COIL = 0x05,
    MB_FUNC_WRITE_SINGLE_REGISTER = 0x06,
    MB_FUNC_WRITE_MULTIPLE_COILS = 0x0F,
    MB_FUNC_WRITE_MULTIPLE_REGISTERS = 0x10
} ModbusFunction;

// 解析后的请求结构体
typedef struct {
    uint8_t slave_id;         // 从站地址
    uint8_t function_code;    // 功能码
    uint16_t start_addr;      // 起始地址
    uint16_t quantity;        // 数量（寄存器或线圈数）
    bool is_valid;            // 校验是否通过
} ModbusRequest;

/**
 * @brief 解析 Modbus RTU 请求帧
 * @param data 接收到的原始字节数组
 * @param len  字节数组长度
 * @param req  解析结果输出指针
 * @return true 解析成功且CRC校验通过, false 失败
 */
bool modbus_rtu_parse_request(const uint8_t *data, uint16_t len, ModbusRequest *req);

/**
 * @brief 计算 Modbus CRC16 (供内部或外部调试使用)
 */
uint16_t modbus_crc16(const uint8_t *data, uint16_t len);

/**
 * @brief 构造 Modbus RTU 读保持寄存器(0x03)的响应帧
 * @param slave_id 从站地址
 * @param reg_data 寄存器数据数组 (大端模式)
 * @param reg_count 寄存器数量
 * @param out_buf 输出缓冲区指针
 * @param buf_size 缓冲区大小
 * @return 成功返回响应帧的总字节数，失败返回 0
 */
uint16_t modbus_rtu_build_read_response(uint8_t slave_id, const uint16_t *reg_data, 
                                        uint16_t reg_count, uint8_t *out_buf, uint16_t buf_size);

/**
 * @brief 构造 Modbus RTU 读保持寄存器(0x03)的请求帧
 * @param slave_id 从站地址
 * @param start_addr 起始寄存器地址
 * @param quantity 读取寄存器数量
 * @param out_buf 输出缓冲区指针
 * @param buf_size 缓冲区大小
 * @return 成功返回请求帧的总字节数(固定为8)，失败返回 0
 */
uint16_t modbus_rtu_build_read_request(uint8_t slave_id, uint16_t start_addr, 
                                       uint16_t quantity, uint8_t *out_buf, uint16_t buf_size);
        
#endif // MODBUS_RTU_H