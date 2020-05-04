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
#include <avr/interrupt.h>
#include "gpio.h"

volatile static unsigned char status;
volatile static unsigned char pressed_duration;
volatile static unsigned char released_duration;

void gpio_init()
{
    DDRB |= (_BV(DDB0) | _BV(DDB1));
    EICRA |= _BV(ISC20);
    EIMSK |= _BV(INT2);

    TCCR0A = 0;
    TCCR0B = 0;
    TIMSK0 = _BV(OCIE0A);
    TCNT0  = 0;
    OCR0A  = 102;

    status = 0;
    pressed_duration = 0;
    released_duration = 0;
    sei();
}

void gpio_set_status_led()
{
    PORTB |= _BV(PORTB0);
}

void gpio_reset_status_led()
{
    PORTB &= ~_BV(PORTB0);
}

void gpio_set_warn_led()
{
    PORTB |= _BV(PORTB1);
}

void gpio_reset_warn_led()
{
    PORTB &= ~_BV(PORTB1);
}

unsigned char gpio_get_button_status()
{
    return status;
}

void gpio_reset_button_status()
{
    status = 0;
    pressed_duration = 0;
    released_duration = 0;
}

unsigned char gpio_get_button_pressed_duration()
{
    return pressed_duration;
}

unsigned char gpio_get_button_released_duration()
{
    return released_duration;
}

ISR(INT2_vect)
{
    // first time execution and the input is pulled low
    if ((status == 0) && !(PINB & _BV(PINB2))) {
        TCNT0  = 0;
        TCCR0B = _BV(CS02) | _BV(CS00); // 1024 prescaler (about 1ms@ 1MHz)
        status |= GPIO_BUTTON_PRESSED;
    }

    // the pin is pulled high so the button was released
    if ((status != 0) && (PINB & _BV(PINB2))) {
        status |= GPIO_BUTTON_RELEASED;
    }
}

ISR(TIMER0_COMPA_vect)
{
    TCNT0 = 0;

    // increment pressed duration
    if (bit_is_clear(PINB, PINB2)) {
        if (pressed_duration < 255) {
            pressed_duration++;
        }

        // after a minimum of 3 measurements set the debounced flag
        if (pressed_duration > 2) {
            status |= GPIO_BUTTON_PRESSED_DEBOUNCED;
        }
    }

    // increment released duration
    if (bit_is_set(PINB, PINB2)) {
        if (released_duration < 255) {
            released_duration++;
        }

        // after a minimum of 3 measurements set the debounced flag
        if (released_duration > 2) {
            status |= GPIO_BUTTON_RELEASED_DEBOUNCED;
        }
    }

    // stop the timer if the program resets the status
    if (status == 0) {
        TCCR0B = 0;
    }
}
