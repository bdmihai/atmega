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

#include "utils.h"
#include "lcd.h"
#include "dcf77.h"
#include "gpio.h"
#include "eeprom.h"

#define CYCLE_COMPLETE_TIME   60000
#define CYCLE_DELAY_TIME      100
#define CYCLE_DELAY_MIN       80
#define CYCLE_DELAY_MAX       120
#define CYCLE_ELAPSED_MIN     50000
#define CYCLE_ELAPSED_MAX     70000
#define CYCLE_ELAPSED_TARGET  (60000-CYCLE_DELAY_TIME)

static char second, minute, hour, day, weekday, month;
static int year;
static long cycle_delay, cycle_elapsed;
static long int_adjustment = 0;
static long lcd_led = LCD_LED_ON;

static void adjust_delay()
{
    long adjustment, cycle_delay_old;

    // check if the elapsed is plausible (in a consecutive cycle)
    if (cycle_elapsed > CYCLE_ELAPSED_MIN && cycle_elapsed < CYCLE_ELAPSED_MAX) {
        // the target is to get the elapsed as 60 seconds - the cycle time
        if (cycle_elapsed < CYCLE_ELAPSED_TARGET || cycle_elapsed > CYCLE_ELAPSED_TARGET) {
            adjustment = CYCLE_ELAPSED_TARGET - cycle_elapsed;
            int_adjustment += adjustment;
            adjustment = adjustment/(CYCLE_COMPLETE_TIME/CYCLE_DELAY_TIME) + int_adjustment/((CYCLE_COMPLETE_TIME*10)/CYCLE_DELAY_TIME);
            cycle_delay_old = cycle_delay;
            cycle_delay -= adjustment / 2;

            // limit the delay to a minimum value
            if (cycle_delay < CYCLE_DELAY_MIN) {
                cycle_delay = CYCLE_DELAY_MIN;
            }
                
            if (cycle_delay > CYCLE_DELAY_MAX) {
                cycle_delay = CYCLE_DELAY_MAX;
            }

            if (cycle_delay_old != cycle_delay) {
                eeprom_write(0, &cycle_delay, sizeof(cycle_delay));
            }
        } 
    }
} 

void config_display()
{
    lcd_init();
    lcd_reset();
    delay_ms(500);

    // set the lcd backlight according to eeprom settings
    eeprom_read(4, &lcd_led, sizeof(lcd_led));
    lcd_led = ((lcd_led == LCD_LED_ON) || (lcd_led == LCD_LED_OFF)) ? lcd_led : LCD_LED_ON;
    lcd_set_led_backlight(lcd_led);
}

void update_display()
{
    lcd_move_cursor(1, 1);
    switch (weekday) {
        case 1: lcd_print("Monday              "); break;
        case 2: lcd_print("Tuesday             "); break;
        case 3: lcd_print("Wednesday           "); break;
        case 4: lcd_print("Thursday            "); break;
        case 5: lcd_print("Friday              "); break;
        case 6: lcd_print("Saturday            "); break;
        case 7: lcd_print("Sunday              "); break;
    }
    lcd_print("Time: %02d:%02d:%02d  \r\nDate: %02d.%02d.%04d", hour, minute, second, day, month, year);
    lcd_caridge_return(); lcd_line_feed();
    lcd_print("Cali: %ld/%ld     ", cycle_delay, cycle_elapsed);
    lcd_set_cursor_type(LCD_CURSOR_HIDDEN);
}

void init_clock()
{
    eeprom_read(0, &cycle_delay, sizeof(cycle_delay));
    // limit the delay to a minimum value
    if ((cycle_delay < CYCLE_DELAY_MIN) || (cycle_delay > CYCLE_DELAY_MAX)) {
        // start with a delay of 1000 ms
        cycle_delay = CYCLE_DELAY_TIME;
    }
    cycle_elapsed = 0;

    // the initial 
    second = minute = hour = day = weekday = month = year = 0;
    int_adjustment = 0;
}

void update_clock()
{
    // new time available -> sync
    second = 0;
    minute = dcf77_get_minute();
    hour = dcf77_get_hour();
    day = dcf77_get_day();
    weekday = dcf77_get_weekday();
    month = dcf77_get_month();
    year = 2000 + dcf77_get_year(); // year in range 2000-2099

    // reset the clock status and wait for the next successfull time update
    dcf77_reset_status();
}

void increment_clock()
{
    second++;

    // adjust the seconds
    if (second >= 60) {
        second = 0;
        minute++;
    }

    // adjust the minute
    if (minute >= 60) {
        minute = 0;
        hour++;
    }

    // adjust the hour
    if (hour >= 24) {
        hour = 0;
    }
}

void button_handler()
{
    const long default_cycle_delay = CYCLE_DELAY_TIME;
    const long default_lcd_led = LCD_LED_ON;

    if (gpio_get_button_status() & GPIO_BUTTON_PRESSED_DEBOUNCED) {
        // the button was pressed and debounced
    } else if (gpio_get_button_status() & GPIO_BUTTON_PRESSED) {
        // the button was pressed
    }

    if (gpio_get_button_status() & GPIO_BUTTON_RELEASED_DEBOUNCED) {
       // the button was released and debounced
       gpio_reset_button_status();
       lcd_led = (lcd_led == LCD_LED_OFF) ? LCD_LED_ON : LCD_LED_OFF;
       lcd_set_led_backlight(lcd_led);
       eeprom_write(4, &lcd_led, sizeof(lcd_led));
    } else if (gpio_get_button_status() & GPIO_BUTTON_RELEASED) {
        // the button was released
    }

    // if the button is pressed for ore than 3 seconds reinitialize the clock
    if ((gpio_get_button_pressed_duration() > 30)) {
        // write the values to eeprom
        eeprom_write(0, &default_cycle_delay, sizeof(default_cycle_delay));
        eeprom_write(4, &default_lcd_led, sizeof(default_lcd_led));

        // notify the user about reset
        for (int i = 0; i < 3; i++) {
            gpio_set_warn_led();
            delay_ms(200);
            gpio_reset_warn_led();
            delay_ms(200);
        }
        // block until the button gets released
        while (!(gpio_get_button_status() & GPIO_BUTTON_RELEASED_DEBOUNCED)) {
            delay_ms(10);
        }

        // reinitialize the clock and update the display
        init_clock();
        config_display();
        update_display();
    }
}

int main()
{
    // initialize the dcf77 clock module
    dcf77_init();

    // initialize the gpio modues
    gpio_init();

    // configure display
    config_display();
    
    // init the time keeping variables
    init_clock();

    // run the program
    while(1) {
        if (dcf77_get_status()) {
            update_clock();

            // adapt the delay according to the elapsed time
            adjust_delay();
            cycle_elapsed = 0;

            // update the display
            update_display();

            // set the status led
            gpio_set_status_led();
        } else {
            cycle_elapsed += CYCLE_DELAY_TIME;
            if (cycle_elapsed % 1000 == 0) {
                // increment the time according to the device internal clock
                increment_clock();

                // update the display
                update_display();

                // reset the status led (this should blink if it was set when dcf time was received)
                gpio_reset_status_led();
            }
        }

        // handle user button
        button_handler();

        // update the time keeping variables
        delay_ms(cycle_delay);
    }
}
