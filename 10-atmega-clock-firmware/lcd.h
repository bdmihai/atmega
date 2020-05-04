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
 
#pragma once

// 0-Hidden cursor, 1-Cursor in form of a flashing block, 2-Cursor as an underscore, 3-Cursor in form of a flashing block with an underscore
#define LCD_CURSOR_HIDDEN                       0
#define LCD_CURSOR_FLASHING_BLOCK               1
#define LCD_CURSOR_UNDERSCORE                   2
#define LCD_CURSOR_FLASHING_BLOCK_UNDERSCORE    3

// LED backlit n1=0: off, n1=1: on; n1=2: invert; n1=3..255 n1/10 sec. on
#define LCD_LED_OFF         0
#define LCD_LED_ON          1
#define LCD_LED_INVERTED    2

void lcd_init();
void lcd_reset();
void lcd_clear();
void lcd_caridge_return();
void lcd_line_feed();
void lcd_version();
void lcd_move_cursor(unsigned char row, unsigned char col);
void lcd_set_cursor_type(unsigned char cursor);
void lcd_set_led_backlight(unsigned char led);
void lcd_putc(char c);
void lcd_print(char *fmt, ...);

