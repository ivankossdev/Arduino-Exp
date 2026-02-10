#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 3

volatile int encoderPos = 0;
int lastEncoderPos = 0;

// Состояния энкодера
enum EncoderState {
  STATE_00,  // A=0, B=0
  STATE_01,  // A=0, B=1
  STATE_10,  // A=1, B=0
  STATE_11   // A=1, B=1
};

volatile EncoderState lastState = STATE_00;
volatile unsigned long lastChangeTime = 0;
const unsigned long DEBOUNCE_TIME = 2000; // 2 мс в микросекундах

void setup() {
  Serial.begin(9600);
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  
  // Читаем начальное состояние
  lastState = readEncoderState();
  
  // Настраиваем прерывания на оба пина
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), encoderISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), encoderISR, CHANGE);
}

void loop() {
  if (encoderPos != lastEncoderPos) {
    Serial.print("Позиция: ");
    Serial.println(encoderPos);
    lastEncoderPos = encoderPos;
  }
}

EncoderState readEncoderState() {
  int a = digitalRead(ENCODER_PIN_A);
  int b = digitalRead(ENCODER_PIN_B);
  
  if (a == LOW && b == LOW) return STATE_00;
  if (a == LOW && b == HIGH) return STATE_01;
  if (a == HIGH && b == LOW) return STATE_10;
  return STATE_11;
}

void encoderISR() {
  unsigned long now = micros();
  
  // Антидребезг по времени
  if (now - lastChangeTime < DEBOUNCE_TIME) {
    return;
  }
  
  EncoderState currentState = readEncoderState();
  
  // Определяем изменение состояния
  switch (lastState) {
    case STATE_00:
      if (currentState == STATE_01) encoderPos--;
      else if (currentState == STATE_10) encoderPos++;
      break;
    case STATE_01:
      if (currentState == STATE_11) encoderPos--;
      else if (currentState == STATE_00) encoderPos++;
      break;
    case STATE_10:
      if (currentState == STATE_00) encoderPos--;
      else if (currentState == STATE_11) encoderPos++;
      break;
    case STATE_11:
      if (currentState == STATE_10) encoderPos--;
      else if (currentState == STATE_01) encoderPos++;
      break;
  }
  
  lastState = currentState;
  lastChangeTime = now;
}