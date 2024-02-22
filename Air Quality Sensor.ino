#include <DHT.h>  //menyertakan library DHT kedalam program
#include <LiquidCrystal_I2C.h>
#include <MQ135.h>
#include <MQUnifiedsensor.h>
#include <Buzzer.h>
#include <SD.h>
#include <SPI.h>


//Definitions
#define placa "Arduino UNO"
#define Voltage_Resolution 5
#define pin A0          //Analog input 0 of your arduino
#define type "MQ-135"   //MQ135
#define pin2 A1         //Analog input 0 of your arduino
#define type2 "MQ-136"  //MQ135

#define DHTPIN 2       //kita pasang sensor pada pin 2 digital
#define DHTTYPE DHT11  //kita menggunakan jenis sensor DHT11

#define ADC_Bit_Resolution 10   // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6  //RS / R0 = 3.6 ppm
#define RatioMQ136CleanAir 3.6  //RS / R0 = 3.6 ppm

int buz = 7;

const int Hijau = 3;
const int Biru = 4;
const int Kuning = 5;
const int Merah = 6;
const int Putih = 7;
const int Hijau_2 = 8;
const int Biru_2 = 9;
const int Kuning_2 = 10;
const int Merah_2 = 11;
const int Putih_2 = 12;

//Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);
MQUnifiedsensor MQ136(placa, Voltage_Resolution, ADC_Bit_Resolution, pin2, type2);
DHT dht(DHTPIN, DHTTYPE);  //deklarasi pin sensor dengan jenis sensor yang dipilih

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {

  lcd.init();       // Inisiasi LCD
  lcd.backlight();  // Menyalakan Backlight LCD
  dht.begin();      //program komunikasi atau setup untuk sensor DHT
  Serial.begin(9600);

  MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ135.setA(605.18);
  MQ135.setB(-3.937);

  MQ136.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ136.setA(40.44);
  MQ136.setB(-1.086);

  MQ135.init();
  MQ136.init();

  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();  // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
  }

  float calcR1 = 0;
  for (int u = 1; u <= 10; u++) {
    MQ136.update();  // Update data, the arduino will read the voltage from the analog pin
    calcR1 += MQ136.calibrate(RatioMQ136CleanAir);
  }
  MQ135.setR0(calcR0 / 10);
  if (isinf(calcR0)) {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR0 == 0) {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }

  MQ136.setR0(calcR1 / 10);
  if (isinf(calcR1)) {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR1 == 0) {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }

  pinMode(PIN4, OUTPUT);

  pinMode(Hijau, OUTPUT);
  pinMode(Biru, OUTPUT);
  pinMode(Kuning, OUTPUT);
  pinMode(Merah, OUTPUT);
  pinMode(Putih, OUTPUT);
  pinMode(Hijau_2, OUTPUT);
  pinMode(Biru_2, OUTPUT);
  pinMode(Kuning_2, OUTPUT);
  pinMode(Merah_2, OUTPUT);
  pinMode(Putih_2, OUTPUT);

  pinMode(buz, OUTPUT);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);


  lcd.setCursor(0, 0);
  lcd.print("Alief Arifin M");
  lcd.setCursor(0, 1);
  lcd.print("STMIK Sinus");
  lcd.setCursor(0, 2);
  lcd.print("Sistem Informasi");
  lcd.setCursor(0, 3);
  lcd.print("2 September 2023");
  delay(300);
}

void loop() {
  MQ135.update();
  MQ135.readSensor();
  MQ136.update();
  MQ136.readSensor();
  float kelembapan = dht.readHumidity();  //menyimpan nilai kelembapan pada variabel kelembapan
  float suhu = dht.readTemperature();     //menyimpan nilai suhu pada variabel suhu
  lcd.clear();

  //menampilkan nilai suhu pada LCD
  lcd.setCursor(0, 0);
  lcd.print("Suhu : ");
  lcd.print((int)suhu);
  lcd.print((char)223);  //Simbol Derajat di LCD
  lcd.print("C ");

  //menampilkan nilai kelembapan pada LCD
  lcd.setCursor(0, 1);  //
  lcd.print("Kelembapan : ");
  lcd.print((int)kelembapan);
  lcd.print("%RH");

  float CO = MQ135.readSensor();  // Sensor will read PPM co-35ncentration using the model, a and b values set previously or from the setup
  lcd.setCursor(0, 2);
  lcd.print("CO : ");
  lcd.print(CO);
  lcd.print(" PPM");

  float SO2 = MQ136.readSensor();  // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  lcd.setCursor(0, 3);
  lcd.print("SO2 : ");
  lcd.print(SO2);
  lcd.print(" PPM");

 Serial.print("Suhu : ");
 Serial.print((int)suhu);
 Serial.print("°");  //Simbol Derajat
 Serial.print("C ");

 Serial.print(" | Kelembapan : ");
 Serial.print((int)kelembapan);
 Serial.println("%RH");

 Serial.print("CO : ");
 Serial.print(CO);
 Serial.print(" PPM");

 Serial.print(" | SO2 : ");
 Serial.print(SO2);
 Serial.println(" PPM");
 delay(1000);
 
    /*
    0 - 50 => SEHAT
    51 - 100 => SEDANG
    101 - 200 => TIDAK SEHAT
    201 - 299 => SANGAT TIDAK SEHAT
    300+ => BERBAHAYA
  */
    /*============= CO ==============*/

    if (CO >= 0){
      digitalWrite(Hijau, LOW);
      digitalWrite(Biru, HIGH);
      digitalWrite(Kuning, HIGH);
      digitalWrite(Merah, HIGH);
      digitalWrite(Putih, HIGH);
    }
    if (CO >= 50){
      digitalWrite(Hijau, HIGH);
      digitalWrite(Biru, LOW);
      digitalWrite(Kuning, HIGH);
      digitalWrite(Merah, HIGH);
      digitalWrite(Putih, HIGH);
    }
    if (CO >= 100){
      digitalWrite(Hijau, HIGH);
      digitalWrite(Biru, HIGH);
      digitalWrite(Kuning, LOW);
      digitalWrite(Merah, HIGH);
      digitalWrite(Putih, HIGH);
    }
    if(CO >= 200){
      digitalWrite(Hijau, HIGH);
      digitalWrite(Biru, HIGH);
      digitalWrite(Kuning, HIGH);
      digitalWrite(Merah, LOW);
      digitalWrite(Putih, HIGH);
    }
    if(CO >= 300){
      digitalWrite(Hijau, HIGH);
      digitalWrite(Biru, HIGH);
      digitalWrite(Kuning, HIGH);
      digitalWrite(Merah, HIGH);
      digitalWrite(Putih, LOW);
    }

    /*============ SO2 ==============*/
    if (SO2 >= 0){
      digitalWrite(Hijau_2, LOW);
      digitalWrite(Biru_2, HIGH);
      digitalWrite(Kuning_2, HIGH);
      digitalWrite(Merah_2, HIGH);
      digitalWrite(Putih_2, HIGH);
    }
    
    if (SO2 >= 50){
      digitalWrite(Hijau_2, HIGH);
      digitalWrite(Biru_2, LOW);
      digitalWrite(Kuning_2, HIGH);
      digitalWrite(Merah_2, HIGH);
      digitalWrite(Putih_2, HIGH);
    }
    
    if (SO2 >= 100){
      digitalWrite(Hijau_2, HIGH);
      digitalWrite(Biru_2, HIGH);
      digitalWrite(Kuning_2, LOW);
      digitalWrite(Merah_2, HIGH);
      digitalWrite(Putih_2, HIGH);
    }
    
    if (SO2 >= 200){
      digitalWrite(Hijau_2, HIGH);
      digitalWrite(Biru_2, HIGH);
      digitalWrite(Kuning_2, HIGH);
      digitalWrite(Merah_2, LOW);
      digitalWrite(Putih_2, HIGH);
    }
    
    if (SO2 >= 300){
      digitalWrite(Hijau_2, HIGH);
      digitalWrite(Biru_2, HIGH);
      digitalWrite(Kuning_2, HIGH);
      digitalWrite(Merah_2, LOW);
      digitalWrite(Putih_2, HIGH);
    }

}
