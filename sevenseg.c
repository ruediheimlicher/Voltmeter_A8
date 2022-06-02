//
//  sevenseg.c
//  Voltmeter
//
//  Created by Ruedi Heimlicher on 26.06.2021.
//

#include "sevenseg.h"


#define DP_DIG 1
void init7segment(void)
{
        // least significant digit, cathode transistor:
   
   //DIGIDDR0 |= (1<<diga)|(1<<digb)|(1<<digc)|(1<<digd)|(1<<dige);
   DIGIPORT0 |= 0x7F; // bit 1,2,3,4,5 HI
   DIGIDDR0 |= 0x7F; // OUTPUT
   
   
   DIGIPORT1 |= 0xC0; // bit 6,7 HI: dp
   DIGIDDR1 |= 0xC0; // OUTPUT
   DIGIPORT1 &= ~(1<<dp1); // dp1 on
   
   SEGPORT |= (1<<seg0); // HI: PNP
   SEGPORT |= (1<<seg1);
   SEGPORT |= (1<<seg2);

   SEGDDR |= (1<<seg0); // Segment output
   SEGDDR |= (1<<seg1);
   SEGDDR |= (1<<seg2);

   
 
}
void setportb(uint8_t wert,uint8_t mask)
{
   uint8_t pos = 8;
   
   while(pos--)
   {
    //  if (mask & (1<<pos))
      {
         if (wert & (1<<pos))
         {
            PORTB |= (1<<pos);
         }
         else
         {
            PORTB &= ~(1<<pos);
         }
      }
   }
}
void setportd(uint8_t wert,uint8_t mask)
{
   uint8_t pos = 8;
   
   while(pos--)
   {
   //   if (mask & (1<<pos))
      {
         if (wert & (1<<pos))
         {
            PORTD &= ~(1<<pos);
         }
         else
         {
            PORTD |= (1<<pos);
         }
      }
   }
}
void rotsegment(void)
{
   
   SEGPORT &= ~(1<<seg0);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg0);// HI: digit off
   _delay_ms(DIGION);
   SEGPORT &= ~(1<<seg1);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg1);// HI: digit off
   _delay_ms(DIGION);
   SEGPORT &= ~(1<<seg2);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg2);// HI: digit off
   _delay_ms(DIGION);
}
// Update the sevent segement display.
// You must loop over this function all the time.
void upd7segment(unsigned int number)
{
  // SEGPORT |= 0x03;
   
 
   // {0x40,0x4F,0x2B,0xA,0x7,0x12,0x10,0x4B,0x1,0x3}
   // LS digit:
   //uint8_t digiwert = d2led[(number%10)]; // modulo
/*
   lcd_gotoxy(0,1);
   lcd_putint2(number%10);
   lcd_putc(' ');
   lcd_puthex(digiwert);
 */  
 
   DIGIPORT0 = d2led[(number%10)]; // modulo
   
   SEGPORT &= ~(1<<seg0);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   number/=10; // divide by 10 (shift off right most digit)
   SEGPORT |= (1<<seg0);// HI: digit off
   DIGIPORT1 &= ~(1<<dp1); // dp on
   _delay_ms(DIGIPAUSE);
   //DIGIPORT0 = 0;
   
   //_delay_ms(2);
#if (DP_DIG == 0)
   if (number>0)
   { // chop off the two left most digits if it is zero
#endif
      // middle digit:
    //  digiwert = d2led[(number%10)];
      /*
      lcd_gotoxy(0,2);
      lcd_putint2(number%10);
      lcd_putc(' ');
      lcd_puthex(digiwert);
      */
      DIGIPORT0 = d2led[(number%10)];
     
      SEGPORT &= ~(1<<seg1);// digit on for a short time:
      
      _delay_ms(DIGION);
      
      number/=10; // divide by 10 (shift off right most digit)
      SEGPORT |= (1<<seg1);// digit off
      
      _delay_ms(DIGIPAUSE);
 //_delay_ms(2);

#if (DP_DIG == 1 || DP_DIG == 0)
      if (number>0)
      { // chop off left most digit if it is zero
#endif
         //
         // left digit, we assume there that the number is
         // smaller than 999, no modulo here:
         //digiwert = d2led[(number)];
    //     lcd_gotoxy(12,1);
    //     lcd_puthex(digiwert);
          
         DIGIPORT0 = d2led[(number)];
   //      lcd_puthex(PINC & 0x3E);
         SEGPORT &= ~(1<<seg2);// digit on for a short time:
         _delay_ms(2*DIGION);
         SEGPORT |= (1<<seg2);// digit off
         _delay_ms(DIGIPAUSE);
#if (DP_DIG == 1 || DP_DIG == 0)
      }
      
 #endif
#if (DP_DIG == 0)
   }
#endif
 }

// Update the sevent segement display for temperatur: Add Grad-icon
// You must loop over this function all the time.
void upd7segmenttemperatur(unsigned int number)
{
  // SEGPORT |= 0x03;
   
 
   // {0x40,0x4F,0x2B,0xA,0x7,0x12,0x10,0x4B,0x1,0x3}
   // LS digit:
   //uint8_t digiwert = d2led[(number%10)]; // modulo
 
   DIGIPORT0 = grad2led;
   SEGPORT &= ~(1<<seg0);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   SEGPORT |= (1<<seg0);// HI: digit off
   _delay_ms(DIGION);
   DIGIPORT0 = d2led[(number%10)]; // modulo
   
   SEGPORT &= ~(1<<seg1);// LO:  digit on for a short time:
   _delay_ms(DIGION);
   number/=10; // divide by 10 (shift off right most digit)
   SEGPORT |= (1<<seg1);// HI: digit off
   DIGIPORT1 |= (1<<dp1); // dp off
   _delay_ms(DIGIPAUSE);
   //DIGIPORT0 = 0;
   
   //_delay_ms(2);
#if (DP_DIG == 0)
   if (number>0)
   { // chop off the two left most digits if it is zero
#endif
      // middle digit:
    //  digiwert = d2led[(number%10)];
      /*
      lcd_gotoxy(0,2);
      lcd_putint2(number%10);
      lcd_putc(' ');
      lcd_puthex(digiwert);
      */
      DIGIPORT0 = d2led[(number%10)];
     
      SEGPORT &= ~(1<<seg2);// digit on for a short time:
      
      _delay_ms(DIGION);
      
      number/=10; // divide by 10 (shift off right most digit)
      SEGPORT |= (1<<seg2);// digit off
      
      _delay_ms(DIGIPAUSE);
 //_delay_ms(2);

#if (DP_DIG == 1 || DP_DIG == 0)
      if (number>0)
      { // chop off left most digit if it is zero
#endif
         //
         // left digit, we assume there that the number is
         // smaller than 999, no modulo here:
         //digiwert = d2led[(number)];
    //     lcd_gotoxy(12,1);
    //     lcd_puthex(digiwert);
          
         DIGIPORT0 = d2led[(number)];
   //      lcd_puthex(PINC & 0x3E);
//         SEGPORT &= ~(1<<seg2);// digit on for a short time:
         _delay_ms(2*DIGION);
//         SEGPORT |= (1<<seg2);// digit off
         _delay_ms(DIGIPAUSE);
#if (DP_DIG == 1 || DP_DIG == 0)
      }
      
 #endif
#if (DP_DIG == 0)
   }
#endif
 }
