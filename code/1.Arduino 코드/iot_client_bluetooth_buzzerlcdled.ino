/*
  blue test: 
  http://www.kccistc.net/
  작성일 : 2024.03.22
  작성자 : IoT 임베디드 PSJ
*/
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MsTimer2.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DEBUG
#define BUTTON_PIN 2
#define DHTPIN 4
#define MOTOR_PIN 6
#define LED_BUILTIN_PIN 13
#define BUZZER_PIN 7
#define LED1_PIN 8
#define LED2_PIN 9

#define ARR_CNT 5
#define CMD_SIZE 128
char lcdLine1[17] = "Smart IoT By PSJ";
char lcdLine2[17] = "";
char lcdLastLine1[17] = "";
char lcdLastLine2[17] = "";
char sendBuf[CMD_SIZE];
char recvId[10] = "PSJ_SQL";  
char deviceState[10] = "";
char deviceDatetime[32] = "";
bool lastButton = LOW;      
bool currentButton = HIGH;  
bool timerIsrFlag = false;
unsigned int secCount;
int lastGetdbSecSent = -1;

bool ledState = false;

SoftwareSerial BTSerial(10, 11);  // RX ==>BT:TXD, TX ==> BT:RXD

bool buzzerEnabled = false;
bool buzzerIsOn = false;
unsigned long buzzerPrevMillis = 0;
const unsigned long buzzerOnTime = 700;
const unsigned long buzzerOffTime = 700;
bool buzzerState = false;

int varValue;
int varValueold;
void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println("setup() start!");
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);  
#endif
  lcd.init();
  lcd.backlight();
  lcdDisplay(0, 0, lcdLine1);
  lcdDisplay(0, 1, lcdLine2);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_BUILTIN_PIN, OUTPUT);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  BTSerial.begin(9600);           
  MsTimer2::set(1000, timerIsr);  
  MsTimer2::start();
}

void loop() {
  if (BTSerial.available())
    bluetoothEvent();

  if (timerIsrFlag) {
    timerIsrFlag = false;

    if ((secCount % 3) == 0 && lastGetdbSecSent != (int)secCount) {
      sprintf(sendBuf, "[PSJ_SQL]GETDB\n"); 
      BTSerial.write(sendBuf);
      lastGetdbSecSent = (int)secCount;
    }
  }

#ifdef DEBUG
  if (Serial.available())
    BTSerial.write(Serial.read());
#endif

  // 부저  
 if (buzzerEnabled) {
  unsigned long currentMillis = millis();
  if (buzzerIsOn && currentMillis - buzzerPrevMillis >= buzzerOnTime) {
    noTone(BUZZER_PIN);  
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    buzzerIsOn = false;
    buzzerPrevMillis = currentMillis;
  } else if (!buzzerIsOn && currentMillis - buzzerPrevMillis >= buzzerOffTime) {
    tone(BUZZER_PIN, 2000); 
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    buzzerIsOn = true;
    buzzerPrevMillis = currentMillis;
  }
}
}

void bluetoothEvent() {
  int i = 0;
  char* pToken;
  char* pArray[ARR_CNT] = { 0 };
  char recvBuf[CMD_SIZE] = { 0 };
  int len = BTSerial.readBytesUntil('\n', recvBuf, sizeof(recvBuf) - 1);

#ifdef DEBUG
  Serial.print("Recv : ");
  Serial.println(recvBuf);
#endif

  pToken = strtok(recvBuf, "[@]");
  while (pToken != NULL) {
    pArray[i] = pToken;
    if (++i >= ARR_CNT)
      break;
    pToken = strtok(NULL, "[@]");
  }
  if (i == 4 && !strcmp(pArray[0], "PSJ_SQL") && !strcmp(pArray[1], "GETDB")) {
    // pArray[2] = YYYY-MM-DD_HH:MM:SS (19자, 16칸 LCD)
    // pArray[3] = OPEN / CLOSE

  
 char lineTime[17] = {0};
const char* dt = pArray[2];
if (strlen(dt) >= 19 && dt[4] == '-' && dt[7] == '-') {

  int idx = 0;

  // MM
  if (dt[5] == '0') {
    lineTime[idx++] = dt[6];  
  } else {
    lineTime[idx++] = dt[5];  
    lineTime[idx++] = dt[6];  
  }

  lineTime[idx++] = '/';
  lineTime[idx++] = dt[8];     
  lineTime[idx++] = dt[9];     
  lineTime[idx++] = ' ';
  lineTime[idx++] = ' ';

  // HH:MM:SS
  const char* t = strchr(dt, '_');
  if (t && strlen(t + 1) >= 8) {
    lineTime[idx++] = t[1];  
    lineTime[idx++] = t[2];  
    lineTime[idx++] = ':';   
    lineTime[idx++] = t[4];  
    lineTime[idx++] = t[5];  
    lineTime[idx++] = ':';   
    lineTime[idx++] = t[7];  
    lineTime[idx++] = t[8];  
  }

  lineTime[idx] = '\0';  
} else {
  strncpy(lineTime, dt, 16);
  lineTime[16] = '\0';
}


    char lineState[17] = {0};
    snprintf(lineState, sizeof(lineState), "%-16s", pArray[3]); 

    
    strncpy(deviceDatetime, pArray[2], sizeof(deviceDatetime)-1);
    strncpy(deviceState, pArray[3], sizeof(deviceState)-1);
    deviceDatetime[sizeof(deviceDatetime)-1] = '\0';
    deviceState[sizeof(deviceState)-1] = '\0';

  
if (strcmp(lineTime, lcdLastLine1) != 0) {
  lcdDisplay(0, 0, lineTime);
  strncpy(lcdLastLine1, lineTime, sizeof(lcdLastLine1) - 1);
}

if (strcmp(lineState, lcdLastLine2) != 0) {
  lcdDisplay(0, 1, lineState);
  strncpy(lcdLastLine2, lineState, sizeof(lcdLastLine2) - 1);
}

    if (!strcmp(pArray[3], "OPEN")) {
      buzzerEnabled = true;               
    } else {
      buzzerEnabled = false;              
      digitalWrite(BUZZER_PIN, LOW);      
      buzzerState = false;
    }

#ifdef DEBUG
    Serial.print("LCD L1: "); Serial.println(lineTime);
    Serial.print("LCD L2: "); Serial.println(lineState);
#endif
    return; 
  }
  
  if (!strcmp(pArray[0], "PSJ_ARD") && !strcmp(pArray[1], "GETDB")) {
    if (i == 2) {
      // 요청: [PSJ_ARD]GETDB → 서버로 전달
      sprintf(sendBuf, "[PSJ_SQL]GETDB\n");
      BTSerial.write(sendBuf);
#ifdef DEBUG
      Serial.print("Send : ");
      Serial.print(sendBuf);
#endif
    } else if (i == 4 && !strcmp(pArray[2], "STATE")) {
      // 응답: [PSJ_SQL]GETDB@DATETIME@STATE
      strncpy(deviceDatetime, pArray[2], sizeof(deviceDatetime) - 1);
      strncpy(deviceState, pArray[3], sizeof(deviceState) - 1);
      deviceDatetime[sizeof(deviceDatetime) - 1] = '\0';
      deviceState[sizeof(deviceState) - 1] = '\0';

      
      lcdDisplay(0, 0, deviceDatetime);
      lcdDisplay(0, 1, deviceState);

#ifdef DEBUG
      Serial.print("Device Datetime = ");
      Serial.println(deviceDatetime);
      Serial.print("Device State = ");
      Serial.println(deviceState);
#endif
    }
    return;
  }

  if (!strncmp(pArray[1], " New", 4)) return;
  if (!strncmp(pArray[1], " Alr", 4)) return;
}

void timerIsr() {
  timerIsrFlag = true;
  secCount++;
}
void lcdDisplay(int x, int y, char* str) {
  int len = 16 - strlen(str);
  lcd.setCursor(x, y);
  lcd.print(str);
  for (int i = len; i > 0; i--)
    lcd.write(' ');
}
boolean debounce(boolean last) {
  boolean current = digitalRead(BUTTON_PIN);  
  if (last != current)                        
  {
    delay(5);                           
    current = digitalRead(BUTTON_PIN);  
  }
  return current;  
}
