// ==============================
//  TAY CẦM PS2 + HIỂN THỊ LCD + UART
//  Viết cho Arduino + thư viện PS2X
// ==============================

#include <PS2X_lib.h>               // Thư viện tay cầm PS2
#include <SoftwareSerial.h>        // UART mềm để giao tiếp với robot
#include <Wire.h>                  // Giao tiếp I2C với LCD
#include <LiquidCrystal_I2C.h>     // Thư viện LCD I2C

// Khai báo LCD với địa chỉ 0x3F (có thể là 0x27 tùy loại LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Khai báo đối tượng tay cầm
PS2X ps2x;

// UART mềm: TX - D2, RX - D3 (có thể đảo nếu cần)
SoftwareSerial mySerial(2,3);

// Các biến cờ trạng thái
bool controllerConnected = false;         // Cờ kiểm tra kết nối tay cầm2
unsigned long lastReconnectAttempt = 0;   // Thời gian lần cuối thử kết nối lại
bool showJoystick = false;                // Cờ hiển thị trục joystick
bool showName = true;                     // Cờ hiển thị tên robot
bool setupMode = false;                   // Cờ vào chế độ cài đặt địa chỉ robot
uint8_t ROBOT = 0x02;                     // Địa chỉ mặc định của robot (2)

uint8_t LoUP = 0;
uint8_t LoDOWN = 0;

void setup() {
  Serial.begin(115200);        // Serial chính để debug
  mySerial.begin(115200);      // UART mềm để truyền dữ liệu
  lcd.init(); lcd.backlight(); // Khởi tạo LCD và bật đèn nền

  showWelcome();  // Hiển thị tên nhóm khi khởi động

  // Cấu hình các chân tay cầm: CLK, CMD, ATT, DAT
  controllerConnected = (ps2x.config_gamepad(13, 11, 10, 12, false, false) == 0);

  if (controllerConnected) {
    Serial.println("✅ Kết nối tay cầm PS2 thành công!");
    //printControllerType(ps2x.readType());
  } else {
    Serial.println("❌ Không tìm thấy tay cầm PS2.");
  }
}

void loop() {
  ps2x.read_gamepad(); // Đọc dữ liệu từ tay cầm
  
  if(ROBOT == 0x01){
    if(ps2x.ButtonPressed(PSB_PAD_UP)) {
        if ( LoUP <= 25 ) LoUP+= 1;
        else LoUP = 26;
      }
    if(ps2x.ButtonPressed(PSB_PAD_DOWN)) {
        if ( LoUP >= 1 ) LoUP-= 1;
        else LoUP = 0;
      }
    if(ps2x.ButtonPressed(PSB_L1)) {
        if ( LoDOWN <= 25 ) LoDOWN+= 1;
        else LoDOWN = 26;
      }
    if(ps2x.ButtonPressed(PSB_L2)) {
        if ( LoDOWN >= 1 ) LoDOWN-= 1;
        else LoDOWN = 0;
      }
    if (ps2x.ButtonPressed(PSB_CROSS)){
        LoUP = 20;
        LoDOWN = 22;
      }
    }

  else if(ROBOT == 0x02){
    if(ps2x.ButtonPressed(PSB_R1)) {
        if ( LoUP <= 25 ) LoUP+= 1;
        else LoUP = 26;
      }
      if(ps2x.ButtonPressed(PSB_R2)) {
        if ( LoUP >= 1 ) LoUP-= 1;
        else LoUP = 0;
      }
      if(ps2x.ButtonPressed(PSB_L1)) {
        if ( LoDOWN <= 25 ) LoDOWN+= 1;
        else LoDOWN = 26;
      }
      if(ps2x.ButtonPressed(PSB_L2)) {
        if ( LoDOWN >= 1 ) LoDOWN-= 1;
        else LoDOWN = 0;
      }

      if (ps2x.ButtonPressed(PSB_CROSS)){
        LoUP = 17;
        LoDOWN = 6;
      }
    }
  else if(ROBOT == 0x03){
    if(ps2x.ButtonPressed(PSB_PAD_UP)) {
        if ( LoUP <= 25 ) LoUP+= 1;
        else LoUP = 26;
      }
      if(ps2x.ButtonPressed(PSB_PAD_DOWN)) {
        if ( LoUP >= 1 ) LoUP-= 1;
        else LoUP = 0;
      }

      if(ps2x.ButtonPressed(PSB_L1)) {
        if ( LoDOWN <= 25 ) LoDOWN+= 1;
        else LoDOWN = 26;
      }
      if(ps2x.ButtonPressed(PSB_L2)) {
        if ( LoDOWN >= 1 ) LoDOWN-= 1;
        else LoDOWN = 0;
      }

      if (ps2x.ButtonPressed(PSB_CROSS)){
        LoUP = 20;
        LoDOWN = 17;
      }
    }
  else if(ROBOT == 0x04){
    if(ps2x.ButtonPressed(PSB_R1)) {
        if ( LoUP <= 25 ) LoUP+= 1;
        else LoUP = 26;
      }
      if(ps2x.ButtonPressed(PSB_R2)) {
        if ( LoUP >= 1 ) LoUP-= 1;
        else LoUP = 0;
      }
      if(ps2x.ButtonPressed(PSB_L1)) {
        if ( LoDOWN <= 25 ) LoDOWN+= 1;
        else LoDOWN = 26;
      }
      if(ps2x.ButtonPressed(PSB_L2)) {
        if ( LoDOWN >= 1 ) LoDOWN-= 1;
        else LoDOWN = 0;
      }

      if (ps2x.ButtonPressed(PSB_CROSS)){
        LoUP = 9;
        LoDOWN = 11;
      }
    }
  // Nếu ấn tổ hợp 4 nút vai và nút X thì vào chế độ setup
  if (ps2x.Button(PSB_L1) && ps2x.Button(PSB_L2) && ps2x.Button(PSB_R1) && ps2x.Button(PSB_R2) && ps2x.ButtonPressed(PSB_CROSS)) {
    setupMode = true;
  }

  // Nếu đang ở chế độ setup, cho phép chỉnh địa chỉ ROBOT bằng mũi tên trái/phải
  if (setupMode) {
    updateRobotAddress();
    if (ps2x.ButtonPressed(PSB_START)) setupMode = false; // Thoát setup bằng nút START
    return; // Không xử lý phần còn lại
  }

  // Toggle giữa hiển thị tên và joystick khi nhấn L3
  if (ps2x.ButtonPressed(PSB_L3)) {
    showJoystick = !showJoystick;
    showName = !showJoystick;
  }

  // Hiển thị dữ liệu joystick hoặc tên robot
  if (showJoystick) {
    showWelcome();
  } else if (showName) {
    displayJoystickData();
  }

  // Nếu tay cầm kết nối OK thì đóng gói và gửi dữ liệu
  if (controllerConnected) {
    byte packet[8];
    packet[0] = 0xAA;                // Byte bắt đầu khung
    packet[1] = ROBOT;               // Địa chỉ robot
    packet[2] = ps2x.Analog(PSS_LX); // Giá trị trục trái X
    packet[3] = ps2x.Analog(PSS_LY); // Giá trị trục trái Y
    packet[4] = ps2x.Analog(PSS_RX); // Giá trị trục phải X
    packet[5] = ps2x.Analog(PSS_RY); // Giá trị trục phải Y

    packet[6] = getButtonByte1();    // Byte các nút mặt trước
    packet[7] = getButtonByte2();    // Byte các nút vai và điều hướng

    mySerial.write(packet, 8);       // Gửi toàn bộ packet qua UART
    //delay(10); // Giữ tần số gửi ~50Hz

    // Nếu cả 4 trục đều là 0xFF thì có thể đã mất kết nối tay cầm
    if (packet[2] == 0xFF && packet[3] == 0xFF && packet[4] == 0xFF && packet[5] == 0xFF) {
      controllerConnected = false;
      //Serial.println("⚠️ Tay cầm PS2 mất kết nối!");
      lastReconnectAttempt = millis();
    }
  }

  // Nếu tay cầm đã mất kết nối, thử kết nối lại mỗi 500ms
  if (!controllerConnected && millis() - lastReconnectAttempt > 500) {
    //Serial.println("🔄 Đang thử kết nối lại tay cầm...");
    controllerConnected = (ps2x.config_gamepad(13, 11, 10, 12, false, false) == 0);
    if (controllerConnected) {
      //Serial.println("✅ Đã khôi phục kết nối tay cầm!");
      //printControllerType(ps2x.readType());
    } else {
      //Serial.println("❌ Kết nối lại thất bại.");
    }
    lastReconnectAttempt = millis();
  }
}

// -----------------------------
// HIỂN THỊ TÊN ROBOT TRÊN LCD
// -----------------------------
void showWelcome() {
  lcd.setCursor(0, 0);
  lcd.print("/UNETI DI-TDH 02");
  lcd.setCursor(0, 1);
  if(ROBOT == 0x04)      lcd.print(">>  Minh Dat  <<");
  else if(ROBOT == 0x03) lcd.print(">>  Tung Lam  <<");
  else if(ROBOT == 0x01)   lcd.print(">>  Quan HiHi <<");
  else if(ROBOT == 0x02)   lcd.print(">>Dog Quan K18<<");
}

// -----------------------------
// CHẾ ĐỘ CÀI ĐẶT ĐỊA CHỈ ROBOT
// -----------------------------
void updateRobotAddress() {
  if (ps2x.ButtonPressed(PSB_PAD_LEFT)) {
    ROBOT = (ROBOT > 0) ? ROBOT - 1 : 1;
  }
  if (ps2x.ButtonPressed(PSB_PAD_RIGHT)) {
    ROBOT = (ROBOT < 4) ? ROBOT + 1 : 4;
  }

  lcd.setCursor(0, 0);
  lcd.print(">> ROBOT : ");
  lcd.print(ROBOT);
  lcd.print("        "); // Xóa phần thừa
  lcd.setCursor(0, 1);
  if(ROBOT == 0x04) lcd.print(">>  Minh Dat  <<");
  else if(ROBOT == 0x03) lcd.print(">>  Tung Lam  <<");
  else if(ROBOT == 0x01)   lcd.print(">>  Quan HiHi <<");
  else if(ROBOT == 0x02)   lcd.print(">>Dog Quan K18<<");
}

// -----------------------------
// HIỂN THỊ DỮ LIỆU JOYSTICK
// -----------------------------
void displayJoystickData() {
  lcd.setCursor(0, 0);
  lcd.print(">> ROBOT : ");
  lcd.print(ROBOT);
  lcd.print("        ");

  lcd.setCursor(0, 1);
  lcd.print(">> Lo1 ");
  lcd.print(LoUP);
  lcd.print("  ");
  lcd.setCursor(10, 1);
  lcd.print("Lo2 ");
  lcd.print(LoDOWN);  
  lcd.print("  ");
}

// -----------------------------
// HIỂN THỊ LOẠI TAY CẦM
// -----------------------------
void printControllerType(byte type) {
  // switch (type) {
  //   case 0: Serial.println("Không nhận dạng được loại tay cầm."); break;
  //   case 1: Serial.println("Đã nhận dạng tay cầm: DualShock"); break;
  //   case 2: Serial.println("Đã nhận dạng tay cầm: GuitarHero"); break;
  //   case 3: Serial.println("Đã nhận dạng tay cầm không dây của Sony"); break;
  // }
}

// -----------------------------
// GÓI NÚT MẶT TRƯỚC VÀ D-PAD
// -----------------------------
byte getButtonByte1() {
  byte b = 0;

  if(ROBOT == 0x02 || ROBOT == 0x04){
  b |= ps2x.ButtonPressed(PSB_CIRCLE)   << 0;
  b |= ps2x.ButtonPressed(PSB_CROSS)    << 1;
  b |= ps2x.ButtonPressed(PSB_SQUARE)   << 2;
  b |= ps2x.ButtonPressed(PSB_TRIANGLE) << 3;
  b |= ps2x.Button(PSB_PAD_UP)          << 4;
  b |= ps2x.Button(PSB_PAD_DOWN)        << 5;
  b |= ps2x.Button(PSB_PAD_LEFT)        << 6;
  b |= ps2x.Button(PSB_PAD_RIGHT)       << 7;

  }
  if(ROBOT == 0x01 || ROBOT == 0x03){
  b |= ps2x.ButtonPressed(PSB_CIRCLE)   << 0;
  b |= ps2x.ButtonPressed(PSB_CROSS)    << 1;
  b |= ps2x.ButtonPressed(PSB_SQUARE)   << 2;
  b |= ps2x.ButtonPressed(PSB_TRIANGLE) << 3;
  b |= ps2x.ButtonPressed(PSB_PAD_UP)   << 4;
  b |= ps2x.ButtonPressed(PSB_PAD_DOWN) << 5;
  b |= ps2x.Button(PSB_PAD_LEFT)        << 6;
  b |= ps2x.Button(PSB_PAD_RIGHT)       << 7;
 
  }

  return b;
}

// -----------------------------
// GÓI NÚT VAI VÀ CHỨC NĂNG
// -----------------------------
byte getButtonByte2() {
  byte b = 0;
  b |= ps2x.ButtonPressed(PSB_R1)     << 0;
  b |= ps2x.ButtonPressed(PSB_R2)     << 1;
  b |= ps2x.ButtonPressed(PSB_R3)     << 2;
  b |= ps2x.ButtonPressed(PSB_L1)     << 3;
  b |= ps2x.ButtonPressed(PSB_L2)     << 4;
  b |= ps2x.ButtonPressed(PSB_L3)     << 5;
  b |= ps2x.ButtonPressed(PSB_SELECT) << 6;
  b |= ps2x.ButtonPressed(PSB_START)  << 7;


  return b;
}
