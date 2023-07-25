#if 1
#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_SPI pn532spi(SPI, 5);
NfcAdapter nfc = NfcAdapter(pn532spi);
#else

#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
#endif

void setup(void) {
  Serial.begin(9600);
  Serial.println("NDEF Reader");
  pinMode(34,OUTPUT); //Set pin 34 as an “output” pin

  digitalWrite(34,HIGH); //Put high voltage (1) en pin 34

  delay(10); //Wait 10 ms
 
  digitalWrite(34, LOW); //Put low voltage (0) in pin 34
  nfc.begin();
}

void loop(void) {
  Serial.println("\nScan a NFC tag\n");

  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    Serial.println(tag.getTagType());
    Serial.print("UID: ");Serial.println(tag.getUidString());

    if (tag.hasNdefMessage()) // every tag won't have a message
    {

      NdefMessage message = tag.getNdefMessage();
      Serial.print("\nThis NFC Tag contains an NDEF Message with ");
      Serial.print(message.getRecordCount());
      Serial.print(" NDEF Record");
      if (message.getRecordCount() != 1) {
        Serial.print("s");
      }
      Serial.println(".");

      // cycle through the records, printing some info from each
      int recordCount = message.getRecordCount();
      for (int i = 0; i < recordCount; i++)
      {
        Serial.print("\nNDEF Record ");Serial.println(i+1);
        NdefRecord record = message.getRecord(i);
        // NdefRecord record = message[i]; // alternate syntax

        Serial.print("  TNF: ");

        switch(record.getTnf()){
          case '0':
            Serial.println("Registro vacio");
            break;
          case '1':
            Serial.println("Registro NFC Forum bien conocido");
            break;
          case '2':
            Serial.println("MIME");
            break;
          case '3':
            Serial.println("URI");
            break;
          case '4':
            Serial.println("Registro NFC Forum de tipo externo");
            break;
          case '5':
            Serial.println("Desconocido");
            break;
          case '6':
            Serial.println("Sin cambios");
            break;
          default:
            Serial.println("no funciona");
        }
        
        Serial.print("  Type: ");Serial.println(record.getType()); // will be "" for TNF_EMPTY

        // switch(record.getType()){
        //   case 54: 
        //     Serial.println("Texto");
        //     break;
        //   case 55:
        //     Serial.println("URL");
        //     break;
        // }
        // The TNF and Type should be used to determine how your application processes the payload
        // There's no generic processing for the payload, it's returned as a byte[]
        int payloadLength = record.getPayloadLength();
        byte payload[payloadLength];
        record.getPayload(payload);

        // Print the Hex and Printable Characters
        Serial.print("  Payload (HEX): ");
        PrintHexChar(payload, payloadLength);

        // Force the data into a String (might work depending on the content)
        // Real code should use smarter processing
        String payloadAsString = "";
        for (int c = 0; c < payloadLength; c++) {
          payloadAsString += (char)payload[c];
        }
        Serial.print("  Payload (as String): ");
        Serial.println(payloadAsString);

        // id is probably blank and will return ""
        String uid = record.getId();
        if (uid != "") {
          Serial.print("  ID: ");Serial.println(uid);
        }
      }
    }
  }
  delay(2000);
}
