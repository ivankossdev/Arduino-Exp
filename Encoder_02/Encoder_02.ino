#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

volatile int encoderPos = 0;
int lastEncoderPos = 0;

// Для антидребезга
unsigned long lastInterruptTime = 0;
const unsigned long interruptInterval = 1000; // микросекунды

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  
  // Настройка прерываний
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), handleInterrupt, CHANGE);
}

void loop() {
  if (encoderPos != lastEncoderPos) {
    Serial.print("Позиция: ");
    Serial.println(encoderPos);
    lastEncoderPos = encoderPos;
  }
}

void handleInterrupt() {
  unsigned long interruptTime = micros();
  
  // Если прерывания приходят слишком часто - игнорируем
  if (interruptTime - lastInterruptTime > interruptInterval) {
    updateEncoder();
    lastInterruptTime = interruptTime;
  }
}

void updateEncoder() {
  static int lastA = LOW;
  static int lastB = LOW;
  
  int a = digitalRead(ENCODER_PIN_A);
  int b = digitalRead(ENCODER_PIN_B);
  
  // Проверяем, изменилось ли состояние
  if (a != lastA) {
    // Определяем направление
    if (b != a) {
      encoderPos++;
    } else {
      encoderPos--;
    }
    lastA = a;
    lastB = b;
  }
}