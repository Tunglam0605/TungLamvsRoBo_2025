
// Hàm thiết lập mảng hướng trước khi gọi TangtocUart/StopUart
  void SetDirection(uint8_t dirArr[4]) {
    for (int i = 0; i < 4; i++) motorTx.DIR[i] = dirArr[i];
  }

// Các lệnh di chuyển: gán hướng rồi tăng tốc
  void TienTx()    { SetDirection(UartmotorMotionControl.TienUart); TangtocUart(); }
  void LuiTx()   { SetDirection(UartmotorMotionControl.LuiUart); TangtocUart(); }
  void PhaiTx()   { uint8_t d[4]={1,1,1,1}; SetDirection(d); TangtocUart2(); }
  void TraiTx()   { uint8_t d[4]={0,0,0,0}; SetDirection(d); TangtocUart2(); }
  void N_TraiTx() { uint8_t d[4]={1,1,0,0}; SetDirection(d); TangtocUart(); }
  void N_PhaiTx() { uint8_t d[4]={0,0,1,1}; SetDirection(d); TangtocUart(); }

  // Đi quành
  void LuiTx2()    { SetDirection(UartmotorMotionControl.LuiUart); TangtocUart4(); }
  void TienTx2()   { SetDirection(UartmotorMotionControl.TienUart); TangtocUart4(); }
  void LuiTx3()    { SetDirection(UartmotorMotionControl.LuiUart); TangtocUart3(); }
  void TienTx3()   { SetDirection(UartmotorMotionControl.TienUart); TangtocUart3(); }
  
  void TraiTx2()   { uint8_t d[4]={1,1,1,1}; SetDirection(d); TangtocUart5(); }
  void PhaiTx2()   { uint8_t d[4]={0,0,0,0}; SetDirection(d); TangtocUart5(); }