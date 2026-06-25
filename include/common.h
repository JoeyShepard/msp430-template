#pragma once

#include <stdint.h>

#define HEX8_MAX_DIGITS     2
#define INT16_MAX_DIGITS    6
#define UINT16_MAX_DIGITS   5
#define INT32_MAX_DIGITS    11
#define UINT32_MAX_DIGITS   10

//Function prototypes
void delay_ms(uint16_t ms);
void set_pwm(uint8_t freq);
uint8_t spi_send(uint8_t data);
void i2c_start_send(uint8_t address);
int i2c_send(uint8_t data);
void i2c_start_receive(uint8_t address);
uint8_t i2c_receive();
uint8_t i2c_receive_nack();
uint8_t i2c_receive_single(uint8_t address);
void i2c_stop();
void text_hex(char *buffer,uint8_t data);
void text_uint16(char *buffer,uint16_t data);
void text_int16(char *buffer,int16_t data);
void text_uint32(char *buffer,uint32_t data);
void text_int32(char *buffer,int32_t data);
void uart_hex(uint8_t data);
uint8_t uart_receive();
void uart_send(uint8_t data);
void uart_text(const char *data);
void uart_crlf();
void uart_space();
void uart_int16(int16_t num);
void uart_uint16(uint16_t num);
void uart_int32(int32_t num);
void uart_uint32(uint32_t num);
