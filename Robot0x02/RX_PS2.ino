void ReadPS()
{
  while (Serial3.available()) {
    byte b = Serial3.read();
    // Tìm header
    if (!receiving) {
      if (b == 0xAA) {
        index = 0;
        packet[index++] = b;
        receiving = true;
      }
    } else {
      packet[index++] = b;
      if (index >= 8) {
        // ✅ Đã nhận đủ 8 byte, xử lý
        if (packet[0] == 0xAA && packet[1] == ROBOT ) {   
          
          LX = packet[2] - 123;
          LY = packet[3] - 123;
          RX = packet[4] - 123;
          RY = packet[5] - 123;

          Button();
        } 
        // Reset lại trạng thái
        receiving = false;
        index = 0;
      }
    }
  }
}


void Button()
{
  // Kiểm tra từng nút theo bit trong packet[6]
  if ((packet[6] & (1 << 0)) != 0) {  // nút Tròn (bit 0)
    buttonState.b0 = !buttonState.b0;

    TimeSTART = millis();

  }
  if ((packet[6] & (1 << 1)) != 0) {  // nút X (bit 1)
    buttonState.b1 = !buttonState.b1;
      speed_esc1 = 60;
      speed_esc2 = 170;
     

  }
  if ((packet[6] & (1 << 2)) != 0) {  // nút Vuông (bit 2)
    SQUARE = !SQUARE;
    buttonState.b2 = !buttonState.b2;
    if (SQUARE) {
      motorSpeedControlUart.duty = 0x30; 
      motorSpeedControlUart.duty2 = 0x33;
    } else {
      motorSpeedControlUart.duty =0x78; 
      motorSpeedControlUart.duty2 = 0x40;
    }
  }
  if ((packet[6] & (1 << 3)) != 0) {  // nút Tam Giác (bit 3)
    buttonState.b3 = !buttonState.b3;
    if (buttonState.b3) Onvan2;
      else Offvan2;

  }
  if (((packet[6] & (1 << 4)) != 0) && ((packet[6] & (1 << 5)) == 0) ) {  // up (bit 4)
      buttonState.b4 = !buttonState.b4;
   //  if (myFlagstart == 0) { myFlag = 1;Onvan2;}
   //  else if (myFlagstart == 1 && (PINE & (1 << PE4))) Ha();
   Ha();
      Serial.println("Nâng");
  }
  if (((packet[6] & (1 << 5)) != 0) && ((packet[6] & (1 << 4)) == 0)) {  // down (bit 5)
      buttonState.b5 = !buttonState.b5;
     Nang();
      Serial.println("Hạ");
  }
  if (((packet[6] & (1 << 5)) == 0) && ((packet[6] & (1 << 4)) == 0)) {StopNH();
      Serial.println("");
    }

  if (((packet[6] & (1 << 6)) != 0) && ((packet[6] & (1 << 7)) == 0)) {  // left (bit 6)
    buttonState.b6 = !buttonState.b6;
    MamTrai();
  }
  if (((packet[6] & (1 << 7)) != 0) && ((packet[6] & (1 << 6)) == 0)) {  // right (bit 7)
    buttonState.b7 = !buttonState.b7;
    MamPhai();
  }

  if(((packet[6] & (1 << 6)) == 0) && ((packet[6] & (1 << 7)) == 0)){
    MamStop();
  }
  // Kiểm tra nút ở packet[7]
  if ((packet[7] & (1 << 0)) != 0) {  // r1
    buttonState.b8 = !buttonState.b8;
    if (speed_esc2 <= 245) speed_esc2 += 10;
    else speed_esc2 = 255;
  }
  if ((packet[7] & (1 << 1)) != 0) {  // r2
    buttonState.b9 = !buttonState.b9;
    if (speed_esc2 >= 10) speed_esc2 -= 10; 
    else speed_esc2 = 0;
  }
  if ((packet[7] & (1 << 2)) != 0) {  // r3
    buttonState.b10 = !buttonState.b10;
    timer1 = millis();
    dang_quay = 1;
  }

    
  if ((packet[7] & (1 << 3)) != 0) {  // l1
    buttonState.b11 = !buttonState.b11;
    if (speed_esc1 <= 245) speed_esc1 += 10;
    else speed_esc1 = 255;
  }
  if ((packet[7] & (1 << 4)) != 0) {  // l2
    buttonState.b12 = !buttonState.b12;
    if (speed_esc1 >= 10) speed_esc1 -= 10;
    else speed_esc1 = 0;
  }
  if ((packet[7] & (1 << 5)) != 0) {  // l3
    buttonState.b13 = !buttonState.b13;
      lastSendTime = millis();
  }
  
  if ((packet[7] & (1 << 6)) != 0) {  // select
    buttonState.b14 = !buttonState.b14;
    SELECT = !SELECT;
    if (SELECT) motorSpeedControlUart.hardLockValue = 0x00;
    else motorSpeedControlUart.hardLockValue = 0x02;
  }
  if ((packet[7] & (1 << 7)) != 0) {  // START
    buttonState.b15 = !buttonState.b15;
    ControlESC = !ControlESC;
  }

  if (ControlESC == 1) ESC();
  else STOP_ESC();
}


void Joystick(){
    if (LX >= -DEADZONE && LX <= DEADZONE && LY > -DEADZONE && LY < DEADZONE) {  
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 4) ? 1 : 0;
        }
      }
    
    if (RX >= -DEADZONE && RX <= DEADZONE && RY > -DEADZONE && RY < 10) {  
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 4) ? 1 : 0;
        }
      }
    
    if (RX >=  120 && RY > -DEADZONE && RY < DEADZONE) {  
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 1) ? 1 : 0;
        }
      }
    
    if (RX <= -120 && RY > -DEADZONE && RY < DEADZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 3) ? 1 : 0;
        }
      }
    
    if (RX > -DEADZONE && RX < DEADZONE && RY <= -120) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 2) ? 1 : 0;
        }
      }
    
    if (RX > -DEADZONE && RX < DEADZONE && RY >=  120) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 0) ? 1 : 0; 
        }
      }
    
    if (LX > -DEADZONE && LX < DEADZONE && LY <= -120) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 2) ? 1 : 0;
        }		
      }
    
    if (LX > -DEADZONE && LX < DEADZONE && LY >=  120) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 0) ? 1 : 0;
        }
      }

    
    if (LX >=  120 && LY > -DEADZONE && LY < DEADZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 1) ? 1 : 0;
        }
      }
    
    if (LX <= -120 && LY > -DEADZONE && LY < DEADZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 3) ? 1 : 0;
        }
      } 
}