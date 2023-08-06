float previousTime = 0.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  String value = String(0);
  String dataPacket = String(value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value + "," + value);

  // Printing 20 values every 1 s
  float currentTime = millis();

  if (currentTime - previousTime >= 1000) {
    Serial.println(dataPacket);
    previousTime = currentTime;
  }
}
