volatile int cont = 0;
volatile int rpm = 0;
volatile int viento= 0;
bool state = LOW;
bool prevState = LOW;
unsigned long previousTime = 0;
unsigned long currentTime = 0;


#define PINENCODER 34

void setup() {
  Serial.begin(115200);
  // pinMode(PINENCODER, INPUT_PULLUP);
}


void loop() {
  countAnemometerPulses();
  currentTime = millis();
  if (currentTime - previousTime >= 1000) {
    previousTime = currentTime;
    speedAnemometer();
  }
}


void speedAnemometer() {
    rpm = (60*cont)/20;
    Serial.print("rpm: ");
    Serial.print(rpm);
    viento =  (rpm*6.2832/60)*(0.025);
    Serial.print("  Velociad viento: ");
    Serial.println(viento);
    cont = 0;  
}

void countAnemometerPulses() {
  state = digitalRead(PINENCODER);
  if (state == HIGH && prevState == LOW) {
    cont = cont+1;
  }
  prevState = state;
}
