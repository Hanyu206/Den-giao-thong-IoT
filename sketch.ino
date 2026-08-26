#define BLYNK_TEMPLATE_ID "TMPL2492931"
#define BLYNK_TEMPLATE_NAME "DenGiaoThongIoT"
#define BLYNK_AUTH_TOKEN "hm01vZfjQhtbqJ2CymHYjE0efIz89mf-"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define LED_GREEN  16
#define LED_YELLOW 17
#define LED_RED    19

#define TRIG_PIN   5
#define ECHO_PIN   18

LiquidCrystal_I2C lcd(0x27, 16, 2);

int thoiGianXanh = 8;
bool cheDoUuTien = false;

long docKhoangCach() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 400;
  return duration * 0.034 / 2;
}
long docKhoangCachTrungBinh() {
  long tong = 0;
  int soLanHopLe = 0;
  for (int i = 0; i < 3; i++) {
    long d = docKhoangCach();
    if (d < 400) { tong += d; soLanHopLe++; }
    delay(30);
  }
  return soLanHopLe > 0 ? tong / soLanHopLe : 400;
}
BLYNK_WRITE(V2) {
  cheDoUuTien = param.asInt();
}
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V2);   // lấy lại giá trị V2 hiện tại từ server mỗi khi kết nối/reconnect
}
void setup() {
  Serial.begin(115200);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dang khoi dong..");

  Blynk.begin(auth, ssid, pass);
  lcd.clear();
}

void loop() {
  Blynk.run();

  if (cheDoUuTien) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);

    Blynk.virtualWrite(V3, "UU TIEN");
    Blynk.virtualWrite(V4, 99);

    lcd.setCursor(0, 0);
    lcd.print("CHEDO: UU TIEN! ");
    lcd.setCursor(0, 1);
    lcd.print("XE CAP CUU PASS ");
    delay(500);
    return;
  }

  long dist = docKhoangCach();
  
  const char* matDo;
if (dist < 10) {
  thoiGianXanh = 15;
  matDo = "CAO!    ";
} else if (dist < 25) {
  thoiGianXanh = 8;
  matDo = "TR.BINH ";
} else {
  thoiGianXanh = 5;
  matDo = "THAP    ";
}

  Blynk.virtualWrite(V0, dist);
  Blynk.virtualWrite(V1, thoiGianXanh);

  // --- PHA 1: ĐÈN XANH ---
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  Blynk.virtualWrite(V3, "XANH");

  for (int i = thoiGianXanh; i > 0; i--) {
    Blynk.run();
    if (cheDoUuTien) break;

    Blynk.virtualWrite(V4, i);

 char line0[17];
snprintf(line0, sizeof(line0), "XANH:%-2ds D:%-3ldcm", i, dist);
lcd.setCursor(0, 0);
lcd.print(line0);
    lcd.setCursor(0, 1);
    lcd.print("MAT DO: ");
    lcd.print(matDo);

    delay(1000);
  }

  if (cheDoUuTien) return;

  // --- PHA 2: ĐÈN VÀNG ---
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  Blynk.virtualWrite(V3, "VANG");

  for (int i = 2; i > 0; i--) {
    Blynk.run();
    if (cheDoUuTien) break;

    Blynk.virtualWrite(V4, i);

      char line0[17];
  snprintf(line0, sizeof(line0), "VANG:%-2ds D:%-3ldcm", i, dist);
  lcd.setCursor(0, 0);
  lcd.print(line0);
    lcd.setCursor(0, 1);
    lcd.print("CHUAN BI DUNG   ");
    delay(1000);
  }

  if (cheDoUuTien) return;

  // --- PHA 3: ĐÈN ĐỎ ---
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);
  Blynk.virtualWrite(V3, "DO");

  for (int i = 5; i > 0; i--) {
    Blynk.run();
    if (cheDoUuTien) break;

    Blynk.virtualWrite(V4, i);

    char line0[17];
    snprintf(line0, sizeof(line0), "DO:%-2ds D:%-3ldcm  ", i, dist);
    lcd.setCursor(0, 0);
    lcd.print(line0);

    lcd.setCursor(0, 1);
    lcd.print("DUNG XE LAI     ");
    delay(1000);
  }
}
