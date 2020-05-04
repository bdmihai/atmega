/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2020 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 02.May.2020  |
 |                                                                            |
 |___________________________________________________________________________*/
 
#include <avr/io.h>
#include <stdio.h>
#include <string.h>

#include "lcd.h"

#define MAX_BUFFER_SIZE 4*20
static char buffer[MAX_BUFFER_SIZE];

static void uart_init()
{
    /* Set baud rate */
    UBRR0H = 0;
    UBRR0L = 12;

    UCSR0A = _BV(U2X0);
    /* Enable the transmitter */
    UCSR0B = _BV(TXEN0);

    /* Set frame format: 8data, 1stop bit */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

static void uart_transmit(char data)
{
    /* Wait for empty transmit buffer */
    while (!(UCSR0A & _BV(UDRE0)))
        ;
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

void lcd_init()
{
    uart_init();
}

void lcd_reset()
{
    uart_transmit('\e');
    uart_transmit('R');
}

void lcd_clear()
{
    uart_transmit('\f');
}

void lcd_caridge_return()
{
    uart_transmit('\r');
}

void lcd_line_feed()
{
    uart_transmit('\n');
}

void lcd_version()
{
    uart_transmit('\e');
    uart_transmit('V');
}

void lcd_move_cursor(unsigned char row, unsigned char col)
{
    uart_transmit('\e');
    uart_transmit('O');
    uart_transmit(col);
    uart_transmit(row);
}

void lcd_set_cursor_type(unsigned char cursor)
{
    uart_transmit('\e');
    uart_transmit('C');
    uart_transmit(cursor);
}

void lcd_set_led_backlight(unsigned char led)
{
    uart_transmit('\e');
    uart_transmit('L');
    uart_transmit(led);
}

void lcd_putc(char c)
{
    uart_transmit(c);
}

void lcd_print(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    vsnprintf(buffer, MAX_BUFFER_SIZE, fmt, va);
    va_end(va);

    for (int i = 0; i < strlen(buffer); i++) {
        lcd_putc(buffer[i]);
    }
}
