// Chọn trạng thái Joy để di chuyển 
void Dichuyen(){
  Joystick();
	if (  StateJoys.JOYPHAI[4] == 1 &&  StateJoys.JOYTRAI[4] == 1)	{ StopUart();}
	
// Just JoyLeft
	if (  StateJoys.JOYPHAI[4] == 1 ){
		
		if ( StateJoys.JOYTRAI[0] == 1 ){ TienTx(); }	

		if ( StateJoys.JOYTRAI[2] == 1 ){ LuiTx(); }
		
		if ( StateJoys.JOYTRAI[1] == 1 ){ N_PhaiTx();}
		
		if ( StateJoys.JOYTRAI[3] == 1 ){ N_TraiTx(); }
	}
// Just JoyRight		
	if (  StateJoys.JOYTRAI[4] == 1 ){
		
		if ( StateJoys.JOYPHAI[0] == 1 ){ TienTx(); }
		
		if ( StateJoys.JOYPHAI[2] == 1 ){ LuiTx(); }
		
		if ( StateJoys.JOYPHAI[3] == 1 ){ PhaiTx(); }
		
		if ( StateJoys.JOYPHAI[1] == 1 ){ TraiTx(); }
	}
	
// cua tien	
	else if ( StateJoys.JOYPHAI[3] == 1  && StateJoys.JOYTRAI[0] == 1 )
    { TienTx3();}
	
	else if ( StateJoys.JOYPHAI[1] == 1  && StateJoys.JOYTRAI[0] == 1 )
    { TienTx2(); } 
	
// cua lui	
	else if ( StateJoys.JOYPHAI[1] == 1  && StateJoys.JOYTRAI[2] == 1 )
    { LuiTx2(); }
	
	else if ( StateJoys.JOYPHAI[3] == 1  && StateJoys.JOYTRAI[2] == 1 )
    { LuiTx3(); } 	
} 