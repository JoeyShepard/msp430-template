#pragma once

#include <stdint.h>

//Function prototypes
void delay_ms(uint16_t ms);
void set_pwm(uint8_t freq);
uint8_t spi_send(uint8_t data);
void uart_hex(uint8_t data);
uint8_t uart_receive();
void uart_send(uint8_t data);
void uart_text(const char *data);
void uart_crlf();
void uart_space();
void uart_uint16(uint16_t num);
void uart_uint32(uint32_t num);
