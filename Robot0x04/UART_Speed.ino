// Hàm tăng/giảm tốc hướng tới duty
  void TangtocUart() {
    motorSpeedControlUart.MODEm = 1;       // Bật chế độ điều khiển  
      for (int i = 0; i < 4; i++) {
      uint8_t cur = motorSpeedControlUart.current[i];
      uint8_t duty = motorSpeedControlUart.duty;
      uint8_t step = CalcStep(cur, duty, motorSpeedControlUart.step_max, motorSpeedControlUart.step_min);
      // Cập nhật current tiến gần hơn tới duty
      cur = (cur < duty) ? min(cur + step, duty) : max(int(cur) - step, int(duty));
      motorSpeedControlUart.current[i] = cur; // Lưu lại current mới
      motorTx.SPEED[i] = cur;                // Cập nhật tốc độ cho gói gửi
     }
  }

// Hàm tăng/giảm tốc xoay hướng tới duty2
  void TangtocUart2() {
    motorSpeedControlUart.MODEm = 1;       // Bật chế độ điều khiển
    for (int i = 0; i < 4; i++) {
      uint8_t cur = motorSpeedControlUart.current[i];
      uint8_t duty = motorSpeedControlUart.duty2;
      uint8_t step = CalcStep(cur, duty, motorSpeedControlUart.step_max, motorSpeedControlUart.step_min);
      // Cập nhật current tiến gần hơn tới duty
      cur = (cur < duty) ? min(cur + step, duty) : max(int(cur) - step, int(duty));
      motorSpeedControlUart.current[i] = cur; // Lưu lại current mới
      motorTx.SPEED[i] = cur;                // Cập nhật tốc độ cho gói gửi
    }
  }

// Hàm tăng tốc 3: bánh 1&4 tăng tốc, bánh 2&3 giảm tốc
  void TangtocUart3() {
    motorSpeedControlUart.MODEm = 1;  // mode điều khiển
    for (int i = 0; i < 4; i++) {
      uint8_t cur = motorSpeedControlUart.current[i];
      // Chọn target: bánh 1 (i=0) & bánh 4 (i=3) dùng duty, bánh 2&3 dùng hardLockValue
      uint8_t target = (i == 0 || i == 3)
                      ? motorSpeedControlUart.duty
                      : motorSpeedControlUart.hardLockValue;

      // Chọn bước step: nếu đang tăng (target > cur) thì dùng step_max/min, ngược lại dùng slowDownStep_max/min
      uint8_t step = (target > cur)
                    ? CalcStep(cur, target, 14, 1)
                    : CalcStep(cur, target, 14, 1);

      // Cập nhật cur tiến gần target
      if (cur < target) {
        cur = min(cur + step, target);
      } else {
        cur = max(int(cur) - step, int(target));
      }

      // Lưu lại và gán cho gói gửi
      motorSpeedControlUart.current[i] = cur;
      motorTx.SPEED[i] = cur;
    }
  }

// Hàm tăng tốc 3: bánh 2&3 tăng tốc, bánh 1&4 giảm tốc
  void TangtocUart4() {
    motorSpeedControlUart.MODEm = 1;  // mode điều khiển
    for (int i = 0; i < 4; i++) {
      uint8_t cur = motorSpeedControlUart.current[i];
      uint8_t target = (i == 1 || i == 2)
                      ? motorSpeedControlUart.duty
                      : motorSpeedControlUart.hardLockValue;

      // Chọn bước step: nếu đang tăng (target > cur) thì dùng step_max/min, ngược lại dùng slowDownStep_max/min
      uint8_t step = (target > cur)
                    ? CalcStep(cur, target, 14, 1)
                    : CalcStep(cur, target, 14, 1);

      // Cập nhật cur tiến gần target
      if (cur < target) {
        cur = min(cur + step, target);
      } else {
        cur = max(int(cur) - step, int(target));
      }
      // Lưu lại và gán cho gói gửi
      motorSpeedControlUart.current[i] = cur;
      motorTx.SPEED[i] = cur;
    }
  }

void TangtocUart5() {
  motorSpeedControlUart.MODEm = 1;       // Bật chế độ điều khiển
    for (int i = 0; i < 4; i++) {
    uint8_t cur = motorSpeedControlUart.current[i];
    uint8_t duty = motorSpeedControlUart.duty3;
    uint8_t step = CalcStep3(cur, duty, motorSpeedControlUart.step_max, motorSpeedControlUart.step_min);
    // Cập nhật current tiến gần hơn tới duty
    cur = (cur < duty) ? min(cur + step, duty) : max(int(cur) - step, int(duty));
    motorSpeedControlUart.current[i] = cur; // Lưu lại current mới
    motorTx.SPEED[i] = cur;                // Cập nhật tốc độ cho gói gửi
    }
}

// Hàm dừng cứng, giảm tốc về hardLockValue
void StopUart() {
    motorSpeedControlUart.MODEm = 0;       // Tắt chế độ điều khiển
    for (int i = 0; i < 4; i++) {
      uint8_t cur = motorSpeedControlUart.current[i];
      uint8_t target = motorSpeedControlUart.hardLockValue;
      uint8_t step = CalcStep2(cur, target, motorSpeedControlUart.slowDownStep_max, motorSpeedControlUart.slowDownStep_min);
      cur = (cur > target) ? max(int(cur) - step, int(target)) : target; // Giảm về target
      motorSpeedControlUart.current[i] = cur;
      motorTx.SPEED[i] = cur;
    }
  }