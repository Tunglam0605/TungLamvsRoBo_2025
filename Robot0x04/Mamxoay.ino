void TUNGLAM_TXtoF4(int16_t angle, uint8_t address) {
  if (address > 127) address = 127;

  uint8_t data[4];
  uint8_t direction = (angle >= 0) ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
  uint16_t abs_angle = (angle >= 0) ? angle : -angle;

  data[0] = (direction << 7) | (address & 0x7F);  // Byte 0: direction + address
  data[1] = abs_angle & 0xFF;                     // Byte 1: low byte
  data[2] = (abs_angle >> 8) & 0xFF;              // Byte 2: high byte
  data[3] = 0xFF;                                 // Byte 3: frame end

  Serial3.write(data, 4);                         // Gửi gói qua Serial1
}