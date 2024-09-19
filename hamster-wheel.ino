#include <bluefruit.h>

unsigned int count = 0;
unsigned int on = 0;
double t = 0;

void setup() 
{
  Bluefruit.begin();

  // Turn off Blue LED for lowest power consumption
  Bluefruit.autoConnLed(false);
  
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(0);

  // Setup the advertising packet
  startAdv();

  Serial.begin(9600);  
  analogReadResolution(10); // Can be 8, 10, 12 or 14
  pinMode(LED_BUILTIN, OUTPUT);
}

void startAdv(void)
{
  setAdvertisingData(Bluefruit.Advertising, count);
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * Apple Beacon specs
   * - Type: Non connectable, undirected
   * - Fixed interval: 100 ms -> fast = slow = 100 ms
   */
  //Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(1600, 1600);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}


void incrementCount() {
  count = (unsigned int) (count +  1);
}

bool setAdvertisingData(BLEAdvertising& adv, unsigned int new_count)
{  
  struct ATTR_PACKED
  {
    uint16_t count;
  } beacon_data =
  {
    count = new_count
  };

  VERIFY_STATIC(sizeof(beacon_data) == 2);
  adv.clearData();
  adv.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  return adv.addData(BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, &beacon_data, sizeof(beacon_data));
}

void loop() {
  // Read current signal difference from our approximate background value
  double i = analogRead(A0);
  double delta = abs(480.0 - i);

  // Look for debounced swings away for our expected center
  if (delta > 15) {
    if (on == 0) {
      t = millis();
      incrementCount();
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println(count);
      setAdvertisingData(Bluefruit.Advertising, count);
    }
    on = 1;
    
  } else {
    if (millis() - t > 100) {
      // Release bounce protection
      on = 0;
      digitalWrite(LED_BUILTIN, LOW);
    }
  }

  delay(10);  
}
