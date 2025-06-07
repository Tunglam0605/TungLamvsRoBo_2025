// Điều tốc BLDC 
void ESC(){
  analogWrite(6, speed_esc1);
  analogWrite(7, 0);
  analogWrite(8, speed_esc2);
  analogWrite(9, 0);
}
void STOP_ESC(){
  analogWrite(6, 0);
  analogWrite(7, 0);
  analogWrite(8, 0);
  analogWrite(9, 0);
}
void Nang(){

  analogWrite(12, 150);
  analogWrite(13, 0);
}
void Ha(){

  analogWrite(12, 0);
  analogWrite(13, 150);
}
void StopNH(){

  analogWrite(12, 0);
  analogWrite(13, 0);
}

void MamTrai(){
    analogWrite(10, 160);
  analogWrite(11, 0);
}

void MamPhai(){
    analogWrite(10, 0);
  analogWrite(11, 160);
}

void MamStop(){
    analogWrite(10, 0);
  analogWrite(11, 0);
}