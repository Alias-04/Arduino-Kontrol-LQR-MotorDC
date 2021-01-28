#define enA 2 // hubungkan encoderA ke pin 21(white)
#define enB 3 // hubungkan encoderB ke pin 20(green)
#include <HX711.h>
#define DOUT  5
#define CLK  4
HX711 scale;

int speed;
double setpoint, input, output, irfan, coba;
float data_s;
float R = 1;
float q = 0.1;
float q2 = 0.5;
float q3 = 1;
float q4 = 5;
float q5 = 10;
float L  = 1.1715 ;
float Err;
float de;
float previous_error;
float k = 0.12;
int kg, kgs;
int calibration_factor = 344; //-7050 worked for my 440lb max scale setup
int pulsa = 0;
int rps, rpm;
int jumlahPulsa, lastjumlahPulsa;
int interval = 500;
unsigned int waktulama, waktusekarang;

int limit2 = 26 ;
int limit3 = 24 ;
int limit1 = 28 ;

int tombol3 = 34;
int tombol2 = 36;
int tombol1 = 38;

int led1 = 11;
int led2 = 9 ;
int led3 = 10;

int pwm1= 8;
int pwmr = 9;
int ml1 = 7;
int ml2 =  6;
int t = 0;
int posisi = 0;
int tujuan = 0;

void setup() {
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); 
  Serial.println(zero_factor);
  Serial.print

  pinMode(enA, INPUT_PULLUP);
  pinMode(enB, INPUT_PULLUP);

  pinMode (limit1, INPUT) ;
  pinMode (limit2, INPUT) ;
  pinMode (limit3, INPUT) ;

  pinMode (tombol1, INPUT) ;
  pinMode (tombol2, INPUT) ;
  pinMode (tombol3, INPUT) ;

  pinMode (led1, OUTPUT) ;
  pinMode (led2, OUTPUT) ;
  pinMode (led3, OUTPUT) ;

  pinMode(pwm1, OUTPUT);
  pinMode(pwmr, OUTPUT);
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);
  attachInterrupt(0, encoder, CHANGE);
  
//LQR
int Output;
Output = (k*L*R);
if(setpoint==15){
    pwm1 =(int)(Output*q);
  }
  else if (setpoint== 25){
    pwm1 = (int)(Output*q);
  }
  if(pwm1>255) {
    pwm1 =255;
  }
  else if (pwm1 < 0) {
    pwm1 = 0;
  }
  //Start Awal
  while (1) {
    analogWrite(pwm1, abs(200));
    digitalWrite(ml1, HIGH);
    digitalWrite(ml2, LOW);

    if (digitalRead(limit1) == HIGH) {
      analogWrite(pwm1, 0);
      posisi = 1;
      break;
    }
  }
}

void rotary () {
  waktusekarang = millis();
  if (waktusekarang - waktulama >= interval) {
    rps = (jumlahPulsa / 360.00) / 0.7 * 100;
    //rpm = (jumlahPulsa/360.00)/0.00833333;
    rpm = rps / 1.6;
    pulsa = 0;
    jumlahPulsa = 0;
    waktulama = waktusekarang;
  }
}

void encoder()
{
  if (digitalRead(enA) == digitalRead(enB))
  {
    pulsa++;
  }
  else
  {
    pulsa--;
  }
  jumlahPulsa = pulsa / 2.5;
}

void loadcell() {
  kg = scale.get_units(), 10;
  kgs = kg / 1000; //rumus
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
} 
  
void naik() {
  analogWrite(pwm1, setpoint);
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
} 
  
void turun() {
  analogWrite(pwm1, setpoint);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
}
 
void berhenti() {
  analogWrite(pwm1, 0);
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
}

void loop() {
  rotary();
  if (Serial.available() > 0) 
  {
    irfan = Serial.parseInt();
  }
  setpoint = map(irfan, 0, 45, 150, 255);
  rotary();
  loadcell();
    
// LANTAI 1
  if (digitalRead(tombol1) == HIGH ){
    tujuan = 1;
    
    switch (posisi){
      case 1:
        berhenti();
        break;
      case 2:
        turun();
        break;
      case 3:
        turun();
        break;
    }
  }
  else if (digitalRead(tombol2) == HIGH ){
    tujuan = 2;
    switch (posisi){
      case 1:
        naik();
        break;
      case 2:
        berhenti();
        break;
      case 3:
        turun();
        break;
    }
  }
  else if (digitalRead(tombol3) == HIGH ){
    tujuan = 3;
    switch (posisi){
      case 1:
        naik();
        break;
      case 2:
        naik();
        break;
      case 3:
        berhenti();
        break;
    };
  }

  if (digitalRead(limit1) == HIGH ) {
    posisi = 1;
    digitalWrite(led1,HIGH);
    digitalWrite(led2,LOW) ;
    digitalWrite(led3,LOW) ;
    if (tujuan == 1)berhenti();
  }
  else if (digitalRead(limit2) == HIGH ) {
    posisi = 2;
    digitalWrite(led2,HIGH);
    digitalWrite(led1,LOW) ;
    digitalWrite(led3,LOW) ;
    if (tujuan == 2)berhenti();
       
  }
  else if (digitalRead(limit3) == HIGH ) {
    posisi = 3;
    digitalWrite(led3,HIGH) ;
    digitalWrite(led2,LOW) ;
    digitalWrite(led1,LOW) ;
    if (tujuan == 3)berhenti();
  }
  else{
    digitalWrite(led3,LOW) ;
    digitalWrite(led2,LOW) ;
    digitalWrite(led1,LOW) ;
  }
  Err = rpm - setpoint;
  de = Err - previous_error;
  previous_error = Err;
  Serial.print(irfan); 
  Serial.print(","); 
  Serial.print(rpm); 
  Serial.print(","); 
  Serial.println(kg);
} 
