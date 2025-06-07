void AUTO_DAPBONG() {
  switch (AUTO) {
    case AUTO_STEP1:
      // Kích đập
      Onvan4;
      TimeSS = millis();
      AUTO = AUTO_STEP2;
      break;

    case AUTO_STEP2:
      if (millis() - TimeSS >= TIM1Van) {
        // Mở kẹp
        Onvan3;
        TimeSS = millis();
        AUTO = AUTO_STEP3;
      }
      break;

    case AUTO_STEP3:
      if (millis() - TimeSS >= TIM2Van) {
        // Thu đập
        Offvan4;
        TimeSS = millis();
        AUTO = AUTO_STEP4;
      }
      break;

    case AUTO_STEP4:
      if ((millis() - TimeSS <= TIM3Van) && sensors[0].state == 1) {
        // Kẹp
        Offvan3;
        AUTO = AUTO_DONE;
      }
      if ((millis() - TimeSS >= TIM3Van) && sensors[0].state == 0) {
        AUTO = AUTO_OFF;
        valveState.WAITING_GET_BALL = true;
        WaitingBALL();
      }
      break;
    default:
      AUTO = AUTO_OFF;
      break;
  }
}

void ThaoTacDapBong() {
  if (valveState.RUNNING) return;
  valveState.RUNNING = true;
  Onvan1;

  if ((sensors[3].state == 1) && (sensors[2].state == 0)) { // Nếu tay đập đang ở trong đưa tay ra ngoài và thu các cơ cấu tránh gây lỗi
    // Đóng kẹp, Thu đập, Đi ra
    if (sensors[0].state == 1) Offvan3;
    if (sensors[0].state == 0) Onvan3;
    Offvan4;
    Onvan1;
  }

  if ((sensors[3].state == 0) && (sensors[2].state == 1)) { // Nếu tay đập đã ở ngoài
    if (sensors[0].state == 1) { // Nếu có bóng trong lồng 
      AUTO = AUTO_STEP1;
      TimeSTART = millis();
      AutoMode = true;
      AUTO_DAPBONG();
    }
  else {
      Onvan3; // Mở kẹp 
      valveState.WAITING_GET_BALL = true;
    }
  }
  valveState.RUNNING = false;
}

void WaitingBALL() {
  if (sensors[0].state == 1 && valveState.WAITING_GET_BALL) {
    // Kẹp bóng
    Offvan3;
    valveState.WAITING_GET_BALL = false;
    valveState.RUNNING = false;
    }
  }

void ThaoTacNapDan() {
  if (valveState.RUNNING) return;
  valveState.RUNNING = true;

  if (sensors[0].state == 0 && sensors[2].state == 1 ) { 
    valveState.WAITING_GET_BALL = true;
    }

  // Đóng kẹp
  Offvan3;
  if (sensors[0].state == 1 && // Tay có bóng
      sensors[1].state == 0 &&  // Thân chưa có bóng
      sensors[3].state == 0) {   // Chưa đi vào trong
    // Hạ nâng
    Offvan2;
    // Đi vào
    Offvan1; 
  }

  if (sensors[3].state == 1) {
    // Mở kẹp
    Onvan3;
    if (sensors[1].state == 1) { // Thân có bóng
      // Đóng kẹp 
      Offvan3;
      chochut = millis();
      if (millis() - chochut >= 100) {
      // Ra
      Onvan1;
      }
    }
  }

  valveState.RUNNING = false;
}

void ThaoTacBanBong() {
  if (valveState.RUNNING) return;
  valveState.RUNNING = true;

  if (sensors[1].state == 1) {
    // Nâng
    Onvan2;
  }

  if ( sensors[3].state == 1 || sensors[1].state == 0) {
    // Hạ
    Offvan2;
  }
  valveState.RUNNING = false;
}

void ThaoTacCapBong(){
    Offvan1; 
    if (sensors[0].state == 1) Offvan3;
    if (sensors[0].state == 0) Onvan3;
    if (sensors[3].state == 1) Offvan2;
    valveState.WAITING_GET_BALL = true;
}