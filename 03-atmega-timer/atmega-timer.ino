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

int main()
{
    DDRB |= _BV(DDB0);
    PORTB &= ~_BV(PORTB0);

    TCCR1A = 0;
    TCCR1B = _BV(CS12) | _BV(CS10);   // 1024 prescaler (about 1ms@ 1MHz)

    // TOP value for the timer (when reached the interrupt will be triggered)
    TIMSK1 = _BV(OCIE1A);
    OCR1A = 1000 ;

    // enable interrupts
    sei();

    while (1) {
    }
}

ISR(TIMER1_COMPA_vect)
{
    if (bit_is_set(PINB, PINB0)) {
        PORTB &= ~_BV(PORTB0);
    } else {
        PORTB |= _BV(PORTB0);
    }

    // reset timer
    TCNT1H = 0;
    TCNT1L = 0;
}
