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

#include "lcd.h"
#include "dcf77.h"
#include "gpio.h"
#include "utils.h"

int main()
{
    int prev_duration_high = -1;
    int prev_duration_low = -1;

    lcd_init();
    lcd_reset();
    delay_ms(500);

    // initialize the dcf77 module
    dcf77_init();

    // initialize the gpio
    gpio_init();


    while(1) {
        if (prev_duration_low != dcf77_get_duration_low()) {             // <---here the next second begins
            // print the times from the last second
            lcd_clear();
            lcd_print("%d-%d [%d]", dcf77_get_duration_high(), dcf77_get_duration_low(), dcf77_get_duration_high() + dcf77_get_duration_low());
            // print the time
            lcd_line_feed();
            lcd_caridge_return();
            lcd_print("Time: %02d:%02d\r\nDate: %02d.%02d.20%02d  ", dcf77_get_hour(), dcf77_get_minute(), dcf77_get_day(), dcf77_get_month(), dcf77_get_year());

            // toggle the red led for 1 sec
            if (dcf77_get_duration_high() + dcf77_get_duration_low() > 1500) {
                gpio_set_warn_led();
            } else {
                gpio_reset_warn_led();
            }
            gpio_set_status_led();
            prev_duration_low = dcf77_get_duration_low();
        } else if (prev_duration_high != dcf77_get_duration_high()) {      // <---the high pulse ends (100/200 ms)
            gpio_reset_status_led();
            prev_duration_high = dcf77_get_duration_high();
        }
    }
}

