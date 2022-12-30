/*
 HCS Programmer
 By Liyanboy74
 https://github.com/liyanboy74
*/

#include <stdio.h>

#define DEBUG 1

#define LED_PIN LED_BUILTIN

#define VDD_PIN 2
#define CLK_PIN 3
#define PWM_PIN 4

uint8_t Byte,i,j,WH,WL;
uint16_t Data[12];
uint16_t DataVF[12];

#if(DEBUG)
char buffer[80];
#endif

void config_pwm_out()
{
  pinMode(PWM_PIN,OUTPUT);
  digitalWrite(PWM_PIN,LOW);
}

void config_pwm_in()
{
  digitalWrite(PWM_PIN,LOW);
  pinMode(PWM_PIN,INPUT);
}

void setup()
{
  Serial.begin(9600);

  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);

  pinMode(VDD_PIN,OUTPUT);
  digitalWrite(VDD_PIN,LOW);

  pinMode(CLK_PIN,OUTPUT);
  digitalWrite(CLK_PIN,LOW);

  config_pwm_out();

  digitalWrite(LED_PIN,HIGH);
  delay(100);
  digitalWrite(LED_PIN,LOW);
}

void loop()
{
  if(Serial.available())
  {
    if(Serial.readBytes(&Byte,1))
    {
      if(Byte=='!')
      {
        digitalWrite(LED_PIN,HIGH);
        Serial.println("!Arduino HCS301 Programmer V0.1");
        digitalWrite(LED_PIN,LOW);
      }
      else if(Byte=='>')
      {
        digitalWrite(LED_PIN,HIGH);
        delay(150);
        for(i=0;i<12;i++)
        {
          Serial.readBytes(&WH,1);
          Serial.readBytes(&WL,1);
          Data[i]=(WH<<8)|WL;
        }

        #if(DEBUG)
        sprintf(buffer,"<CHECK %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X",Data[0],Data[1],Data[2],Data[3],Data[4],Data[5],Data[6],Data[7],Data[8],Data[9],Data[10],Data[11]);
        Serial.println(buffer);
        #endif

        config_pwm_out();

        digitalWrite(VDD_PIN,HIGH);
        digitalWrite(CLK_PIN,HIGH);
        digitalWrite(PWM_PIN,LOW);
        delay(4);
        digitalWrite(PWM_PIN,HIGH);
        delay(4);
        digitalWrite(CLK_PIN,LOW);
        digitalWrite(PWM_PIN,LOW);
        delay(5);
        for(i=0;i<12;i++)
        {
          for(j=0;j<16;j++)
          {
            if((Data[i]>>j)&0x0001)digitalWrite(PWM_PIN,HIGH);
            else digitalWrite(PWM_PIN,LOW);
            digitalWrite(CLK_PIN,HIGH);
            delayMicroseconds(100);
            digitalWrite(CLK_PIN,LOW);
            delayMicroseconds(100);
          }
          digitalWrite(PWM_PIN,LOW);
          if(i==11)config_pwm_in();
          delay(60);
        }

        for(i=0;i<12;i++)
        {
          DataVF[i]=0;
          for(j=0;j<16;j++)
          {
            digitalWrite(CLK_PIN,HIGH);
            DataVF[i]|=((digitalRead(PWM_PIN))?1:0)<<j;
            delayMicroseconds(100);
            digitalWrite(CLK_PIN,LOW);
            delayMicroseconds(100);
          }
        }

        #if(DEBUG)
        sprintf(buffer,"<VERIFY %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X",DataVF[0],DataVF[1],DataVF[2],DataVF[3],DataVF[4],DataVF[5],DataVF[6],DataVF[7],DataVF[8],DataVF[9],DataVF[10],DataVF[11]);
        Serial.println(buffer);
        #endif

        digitalWrite(VDD_PIN,LOW);
        config_pwm_out();

        digitalWrite(LED_PIN,LOW);

        j=0;
        for(i=0;i<12;i++)if(Data[i]!=DataVF[i])j++;
        if(j)Serial.println("eVerify not match!");
        else Serial.println("<Verify  Done Successfuly!");

      }
    }
  }
}
