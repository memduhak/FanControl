#include <PID_v1.h>

#define FAN_PIN 9              // Fan pinini belirledik
#define RPM_SENSOR_PIN 2       // RPM sensörünü bağladık
#define POT_PIN A0             // Potansiyometreyi bağladık

double setpoint = 0;          // Hedef hız (RPM)
double input = 0;             // Gerçek hız (RPM)
double output = 0;            // PID çıktısı (PWM)

double Kp = 2.0, Ki = 1.0, Kd = 0.1;  // PID ayarları
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

volatile int rpmCount = 0;   // RPM sensöründen gelen veriyi sayıyoruz
unsigned long lastTime = 0;  // Zaman kontrolü için

void setup() {
  Serial.begin(9600);   // Seri haberleşmeyi başlat

  pinMode(FAN_PIN, OUTPUT);        // Fanı kontrol etmek için pin çıkışı
  pinMode(RPM_SENSOR_PIN, INPUT); // RPM sensörü için giriş pini
  pinMode(POT_PIN, INPUT);        // Potansiyometre giriş pini

  attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), countRPM, RISING);  // RPM'yi saymak için kesme

  pid.SetMode(AUTOMATIC);  // PID'yi otomatik moda alıyoruz
  pid.SetOutputLimits(0, 255);  // Çıktıyı 0-255 arası PWM değerlerine sınırlıyoruz
}

void loop() {
  setpoint = map(analogRead(POT_PIN), 0, 1023, 0, 1000);  // Potansiyometre ile hedef hızı ayarla

  unsigned long currentTime = millis();
  if (currentTime - lastTime >= 1000) {  // 1 saniyede bir RPM hesapla
    input = rpmCount * 60;  // RPM'yi dakikaya çevir
    rpmCount = 0;           // Sayaç sıfırla
    lastTime = currentTime;
  }

  pid.Compute();  // PID hesaplamalarını yap

  analogWrite(FAN_PIN, output);  // Fanı hızla çalıştır

  Serial.print("Setpoint: ");
  Serial.print(setpoint);
  Serial.print(" RPM, Input: ");
  Serial.print(input);
  Serial.print(" RPM, Output: ");
  Serial.println(output);

  delay(100);  // 100ms bekle
}

// RPM sensöründeki pulse'ları say
void countRPM() {
  rpmCount++;
}

