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
#include "lcd.h"
#include "utils.h"

int main()
{
    lcd_init();
    lcd_reset();
    delay_ms(500);

    lcd_version();
    delay_ms(2000);

    lcd_clear();
    lcd_print("Hello\r\nworld %d", 2020);
    delay_ms(2000);

    lcd_set_led_backlight(LCD_LED_OFF);
    lcd_clear();
    lcd_putc('0');
    delay_ms(2000);

    lcd_set_led_backlight(LCD_LED_ON);
    lcd_clear();
    lcd_putc('1');
    delay_ms(2000);

    lcd_set_led_backlight(50);
    lcd_clear();
    lcd_putc('5');lcd_putc('0');
    delay_ms(6000);

    lcd_set_led_backlight(LCD_LED_ON);
    lcd_set_cursor_type(LCD_CURSOR_HIDDEN);
    lcd_clear();
    while (1) {
        lcd_clear();
        for (char c='A'; c<='Z'; c++) {
          lcd_putc(c);
          delay_ms(100);
        }
        lcd_move_cursor(3, 1);
        for (char c='0'; c<='9'; c++) {
          lcd_putc(c);
          delay_ms(100);
        }
    }
}
