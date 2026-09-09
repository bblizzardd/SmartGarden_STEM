# 1 "C:\\Users\\Admin\\AppData\\Local\\Temp\\tmpvhrfj83x"
#include <Arduino.h>
# 1 "C:/Users/Admin/OneDrive/Documents/PlatformIO/Projects/260909-095733-4d_systems_esp32s3_gen4_r8n16/src/Code_sieu_chuan_4.ino"




#define BLYNK_TEMPLATE_ID "TMPL6k6R-M7O-"
#define BLYNK_TEMPLATE_NAME "STEM No 1"
#define BLYNK_AUTH_TOKEN "bd_kTrSSlwN_asxr6F30kqH4pvrsR8Jj"

#include <Arduino.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Wire.h>


#define DHTPIN 14
#define DHTTYPE DHT22
#define SOIL_PIN 34
const int pumpPin = 25;
float t, h;


DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;


int temp = 0, humi = 0;
int soilValue = 0;
int threshold = 2500;
bool autoMode = true;
int pumpState = 0;
void controlPump(int state);
void update_sensor();
void updateSystem();
void setup();
void loop();
#line 36 "C:/Users/Admin/OneDrive/Documents/PlatformIO/Projects/260909-095733-4d_systems_esp32s3_gen4_r8n16/src/Code_sieu_chuan_4.ino"
void controlPump(int state) {
  if (state == 1) {
    digitalWrite(pumpPin, LOW);
    pumpState = 1;
  } else {
    digitalWrite(pumpPin, HIGH);
    pumpState = 0;
  }
}


BLYNK_WRITE(V6) {
  autoMode = param.asInt();
  Serial.print("Che do: ");
  Serial.println(autoMode ? "TU DONG" : "THU CONG");
}


BLYNK_WRITE(V1) {
  if (!autoMode) {
    pumpState = param.asInt();
    controlPump(pumpState);
  } else {

    Blynk.virtualWrite(V1, pumpState);
    Serial.println("Dang o che do Auto, hay chuyen sang Manual de bam!");
  }
}


BLYNK_WRITE(V5) {
  threshold = param.asInt();
  Serial.print("Nguong moi: ");
  Serial.println(threshold);
}
void update_sensor() {
  h = dht.readHumidity();
  t = dht.readTemperature();
  soilValue = analogRead(SOIL_PIN);


  if (!isnan(h) && !isnan(t)) {
    temp = (int)t;
    humi = (int)h;
  }
}

void updateSystem() {
  update_sensor();

  if (autoMode) {
    if (soilValue > threshold) {
      controlPump(1);
    } else {
      controlPump(0);
    }

    Blynk.virtualWrite(V1, pumpState);
  }



  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print("C ");
  lcd.setCursor(6, 0);
  lcd.print("H:");
  lcd.print(humi);
  lcd.print("% ");
  lcd.setCursor(13, 0);
  lcd.print(autoMode ? "[A]" : "[M]");

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(soilValue);
  lcd.print("   ");
  lcd.setCursor(11, 1);
  lcd.print(pumpState == 1 ? "P:ON " : "P:OFF");


  Blynk.virtualWrite(V2, t);
  Blynk.virtualWrite(V3, h);
  Blynk.virtualWrite(V4, soilValue);
}

void setup() {
  Serial.begin(9600);


  Wire.begin(32, 33, 100000);
  Wire.setTimeOut(50);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SYSTEM STARTING");
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi...");


  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH);
  dht.begin();



  Blynk.begin(BLYNK_AUTH_TOKEN, "Binh", "12345678");


  Blynk.virtualWrite(V6, 1);


  timer.setInterval(2000L, updateSystem);

  lcd.clear();
  Serial.println("System Ready!");
}

void loop() {
  Blynk.run();
  timer.run();
}