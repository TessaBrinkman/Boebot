/*******************************************
*  Auteur: Piotr van Houwelingen, Tessa Brinkman, Robin van Engen
*  Datum: 27/10/2023
*  Versie 2023_11_01_V2.4
********************************************/

#include <Servo.h>   //voegt de library's toe die nodig zijn om de servo's aan te sturen via PWM.
#define LEVEL 950    // Analoog grensniveau van de (lijn/geen lijn) detectie van de reflectiemeting met de CNY70 
#define STP 1500     // waarde in micro seconden om de rotating servo's te laten stoppen.
Servo servo_L;       // maakt servo object (L)inks aan.
Servo servo_R;       // maakt servo object (R)echts aan.
Servo servo_Arm;     // maakt servo object arm aan.

const int Reflectiesensor_1 = A0;    // selecteer de input pin A0 voor reflectiesensor a met een vaste waarde (const)
const int Reflectiesensor_2 = A2;    // selecteer de input pin A1 voor reflectiesensor b met een vaste waarde (const)
const int Reflectiesensor_3 = A1;    // selecteer de input pin A2 voor reflectiesensor c met een vaste waarde (const)
const int Afstandssensor    = A3;    // select the input pin for Sharp afstandssensor 

int led_rood = 2;       // led_rood voor de LED monitor
int led_groen = 4;      // led_groen voor de LED monitor
int led_geel = 7;       // led_geel voor de LED monitor
int SensorToggle = 6;   // Digitale pin om sensoren aan en uit te zetten ivm batterijbesparing
short i = 0;            // Geeft aan hoelang case 8 waar is

 
//Prototypes:
char SensorRead(void);                        // Prototype functie: returnwaarde:  0000DCBA  A, B, C, D    0 of 1 afhankelijk of sensorwaarde hoger of lager dan LEVEL is.
void ServoActie(int);                         // Prototype functie: bepaald servoactie op basis returnwaarde SensorRead 
void SensorStatus2LEDS(char);                 // Geeft met LEDS de status van de sensoren aan.
void ObstakelDetectie_en_Verwijdering(void);  // Functie om obstakel te detecteren en te verwijderen

void setup()             // initialisatie, wordt één keer uitgevoerd
{
  servo_L.attach(11);    //koppelt servo_L (linker servo) aan pin 11
  servo_R.attach(10);    //koppelt servo_R (linker servo) aan pin 10
  servo_Arm.attach(3);   //koppelt servo_Arm aan pin 3

  servo_Arm.write(0);    //Zet servo_Arm op de 0 positie
  
  pinMode(led_rood, OUTPUT);      // stel pin "led_rood" in als Output
  pinMode(led_groen, OUTPUT);     // stel pin "led_groen" in als Output
  pinMode(led_geel, OUTPUT);      // stel pin "led_geel" in als Output
  pinMode(SensorToggle, OUTPUT);  // Optie batterijbesparing stel pin "SensorToggle" in als Output
  
  Serial.begin(9600);             //initiatie serial modem 9600 bps
  //Serial.println("Welkom bij de Boebot lijnvolger \n Seriele Communicatie OK\n"); // test de seriële communicatie
}

void loop()                             //  hoofdprogramma met een aantal functieaanroepen
{ 
  char SensorOutput;                    // Definiert de char SensorOuput
  SensorOutput = SensorRead();          // Zorgt dar SensorOutput gelijk is aan de waarde van de SensorRead functie
  SensorStatus2LEDS(SensorOutput);      // Roept de SensorStatus2LEDS aan en geeft deze de SensorOutput waarde mee
  ServoActie(SensorOutput);             // Roept de ServoActie aan en geeft deze de SensorOutput waarde mee
  ObstakelDetectie_en_Verwijdering();   // Roept de ObstakelDetectie_en_Verwijdering aan 
}

//Functies:
char SensorRead(void)
{
  //digitalWrite(SensorToggle, HIGH); // Optie: batterij besparing: Schakel de sensoren in
    
  int ReflectValue_1 = 0; //definiert ReflectValue_1
  int ReflectValue_2 = 0; //definiert ReflectValue_2
  int ReflectValue_3 = 0; //definiert ReflectValue_3
  int ReturnValue = 0;    //definiert ReturnValue
  
  ReflectValue_1 = analogRead(Reflectiesensor_1); //zorgt dat ReflectValue_1 reflectiesensor 1 uitleest
  ReflectValue_2 = analogRead(Reflectiesensor_2); //zorgt dat ReflectValue_2 reflectiesensor 2 uitleest
  ReflectValue_3 = analogRead(Reflectiesensor_3); //zorgt dat ReflectValue_3 reflectiesensor 3 uitleest
  
  //Serial.print("sensor_1 = ");        // debug                
  //Serial.println(ReflectValue_1);     // debug
  //Serial.print("sensor_2 = " );       // debug                
  //Serial.println(ReflectValue_2);     // debug
  //Serial.print("sensor_3 = " );       // debug                
  //Serial.println(ReflectValue_3);     // debug
  
  // True set: -------1, False reset: -------0
  if((ReflectValue_1 < LEVEL) && (ReflectValue_2 > LEVEL) && (ReflectValue_3 > LEVEL)) ReturnValue |= 0x01; else ReturnValue &= ~(0x00); 
  
  if((ReflectValue_1 > LEVEL) && (ReflectValue_2 < LEVEL) && (ReflectValue_3 > LEVEL)) ReturnValue |= 0x02; else ReturnValue &= ~(0x00);
  
  if((ReflectValue_1 > LEVEL) && (ReflectValue_2 > LEVEL) && (ReflectValue_3 < LEVEL)) ReturnValue |= 0x03; else ReturnValue &= ~(0x00);
  
  if((ReflectValue_1 < LEVEL) && (ReflectValue_2 < LEVEL) && (ReflectValue_3 > LEVEL)) ReturnValue |= 0x04; else ReturnValue &= ~(0x00); 
  
  if((ReflectValue_1 < LEVEL) && (ReflectValue_2 > LEVEL) && (ReflectValue_3 < LEVEL)) ReturnValue |= 0x05; else ReturnValue &= ~(0x00); 
  
  if((ReflectValue_1 > LEVEL) && (ReflectValue_2 < LEVEL) && (ReflectValue_3 < LEVEL)) ReturnValue |= 0x06; else ReturnValue &= ~(0x00); 

  if((ReflectValue_1 < LEVEL) && (ReflectValue_2 < LEVEL) && (ReflectValue_3 < LEVEL)) ReturnValue |= 0x07; else ReturnValue &= ~(0x00);
  
  if((ReflectValue_1 > LEVEL) && (ReflectValue_2 > LEVEL) && (ReflectValue_3 > LEVEL)) ReturnValue |= 0x08; else ReturnValue &= ~(0x00);

  if(i == 20) ReturnValue |= 0x09; else ReturnValue &= ~(0x00);
  
  //digitalWrite(SensorToggle, LOW); // Optie batterijbesparing: Schakel de sensoren uit
  
  Serial.println(ReturnValue);        // debug
  return ReturnValue;
}

void ServoActie(char Waarde)
{
  if(Waarde == 0x08)     //kijkt hoelang case 8 al waar is
  {
    i++;
  }
  if(Waarde < 0x08)      //kijkt of case 8 waar is en zo niet zet i weer terug op nul
  {
    i = 0;
  }
  //Serial.println(i);   // print waarde i voor debug
  switch(Waarde)         // 9 mogelijke waarden met 3 sensoren
  {
    case 0x01:
      Serial.println("vooruit \n");    
      servo_L.writeMicroseconds(2000);
      servo_R.writeMicroseconds(1000);
    break;

    case 0x02:
      Serial.println("draai langzaam naar links");    
      servo_L.writeMicroseconds(1300);
      servo_R.writeMicroseconds(1300);
    break;

    case 0x03:
      Serial.println("draai langzaam naar rechts"); 
      servo_L.writeMicroseconds(1700);
      servo_R.writeMicroseconds(1700); 
    break;

    case 0x04:
      Serial.println("vooruit want links en midden wit");
      servo_L.writeMicroseconds(1550);
      servo_R.writeMicroseconds(1450);
    break;

    case 0x05:
      Serial.println("vooruit want rechts en midden wit");
      servo_L.writeMicroseconds(1550);
      servo_R.writeMicroseconds(1450);
    break;

    case 0x06:
      Serial.println("vooruit want achterste 2 sensoren wit");    
      servo_L.writeMicroseconds(1550);
      servo_R.writeMicroseconds(1450);
    break;

    case 0x07:
      Serial.println("vooruit want kruispunt");    
      servo_L.writeMicroseconds(1550);
      servo_R.writeMicroseconds(1450);
    break;

    case 0x08:
      Serial.println("vooruit want opzoek naar lijn");   
      servo_L.writeMicroseconds(1550);
      servo_R.writeMicroseconds(1450);
    break;

    case 0x09:
      Serial.println("achteruit wan topzoek naar lijn");
      servo_L.writeMicroseconds(1450);
      servo_R.writeMicroseconds(1550);
      delay(1500);
      i = 0;
    break;

    default:
    break;
  }
}

void SensorStatus2LEDS(char LEDs)
{ 
  digitalWrite(led_rood, LOW);    //zet alle LEDS uit zodra de functie word aan geroepen
  digitalWrite(led_groen, LOW);
  digitalWrite(led_geel, LOW);
  //Serial.println(LEDs);         //debug
  switch(LEDs)
  {
    case 0x01: digitalWrite(led_rood, HIGH);
    break;
    
    case 0x02: digitalWrite(led_groen, HIGH);
    break;
    
    case 0x03: digitalWrite(led_geel, HIGH);
    break;
    
    case 0x04: {digitalWrite(led_rood, HIGH); digitalWrite(led_groen, HIGH);}
    break;
    
    case 0x05: {digitalWrite(led_rood, HIGH); digitalWrite(led_geel, HIGH);}
    break;
    
    case 0x06: {digitalWrite(led_groen, HIGH); digitalWrite(led_geel, HIGH);}
    break;
    
    case 0x07: {digitalWrite(led_rood, HIGH); digitalWrite(led_groen, HIGH); digitalWrite(led_geel, HIGH);}
    break;
  
    case 0x08: {digitalWrite(led_rood, LOW); digitalWrite(led_groen, LOW); digitalWrite(led_geel, LOW);}
    break;
  
    default:
    break;
  }
}

void ObstakelDetectie_en_Verwijdering(void)
{
  int val = 0;                        //definiert de waarde voor het uitlezen van de afstandssensor
  val = analogRead(Afstandssensor);   //zorgt dat de waarde de afstandssensor uitgelezen kan worden
  //Serial.print("  afstandssensor"); //debug van de sensor
  //Serial.println(val);              //debug van de sensor
  if(val > 530)                       // kijkt of val hoger is dan de door ons aangegeven waarde
  {
    servo_L.writeMicroseconds(1500);  //zet beide servo stil
    servo_R.writeMicroseconds(1500); 
    
    delay(200);
    servo_Arm.write(180);             //beweegt de servo naar de 180 positie zodat het obstakel geelimineerd kan worden
    delay(1000);
    servo_Arm.write(0);               //beweegt de servo terug naar zijn start positie
    delay(1000);
  }
}
