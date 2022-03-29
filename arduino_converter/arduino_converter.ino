void setup() {
  // put your setup code here, to run once:
  analogReference(EXTERNAL);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(5, digitalRead(A3));
  digitalWrite(6, digitalRead(A2));
  digitalWrite(7, digitalRead(A1));
}
