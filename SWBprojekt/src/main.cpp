#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

int dataPin=19;
int latchPin=18;
int clockPin=17;

int czujnikPin=26;

int pinCzerwona=27;
int pinZolta=32;
int pinZielona=33;

SemaphoreHandle_t sygnal;
TaskHandle_t taskhandle;


byte digits[10] = {
  //Gfedcba DP
  B01111110, 
  B00001100, 
  B10110110, 
  B10011110, 
  B11001100, 
  B11011010, 
  B11111010, 
  B00001110, 
  B11111110, 
  B11011111
};

byte digitOff = B00000000;


void wyswietlLiczby(byte wartosc){
  digitalWrite(latchPin,LOW);
  shiftOut(dataPin,clockPin,MSBFIRST,wartosc);
  digitalWrite(latchPin,HIGH);
}

LiquidCrystal_I2C lcd(0x27, 16, 2);

void TaskCzujnik(void *a){

  for(;;){
  int stanCzujnik=digitalRead(czujnikPin);
  if(stanCzujnik==HIGH){
    Serial.println("wykrylem ruch");
    xSemaphoreGive(sygnal);
    vTaskDelay(pdMS_TO_TICKS(25000));
  }
  else{
    Serial.println("nic nie wykrylem");

  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}
}

void TaskOdliczanie(void *a){
  for(;;){
    if(xSemaphoreTake(sygnal,portMAX_DELAY)==pdTRUE){
      for(int i=0;i<10;i++){
        wyswietlLiczby(digits[i]);
          vTaskDelay(pdMS_TO_TICKS(1000));
          
        }
        // xTaskNotify(taskhandle, 0, eSetValueWithoutOverwrite);
        xTaskNotifyGive(taskhandle);

      }
      wyswietlLiczby(digitOff);

    }


  }

  void TaskLampki(void *a){
    for(;;){
      uint32_t wartosc;
      ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
      // if(wartosc==0){
        // digitalWrite(pinCzerwona,HIGH);
        // lcd.setCursor(0,0);
        // lcd.print("czerwone swiatlo");
        // vTaskDelay(1000);
        digitalWrite(pinCzerwona,LOW);
        lcd.setCursor(0,0);
        lcd.print("                 ");


        digitalWrite(pinZolta,HIGH);
        lcd.setCursor(0,0);
        lcd.print("przygotuj sie");
        vTaskDelay(1000);
        digitalWrite(pinZolta,LOW);
        lcd.setCursor(0,0);
        lcd.print("                 ");


        digitalWrite(pinZielona,HIGH);
        lcd.setCursor(0,1);
        lcd.print("mozesz jechac");
        
        vTaskDelay(10000);
        digitalWrite(pinZielona,LOW);
        lcd.setCursor(0,1);
        lcd.print("                 ");
        digitalWrite(pinCzerwona,HIGH);
        lcd.setCursor(0,0);
        lcd.print("czerwone swiatlo");


      // }


    }
    

  }
  




void setup(){

  
  Serial.begin(9600);
  

  pinMode(latchPin,OUTPUT);
  pinMode(clockPin,OUTPUT);
  pinMode(dataPin,OUTPUT);
  pinMode(czujnikPin,INPUT_PULLDOWN);
  pinMode(pinCzerwona,OUTPUT);
  pinMode(pinZolta,OUTPUT);
  pinMode(pinZielona,OUTPUT);

  digitalWrite(latchPin,LOW);
  shiftOut(dataPin,clockPin,MSBFIRST,B00000000);
  digitalWrite(latchPin,HIGH);


  lcd.init();       
  lcd.backlight();  

  digitalWrite(pinCzerwona,HIGH);
  lcd.setCursor(0,0);
  lcd.print("czerwone swiatlo");
  
  
  sygnal=xSemaphoreCreateBinary();

  xTaskCreate(TaskCzujnik,"TC",2048,NULL,1,NULL);
  xTaskCreate(TaskOdliczanie,"TO",2048,NULL,1,NULL);
  xTaskCreate(TaskLampki,"TL",2048,NULL,1,&taskhandle);

  
}

void loop(){

  


  
}

