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
#include <stdlib.h>

static unsigned char read(unsigned int address)
{
    /* wait for completion of previous write */
    loop_until_bit_is_clear(EECR, EEPE);

    /* set up address register */
    EEAR = address;

    /* start eeprom read by writing EERE */
    EECR |= _BV(EERE);
    
    return EEDR;
}

static void write(unsigned int address, const unsigned char data)
{
    /* wait for completion of previous write */
    loop_until_bit_is_clear(EECR, EEPE);

    /* set up address and Data Registers */
    EEAR = address;
    EEDR = data;

    /* write logical one to EEMPE */
    EECR |= _BV(EEMPE);
    /* start eeprom write by setting EEPE */
    EECR |= _BV(EEPE);
}

void eeprom_read(unsigned int address, void* data, size_t size)
{
    unsigned char *buffer = (unsigned char*)data;

    while (size--) {
        *(buffer++) = read(address++);
    }
}

void eeprom_write(unsigned int address, const void* data, size_t size)
{
    const unsigned char *buffer = (const unsigned char*)data;

    while (size--) {
        write(address++, *(buffer++));
    }
}
