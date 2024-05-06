/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
   Changed to a beacon scanner to report iBeacon, EddystoneURL and EddystoneTLM beacons by beegee-tokyo
*/

#include <M5Stack.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>


void getElementType(uint8_t ElementType)
{
  switch (ElementType) {
    case 2:
    case 4:
    case 6:
      Serial.printf("ET1");
      break;
    case 3:
    case 5:
    case 7:
      Serial.printf("EA1");
      break;
    case 8:
      Serial.printf("ET2");
      break;
    case 9:
      Serial.printf("EA2");
      break;
    case 19:
      Serial.printf("EB1");
      break;
    case 20:
      Serial.printf("EU2");
      break;
    case 21:
      Serial.printf("EU3");
      break;
    case 22:
      Serial.printf("ET3");
      break;
    case 23:
      Serial.printf("EB2");
      break;
    default:
      Serial.printf("NA");
      break;
    }

  return;
}


int scanTime = 5; //In seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
      if (advertisedDevice.haveManufacturerData() == true)
      {
        std::string strManufacturerData = advertisedDevice.getManufacturerData();

        uint8_t cManufacturerData[100];
        strManufacturerData.copy((char *)cManufacturerData, strManufacturerData.length(), 0);

        if (cManufacturerData[0] == 0xE3 && cManufacturerData[1] == 0x05)
        {
          Serial.printf("RSSI %d ", advertisedDevice.getRSSI());
          Serial.printf("MAC %s ", advertisedDevice.getAddress().toString().c_str());

          Serial.printf("EM: %d ", strManufacturerData.length());

          getElementType(cManufacturerData[2]);

          for (int i = 0; i < strManufacturerData.length(); i++)
          {
            Serial.printf("[%02X]", cManufacturerData[i]);
          }
          Serial.printf("\n");
        }
      }

    }
};

void setup()
{
  M5.begin();
  M5.Lcd.println("Elemental BLE Scanner start.");

  Serial.begin(115200);
  Serial.println("Scanning...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  //pBLEScan->setWindow(99); // less or equal setInterval value
  pBLEScan->setWindow(20); // less or equal setInterval value
}

void loop()
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  Serial.println("Scan done!");
  pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
  delay(2000);
}
