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
#include "lcd.h"
#include "utils.h"

volatile int duration_high = 0;
volatile int duration_low = 0;

int main()
{
    int prev_duration_high = -1;
    int prev_duration_low = -1;

    lcd_init();
    lcd_reset();
    delay_ms(500);

    DDRB |= _BV(DDB0);

    DDRB |= _BV(DDB0);
    PORTB &= ~_BV(PORTB0);

    TCCR1A = 0;
    TCCR1B = _BV(ICNC1) | _BV(ICES1) | _BV(CS12) | _BV(CS10);   // 1024 prescaler (about 1ms@ 1MHz)
    TIMSK1 = _BV(ICIE1);

    sei();

    while(1) {
        if (prev_duration_low != duration_low) {
            lcd_clear();
            lcd_print("%d-%d [%d]", duration_high, duration_low, duration_high + duration_low);
            prev_duration_high = duration_high;
            prev_duration_low = duration_low;
        }
    }
}

ISR(TIMER1_CAPT_vect)
{
    if (bit_is_clear(TCCR1B, ICES1)) {
        duration_high = ICR1;
        PORTB |= _BV(PORTB0);
    } else {
        duration_low = ICR1;
        PORTB &= ~_BV(PORTB0);
    }
    TCCR1B ^= _BV(ICES1);
    TCNT1 = 0;
}
