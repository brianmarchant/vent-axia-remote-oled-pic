/*
** Test Project: Vent-Axia Remote Control using PIC16F627A.
*/
// ©2021 bmd (brian@brianmarchant.com)

// *****************************************************************************
// INCLUDES
// *****************************************************************************
#include "oled.h"

// *****************************************************************************
// DEFINITIONS
// *****************************************************************************
//unsigned int loop_count;

typedef struct _BTN_STATE {
    unsigned char   port_bit;
    unsigned char   curr_state;
    unsigned char   last_state;
} BTN_STATE;

BTN_STATE btn_up;
BTN_STATE btn_dwn;
BTN_STATE btn_set;
BTN_STATE btn_fan;
unsigned char button_bits;
unsigned int long_press_counter;
unsigned char commission_set;

char data_read;
unsigned char i;

unsigned char uart_chr;
char oled_chr;
char oled_pos;
char line1_active;
char line2_active;

// Extra chr as it's easier for the array to be 1-based.
char line1[18];
char line2[18];

// Message sent to MVHR unit at start-up.
unsigned char alive_str[] = {0x04, 0x06, 0xFF, 0xFF, 0xFF, 0x10, 0xFC, 0xE8, '\0'};

#define DATA_DWN_BIT            0x01
#define DATA_UP_BIT             0x02
#define DATA_SET_BIT            0x04
#define DATA_FAN_BIT            0x08

#define BUTTON_MASK             0b00111001
#define NONE_PRESSED            0b00111001

#define BTN_STATE_PLACEHOLDER   0x00
#define CRC_PLACEHOLDER         0x00

#define BTN_STATE_BYTE_POS      5U
#define BTN_CRC_BYTE_POS        7U

#define COMMISSION_BTN_DELAY    5

// Data representing each possible key-press.
unsigned char btn_press_str[] = {0x04, 0x06, 0xFF, 0xFF, 0xFF, BTN_STATE_PLACEHOLDER, 0xFC, CRC_PLACEHOLDER, '\0'};
unsigned char btn_byte;

// *****************************************************************************
// FUNCTIONS
// *****************************************************************************

// Write the two lines of text to the OLED display.
void update_oled(void)
{
    // Overwrite the display with an indication of button long-press
    // and a hint to move to the top menu item for them to be effective.
    if (long_press_counter >= 0x2000) {
        line1[16] = '^';
    }

    Lcd_Out(1, 1, &line1[1]); // Line array indexes are 1-based.
    
    if ((btn_set.curr_state) && (long_press_counter >= 0x4FFF)) {
        // Conversely to below it this used less ROM than individual writes.
        Lcd_Out(2, 1, "ENTER COMMISSION");
    }
    else {
        Lcd_Out(2, 1, &line2[1]);
    }
}

// Initialise/Clear the state of a button.
void init_btn(BTN_STATE *btn, const unsigned char port_bit)
{
    btn->port_bit = port_bit;
    btn->curr_state = 0;
    btn->last_state = 0;
}

// Update the state of a button from its port-pin definition.
void update_btn(const unsigned char btn_bits, BTN_STATE *btn)
{
    btn->last_state = btn->curr_state;
    if ((btn_bits & btn->port_bit) == 0) {
        btn->curr_state = 1;
    }
    else {
        btn->curr_state = 0;
    }
    
    // Holding ANY button down will trigger a special HOLD action,
    // or just "pass through" the repeating button output.
    if (btn_bits != NONE_PRESSED) {
        if (long_press_counter < 0xFFFE) { // Approx. 3 seconds to 0x7FFF.
            long_press_counter++;
        }
    }
    else {
        long_press_counter = 0;
    }
}

// *****************************************************************************
// MAIN PROGRAM LOOP
// *****************************************************************************
void main()
{
    PORTA = 0x00;   // Set PORTA data latches LOW.
    TRISA = 0x00;   // All ports except RA5 are outputs.
    TRISB = 0x39;   // All ports are outputs.... except for the inputs below.
    OPTION_REG &= 0x7F;     // Set PORTB input pull-ups on (required for buttons).

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

    Lcd_Out(1, 1, "VENT-AXIA Remote");
    Lcd_Out(2, 1, "V1.0 bmd");
    
    // Initialize UART module (the link to the MVHR unit).
    UART1_Init(9600);

    // Show that the display is active with a little animation.
    for (i = 10; i <= 16; i++) {
        Lcd_Chr(2, i, '.');
        Delay_ms(300);
    }
    Delay_ms(1000);
    
    // Send an "alive" message to the MVHR unit:
    // (This is what the original remote did).
    // NOT ESSENTIAL : REMOVE IF ROM SPACE IS REQUIRED.
    UART1_Write_Text(alive_str);

    // Terminate the two line strings and clear the display.
    line1[17] = '\0';
    line1[17] = '\0';
    Lcd_Cmd(_LCD_CLEAR);
    
    // Initialise the button states.
    init_btn(&btn_dwn, 0x01);
    init_btn(&btn_up,  0x08);
    init_btn(&btn_set, 0x10);
    init_btn(&btn_fan, 0x20);
    long_press_counter = 0;
    commission_set = 0;
    btn_byte = 0x00;

    // Initialise the character counters.
    line1_active = 0;
    line2_active = 0;
    oled_pos = 0;

    // Loop count bit-0 is now output on PIC Pin 1.
    //loop_count = 1;

    while (1)
    {
        //
        // Priority 1 : Check for characters received from the MVHR unit.
        //
        if (UART1_Data_Ready()) {
            uart_chr = UART1_Read();
            data_read = 1;
            
            switch (uart_chr) {
            case 0x15:
                // NAK - start of line 1
                line1_active = 1;
                oled_pos = 0;
                break;

            case 0x16:
                // SYN - end of line 1, start of line 2
                line2_active = 1;
                oled_pos = 0;
                break;

            default:
                // Record the characters for Line#1 (top)
                if ((line1_active) && (oled_pos > 0)) {
                    line1[oled_pos] = uart_chr;
                    
                    if (oled_pos >= 16) {
                        line1_active = 0;
                    }
                }
                
                // Record the characters for Line#2 (bottom)
                if ((line2_active) && (oled_pos > 0)) {
                    line2[oled_pos] = uart_chr;

                    if (oled_pos >= 16) {
                        line2_active = 0;
                        
                        update_oled();
                    }
                }

                break;
            }
            
            oled_pos++;
        }
        else {
            data_read = 0;
            //
            // While we don't have data to read, do something else useful.
            //
            
            // Read the input port once.
            button_bits = PORTB & BUTTON_MASK;

            // And update the state of the buttons.
            update_btn(button_bits, &btn_set);
            update_btn(button_bits, &btn_dwn);
            update_btn(button_bits, &btn_up);
            update_btn(button_bits, &btn_fan);

            if (UART1_Tx_Idle()) {
                // Clear the state all all OR'd buttons.
                btn_press_str[BTN_STATE_BYTE_POS] = 0x00;

                // "DOWN" arrow pressed (edge trigger - sent only once).
                // PLUS: UP+DWN for 5s+ resets "Check Filter" message.
                if ((btn_dwn.curr_state & !btn_dwn.last_state) ||
                    ((btn_dwn.curr_state) && (long_press_counter >= 0x3FFF))) {
                    btn_press_str[BTN_STATE_BYTE_POS] |= DATA_DWN_BIT;
                }

                // "UP" arrow pressed (edge trigger - sent only once).
                // PLUS: UP+DWN for 5s+ resets "Check Filter" message.
                // PLUS: UP 5s+ should also exit from Commission screens.
                if ((btn_up.curr_state & !btn_up.last_state) ||
                    ((btn_up.curr_state) && (long_press_counter >= 0x3FFF))) {
                    btn_press_str[BTN_STATE_BYTE_POS] |= DATA_UP_BIT;
                }

                // "FAN" button pressed (edge trigger - sent only once).
                // PLUS long-press pass-through to set PURGE mode.
                if ((btn_fan.curr_state & !btn_fan.last_state) ||
                    ((btn_fan.curr_state) && (long_press_counter >= 0x7FFF))) {
                    btn_press_str[BTN_STATE_BYTE_POS] |= DATA_FAN_BIT;
                }
                
                // "SET" button pressed (edge trigger - sent only once).
                if (btn_set.curr_state & !btn_set.last_state) {
                    btn_press_str[BTN_STATE_BYTE_POS] |= DATA_SET_BIT;
                }
                // Add in a "long-press" of the SET button to send the special
                // commissioning mode sequence of SET then SET+DWN+UP together.
                // IMPROVEMENT TO ORIGINAL VENT-AXIA SOFTWARE THAT REQUIRED AN
                // AWKWARD 'SET' plus immediately 'SET+UP+DOWN' SEQUENCE.
                if ((btn_set.curr_state) && (long_press_counter >= 0x7FFF)) {
                    btn_press_str[BTN_STATE_BYTE_POS] |= DATA_SET_BIT;
                    if (commission_set > COMMISSION_BTN_DELAY) {
                        btn_press_str[BTN_STATE_BYTE_POS] |= DATA_DWN_BIT;
                        btn_press_str[BTN_STATE_BYTE_POS] |= DATA_UP_BIT;
                    }
                }
                else {
                    commission_set = 0;
                }

                // Calculate the checksum byte and send the byte string.
                if (btn_press_str[BTN_STATE_BYTE_POS] != 0x00) {
                    // Usefully the last-sent value will be remembered and
                    // is used for diagnostic output. CRC indicates buttons.
                    btn_press_str[BTN_CRC_BYTE_POS] = 0xF8 - btn_press_str[BTN_STATE_BYTE_POS];

                    // TODO : We might want to restrict send repetition by
                    //        looking at an upper loop_count bit but for now
                    //        we just use a fixed delay. This will stop the
                    //        display from updating when a repeated button
                    //        code is being sent, but we don't really care.
                    UART1_Write_Text(btn_press_str);
                    Delay_ms(15);

                    if (commission_set < 255) {
                        commission_set++;
                    }

                    // Reset/inhibit the reception of display data.
                    line1_active = 0;
                    line2_active = 0;
                }
            }
        }

        //loop_count++;
    }
}

// END OF FILE