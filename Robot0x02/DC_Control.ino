// Điều tốc BLDC 
void ESC(){
  analogWrite(6, speed_esc1);
  analogWrite(7, 0);
  analogWrite(9, speed_esc2);
  analogWrite(8, 0);
}
void STOP_ESC(){
  analogWrite(6, 0);
  analogWrite(7, 0);
  analogWrite(8, 0);
  analogWrite(9, 0);
}
void Nang(){

  analogWrite(10, 100);
  analogWrite(11, 0);
}
void Ha(){

  analogWrite(10, 0);
  analogWrite(11, 255);
}
void StopNH(){

  analogWrite(11, 0);
  analogWrite(12, 0);
}

void MamTrai(){
  analogWrite(13, 89);
  analogWrite(12, 0);
}

void MamPhai(){
    analogWrite(13, 0);
  analogWrite(12, 89);
}

void MamStop(){
    analogWrite(12, 0);
  analogWrite(13, 0);
}