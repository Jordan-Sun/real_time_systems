int to5 = 1, to6 = 1, to7;
void setup() {
  // put your setup code here, to run once:
  analogReference(DEFAULT);
  Serial.begin(9600);
  pinMode(A1, INPUT);
  pinMode(A3, INPUT);
  pinMode(A5, INPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
//  to5 = digitalRead(A5);
//  to6 = digitalRead(A3);
  to7 = digitalRead(A1);
  Serial.print("ena:");
  Serial.print(to5, DEC);
  Serial.print(" dir:");
  Serial.print(to6, DEC);
  Serial.print(" pul:");
  Serial.println(to7, DEC);
  digitalWrite(5, to5);
  digitalWrite(6, to6);
  digitalWrite(7, to7);
}
