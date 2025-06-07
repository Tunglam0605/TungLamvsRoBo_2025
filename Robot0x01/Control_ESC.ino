// Điều tốc BLDC 
void ESC(){
  analogWrite(7, speed_esc1);
  analogWrite(9, speed_esc2);
}
void STOP_ESC(){
  analogWrite(7, 0);
  analogWrite(9, 0);
}

void MamTrai(){
  for(int i= 0; i<= 100;i++){
    analogWrite(10, i);
  }
  analogWrite(11, 0);
}

void MamPhai(){
  analogWrite(10, 0);
  for(int i= 0; i<= 100;i++){
    analogWrite(11, i);
  }
}

void MamStop(){
  analogWrite(10, 0);
  analogWrite(11, 0);
}