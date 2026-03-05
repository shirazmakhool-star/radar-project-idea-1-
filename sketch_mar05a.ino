#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(128, 64, &Wire, -1);

Servo radarServo;

#define TRIG 7
#define ECHO 6
#define SERVO_PIN 9
#define BUZZER 3

int angle = 0;
int direction = 1;
int radarMemory[181];

unsigned long lastBeep = 0;

long readDistance() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 20000);
  long distance = duration * 0.034 / 2;

  return distance;
}

void drawRadar() {

  display.drawCircle(64, 63, 20, WHITE);
  display.drawCircle(64, 63, 40, WHITE);
  display.drawCircle(64, 63, 60, WHITE);
}

void setup() {

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  radarServo.attach(SERVO_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  for (int i = 0; i < 181; i++)
    radarMemory[i] = 0;
}

void loop() {

  radarServo.write(angle);

  long distance = readDistance();

  if (distance > 0 && distance <= 25)
    radarMemory[angle] = distance;
  else
    radarMemory[angle] = 0;

  display.clearDisplay();

  drawRadar();

  // ✅ Draw stored dots
  for (int a = 0; a <= 180; a++) {

    if (radarMemory[a] > 0) {

      float rad = a * 3.14159 / 180;
      int r = map(radarMemory[a], 0, 25, 0, 60);

      int x = 64 + cos(rad) * r;
      int y = 63 - sin(rad) * r;

      display.fillCircle(x, y, 2, WHITE);
    }
  }

  // ✅ Sweep line
  float rad = angle * 3.14159 / 180;

  int lx = 64 + cos(rad) * 60;
  int ly = 63 - sin(rad) * 60;

  display.drawLine(64, 63, lx, ly, WHITE);

  display.display();

  // ✅ Beep faster when closer
  if (distance > 0 && distance <= 25) {

    int beepDelay = map(distance, 1, 25, 40, 600);

    if (millis() - lastBeep > beepDelay) {
      tone(BUZZER, 1200, 30);
      lastBeep = millis();
    }
  }

  angle += direction;

  if (angle >= 180 || angle <= 0)
    direction = -direction;

  delay(20);
}
