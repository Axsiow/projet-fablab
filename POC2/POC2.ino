#include <Wire.h>
#include "rgb_lcd.h"
#include <SoftwareSerial.h>

rgb_lcd lcd;

//Déclaration et mise en place de la partie radio

#include <RH_RF95.h>
#ifdef __AVR__
    #include <SoftwareSerial.h>
    SoftwareSerial SSerial(5, 6); // RX, TX
    #define COMSerial SSerial
    #define ShowSerial Serial

    RH_RF95<SoftwareSerial> rf95(COMSerial);
#endif

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define COMSerial Serial1
    #define ShowSerial SerialUSB

    RH_RF95<Uart> rf95(COMSerial);
#endif

#ifdef ARDUINO_ARCH_STM32F4
    #define COMSerial Serial
    #define ShowSerial SerialUSB

    RH_RF95<HardwareSerial> rf95(COMSerial);
#endif


void setup()
{
  //Gestion de la ligne série
    Serial.begin(9600);

  //Gestion du bouton
    pinMode(7, INPUT_PULLUP);

  //Gestion de l'écran LCD
  lcd.begin(16, 2);
  lcd.print("Booting...");
  delay(1000);

  //Gestion de la carte radio
  ShowSerial.begin(115200);
  ShowSerial.println("RF95 server test.");
  if (!rf95.init()) {
      ShowSerial.println("init failed");
      while (1);
  }
  // puissance autorisée de 5 à 23 dBm:
  rf95.setTxPower(13, false);
  rf95.setFrequency(868.0);

}

void loop()
{
  int etatBouton = digitalRead(7);  
  // Serial.println(etatBouton);

  if (etatBouton == 1 ) {
    // Affichage standard si le bouton n'est pas cliqué
    lcd.begin(16, 2);
    lcd.print("En attente...");
    // On regarde si on reçois un message
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
            if (strncmp((char*)buf, "alerte", 6) == 0) {
                lcd.clear();
                lcd.print("Received: ");
                lcd.setCursor(0, 1);
                lcd.print((char*)buf);
              ShowSerial.print("reponse recue: [validée] ");
              ShowSerial.println((char*)buf);
              delay(10000);
            }
            else {
              ShowSerial.print("reponse recue: [ignorée] ");
              ShowSerial.println((char*)buf);
            }
        } 
    delay(100);
  }

  if (etatBouton == 0 ) {
    // Ici, le bouton à été cliqué. On doit donc transmettre une alerte.
    lcd.begin(16, 2);
    lcd.print("Envoi en cours...");
    //Envoi d'une notification Lora et attente de la réponse
        ShowSerial.println("Envoi en cours d'une alerte...");
    // Envoi de la notification
    uint8_t data[] = "alerte Enzo !!!";
    rf95.send(data, sizeof(data));
    

    rf95.waitPacketSent();
    lcd.print("Alerte envoyee");
    delay(9000);

/** On commente la partie "vérification de réponse" pour l'instant
    // Attente de la réponse
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    lcd.print("Alerte envoyee");
    delay(9000);

    if (rf95.waitAvailableTimeout(3000)) {
        // Should be a reply message for us now
        if (rf95.recv(buf, &len)) {
            ShowSerial.print("reponse recue: ");
            ShowSerial.println((char*)buf);
            lcd.begin(16, 2);
            lcd.print("Alerte envoyee");
            delay(10000);
        } else {
            ShowSerial.println("recv failed");
        }
    } else {
        ShowSerial.println("Aucune reponse...");
    }
**/
delay(1000);
}
}