#include <mega328p.h>
#include <delay.h>
#include <alcd.h>

#define openBtn 1023
#define closeBtn 713
#define leftBtn 539
#define rightBtn 394
#define upBtn 269
#define downBtn 146

#define ADC_VREF_TYPE 0x40
unsigned int read_adc(unsigned char adc_input)
{
ADMUX=adc_input | (ADC_VREF_TYPE & 0xff);
// Delay needed for the stabilization of the ADC input voltage
delay_us(10);
// Start the AD conversion
ADCSRA|=0x40;
// Wait for the AD conversion to complete
while ((ADCSRA & 0x10)==0);
ADCSRA|=0x10;
return ADCW;
}

void hienThi(unsigned int thamSo)
{
    unsigned char ngan, tram, chuc, donVi;
    ngan = thamSo/1000;
    tram = thamSo/100%10;
    chuc = thamSo/10%10;
    donVi = thamSo%10;
    lcd_putchar(ngan + 48);    
    lcd_putchar(tram + 48);
    lcd_putchar(chuc + 48);
    lcd_putchar(donVi + 48);
}

void main(void)
{
unsigned int curBtn;

// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Timer/Counter 0 initialization
TCCR0A=0x00;
TCCR0B=0x00;
TCNT0=0x00;
OCR0A=0x00;
OCR0B=0x00;

// Timer/Counter 1 initialization
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 0 Interrupt(s) initialization
TIMSK0=0x00;

// Timer/Counter 1 Interrupt(s) initialization
TIMSK1=0x00;

// ADC initialization
// ADC Clock frequency: 500,000 kHz
// ADC Voltage Reference: AVCC pin
// ADC Auto Trigger Source: ADC Stopped
// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
// ADC4: On, ADC5: Off
DIDR0=0x20;
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x81;

// Characters/line: 16
lcd_init(16);

#asm("sei")

while (1)
      {  
        curBtn = read_adc(5);   
        lcd_clear();   
        lcd_gotoxy(0,0);
        hienThi(curBtn);
        delay_ms(300);
      }
}
