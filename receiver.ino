void dumpMACs(){
  int digit;
  
  PRINT_STRING(Macs);
  for(int i = 0; i < numAnswers; i++){
    for(int j = 0; j < MAC_SIZE; j++){
      digit = (int)record[i].MAC[j];
      sprintf(string, STRING(MacsFormat), digit);
      Serial.print(string);
    }
    PRINT_STRING(NewLine);
  }
  PRINT_STRING(MacsN);
}

void showStats(){
  byte tmp;
  double percent;
  
  PRINT_STRING(Stats);
  for(int i = 0; i < sizeof(stats); i++){
    if(stats[i] != 0){
      tmp = i;
      percent = ((double)stats[i] * 100.0 ) / (double)numAnswers;
      sprintf(string, STRING(StatFormat), tmp, stats[i], percent);
      Serial.print(string);
      Serial.print(dtostrf(percent, 6, 2, string));
      PRINT_STRING(Percent);
    }
  }
  PRINT_STRING(Total);
  Serial.println(numAnswers);
  PRINT_STRING(PopularAnswer);
  Serial.write(maxAnswer);
  PRINT_STRING(StatsN);
}

void newQuestion(){
  memset(stats, 0, sizeof(stats));
  numAnswers = 0;
  maxAnswer = '1';
  oldAnswer = 255;
}

void compileStats(){
  memset(stats, 0, sizeof(stats));
  for(int i = 0; i < numAnswers; i++){
    if(++stats[record[i].answer[0]] > stats[maxAnswer])
      maxAnswer = record[i].answer[0];
  }
}

void recordAnswer(){
  byte buf[DATA_SIZE] = {0};
  
  digitalSpiRead(R_RX_PAYLOAD, buf, DATA_SIZE);
  digitalSpiWrite(W_REGISTER | STATUS, 0x70);    // Clear interrupts
  
  /* Read contents of payload */
  Serial.print("R: ");
  for(byte j = 0; j < DATA_SIZE; j++){
      Serial.print("\t ");
      Serial.print(buf[j], HEX);
  }
  Serial.println();
  
//  for(int i = 0; i < numAnswers; i++){
//    if(memcmp(buf, record[i].MAC, MAC_SIZE) == 0){
//      memcpy(record[i].answer, buf+MAC_SIZE, DATA_SIZE);
//      return;
//    }
//  }
//  if(numAnswers < MAX_RECORDS){
//    memcpy(record[numAnswers].MAC, buf, MAC_SIZE);
//    memcpy(record[numAnswers].answer, buf+MAC_SIZE, DATA_SIZE);
//    numAnswers++;
//  }
}

void receiveAsClickerMasterStart(){
    PRINT_STRING(EnterReceiveMode);
    digitalWrite(CE,LOW);                          // Modify configuration Registers
    digitalSpiWrite(W_REGISTER | CONFIG, 0x0A);    // Power On
    delay(2);
    digitalSpiWrite(W_REGISTER | CONFIG, 0x3F);    // 2-bit CRC, Receive mode
    digitalSpiWrite(W_REGISTER | EN_RXADDR, 0x01); // Only enable rx data pipe 1
    digitalSpiWrite(W_REGISTER | RX_PW_P0, DATA_SIZE);     // Set size of Receive pipe
    digitalSpiWrite(W_REGISTER | EN_AA, 0x01);     // Enable Auto-Acknowledge on pipe 0
    digitalSpiWrite(W_REGISTER | RF_CH, channel);  // Select the channel to receive on
    digitalSpiWrite(W_REGISTER | SETUP_AW, 0x03);  // Select 5-byte MAC length
    digitalSpiWrite(W_REGISTER | RF_SETUP, 0x0F);  // Set data rate to 2Mbps at high power
    digitalSpiWrite(FLUSH_RX);                     // Flush the receive buffer
    digitalSpiWrite(W_REGISTER | STATUS, 0x70);    // Clear interrupts
    digitalSpiWrite(W_REGISTER | RX_ADDR_P0, clickerMasterMAC, MAC_SIZE);  // Set the MAC address to listen on
    digitalWrite(CE,HIGH);                         //Finish modifying configuration Registers
    trigger = 0;
}
