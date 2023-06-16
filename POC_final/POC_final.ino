#include <Wire.h>
#include "rgb_lcd.h"
#include <SoftwareSerial.h>

rgb_lcd lcd;

// Déclaration et mise en place de la partie radio

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
  // Gestion de la ligne série
  Serial.begin(9600);

  // Gestion du bouton
  pinMode(7, INPUT_PULLUP);
  
  // Gestion du Buzzer
  pinMode(3, OUTPUT);


  // Gestion de l'écran LCD
  lcd.begin(16, 2);
  lcd.print("Booting...");
  delay(1000);

  // Gestion de la carte radio
  ShowSerial.begin(115200);
  ShowSerial.println("RF95 server test.");
  if (!rf95.init()) {
      ShowSerial.println("init failed");
      while (1);
  }
  // puissance autorisée de 5 à 23 dBm:
  rf95.setTxPower(23, false);
  rf95.setFrequency(868.0);

  // Affichage de logs
  Serial.println("Boitier prêt!");
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

    // Si il y a un message dans le buffer on le lis
    if (rf95.recv(buf, &len)) {
      //On vérifie si le message commence par "alerte"
            if (strncmp((char*)buf, "alerte", 6) == 0) {
                lcd.clear();
                // lcd.print("Received: ");
                // lcd.setCursor(0, 1);
                lcd.print((char*)buf);

                //On fait sonner le Buzzer
                
                tone (3, 600);
                for (int x = 0; x < 500 ; x++){
                  tone (3, x);
                  delay(1);
                }
                for (int x = 500; x > 1 ; x--){
                  tone (3, x);
                  delay(1);
                }
                
              // tone (3, 600);
              // delay(100);
              noTone (3);

              ShowSerial.print("reponse recue: [validée] ");
              ShowSerial.println((char*)buf);
              lcd.setCursor(0, 1);
              lcd.print("Clic pour valid");

              //On attends que le boutton soit appuyé
              do {
                etatBouton = digitalRead(7);
                delay(5);
              } while (etatBouton == 1);
              lcd.clear();
              lcd.print("OK");
              ShowSerial.print("L'utilisateur à cliqué pour aquiter l'alerte : ");

              // Il faut reset l'état du bouton
              etatBouton = 1;

              // Envoi d'une réponse à la personne attaquée
              uint8_t data[] = "ack Timothe";
              rf95.send(data, sizeof(data));
              rf95.waitPacketSent();
              ShowSerial.print("ack envoyé: ");
              ShowSerial.println((char*)buf);
              delay(4000);

            }

            // On vérifie si le message commence par "ack"
            if (strncmp((char*)buf, "ack", 6) == 0) {
              ShowSerial.println("Message d'aquitement reçu");
            }

            else {
              // ShowSerial.print("reponse recue: [ignorée] ");
              // ShowSerial.println((char*)buf);
            }
        }
    delay(100);
  }

  if (etatBouton == 0 ) {
    // Ici, le bouton à été cliqué. On doit donc transmettre une alerte.
    lcd.begin(16, 2);
    lcd.print("Envoi en cours...");

    // Affichage de logs sur la console
    ShowSerial.println("Envoi en cours d'une alerte...");

    // Envoi de la notification Lora
    uint8_t data[] = "alerte Timothe!!!";
    rf95.send(data, sizeof(data));
    rf95.waitPacketSent();
    lcd.clear();
    lcd.print("Alerte envoyee");

    //Affichage de logs sur la console
    ShowSerial.println("Alerte envoyée");
    delay(1000);
}
}
