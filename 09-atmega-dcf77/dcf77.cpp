
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
 
#include <avr/interrupt.h>
#include <avr/io.h>
#include "dcf77.h"

#define DCF77_BEGIN_TIMEINFO        20 // Beginn der Zeitinformation (ist immer „1“)
#define DCF77_MINUTE_1_1            21 // Minute (Einer)	             Bit für 1
#define DCF77_MINUTE_1_2            22 //                                Bit für 2
#define DCF77_MINUTE_1_4            23 //                                Bit für 4
#define DCF77_MINUTE_1_8            24 //                                Bit für 8
#define DCF77_MINUTE_10_10          25 // Minute (Zehner)                Bit für 10
#define DCF77_MINUTE_10_20          26 //                                Bit für 20
#define DCF77_MINUTE_10_40          27 //                                Bit für 40
#define DCF77_MINUTE_PARITY         28 // Parität Minute ___________________________
#define DCF77_HOUR_1_1              29 // Stunde (Einer)	             Bit für 1
#define DCF77_HOUR_1_2              30 //                                Bit für 2
#define DCF77_HOUR_1_4              31 //                                Bit für 4
#define DCF77_HOUR_1_8              32 //                                Bit für 8
#define DCF77_HOUR_10_10            33 // Stunde (Zehner)                Bit für 10
#define DCF77_HOUR_10_20            34 //                                Bit für 20
#define DCF77_HOUR_PARITY           35 // Parität Stunde ___________________________
#define DCF77_DAY_1_1               36 // Kalendertag (Einer)	         Bit für 1
#define DCF77_DAY_1_2               37 //                                Bit für 2
#define DCF77_DAY_1_4               38 //                                Bit für 4
#define DCF77_DAY_1_8               39 //                                Bit für 8
#define DCF77_DAY_10_10             40 // Kalendertag (Zehner)	         Bit für 10
#define DCF77_DAY_10_20             41 //                                Bit für 20
#define DCF77_WEEKDAY_1_1           42 // Wochentag	                     Bit für 1
#define DCF77_WEEKDAY_1_2           43 //                                Bit für 2
#define DCF77_WEEKDAY_1_4           44 //                                Bit für 4
#define DCF77_MONTH_1_1             45 // Monatsnummer (Einer)	         Bit für 1
#define DCF77_MONTH_1_2             46 //                                Bit für 2
#define DCF77_MONTH_1_4             47 //                                Bit für 4
#define DCF77_MONTH_1_8             48 //                                Bit für 8
#define DCF77_MONTH_10_10           49 // Monatsnummer (Zehner)	         Bit für 10
#define DCF77_YEAR_1_1              50 // Jahr (Einer)	                 Bit für 1
#define DCF77_YEAR_1_2              51 //                                Bit für 2
#define DCF77_YEAR_1_4              52 //                                Bit für 4
#define DCF77_YEAR_1_8              53 //                                Bit für 8
#define DCF77_YEAR_10_10            54 // Jahr (Zehner)	                 Bit für 10
#define DCF77_YEAR_10_20            55 //                                Bit für 20
#define DCF77_YEAR_10_40            56 //                                Bit für 40
#define DCF77_YEAR_10_80            57 //                                Bit für 80
#define DCF77_DATE_PARITY           58 // Parität Datum____________________________

// decode one bit from the data buffer
#define DCF77_DECODE_DATA(bit, value)  ((data & ((unsigned long long)1 << bit)) ? value : 0)

static volatile int duration_high, duration_low;
static volatile unsigned long long data;
static volatile unsigned char index;
static volatile char status, minute, hour, day, weekday, month, year;

static void reset()
{
    data = 0;
    index = 0;
}

static void add_one()
{
    data |= ((unsigned long long)1 << index);
    index++;
}

static void add_zero()
{
    index++;
}

static void decode()
{
    unsigned char parity_minute, parity_hour, parity_date;

    parity_minute = 0;
    for (int i = DCF77_MINUTE_1_1; i < DCF77_MINUTE_PARITY; i++) {
        parity_minute += DCF77_DECODE_DATA(i, 1);
    }
    parity_minute = parity_minute % 2;

    parity_hour = 0;
    for (int i = DCF77_HOUR_1_1; i < DCF77_HOUR_PARITY; i++) {
        parity_hour += DCF77_DECODE_DATA(i, 1);
    }
    parity_hour = parity_hour % 2;

    parity_date = 0;
    for (int i = DCF77_DAY_1_1; i < DCF77_DATE_PARITY; i++) {
        parity_date += DCF77_DECODE_DATA(i, 1);
    }
    parity_date = parity_date % 2;

    if ((DCF77_DECODE_DATA(DCF77_BEGIN_TIMEINFO, 1) == 1            ) && 
        (DCF77_DECODE_DATA(DCF77_MINUTE_PARITY, 1)  == parity_minute) &&
        (DCF77_DECODE_DATA(DCF77_HOUR_PARITY, 1)    == parity_hour  ) &&
        (DCF77_DECODE_DATA(DCF77_DATE_PARITY, 1)    == parity_date  )) {

        minute =  DCF77_DECODE_DATA(DCF77_MINUTE_1_1  ,   1);
        minute += DCF77_DECODE_DATA(DCF77_MINUTE_1_2  ,   2);
        minute += DCF77_DECODE_DATA(DCF77_MINUTE_1_4  ,   4);
        minute += DCF77_DECODE_DATA(DCF77_MINUTE_1_8  ,   8);
        minute += DCF77_DECODE_DATA(DCF77_MINUTE_10_10,  10);
        minute += DCF77_DECODE_DATA(DCF77_MINUTE_10_20,  20);
        minute += DCF77_DECODE_DATA(DCF77_MINUTE_10_40,  40);

        hour =  DCF77_DECODE_DATA(DCF77_HOUR_1_1  ,  1);
        hour += DCF77_DECODE_DATA(DCF77_HOUR_1_2  ,  2);
        hour += DCF77_DECODE_DATA(DCF77_HOUR_1_4  ,  4);
        hour += DCF77_DECODE_DATA(DCF77_HOUR_1_8  ,  8);
        hour += DCF77_DECODE_DATA(DCF77_HOUR_10_10, 10);
        hour += DCF77_DECODE_DATA(DCF77_HOUR_10_20, 20);

        day =  DCF77_DECODE_DATA(DCF77_DAY_1_1  ,  1);
        day += DCF77_DECODE_DATA(DCF77_DAY_1_2  ,  2);
        day += DCF77_DECODE_DATA(DCF77_DAY_1_4  ,  4);
        day += DCF77_DECODE_DATA(DCF77_DAY_1_8  ,  8);
        day += DCF77_DECODE_DATA(DCF77_DAY_10_10, 10);
        day += DCF77_DECODE_DATA(DCF77_DAY_10_20, 20);

        weekday =  DCF77_DECODE_DATA(DCF77_WEEKDAY_1_1, 1);
        weekday += DCF77_DECODE_DATA(DCF77_WEEKDAY_1_2, 2);
        weekday += DCF77_DECODE_DATA(DCF77_WEEKDAY_1_4, 4);

        month =  DCF77_DECODE_DATA(DCF77_MONTH_1_1  ,  1);
        month += DCF77_DECODE_DATA(DCF77_MONTH_1_2  ,  2);
        month += DCF77_DECODE_DATA(DCF77_MONTH_1_4  ,  4);
        month += DCF77_DECODE_DATA(DCF77_MONTH_1_8  ,  8);
        month += DCF77_DECODE_DATA(DCF77_MONTH_10_10, 10);

        year =  DCF77_DECODE_DATA(DCF77_YEAR_1_1  ,   1);
        year += DCF77_DECODE_DATA(DCF77_YEAR_1_2  ,   2);
        year += DCF77_DECODE_DATA(DCF77_YEAR_1_4  ,   4);
        year += DCF77_DECODE_DATA(DCF77_YEAR_1_8  ,   8);
        year += DCF77_DECODE_DATA(DCF77_YEAR_10_10,  10);
        year += DCF77_DECODE_DATA(DCF77_YEAR_10_20,  20);
        year += DCF77_DECODE_DATA(DCF77_YEAR_10_40,  40);
        year += DCF77_DECODE_DATA(DCF77_YEAR_10_80,  80);

        status = 1;
    }
}

void dcf77_init()
{
    TCCR1A = 0;
    TCCR1B = _BV(ICNC1) | _BV(ICES1) | _BV(CS12) | _BV(CS10); // 1024 prescaler (about 1ms@ 1MHz)
    TIMSK1 = _BV(ICIE1);
    status = minute = hour = day = weekday = month = year = 0;
    duration_high = 0;
    duration_low = 0;
    reset();
    sei();
}

int dcf77_get_duration_low()
{
    return duration_low;
}

int dcf77_get_duration_high()
{
    return duration_high;
}

unsigned char dcf77_get_status()
{
    return status;
}

void dcf77_reset_status()
{
    status = 0;
}

unsigned char dcf77_get_minute()
{
    return minute;
}

unsigned char dcf77_get_hour()
{
    return hour;
}

unsigned char dcf77_get_day()
{
    return day;
}

unsigned char dcf77_get_weekday()
{
    return weekday;
}

unsigned char dcf77_get_month()
{
    return month;
}

unsigned char dcf77_get_year()
{
    return year;
}

ISR(TIMER1_CAPT_vect)
{
    if (bit_is_clear(TCCR1B, ICES1)) {
        duration_high = ICR1;
        if (duration_high > 120) {
            add_one();
        } else {
            add_zero();
        }
    } else {
        duration_low = ICR1;
        if (duration_low > 1500) {
            decode();
            reset();
        }
    }
    TCCR1B ^= _BV(ICES1);
    TCNT1 = 0;
}
