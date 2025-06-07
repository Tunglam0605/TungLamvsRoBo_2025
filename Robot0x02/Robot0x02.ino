//================================================== Địa chỉ ROBOT ==================================================

  uint8_t ROBOT = 0x02;

//================================================== Biến tay cầm ==================================================
  byte packet[8];   // Buffer chứa dữ liệu nhận
  byte index = 0;   // Vị trí byte đang nhận
  bool receiving = false;

  int LX,LY,RX,RY;

  #define DEADZONE 60      // Vùng chết Joystick
  typedef struct {
    uint8_t JOYTRAI[5];
    uint8_t JOYPHAI[5];
  } JOYSTICK_STATE;

  JOYSTICK_STATE StateJoys = {
      .JOYTRAI = {0},
      .JOYPHAI = {0}
  };

  bool Mode = 0;
  bool SQUARE = 0;
  bool SELECT = 0;

  bool dang_quay = 0;
  unsigned long timer1;


  typedef struct {
      bool b0;
      bool b1;
      bool b2;
      bool b3;
      bool b4;
      bool b5;
      bool b6;
      bool b7;
      bool b8;
      bool b9;
      bool b10;
      bool b11;
      bool b12;
      bool b13;
      bool b14;
      bool b15;
  } ButtonState_t;

  ButtonState_t buttonState = {0};  // Gán tất cả = false

  void printButtonState(ButtonState_t btn) {
    Serial.print("BTN: ");
    Serial.print(btn.b0);   Serial.print(" ");
    Serial.print(btn.b1);   Serial.print(" ");
    Serial.print(btn.b2);   Serial.print(" ");
    Serial.print(btn.b3);   Serial.print(" ");
    Serial.print(btn.b4);   Serial.print(" ");
    Serial.print(btn.b5);   Serial.print(" ");
    Serial.print(btn.b6);   Serial.print(" ");
    Serial.print(btn.b7);   Serial.print(" ");
    Serial.print(btn.b8);   Serial.print(" ");
    Serial.print(btn.b9);   Serial.print(" ");
    Serial.print(btn.b10);  Serial.print(" ");
    Serial.print(btn.b11);  Serial.print(" ");
    Serial.print(btn.b12);  Serial.print(" ");
    Serial.print(btn.b13);  Serial.print(" ");
    Serial.print(btn.b14);  Serial.print(" ");
    Serial.print(btn.b15);  Serial.println();
  }


//================================================== Khai báo cấu trúc gói UART gửi đến driver động cơ ==================================================

  struct TxSendPID {
    uint8_t address;       // Địa chỉ (1-127) của động cơ
    uint8_t speed;         // Mức tốc (0-255)
    uint8_t direction;     // Hướng quay (0: ngược, 1: thuận)
  };

//================================================== Khai báo bộ lưu trữ trạng thái 4 động cơ ==================================================

  struct MotorTx {
    uint8_t SPEED[4];      // Mảng tốc độ của 4 động cơ
    uint8_t DIR[4];        // Mảng hướng của 4 động cơ
  } motorTx;
  uint8_t DataTX[4][3];     // Buffer chứa 4 gói, mỗi gói 3 byte

//================================================== Trạng thái điều khiển tăng/giảm tốc ==================================================

  struct MotorSpeedControlUart {
    uint8_t current[4];       // Tốc độ hiện tại của 4 động cơ
    uint8_t duty;             // Tốc độ mục tiêu (duty cycle)   
    uint8_t duty2;            // Tốc độ  xoay mục tiêu (duty cycle)
    uint8_t duty3;            // Tốc độ  xoay mục tiêu (duty cycle)
    uint8_t step_max;         // Bước tăng/giảm tối đa
    uint8_t step_min;         // Bước tăng/giảm tối thiểu
    uint8_t hardLockValue;    // Giá trị tốc độ khi dừng cứng
    uint8_t slowDownStep_max; // Bước giảm tốc tối đa
    uint8_t slowDownStep_min; // Bước giảm tốc tối thiểu
    uint8_t MODEm;            // Chế độ: 1=điều khiển tốc độ, 0=dừng
  } motorSpeedControlUart = {
    {0,0,0,0},                // current[] khởi tạo 0
    0,                        // duty khởi tạo 0 
    0,                        // duty2 khởi tạo 0 
    0xFE,                     // duty3 khởi tạo 254 
    14, 4,                    // step_max=16, step_min=3
    0x00,                     // hardLockValue=2
    14, 3,                    // slowDownStep_max=20, slowDownStep_min=3
    0                         // MODEm=0
  };


//================================================== Cấu hình hướng di chuyển đế Robot : tiến/lùi ==================================================

  struct UartMotorMotionControl {
    uint8_t TienUart[4];    // Mảng hướng tiến
    uint8_t LuiUart[4];     // Mảng hướng lùi
  } UartmotorMotionControl = {
    {0,1,1,0} ,              // Lui: ngược lại
    {1,0,0,1}             // Tiến: động cơ 1,4 quay thuận; 2,3 quay ngược
  };

//================================================== Hàm tính bước thay đổi tốc độ dựa vào hiệu giữa current và duty ==================================================

  uint8_t CalcStep(uint8_t cur, uint8_t duty, uint8_t maxStep, uint8_t minStep) {
    uint8_t diff = (cur > duty) ? cur - duty : duty - cur;  // Hiệu tuyệt đối
    if (diff > 120) return minStep;                         // Nếu chênh >100, bước lớn nhất
    else if (diff > 100) return maxStep/4;                   // Nếu chênh >50, bước giữa
    else if (diff > 80) return maxStep/3;                   // Nếu chênh >50, bước giữa
    else if (diff > 60) return maxStep/2;                   // Nếu chênh >50, bước giữa
    else if (diff > 40) return maxStep;                   // Nếu chênh >50, bước giữa
    else if (diff > 30) return maxStep/2;                   // Nếu chênh >50, bước giữa
    else if (diff > 20) return minStep;                     // Nếu chênh >10, bước nhỏ
    return 1;                                               // Nếu gần bằng, bước 1
  }

  uint8_t CalcStep2(uint8_t cur, uint8_t duty, uint8_t maxStep, uint8_t minStep) {
    uint8_t diff = (cur > duty) ? cur - duty : duty - cur;  // Hiệu tuyệt đối
    if (diff > 65) return maxStep;                         // Nếu chênh >100, bước lớn nhất
    else if (diff > 48) return maxStep/2;                   // Nếu chênh >50, bước giữa
    else if (diff > 24) return maxStep/3;                     // Nếu chênh >10, bước nhỏ
    return 1;                                               // Nếu gần bằng, bước 1
  }

  uint8_t CalcStep3(uint8_t cur, uint8_t duty, uint8_t maxStep, uint8_t minStep) {
    uint8_t diff = (cur > duty) ? cur - duty : duty - cur;  // Hiệu tuyệt đối
     if (diff > 50) return maxStep;                   // Nếu chênh >50, bước giữa
    else if (diff > 40) return maxStep/2;                   // Nếu chênh >50, bước giữa
    else if (diff > 20) return minStep;                     // Nếu chênh >10, bước nhỏ
    return 1;                                               // Nếu gần bằng, bước 1
  }

//================================================== Điều khiển động cơ BLDC ==================================================

  uint8_t speed_esc1 = 60; // Tốc độ cho ESC - BLDC
  uint8_t speed_esc2 = 170; // Tốc độ cho ESC - BLDC
  bool ControlESC = 0;    // Biến bật tắt BLDC

//================================================== Cấu trúc van khí ==================================================

  struct VanControl {
    bool state;               // ON/OFF
  };
  VanControl van[4] = {0};      // Khởi tạo mảng 4 van
  const uint8_t vanPins[4] = {45,46,47,48};

  #define Onvan1  digitalWrite(46,HIGH)
  #define Offvan1 digitalWrite(46,LOW)
  #define Onvan2 digitalWrite(45,HIGH)
  #define Offvan2 digitalWrite(45,LOW)

//================================================== Chiều góc xoay mâm bắn ==================================================

  #define DIRECTION_FORWARD  1
  #define DIRECTION_BACKWARD 0

  int16_t angle = 0;              // Góc sẽ tăng/giảm khi nhấn nút
  uint32_t lastSendTime = 0;      // Thời điểm lần gửi trước
  const uint32_t sendInterval = 20; // Gửi mỗi 100ms

  uint32_t lastTimeUP ;         // Thời điểm nâng lưới trước
  uint32_t lastTimeDown = 0;       // Thời điểm hạ lưới trước
  const uint32_t TimerLuoi = 5000; // Nâng hạ tự động 5s
   
  uint32_t  TimeSTART = 0;         // Thời điểm nâng lưới trước
  

 bool myFlag = 0; // Nâng tối đa
 bool myFlagstart = 0; // Nâng tối đa
 
void handleInterrupt() {
    if (!(PINE & (1 << PE4))) {
    // PE4 đang ở mức LOW
     myFlag = 0;
     myFlagstart = 1;
  }
}

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

  // Khởi tạo van(OFF)
  for (int i = 0; i < 2; i++) {
    pinMode(vanPins[i], OUTPUT);
    digitalWrite(vanPins[i], LOW);
    van[i].state = false;
  }

  // Khởi tạo chân PWM cho động cơ nâng hạ và lô bắn 
  for (int i = 7; i <= 13; i++) {
    pinMode(i, OUTPUT); // Khai báo chân 7 -> 13 là OUTPUT
  }

  pinMode(2, INPUT_PULLUP);  // Chân số 2 làm đầu vào, dùng điện trở kéo lên
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, CHANGE); 

}

void loop() {
  ReadPS();  // Đọc tay cầm 
  Dichuyen(); // Điều khiển đế 
  Prepare_Data(); // Gửi dữ liệu tới PID 

  if (myFlag == 1 && myFlagstart == 0) Ha();

  if(millis() - timer1 < 130 && dang_quay) { TraiTx2(); }
  if(millis() - timer1 > 130 && millis() - timer1 < 1500) { dang_quay = 0;}

  if(buttonState.b0 == 1 && (millis() - TimeSTART > 5000)) {buttonState.b0 = 0;}

  if (buttonState.b0 == 1) Onvan1; 
  else Offvan1;

  //  In hướng và tốc độ từng bánh
    for (int i = 0; i < 4; i++) {
      Serial.print("Motor "); Serial.print(i + 1);
      Serial.print(" - DIR: "); Serial.print(motorTx.DIR[i]);
      Serial.print(" | SPEED: "); Serial.print(motorTx.SPEED[i]);
    }
    
      Serial.print(" | mode: "); Serial.print(myFlag);
      Serial.print(" | mode2: "); Serial.print(myFlagstart);
//      Serial.print(" | modeAUTO: "); Serial.print(AutoMode);
      // Serial.print(" | esc1: "); Serial.print(lastTimeUP);
      // Serial.print(" | esc2: "); Serial.print(speed_esc2);

     printButtonState(buttonState);
}