/*
** Test Project: Vent-Axia Remote Control using PIC16F627A.
*/

#include "oled.h"

unsigned long long_no = 1000000;
char  buffer[15];

bit btn_dwn;
bit btn_up;
bit btn_set;
bit btn_fan;

unsigned char loop_count;

void main()
{
    loop_count = 1;

    TRISA = 0x00;    // All ports are outputs (RA4/RA5 are pulled-up. Inputs?)
    TRISB = 0x00;    // All ports are outputs.... except for the inputs below.
    
    RA2_bit = loop_count & 0x01; // which starts HIGH.
    
    TRISB0_bit = 1;  // DWN
    TRISB3_bit = 1;  // UP
    TRISB4_bit = 1;  // SET
    TRISB5_bit = 1;  // FAN
    
    OPTION_REG &= 0x7F; // Set PORTB input pullups on.

     
    // Standard LCD init leaves the OLED display in R>L mode.
    Lcd_Init();

    // Re-initialise for OLED.
    Lcd_Cmd(0x08);  // DISPLAY OFF - required for OLED display init.
    Lcd_Cmd(0x17);  // Char mode, power on, be certain not to allow Graphics mode
    Lcd_Cmd(0x14);  // Cursor shift right, display shift off (default is shift left)
    Lcd_Cmd(0x01);  // _LCD_CLEAR
    Lcd_Cmd(0x0C);  // Display ON, curor off, blink off
    Lcd_Cmd(0x06);  // Auto increment on
    Lcd_Cmd(0x02);  // Cursor to home

    // Back to LCD commands to finish.
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);
    
//    Lcd_Cmd(/*SET L>R MODE*/);
    
    Lcd_Out(1, 1, "VENT-AXIA Remote");
    Lcd_Out(2, 1, "BMD v1.0");

    UART1_Init(9600);                                  // Initialize UART module at 9600 bps
    Delay_ms(10);
    
    btn_dwn = 0;
    btn_up = 0;
    btn_set = 0;
    btn_fan = 0;

    UART1_Write_Text("VENT-AXIA: BMD\n"); // Write message on UART
    Delay_ms(3000);
    
    Lcd_Cmd(_LCD_CLEAR);
 
    while (1)
    {
        // DWN UP SET FAN
        // 123456789012345

        if (Button(&PORTB, 0, 1, 0)) {
            UART1_Write_Text("DWN\n");
            Lcd_Out(2, 1, "DWN");
        }
        else {
            Lcd_Out(2, 1, "---");
        }
        
        if (Button(&PORTB, 3, 1, 0)) {
            UART1_Write_Text("UP\n");
            Lcd_Out(2, 5, "UP");
        }
        else {
            Lcd_Out(2, 5, "--");
        }

        if (Button(&PORTB, 4, 1, 0)) {
            UART1_Write_Text("SET\n");
            Lcd_Out(2, 8, "SET");
        }
        else {
            Lcd_Out(2, 8, "---");
        }
        
        if (Button(&PORTB, 5, 1, 0)) {
            UART1_Write_Text("FAN\n");
            Lcd_Out(2, 12, "FAN");
        }
        else {
            Lcd_Out(2, 12, "---");
        }

        if (loop_count == 0x00) {
            Lcd_Out(1, 1, "*");
        }
        if (loop_count == 0x7F) {
            Lcd_Out(1, 1, "-");
        }

        RA2_bit = loop_count & 0x01;
        loop_count++;

        Delay_ms(10);
    }
}