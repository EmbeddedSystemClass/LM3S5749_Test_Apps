/*
 * lcd.h
 *
 *  Created on: May 12, 2013
 *      Author: root
 */

#ifndef LCD_H_
#define LCD_H_

void lcdInit(void);
void setDataBus(unsigned char data);
void fillDisplay(unsigned char r, unsigned char g, unsigned char b);
void drawBox(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,
			 unsigned char r, unsigned char g, unsigned char b);

#endif /* LCD_H_ */
