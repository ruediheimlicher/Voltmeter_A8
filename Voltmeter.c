//
//  Tastenblinky.c
//  Tastenblinky
//
//  Created by Sysadmin on 03.10.07.
//  Copyright Ruedi Heimlihcer 2007. All rights reserved.
//



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "defines.h"

#include "lcd.c"
#include "adc.c"
#include "sevenseg.c"

uint16_t loopCount0=0;
uint16_t loopCount1=0;
uint16_t loopCount2=0;

#define OSZIPORT   PORTB      // 
#define OSZIDDR   DDRB

#define OSZIA 4            // 
#define OSZIB 5            // 
#define OSZIALO OSZIPORT &= ~(1<<OSZIA)
#define OSZIAHI OSZIPORT |= (1<<OSZIA)
#define OSZIATOG OSZIPORT ^= (1<<OSZIA)

#define OSZIBLO OSZIPORT &= ~(1<<OSZIB)
#define OSZIBHI OSZIPORT |= (1<<OSZIB)
#define OSZIBTOG OSZIPORT ^= (1<<OSZIB)


#define LOOPLED_PORT   PORTD
#define LOOPLED_DDR   DDRD
#define LOOPLED_PIN   7

#define  IMPULS_MAX 5
#define  IMPULS_MIN 5
#define ANZAHL_IMPULSE 3

#define MITTELWERTBREITE 4



uint16_t mittelwertarray[MITTELWERTBREITE] = {};
uint8_t ringpos = 0;
uint16_t mittelwert = 0;

volatile uint8_t   INT0status=0x00;   

volatile uint8_t INT0counter = 0; // Anzahl impulse auf Spule

volatile uint8_t impulscounter = 0; // Anzahl impulse an relais: Sicher ist sicher


volatile uint16_t loopcounter = 0; // Anzahl impulse an relais: Sicher ist sicher

volatile uint16_t adcspannung = 0;
float adcspannungfloat = 0;
volatile uint16_t spannung = 0;



uint16_t adctemperatur = 0; // Spannung an Diode
float adctemperaturfloat = 0;
#define ADC_PIN 2       // Temperaturmessung Diode Anode mit Pullup
#define ADC_GND_PIN 3   // Temperaturmessung Diode Kathode: LOW


#define KENNLINIE_SIZE 50
#define MAX_TEMPERATUR 60
#define MAX_ADCTEMPERATUR 650
#define MIN_ADCTEMPERATUR 194

#define MIN_ANZEIGETEMPERATUR 46

#define ADC_DIODE_SIZE 36

#define VOLTMETERFAKTOR 0.633


//uint8_t kennlinie[KENNLINIE_SIZE] = {200,199,198,197,197,196,195,194,193,193,192,191,190,189,189,188,187,186,185,185,184,183,182,181,181,180,179,178,177,177,176,175,174,173,173,172,171,170,169,169,168,167,166,165,165,164,163,162,161,161};

uint8_t diode_temperatur[ADC_DIODE_SIZE] = {116,117,119,121,123,125,126,128,130,132,133,135,137,139,141,142,144,146,148,149,151,153,155,157,158,160,162,164,165,167,169,171,173,174,176,178};

void slaveinit(void)
{
   LOOPLED_DDR |= (1<<LOOPLED_PIN);
   
   OSZIDDR |= (1<<OSZIA);
   OSZIDDR |= (1<<OSZIB);

   DDRC &= ~(1<<0);
   //LCD
 //  LCD_DDR |= (1<<LCD_RSDS_PIN);   //Pin 5 von PORT B als Ausgang fuer LCD
 //  LCD_DDR |= (1<<LCD_ENABLE_PIN);   //Pin 6 von PORT B als Ausgang fuer LCD
 //  LCD_DDR |= (1<<LCD_CLOCK_PIN);   //Pin 7 von PORT B als Ausgang fuer LCD

   DDRC &= ~(1<<PC1); // Eingang ADC
   
   DDRC &= ~(1<<PC0);
   PORTC &= ~(1<<PC0);      // LOW  

   
   DDRC &= ~(1<<ADC_PIN); // Input Temperatur
   PORTC |= (1<<ADC_PIN);  // Pullup aktiviert

   
   DDRC |= (1<<ADC_GND_PIN); // Output Kathode auf Masse
   PORTC &= ~(1<<ADC_GND_PIN);
   
   
   /*
   DDRC |= (1<<PC2);
   PORTC &= ~(1<<PC2);      // LOW  
   DDRC |= (1<<PC3);
   PORTC &= ~(1<<PC3);      // LOW  
*/
   
}




int main (void) 
{
   /* INITIALIZE */
   //watchdog_init();
   slaveinit();
   _delay_ms(100);
 //  lcd_initialize(LCD_FUNCTION_8x2, LCD_CMD_ENTRY_INC, LCD_CMD_ON);
 //  lcd_puts("Guten Tag\0");
   _delay_ms(100);
    

   initADC(0);
//   lcd_gotoxy(16,0);
//   _delay_ms(100);
//   int i=0;
 //  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //  SLEEP_MODE_PWR_SAVE SLEEP_MODE_PWR_DOWN
   init7segment();
   sei();
  // unsigned int voltage=0;
   uint16_t voltage=0;
   //MARK  while
 //  lcd_clr_line(0);
   uint8_t anzeigestatus = 0;
   uint8_t temperatur = 0;
   while (1) 
   {
      loopCount0++;
      if (loopCount0 > 0x4F)
      {
         
         LOOPLED_PORT ^= (1<<LOOPLED_PIN);
         loopCount0 = 0;
         /*
         lcd_gotoxy(0,0);
         lcd_putint12(adcspannung);
         lcd_gotoxy(0,1);
         //voltage = 43;
         lcd_putint12(voltage);
         lcd_putc(' ');
         lcd_putint12(mittelwert);
*/
      }
      anzeigestatus++;
      loopcounter++;
      if (loopcounter == 2)
      {
         
         
         loopCount1++;
         
         // Temperaturmessung
         adctemperatur = readKanal(ADC_PIN);
         if (adctemperatur < MIN_ADCTEMPERATUR )
         {
            //          adctemperatur = MIN_ADCTEMPERATUR;
         }
         // diode_temperatur[ADC_DIODE_SIZE]
         
         /*
         for (uint8_t index = 0;index < ADC_DIODE_SIZE; index++)
         {
            if (adctemperatur > diode_temperatur[index] )
            {
               temperatur = index/2;
               break;
            }
         }
         */
         
         
         if (MAX_ADCTEMPERATUR > adctemperatur)
         {
            temperatur = MAX_ADCTEMPERATUR - adctemperatur;
         }
         else 
         {
            temperatur = 0;
         }
         
         for (uint8_t index = 0;index < ADC_DIODE_SIZE; index++)
         {
            if (temperatur < diode_temperatur[index] )
            {
               temperatur =  10 + 2*index;
               break;
            }
         }
         

         
         adcspannung = readKanal(1) ;
         
         mittelwertarray[(ringpos & 0x03)] = adcspannung;
         ringpos ++;
         
         mittelwert = 0;
         for(uint8_t i=0;i<MITTELWERTBREITE;i++)
         {
            mittelwert += mittelwertarray[i];
         }
         
         // korrektur ADC
         float mittelwertfloat = mittelwert;
         mittelwertfloat *= VOLTMETERFAKTOR;
         // Korrektur Temperatur
         //  mittelwertfloat -= 2*temperatur;
         
         mittelwertfloat /= MITTELWERTBREITE;
         
         voltage = (mittelwertfloat);
         
         voltage /= 2;
         
      }
      
      
      if (loopcounter > 0x14)
      {
         loopcounter = 0;
      }
      if (temperatur < MIN_ANZEIGETEMPERATUR)
      {
         upd7segment(voltage);
      }
      else 
      {
      if (anzeigestatus < 0xDF)
      {
         upd7segment(voltage);
         //upd7segment(adcspannung);
         //upd7segment(mittelwertarray[(ringpos & 0x02)] );
      }
       else 
       {
          //upd7segment(temperatur);
          upd7segmenttemperatur(temperatur);
       }
      }
   }
   
   
   return 0;
}
