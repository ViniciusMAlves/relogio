#include <TimerOne.h>                     // Biblioteca TimerOne 
#include <Wire.h>                         // Biblioteca Wire 
#include <MultiFuncShield.h>              // Biblioteca Multifunction shield

int control = 0;
unsigned long tempo1  = 0;
unsigned long tempo2  = 1000;

volatile unsigned int milliSeconds = 0;
signed int seconds = 0;
signed int minutes = 0;
signed int hours = 0;
signed int conta = 1; 

int alarmMinutes = 30;                                 // alarm inicial 06:30 hs
int alarmHours = 6;
volatile byte alarmEnabled = false;  
byte precionadoAlarme = false; 

byte btn; 

enum displayModeValues                                  // modos de amostragem do relógio
{
  normal,
  setaHora,
  setaMinuto,
  Alarme,
  setaAHora,
  setaAMinuto
};

byte displayMode = normal;

void setup()
{
  Serial.begin(9600);
  Timer1.initialize();
  MFS.userInterrupt = clockISR;   
  MFS.initialize(&Timer1);
  Serial.println("Pressione um ou mais Botoes");

}
void loop()
{
  tempo1 = millis();
  btn = MFS.getButton();

  switch(displayMode)
  {
    case normal:
      FuncNormal();      
      break;
       
    case setaHora:
      FuncSetaHora();      
      break;
      
    case setaMinuto:
      FuncSetaMinuto();
      break;
      
    case Alarme:
      FuncAlarme();
      break;
      
    case setaAHora:
      FuncSetaAHora();      
      break;
      
    case setaAMinuto:                   // modo acerto dos minutos do alarme
      FuncSetaAMinuto();      
      break; 
  }
}

void displayTime (byte hours, byte minutes)           //  mostra relógio
{
  char time[5];                                       // vetor de 5 caracteres
       // mostra horas e minutos
  if((seconds % 2)==0 ){
    sprintf(time, "%02d.%02d", hours, minutes);  
  }else{
    sprintf(time, "%02d%02d", hours, minutes);
  }
  MFS.write(time, 1);
}

void verificaMin(){
  if(minutes >= 60){
    verificaHour();    
    hours++;
    
    minutes = 0;
  }  
}

void verificaHour(){
  if(hours >= 24){
    hours = 0;  
  }  
}

void verificaSec(){
  if(seconds >= 60){
    minutes ++;
    seconds = 0;  
  }  
}

void clockISR ()
{
  // Procedimento de contagem de tempo
  if(conta)
  {
    milliSeconds++;
    if (milliSeconds >= 1000)
    {
      milliSeconds = 0;
      seconds++;
      if (seconds >= 60)
      {
        seconds = 0;
        minutes++;
        if (minutes >= 60)
        {
          minutes = 0;
          hours++;
          if (hours >= 24)
          {
            hours = 0;
          }
        }
        // se o horario do relógio coincide com o alarme, dispara o bip
        if (alarmEnabled && (minutes == alarmMinutes) && (hours == alarmHours))
        {
          MFS.beep(
            10,        // 10 ms ON
            5,         // 5 ms OFF
            4,         // numero de repetições
            100,       // número de sequências
            50         // atraso entre as sequências
          );
        }
      }
    }
  }
}

void FuncNormal(){
  displayTime(hours,minutes); 
  if (btn == BUTTON_1_LONG_PRESSED)
  {
    MFS.blinkDisplay(DIGIT_ALL, OFF);                                 // para de piscar todos os dígitos
    MFS.blinkDisplay(DIGIT_1 | DIGIT_2);                              // pisca somente as horas
    displayMode = setaHora;                                           // seleciona modo de acerto de horas
    conta = false;                                                    // contagem de tempo parada
    milliSeconds = 0;                                                 // zera os contadores de milisegundos
    seconds = 0;       
  }
  else if (btn == BUTTON_2_LONG_PRESSED)                              // se o botão 2 foi pressionado
  {                                                     
    displayMode = Alarme;                                             // seleciona o modo de alarme
  }  
  else if (btn == BUTTON_2_PRESSED) 
  {
    MFS.beep(0);     
  }
  else if (btn == BUTTON_3_LONG_PRESSED && !precionadoAlarme)         // se o botão 3 for pressionado
  {
    precionadoAlarme = true;                                          // liga o alarme
    alarmEnabled = !alarmEnabled;
    MFS.writeLeds(LED_1, alarmEnabled);                               // acende o Led 1
  }
  else if (btn == BUTTON_3_LONG_RELEASE)                              // soltando o botão 3
  {
    precionadoAlarme = false;                                         // alarme pressionado é falso
  }  
}

void FuncSetaHora(){
  if (btn == BUTTON_1_PRESSED)                                        // se o botão 1 for pressionado
  {
    MFS.blinkDisplay(DIGIT_1 | DIGIT_2, OFF);                         // para de piscar as horas
    MFS.blinkDisplay(DIGIT_3 | DIGIT_4);                              // pisca os minutos
    displayMode = setaMinuto;                                         // modo para acerto de minutos
  }
  else if (btn == BUTTON_2_PRESSED || btn == BUTTON_2_LONG_PRESSED)   // se o botão 2 for pressionado
  {
    hours++;                                                          // incrementa as horas
    if (hours >= 24)                                                  // se for mais de 24 horas
    {
      hours = 0;                                                      // zera as horas
    }
    displayTime(hours, minutes);                                      // mostra horas e minutos
  }  
  else if (btn == BUTTON_3_PRESSED || btn == BUTTON_3_LONG_PRESSED)   // se o botão 2 for pressionado
  {
    hours--;                                                          // incrementa as horas
    if (hours < 0)                                                  // se for menos de 0 horas
    {
      hours = 23;                                                      // zera as horas
    }
    displayTime(hours, minutes);                                      // mostra horas e minutos
  }  
}

void FuncSetaMinuto(){
  if (btn == BUTTON_1_PRESSED)                                        // se o botão 1 for pressionado
  {
    MFS.blinkDisplay(DIGIT_3 | DIGIT_4, OFF);                         // para de piscar os minutos
    displayMode = normal;                                             // modo normal
    conta = true;                                                     // contagem de tempo iniciada
  }
  else if (btn == BUTTON_2_PRESSED || btn == BUTTON_2_LONG_PRESSED)   // se o botão 3 for pressionado
  {
    minutes++;                                                        // incrementa os minutos
    if (minutes >= 60)                                                // se for mais de 60 minutos
    {
      minutes = 0;                                                    // zera os minutos
    }
    displayTime(hours, minutes);                                      // mostra horas e minutos
  }  
  else if (btn == BUTTON_3_PRESSED || btn == BUTTON_3_LONG_PRESSED)   // se o botão 2 for pressionado
  {
    minutes--;                                                          // incrementa as horas
    if (minutes < 0)                                                // se for mais de 60 minutos
    {
      minutes = 59;                                                    // zera os minutos
    }
    displayTime(hours, minutes);                                      // mostra horas e minutos
  }  
}

void FuncAlarme(){
  displayTime(alarmHours, alarmMinutes);                              // mostra horas e minutos do alarme
  if (btn == BUTTON_2_PRESSED)                                        // se o botão 2 for pressionado
  {
    displayMode = normal;                                             // modo normal
  }
  else if (btn == BUTTON_1_LONG_PRESSED)                              // se o botão 1 for pressionado longo
  {
    MFS.blinkDisplay(DIGIT_ALL, OFF);                                 // para de piscar todos os dígitos
    MFS.blinkDisplay(DIGIT_1 | DIGIT_2);                              // pisca somente as horas
    displayMode = setaAHora;                                          // modo acerto de horas do alarme
    alarmEnabled = false;                                             // alarme desligado
  }  
}

void FuncSetaAHora(){
  if (btn == BUTTON_1_PRESSED)                                        // se o botão 1 for pressionado
  {
    MFS.blinkDisplay(DIGIT_1 | DIGIT_2, OFF);                         // para de piscar todos os dígitos
    MFS.blinkDisplay(DIGIT_3 | DIGIT_4);                              // pisca os minutos
    displayMode = setaAMinuto;                                        // modo acerto dos minutos do alarme
  }
  else if (btn == BUTTON_2_PRESSED || btn == BUTTON_2_LONG_PRESSED)   // se o botão 3 for pressionado
  {
    alarmHours++;                                                     // incrementa horas do alarme
    if (alarmHours >= 24)                                             // se for mais de 24 horas
    {
      alarmHours = 0;                                                 // zera as horas
    }
    displayTime(alarmHours, alarmMinutes);                            // mostra horas e minutos do alarme
  }  
  else if (btn == BUTTON_3_PRESSED || btn == BUTTON_3_LONG_PRESSED)   // se o botão 3 for pressionado
  {
    alarmHours--;                                                     // incrementa horas do alarme
    if (alarmHours < 0)                                             // se for mais de 24 horas
    {
      alarmHours = 23;                                                 // zera as horas
    }
    displayTime(alarmHours, alarmMinutes);                            // mostra horas e minutos do alarme
  }  
}

void FuncSetaAMinuto(){
  if (btn == BUTTON_1_PRESSED)                                        // se o botão 1 for pressionado
  {
    MFS.blinkDisplay(DIGIT_3 | DIGIT_4, OFF);                         // para de piscar os minutos
    displayMode = normal;                                             // modo normal
    alarmEnabled = true;                                              // alarme ligado
    MFS.writeLeds(LED_1, ON);                                         // acende o Led 1
  }
  else if (btn == BUTTON_2_PRESSED || btn == BUTTON_2_LONG_PRESSED)   // se o botão 3 for pressionado
  {
    alarmMinutes++;                                                   // incrementa minutos do alarme
    if (alarmMinutes >= 60)                                           // se for mais de 60 minutos
    {
      alarmMinutes = 0;                                               // zera os minutos
    }
    displayTime(alarmHours, alarmMinutes);                            // mostra horas e minutos do alarme
  }  
  else if (btn == BUTTON_3_PRESSED || btn == BUTTON_3_LONG_PRESSED)   // se o botão 3 for pressionado
  {
    alarmMinutes--;                                                   // incrementa minutos do alarme
    if (alarmMinutes < 0)                                           // se for mais de 60 minutos
    {
      alarmMinutes = 60;                                               // zera os minutos
    }
    displayTime(alarmHours, alarmMinutes);                            // mostra horas e minutos do alarme
  }  
}
