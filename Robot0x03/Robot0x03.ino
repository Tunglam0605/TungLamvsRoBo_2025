//================================================== ROBOT ==================================================

  uint8_t ROBOT = 0x03; // Địa chỉ định danh cho robot hiện tại (ví dụ trong hệ thống nhiều robot, mỗi con có một địa chỉ)

//================================================== Biến tay cầm ==================================================
  // Biến cho việc xử lý dữ liệu nhận từ tay cầm điều khiển

  byte packet[8];         // Bộ đệm 8 byte để chứa gói dữ liệu từ tay cầm
  byte index = 0;         // Chỉ số hiện tại trong bộ đệm
  bool receiving = false; // Cờ đánh dấu đang trong quá trình nhận dữ liệu

  int LX, LY, RX, RY;     // Lưu trữ giá trị joystick trái (LX, LY) và phải (RX, RY)

  #define DEADZONE 45     // Vùng chết: nếu giá trị joystick nằm trong khoảng này, coi như không di chuyển
  #define ATKZONE 100     // Ngưỡng tối đa cho các giá trị joystick (ví dụ: để nhận biết đang nhấn mạnh)

  typedef struct {        
    uint8_t JOYTRAI[5];   // Trạng thái joystick trái: lưu các trạng thái khác nhau (tùy theo ứng dụng)
    uint8_t JOYPHAI[5];   // Trạng thái joystick phải
  } JOYSTICK_STATE;

  JOYSTICK_STATE StateJoys = {
    .JOYTRAI = {0},
    .JOYPHAI = {0}
  };

  // Trạng thái một số nút cơ bản trên tay cầm
  bool Mode = 0;
  bool SQUARE = 0;
  bool SELECT = 0;

  bool dang_quay = 0;         // Biến đánh dấu robot đang xoay
  unsigned long timer1;       // Biến đếm thời gian, dùng để delay hoặc kiểm tra timeout

  // Cấu trúc lưu trạng thái từng nút nhấn (tối đa 16 nút)
    typedef struct {
      bool b0; bool b1; bool b2; bool b3;
      bool b4; bool b5; bool b6; bool b7;
      bool b8; bool b9; bool b10; bool b11;
      bool b12; bool b13; bool b14; bool b15;
    } ButtonState_t;

    ButtonState_t buttonState = {0};  // Khởi tạo tất cả các nút ở trạng thái "chưa nhấn"

  // Hàm in trạng thái các nút ra Serial để debug
    void printButtonState(ButtonState_t btn) {
      Serial.print("BTN: ");
      Serial.print(btn.b0); Serial.print(" ");
      Serial.print(btn.b1); Serial.print(" ");
      Serial.print(btn.b2); Serial.print(" ");
      Serial.print(btn.b3); Serial.print(" ");
      Serial.print(btn.b4); Serial.print(" ");
      Serial.print(btn.b5); Serial.print(" ");
      Serial.print(btn.b6); Serial.print(" ");
      Serial.print(btn.b7); Serial.print(" ");
      Serial.print(btn.b8); Serial.print(" ");
      Serial.print(btn.b9); Serial.print(" ");
      Serial.print(btn.b10); Serial.print(" ");
      Serial.print(btn.b11); Serial.print(" ");
      Serial.print(btn.b12); Serial.print(" ");
      Serial.print(btn.b13); Serial.print(" ");
      Serial.print(btn.b14); Serial.print(" ");
      Serial.println(btn.b15);
    }

//================================================== Gói UART gửi đến driver động cơ ==================================================
  // Cấu trúc dữ liệu cho 1 gói gửi đến 1 động cơ
    struct TxSendPID {
      uint8_t address;    // Địa chỉ của động cơ (1 ~ 127)
      uint8_t speed;      // Tốc độ cần gửi (0 ~ 255)
      uint8_t direction;  // Hướng quay: 0 = ngược, 1 = thuận
    };

//================================================== Trạng thái 4 động cơ ==================================================
  // Bộ nhớ lưu tốc độ và hướng của 4 động cơ
    struct MotorTx {
      uint8_t SPEED[4];   // Tốc độ từng động cơ
      uint8_t DIR[4];     // Hướng từng động cơ
    } motorTx;

    uint8_t DataTX[4][3]; // Mỗi động cơ có 1 gói gồm 3 byte: address, speed, direction

//================================================== Điều khiển tăng/giảm tốc ==================================================
  // Điều khiển tốc độ mượt mà bằng cách tăng/giảm theo bước
    struct MotorSpeedControlUart {
      uint8_t current[4];       // Tốc độ hiện tại từng động cơ
      uint8_t duty;             // Mức tốc độ đích (duty cycle)
      uint8_t duty2;            // Duty thứ 2: có thể dùng để xoay
      uint8_t duty3;            // Duty thứ 3: dùng riêng cho xoay/di chuyển nhanh
      uint8_t step_max;         // Bước tăng tối đa
      uint8_t step_min;         // Bước tăng tối thiểu
      uint8_t hardLockValue;    // Tốc độ dừng gấp
      uint8_t slowDownStep_max; // Bước giảm tốc tối đa
      uint8_t slowDownStep_min; // Bước giảm tốc tối thiểu
      uint8_t MODEm;            // Chế độ điều khiển (0: dừng, 1: chạy)
    } motorSpeedControlUart = {
      {0,0,0,0},    // current
      0,            // duty
      0,            // duty2
      0xFE,         // duty3
      26, 4,        // step_max, step_min
      0x00,         // hardLockValue
      16, 3,        // slowDownStep_max, slowDownStep_min
      0             // MODEm
    };

//================================================== Điều khiển hướng di chuyển của robot ==================================================
  struct UartMotorMotionControl {
    uint8_t TienUart[4]; // Hướng quay khi tiến
    uint8_t LuiUart[4];  // Hướng quay khi lùi
  } UartmotorMotionControl = {
    {0,1,1,0},    // Động cơ 1,4 quay thuận; 2,3 ngược => tiến
    {1,0,0,1}     // Ngược lại => lùi
  };

//================================================== Tính toán bước thay đổi tốc độ ==================================================
  // Tính bước tăng tốc dựa theo hiệu giữa tốc độ hiện tại và tốc độ đích
    uint8_t CalcStep(uint8_t cur, uint8_t duty, uint8_t maxStep, uint8_t minStep) {
      uint8_t diff = (cur > duty) ? cur - duty : duty - cur;
      if (diff > 200) return minStep;
      else if (diff > 150) return maxStep/4;
      else if (diff > 113) return maxStep/3;
      else if (diff > 89)  return maxStep/2;
      else if (diff > 65)  return maxStep;
      else if (diff > 46)  return maxStep/2;
      else if (diff > 24)  return minStep;
      return 1; // nếu gần bằng thì thay đổi rất nhỏ
    }

    uint8_t CalcStep2(uint8_t cur, uint8_t duty, uint8_t maxStep, uint8_t minStep) {
      uint8_t diff = (cur > duty) ? cur - duty : duty - cur;
      if (diff > 65) return maxStep;
      else if (diff > 48) return maxStep/2;
      else if (diff > 24) return maxStep/3;
      return 1;
    }

    uint8_t CalcStep3(uint8_t cur, uint8_t duty, uint8_t maxStep, uint8_t minStep) {
      uint8_t diff = (cur > duty) ? cur - duty : duty - cur;
      if (diff > 50) return maxStep;
      else if (diff > 40) return maxStep/2;
      else if (diff > 20) return minStep;
      return 1;
    }

//================================================== Điều khiển động cơ BLDC (ESC) ==================================================
  uint8_t speed_esc1 = 150;   // Tốc độ ESC 1
  uint8_t speed_esc2 = 185;   // Tốc độ ESC 2
  bool ControlESC = 0;        // Cờ bật/tắt động cơ BLDC
 
//================================================== Cấu trúc van khí ==================================================
    struct VanControl {
      bool state;  // Trạng thái bật/tắt van
    };
    VanControl van[4] = {0};  // Mảng chứa 4 van khí

  // Chân kết nối đến các van (sử dụng PORTL - Arduino Mega)
    const uint8_t vanPins[4] = {45,46,47,48};

  // Macro bật/tắt từng van (điều khiển trực tiếp thanh ghi PORTL)
    #define Onvan1  PORTL |=  (1 << 4)
    #define Offvan1 PORTL &= ~(1 << 4)
    #define Onvan2  PORTL |=  (1 << 3)
    #define Offvan2 PORTL &= ~(1 << 3)
    #define Onvan3  PORTL |=  (1 << 2)
    #define Offvan3 PORTL &= ~(1 << 2)
    #define Onvan4  PORTL |=  (1 << 1)
    #define Offvan4 PORTL &= ~(1 << 1)

//================================================== Cảm biến trạng thái ==================================================
  #define NUM_SENSORS 4 // Số lượng cảm biến sử dụng

  // Cấu trúc cảm biến đơn giản
  struct Sensor {
    uint8_t pin;            // Chân digital nối vào cảm biến
    volatile int state;     // Trạng thái cảm biến (HIGH / LOW)
  };

  // Khởi tạo 4 cảm biến gắn vào các chân digital
  Sensor sensors[NUM_SENSORS] = {
    {18, 0},   // Sensor 0
    {19, 0},   // Sensor 1
    {20, 0},   // Sensor 2
    {21, 0}    // Sensor 3
  };

  // Các hàm ngắt tương ứng với cảm biến (viết tiếp ISR nếu cần)
  void ISR_Sensor0() { sensors[0].state = (PIND & (1 << PD3)) ? 1 : 0; } // Trong lồng
  void ISR_Sensor1() { sensors[1].state = (PIND & (1 << PD2)) ? 1 : 0; } // Trong thân
  void ISR_Sensor2() { sensors[2].state = (PIND & (1 << PD1)) ? 1 : 0; } // Ra 
  void ISR_Sensor3() { sensors[3].state = (PIND & (1 << PD0)) ? 1 : 0; } // Vào

  // ============================
  // 4. Hàm trả về con trỏ tới ISR tương ứng
  // ============================
  void (*getISR(int index))() {
    switch (index) {
      case 0: return ISR_Sensor0;
      case 1: return ISR_Sensor1;
      case 2: return ISR_Sensor2;
      case 3: return ISR_Sensor3;
      default: return NULL;
    }
  }

//================================================== Cấu trúc Các khâu tự động ==================================================
  enum AutoBall {
    AUTO_OFF = 0,
    AUTO_STEP1,
    AUTO_STEP2,
    AUTO_STEP3,
    AUTO_STEP4,
    AUTO_STEP5,
    AUTO_STEP6,
    AUTO_DONE
  };   AutoBall AUTO = AUTO_OFF;

  struct ValveState {
    bool kickBall;
    bool prepareShoot;
    bool turn;
    bool keepBall;
    bool changeShootingAngle;
    bool holdPosition;
    bool getTheBall;
    bool RUNNING;
    bool WAITING_GET_BALL;
  };  ValveState valveState = {false, false, false, false, false, false, false, false, false};

  uint8_t  TIM1Van  = 25;  // Thời gian van kẹp, kích mở luân phiên
  uint8_t  TIM2Van  = 500; // Thời gian van kick đóng
  uint16_t TIM3Van  = 0;

  uint32_t TimeSS = 0; // Timer so sánh mốc khâu đập bóng
  uint32_t TimeSTART = 0; // Timer so sánh mốc khâu nạp đạn
  bool AutoMode = false;

  uint8_t chochut = 0;
  uint32_t TimeNapDan = 0;
  bool AutoNapDan = false;

  uint32_t  TimeSTART2 = 0;         // Thời điểm nâng lưới trước

void setup() {

  // Serial Init
    Serial.begin(115200);       // Khởi động Serial chính để debug
    Serial2.begin(115200);      // Khởi động Serial2 để gửi UART tới driver động cơ Planet
    Serial3.begin(115200);      // Khởi động Serial3 nhận tín hiệu tay cầm

  // Thiết lập tốc độ/hướng ban đầu = 0 cho 4 động cơ   
    for (int i = 0; i < 4; i++) {
        motorTx.SPEED[i] = 0;   // Đặt tốc độ = 0
        motorTx.DIR[i] = 0;     // Đặt hướng = 0 (ngược)
        }

  // Khởi tạo chân ngắt đọc cảm biến 
    for (int i = 0; i < NUM_SENSORS; i++) {
      pinMode(sensors[i].pin, INPUT_PULLUP);  // Thiết lập chân INPUT_PULLUP
      void (*isrFunc)() = getISR(i);   // Lấy hàm ngắt tương ứng
      if (isrFunc != NULL) { attachInterrupt(digitalPinToInterrupt(sensors[i].pin), isrFunc, CHANGE);}
      }

  // Đọc trạng thái các chân cảm biến lần đầu
    sensors[0].state = (PIND & (1 << PD3)) ? 1 : 0;  // Trong lồng
    sensors[1].state = (PIND & (1 << PD2)) ? 1 : 0;  // Trong thân 
    sensors[2].state = (PIND & (1 << PD1)) ? 1 : 0;  // Ra 
    sensors[3].state = (PIND & (1 << PD0)) ? 1 : 0;  // Vào

  // Khởi tạo van (OFF)
    for (int i = 0; i < 4; i++) {
      pinMode(vanPins[i], OUTPUT);
      van[i].state = false;
      }

  // Khởi tạo các chân PWM lô bắn
    for (int i = 2; i <= 13; i++) {
      pinMode(i, OUTPUT);
      }

  // Trạng thái van lần đầu 
    ThaoTacCapBong();

     Mode = 1;
}

void loop() {
  ReadPS();
  Dichuyen();
  Prepare_Data();
  //--------------------- AUTOMATION ---------------------------------------------------------
    // Quay tự động
      if(millis() - timer1 < 444 && dang_quay) { TraiTx2(); }
      if(millis() - timer1 > 444 && millis() - timer1 < 1500) { dang_quay = 0;}

    // Thao tác bóng tự động : Phát hiện bóng, đạp bóng, nạp đạn
      WaitingBALL(); 																									
      if (Mode == 1 && AutoMode == 1 && (millis() - TimeSTART < 1500)){ AUTO_DAPBONG();  } 		// START TIMER KICK BALL
      if (Mode == 1 && AutoMode == 1 && (millis() - TimeSTART > 1500) && AUTO == AUTO_OFF){
          valveState.WAITING_GET_BALL = true;
          AutoMode = 0;
          WaitingBALL();
        }																	
      if (Mode == 1 && AUTO == AUTO_DONE ) AutoMode = 0;		                                  // OFF TIMER KICK BALL
      if (Mode == 1 &&  AutoNapDan == 1 && (millis() - TimeNapDan < 5000)) { ThaoTacNapDan(); } // START TIMER NAP DAN
      if (Mode == 1 && (AutoNapDan == 1 && (millis() - TimeNapDan > 5000) && (millis() - TimeNapDan < 5010)||valveState.WAITING_GET_BALL == true)){ AutoNapDan = 0; }	// OFF TIMER NAP DAN

    // Tự động hạ bắn

      if(buttonState.b0 == 1 && (millis() - TimeSTART2 > 2500)) {buttonState.b0 = 0;}

      if (buttonState.b0 == 1) Onvan2;      // BẬT TẮT NÂNG HẠ BẮN BÓNG
      else Offvan2; 


  //  // In hướng và tốc độ từng bánh
  //   for (int i = 0; i < 4; i++) {
  //     Serial.print("Motor "); Serial.print(i + 1);
  //     Serial.print(" - DIR: "); Serial.print(motorTx.DIR[i]);
  //     Serial.print(" | SPEED: "); Serial.print(motorTx.SPEED[i]);
  //     }

    // for (int i = 0; i < 4; i++) {
    //   Serial.print(" | Sensor "); 
    //   Serial.print(i + 1);
    //   Serial.print(sensors[i].state);
    // }
    // //   Serial.println(""); 

       Serial.print(" | mode: "); Serial.print(Mode);
       Serial.print(" | modeAUTO: "); Serial.print(AutoNapDan);
      Serial.print(" | esc1: "); Serial.print(speed_esc1);
      Serial.print(" | esc2: "); Serial.print(speed_esc2);

    printButtonState(buttonState);
}