#include <LedProject.h>
#fuses XT,NOWDT
#use delay(clock=4M)
#use fast_io(B)

#define SHIFT_OUTPUT_NUMBER 8

enum state{
   INIT,
   SEQUENCE_1,
   SEQUENCE_2,
   SEQUENCE_3,
};

enum state currentState;

#INT_EXT
void ext_isr(){
   currentState++;
   //Next case must start with LEDS OFF
   output_low(PIN_B4); //Q1
   output_low(PIN_B5); //Q2
   if(currentState>3)currentState=0;
}


//Function declaration
void ShiftBits(int);

void main()
{
   //PORT B AS OUTPUT EXCEPT RB0
   set_tris_b(0b00000001);
   //INITIAL STATE -- ALL LEDS TURNED OFF
   currentState=INIT;
   //Enable General Interrupts
   enable_interrupts(GLOBAL);
   //Enable RB0 as INT_EXT
   enable_interrupts(int_ext);
   //Falling edge
   ext_int_edge(H_TO_L);
   
   int outputCounter=0;
   int1 firstTimeflag=1;
   
   while(TRUE)
   {
      
      switch (currentState)
      {
         case INIT:
            firstTimeflag=1;
         break;
         case SEQUENCE_1:
            //ACTIVATION FOR Q1 & Q2 (NPN)
            output_high(PIN_B4); //Q1
            output_high(PIN_B5); //Q2   
            //TURN ON LEDS
            output_high(PIN_B1);             //SERIAL DATA --> 0b11111111
            ShiftBits(SHIFT_OUTPUT_NUMBER);  //SHIFT DATA TO REGISTER OUTPUTS
            output_high(PIN_B3);
            delay_ms(5);                     //LATCH PULSE              
            output_low(PIN_B3);
            delay_ms(500);
            //TURN OFF LEDS
            output_low(PIN_B1);             //SERIAL DATA --> 0b00000000
            ShiftBits(SHIFT_OUTPUT_NUMBER);  //SHIFT DATA TO REGISTER OUTPUTS
            output_high(PIN_B3);
            delay_ms(5);                     //LATCH PULSE              
            output_low(PIN_B3);
            delay_ms(500);
         break;
         case SEQUENCE_2:
            //ACTIVATION FOR Q1 & Q2 (NPN)
            output_high(PIN_B4); //Q1
            output_high(PIN_B5); //Q2
            //TURN ON LEDS
            output_high(PIN_B1);
            ShiftBits(1);  //SHIFT DATA TO REGISTER OUTPUTS
            output_high(PIN_B3);
            delay_ms(5);                     //LATCH PULSE              
            output_low(PIN_B3);
            delay_ms(500);
            //TURN OFF LEDS
            output_low(PIN_B1);
            ShiftBits(1);  //SHIFT DATA TO REGISTER OUTPUTS
            output_high(PIN_B3);
            delay_ms(5);                     //LATCH PULSE              
            output_low(PIN_B3);
            delay_ms(500);
            //DEACTIVATION FOR Q2 (NPN)
            output_low(PIN_B5);
         break;
         case SEQUENCE_3:
            if(outputCounter<5 && !firstTimeflag){
               //ACTIVATION FOR Q1 & Q2 (NPN)
               output_high(PIN_B4); //Q1
               output_high(PIN_B5); //Q2
               //TURN ON LEDS
               output_high(PIN_B1);
               ShiftBits(1);  //SHIFT DATA TO REGISTER OUTPUTS
               output_high(PIN_B3);
               delay_ms(5);                     //LATCH PULSE              
               output_low(PIN_B3);
               delay_ms(500);
               outputCounter++;
            }
            if(outputCounter>4 || firstTimeflag){
               outputCounter=0;
               firstTimeflag=0;
               //DEACTIVATION FOR Q1 & Q2 (NPN)
               output_low(PIN_B4);  //Q1
               output_low(PIN_B5);  //Q2
               //TURN OFF LEDS
               output_low(PIN_B1);
               ShiftBits(8);  //SHIFT DATA TO REGISTER OUTPUTS
               output_high(PIN_B3);
               delay_ms(5);                     //LATCH PULSE              
               output_low(PIN_B3);
            }
         break;
      }
   }
}

void ShiftBits(int n)
{
   for (int i=0;i<n;i++)
   {
      output_high(PIN_B2);
      delay_ms(5);
      output_low(PIN_B2);
      delay_ms(5);
   }
}
