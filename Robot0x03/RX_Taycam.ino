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
        if (packet[0] == 0xAA && packet[1] == ROBOT) {   

          LX = packet[2] - 123;
          LY = packet[3] - 132;
          RX = packet[4] - 123;
          RY = packet[5] - 132;

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
    //ThaoTacBanBong();
      TimeSTART2 = millis();


  }
  if ((packet[6] & (1 << 1)) != 0) {  // nút X (bit 1)
    buttonState.b1 = !buttonState.b1;
    speed_esc1 = 150; speed_esc2 = 185;
  }
  if ((packet[6] & (1 << 2)) != 0) {  // nút Vuông (bit 2)
    buttonState.b2 = !buttonState.b2;
    
    if (buttonState.b2) {  // Thay đổi tốc độ đế Robot
      motorSpeedControlUart.duty = 0x40; 
      motorSpeedControlUart.duty2 = 0x30;
    } else {
      motorSpeedControlUart.duty = 0xFE; 
      motorSpeedControlUart.duty2 = 0x65;
    }

  }
  if ((packet[6] & (1 << 3)) != 0) {  // nút Tam Giác (bit 3)
    buttonState.b3 = !buttonState.b3;
    ThaoTacCapBong(); // Trạng thái cấp bóng
  }
  if ((packet[6] & (1 << 4)) != 0) {  // up (bit 4)
      buttonState.b4 = !buttonState.b4;
      if (Mode == 0){  // Mode điều khiển xi lanh bằng tay
        if (buttonState.b4) Onvan1;
        else Offvan1;
      }
      if (Mode == 1){  // Mode điều khiển xi lanh ở trạng thái tự động tằng giảm tốc độ lô bắn
        if (speed_esc1 <= 245) speed_esc1 += 5;
        else speed_esc1 = 255;
      }
    
  }
  if ((packet[6] & (1 << 5)) != 0) {  // down (bit 5)
      buttonState.b5 = !buttonState.b5;
      if (Mode == 0){  // Mode điều khiển xi lanh bằng tay
        if (buttonState.b5) Onvan2;
        else Offvan2;
        }
      if (Mode == 1){  // Mode điều khiển xi lanh ở trạng thái tự động tằng giảm tốc độ lô bắn
        if (speed_esc1 >= 10) speed_esc1 -= 5; 
        else speed_esc1 = 0;
        }
  }
  if (((packet[6] & (1 << 6)) !=0) && ((packet[6] & (1 << 7)) == 0))  {  // left (bit 6)
    buttonState.b6 = !buttonState.b6;
      if (Mode == 1){ // Mode điều khiển xi lanh ở trạng thái tự động quay góc bắn
      MamTrai();
        }
      if (Mode == 0){  // Mode điều khiển xi lanh bằng tay
        if (buttonState.b6) Onvan3;
        else Offvan3;
      }
  }
  if (((packet[6] & (1 << 7)) !=0) && ((packet[6] & (1 << 6)) == 0)) {  // right (bit 7)
    buttonState.b7 = !buttonState.b7;
      if (Mode == 1){  // Mode điều khiển xi lanh ở trạng thái tự động quay góc bắn
        MamPhai();
        }
      if (Mode == 0){ // Mode điều khiển xi lanh bằng tay
        if (buttonState.b5) Onvan4;
        else Offvan4;
      }
  }
  if (((packet[6] & (1 << 7)) ==0 ) && ((packet[6] & (1 << 6)) == 0)){
    if (Mode == 1)  MamStop(); // Mode điều khiển xi lanh ở trạng thái tự động quay góc bắn
  }

  // Kiểm tra nút ở packet[7]
  if ((packet[7] & (1 << 0)) != 0) {  // r1
    buttonState.b8 = !buttonState.b8;
    speed_esc1 = 156; speed_esc2 = 190;
    
    // if (Mode == 1) ThaoTacDapBong(); // Mode điều khiển xi lanh ở trạng thái tự động đập bóng
  }
  if ((packet[7] & (1 << 1)) != 0) {  // r2
    buttonState.b9 = !buttonState.b9;
    speed_esc1 = 160; speed_esc2 = 195;

    /*
    if (Mode == 1){ // Mode điều khiển xi lanh ở trạng thái tự động nạp bóng
      AutoNapDan = true;
      TimeNapDan = millis();
      }
    */
  }
  if ((packet[7] & (1 << 2)) != 0) {  // r3
    buttonState.b10 = !buttonState.b10;
   // timer1 = millis(); // Quay tự động
   // dang_quay = 1;
  }
    
  if ((packet[7] & (1 << 3)) != 0) {  // l1
    buttonState.b11 = !buttonState.b11;
    if (Mode == 1){
      if (speed_esc2 <= 245) speed_esc2 += 5;
      else speed_esc2 = 255;
      }
  }
  if ((packet[7] & (1 << 4)) != 0) {  // l2
    buttonState.b12 = !buttonState.b12;
    if (Mode == 1){
      if (speed_esc2 >= 10) speed_esc2 -= 5;
      else speed_esc2 = 0;
      }
  }
  if ((packet[7] & (1 << 5)) != 0) {  // l3
    buttonState.b13 = !buttonState.b13; // Thay đổi mode tự động vs bằng tay
     Mode = 1;  
  }
    //if (buttonState.b13 == 0) Mode = 1;
    //else Mode = 0; 

  if ((packet[7] & (1 << 6)) != 0) {  // select
    buttonState.b14 = !buttonState.b14; // Bật tắt khóa bánh
    if (buttonState.b14) motorSpeedControlUart.hardLockValue = 0x00;
    else motorSpeedControlUart.hardLockValue = 0x02;
  }
  if ((packet[7] & (1 << 7)) != 0) {  // START
    buttonState.b15 = !buttonState.b15;
    ControlESC = !ControlESC; // Bật tắt động cơ lô bắn 
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
    
    if (RX >= -DEADZONE && RX <= DEADZONE && RY > -DEADZONE && RY < DEADZONE) {  
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 4) ? 1 : 0;
        }
      }
    
    if (RX >=  ATKZONE && RY > -DEADZONE && RY < DEADZONE) {  
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 1) ? 1 : 0;
        }
      }
    
    if (RX <= -ATKZONE && RY > -DEADZONE && RY < DEADZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 3) ? 1 : 0;
        }
      }
    
    if (RX > -DEADZONE && RX < DEADZONE && RY <= -ATKZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 2) ? 1 : 0;
        }
      }
    
    if (RX > -DEADZONE && RX < DEADZONE && RY >=  ATKZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYPHAI[i] = (i == 0) ? 1 : 0; 
        }
      }
    
    if (LX > -DEADZONE && LX < DEADZONE && LY <= -ATKZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 2) ? 1 : 0;
        }		
      }
    
    if (LX > -DEADZONE && LX < DEADZONE && LY >=  ATKZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 0) ? 1 : 0;
        }
      }

    if (LX >=  ATKZONE && LY > -DEADZONE && LY < DEADZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 1) ? 1 : 0;
        }
      }
    
    if (LX <= -ATKZONE && LY > -DEADZONE && LY < DEADZONE) { 
        for (int i = 0; i < 5; i++) {
          StateJoys.JOYTRAI[i] = (i == 3) ? 1 : 0;
        }
      } 
}