#ifndef OLED_H
#define OLED_H

// Control lines.
sbit LCD_RS at RA4_bit;
sbit LCD_EN at RA3_bit;

sbit LCD_RS_Direction at TRISA4_bit;
sbit LCD_EN_Direction at TRISA3_bit;

// 4-bit data lines.
sbit LCD_D7 at RB7_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D5 at RA7_bit;
sbit LCD_D4 at RA6_bit;

sbit LCD_D7_Direction at TRISB7_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D5_Direction at TRISA7_bit;
sbit LCD_D4_Direction at TRISA6_bit;

#endif