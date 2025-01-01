/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial using ESP8266.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * This example demonstrates how to read data from a PICC (RFID Tag or Card) using an MFRC522-based RFID Reader
 * on the ESP8266 SPI interface.
 * 
 * Typical pin layout used for ESP8266:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      ESP8266
 *             Reader/PCD   NodeMCU       ESP-12/ESP-07
 * Signal      Pin          Pin           Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          D3 (GPIO0)    GPIO5
 * SPI SS      SDA(SS)      D4 (GPIO2)    GPIO4
 * SPI MOSI    MOSI         D7 (GPIO13)   GPIO13
 * SPI MISO    MISO         D6 (GPIO12)   GPIO12
 * SPI SCK     SCK          D5 (GPIO14)   GPIO14
 * 
 * Note: Adjust the GPIO pins as necessary for your specific ESP8266 board.
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 2
#define SS_PIN 15

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  
byte blockData[16] = {0};
MFRC522::StatusCode status;

void setup() {
	Serial.begin(115200);
	while (!Serial);
	SPI.begin();
	mfrc522.PCD_Init();
	delay(4);
	mfrc522.PCD_DumpVersionToSerial();
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
  
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
}

void loop() {
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

  WriteDataToBlock(1, blockData);
  WriteDataToBlock(2, blockData);
  for (int numBlock = 4; numBlock < 63 ; numBlock++) {
    if ((numBlock % 4) != 3) {
      WriteDataToBlock(numBlock, blockData);
    }
  }

	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

void WriteDataToBlock(int blockNum, byte blockData[]) 
{
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return;
  }

  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK) {
    return;
  }
}
