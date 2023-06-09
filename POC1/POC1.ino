#include <RH_RF95.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int colorR = 255;
const int colorG = 0;
const int colorB = 0;

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

void setup() {
    ShowSerial.begin(115200);
    ShowSerial.println("RF95 client test.");

    if (!rf95.init()) {
        ShowSerial.println("init failed");
        while (1);
    }

    // you can set transmitter powers from 5 to 23 dBm:
    rf95.setTxPower(13, false);
    rf95.setFrequency(868.0);
    
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    
    // Print a message to the LCD.
    lcd.print("hello, world!");
    delay(1000);


}

void loop() {
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(millis()/1000);

    // Sending and receiving code
    ShowSerial.println("Sending to rf95_server");
    // Send a message to rf95_server
    uint8_t data[] = "Hello la terre ici la lune";
    rf95.send(data, sizeof(data));

    rf95.waitPacketSent();

    // Now wait for a reply
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.waitAvailableTimeout(3000)) {
        // Should be a reply message for us now
        if (rf95.recv(buf, &len)) {
            ShowSerial.print("got reply: ");
            ShowSerial.println((char*)buf);

            // Afficher le contenu du buffer sur l'écran LCD
            lcd.clear();
            lcd.print("Received: ");
            lcd.setCursor(0, 1);
            lcd.print((char*)buf);
        } else {
            ShowSerial.println("recv failed");
        }
    } else {
        ShowSerial.println("No reply, on a perdu la terre ?");
    }

    delay(1000);
}
