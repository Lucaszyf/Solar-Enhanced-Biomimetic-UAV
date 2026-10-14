void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

float V_data = 0.0,I_data = 0.0;
void loop() {
  // put your main code here, to run repeatedly:
  // V_data = analogRead(A0) / 1023.0 * 5;
  // I_data = (V_data - 2.5) * 10 + 25;
  // Serial.println(I_data);
  Serial.println(analogRead(A5));
  delay(20);
}
