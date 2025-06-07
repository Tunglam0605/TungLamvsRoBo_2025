// Chuẩn bị và gửi gói UART cho 4 động cơ Planet
void Prepare_Data() {
  // Đóng gói thông tin từng động cơ
  TxSendPID motors[4] = {
    {1, motorTx.SPEED[0], motorTx.DIR[0]},
    {2, motorTx.SPEED[1], motorTx.DIR[1]},
    {3, motorTx.SPEED[2], motorTx.DIR[2]},
    {4, motorTx.SPEED[3], motorTx.DIR[3]}
  };
  // Duyệt từng motor, tạo frame, gửi và in debug
  for (int i = 0; i < 4; i++) {
    DataTX[i][0] = (motors[i].direction << 7) | (motors[i].address & 0x7F);
    DataTX[i][1] = motors[i].speed;
    DataTX[i][2] = 0xFF;
    // Gửi lên driver
    Serial2.write(DataTX[i], 3);
  }
}
