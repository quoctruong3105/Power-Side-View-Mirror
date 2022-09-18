#include <mega328p.h>
#include <delay.h>
#include <alcd.h>
#include <stdio.h>

#define openBtn 1023
#define closeBtn 731
#define leftBtn 539
#define rightBtn 394
#define upBtn 269
#define downBtn 146
#define OC_DDR DDRB.3
#define LR_DDR DDRD.5
#define UD_DDR DDRD.6
#define OC_PORT PORTB.3
#define LR_PORT PORTB.5
#define UD_PORT PORTB.6
#define TRIGGER_DDR DDRD.0
#define ECHO_DDR DDRD.1
#define TRIGGER_PORT PORTD.0
#define ECHO_PORT PORTD.1
#define ECHO_PIN PIND.1
#define ADC_VREF_TYPE 0x40

int distant;

// Read the AD conversion result
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

void activeDistantSensor()
{
    int duration; 
    unsigned int i;  
    char buffer[];
    // Trigger transmit pulse
    TRIGGER_PORT = 0;
    delay_ms(2);
    TRIGGER_PORT = 1;
    delay_ms(10);
    TRIGGER_PORT = 0;                   
    //Measure duty cycle of Echo
    TCNT1H=0x00;
    TCNT1L=0x00;
    TCCR1B=0b00000101; 
    while(ECHO_PIN == 0) 
    {    
          
    }  
    i = (TCNT1H*256 + TCNT1L);               
    duration = i*0.128;
    distant = duration/2/29.412;   
    sprintf(buffer, "Dis: %0.3f cm", i);
    lcd_puts(buffer);
}

void disPlayLCD(unsigned int thamSo)        
{
    unsigned char chucNgan, ngan, tram, chuc, donVi;
    chucNgan = thamSo/10000;
    ngan = thamSo/1000%10;
    tram = thamSo/100%10;
    chuc = thamSo/10%10;
    donVi = thamSo%10;
    lcd_putchar(chucNgan + 48);    
    lcd_putchar(ngan + 48);    
    lcd_putchar(tram + 48);    
    lcd_putchar(chuc + 48);    
    lcd_putchar(donVi + 48);          
}

// Open mirror
void open()
{
    OCR2A = 20;            
    lcd_puts("Opening");   
    delay_ms(1500);  
}

// Close mirror
void close()
{
    OCR2A = 5;  
    OCR0B = 15;
    OCR0A = 15; 
    lcd_puts("Closing");  
    delay_ms(1500);   
}

// Adjust left
void adjustLeft()
{
    if(OCR0B > 10)
    {
        OCR0B--; 
        lcd_puts("Adjusting Left");  
    }   
    else
    {
        lcd_puts("Completely Left");  
        delay_ms(500);     
    }
}

// Adjust right
void adjustRight()
{
    if(OCR0B < 19) 
    {
        OCR0B++;
        lcd_puts("Adjusting Right");  
    }                
    else
    {
        lcd_puts("Completely Right");  
        delay_ms(500); 
    }
}

// Adjust up
void adjustUp()
{
    if(OCR0A < 19) 
    {
        OCR0A++;  
        lcd_puts("Adjusting Up"); 
    }  
    else
    {
        lcd_puts("Completely Up"); 
        delay_ms(500);
    }
}

// Adjust down
void adjustDown()
{
    if(OCR0A > 10)
    {
        OCR0A--;   
        lcd_puts("Adjusting Down");
    }
    else
    {
        lcd_puts("Completely Down"); 
        delay_ms(500);
    }
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
TCCR0A=0b10100011;
TCCR0B=0b00000101;
TCNT0=0;
OCR0A=15;
OCR0B=15;

// Timer/Counter 2 initialization
TCCR2A=0b10000011;
TCCR2B=0b00000111;
TCNT2=0;
OCR2A=0;
OCR2B=0;

// Timer/Counter 1 initialization
TCCR1A=0x00;
TCCR1B=0b00000101;
TCNT1H=10;
TCNT1L=10;

// ADC initialization
DIDR0=0x20;
ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x81;

// Characters/line: 16
lcd_init(16);

//Set up PWM Servo ports is output
OC_DDR = 1;
LR_DDR = 1;
UD_DDR = 1;

// Set up Trigger and Echo for Sensor
TRIGGER_DDR = 1; // Trigger port is output
ECHO_DDR = 0; // Echo port is input

while (1)
      {    
        lcd_clear();           
        lcd_gotoxy(0,0);  
        activeDistantSensor();      
        curBtn = read_adc(5);  
        
        if(curBtn == openBtn)
        {
            open();            
        }   
        else if(curBtn == closeBtn)
        {
            close();
        }     
        else if(curBtn == leftBtn && OCR2A == 20)
        {
            adjustLeft();
        } 
        else if(curBtn == rightBtn && OCR2A == 20)
        {
            adjustRight();
        }
        else if(curBtn == upBtn && OCR2A == 20)
        {
            adjustUp();
        }
        else if(curBtn == downBtn && OCR2A == 20)
        {
            adjustDown();
        }
        delay_ms(200); 
      }
}
