void setup()
{
    Serial.begin(9600);
    pinMode(4, INPUT);
}
void loop()
{
  int etatBouton = digitalRead(4);  
  Serial.println(etatBouton);
}
