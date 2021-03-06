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
#include "utils.h"
#include "gpio.h"
#include "lcd.h"

int main()
{
    // initialize the lcd display
    lcd_init();
    lcd_reset();
    delay_ms(500);

    // init the gpio module
    gpio_init();

    int long_press = 0;
    while (1) {
        if (gpio_get_button_status() & GPIO_BUTTON_PRESSED_DEBOUNCED) {
            gpio_reset_status_led();
        } else if (gpio_get_button_status() & GPIO_BUTTON_PRESSED) {
            gpio_set_status_led();
            long_press = 0;
        } 

        if (gpio_get_button_status() & GPIO_BUTTON_RELEASED_DEBOUNCED) {
            gpio_reset_warn_led();
            lcd_clear();
            lcd_print("P: %d\r\nR: %d\r\nS: 0x%02X", gpio_get_button_pressed_duration() , gpio_get_button_released_duration(), gpio_get_button_status());
            gpio_reset_button_status();
        } else if (gpio_get_button_status() & GPIO_BUTTON_RELEASED) {
            gpio_set_warn_led();
        }

        if ((gpio_get_button_pressed_duration() > 30) && !long_press) {
            lcd_clear();
            lcd_print("Long press...");
            long_press = 1;
        }

        delay_ms(100);
    }
}

