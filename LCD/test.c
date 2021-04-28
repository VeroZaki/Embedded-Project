#include <reg51.h>

unsigned char Password[4];

sbit r1 = P1^0;
sbit r2 = P1^1;
sbit r3 = P1^2;
sbit r4 = P1^3;

sbit c1 = P1^4;
sbit c2 = P1^5;
sbit c3 = P1^6;

sbit Red = P3^3;
sbit Green = P3^4;

sbit rs = P3^5;
sbit rw = P3^6;
sbit en = P3^7;

sfr lcd = 0xA0;


unsigned char count =0;
unsigned char x = 0;
unsigned char Max = 3;
unsigned char Repeat = 4;
void lcddisplay(unsigned char *s , unsigned char r , unsigned char flag);
void delay(unsigned int time);
void lcdcommand(unsigned char val);
void check();
void lcddat(unsigned char dat);
void lcddat2(unsigned char dat);

void delay(unsigned int time){  
	unsigned i,j;
	for(i=0;i<time;i++)
	for(j=0;j<1275;j++);
}

void ToDelay(unsigned char loop){ 
	unsigned char i;
	for(i=0;i<loop;i++){
		TMOD=0x01;
		TL0=0xFE;
		TH0=0xA5;
		TR0=1;
		while(TF0==0);
		TR0=0;
		TF0=0;
	}
}

void lcdcommand(unsigned char vale){
	lcd = vale;
	rs = 0;
	rw = 0;
	en = 1;
	ToDelay(1);
	en = 0;
}

void lcddisplay(unsigned char *s , unsigned char r , unsigned char flag){  
	if(flag == 0){
		unsigned char w;
		for(w=0;w<r;w++){
			lcddat(s[w]);
			ToDelay(1);
		}
  }
	else{
		unsigned char z;
		for(z=0;z<r;z++){
			lcddat2(s[z]);
			ToDelay(1);
		}
	}
}

void SerialCommunication(unsigned char *value , unsigned char length){ 
	unsigned char k;
	SCON = 0x50;
	TMOD = 0X20;
	TH1 = -3;       
	TR1 = 1;
	for(k=0;k<length;k++){
		SBUF = (value[k]);
		delay(1);
	}
	while(TI==0);
	TI = 0;
}

void Serial(unsigned char value){   
	SCON = 0x50;
	TMOD = 0X20;
	TH1 = -3;      
	TR1 = 1;
	SBUF = value;
	while(TI==0);
	TI = 0;
}

void CountDown(unsigned char disp , unsigned char disp2){  
	unsigned char i , j;
	for(j=disp2;j>=48;j--){
		lcdcommand(0xC8);
		lcddat2(j);
		for(i=disp;i>=48;i--){
			lcdcommand(0xC9);
			lcddat2(i);
			Serial(j);
			Serial(i);
			SerialCommunication("\r" , 1);
			ToDelay(40);
	  }
  }
	lcdcommand(0x01);
}

void check(){   
	if(Password[0]=='2' && Password[1]=='3' && Password[2]=='4' && Password[3]=='5'){
		lcdcommand(0x01);
		lcddisplay("Access Granted",14,0);
		Green = 1;
		SerialCommunication("Access Granted\r" , 14);
		
	}
	else{
		lcdcommand(0x01);
		lcddisplay("Access Denied",13,1);
		Max = Max - 1;
		Red = 1;
		SerialCommunication("Access Denied\r" , 14);
		Red = 0;
		ToDelay(20);
		lcdcommand(0x01);
		if(Max == 2){
		 lcddisplay("Two chances",11,1);
		 SerialCommunication("Two chances\r" , 12);	
		}
		else if(Max == 1){
			lcddisplay("One chance",10,1);
			SerialCommunication("One chance\r" , 11);	
		}
		ToDelay(20);
		lcdcommand(0x01);
		if(Max == 0){
			Repeat = Repeat -1;
			lcddisplay("Access Denied!!",15,1);
			SerialCommunication("Access Denied!!\r" , 16);	
			Red = 1;
			ToDelay(2);
			lcdcommand(0x01);
			if(Repeat == 3){
			  lcddisplay("Wait for 10 sec",15,1);
				SerialCommunication("Wait for 10 sec\r" , 16);
				ToDelay(2);
			  CountDown(57,48);                       
		  }
			else if(Repeat == 2){
				lcddisplay("Wait for 30 sec",15,1);
				SerialCommunication("Wait for 30 sec\r" , 16);
				ToDelay(2);
			  CountDown(57,50);                      
			}
			else if(Repeat == 1){
				lcddisplay("Wait for 50 sec",15,1);
				SerialCommunication("Wait for 50 sec\r" , 16);
				ToDelay(1);
			  CountDown(57,52);                      
			}
			else if(Repeat == 0){
				lcddisplay("Refused !!!",11,1);
				SerialCommunication("Refused !!!\r" , 12);
				return ;
			}
			Max=3;
			Red=0;
		}
		
		lcddisplay("Enter the pin:",14,1);
		lcdcommand(0xC0);
		x=0;
		count =0;
	}
}
void lcddat(unsigned char dat){      
	lcd = dat;
	rs = 1;
	rw = 0;
	en = 1;
	ToDelay(1);
	en=0;
	Password[x] = dat;
	x = x + 1;
	count = count + 1;
	if(count == 4){
		ToDelay(20);
		check();
	}
}

void lcddat2(unsigned char dat){
	lcd = dat;
	rs = 1;
	rw = 0;
	en = 1;
	ToDelay(1);
	en=0;
	
}

void main(){
	Red = 0;
	Green = 0;
	lcd = 0x00;
	lcdcommand(0x38);
	ToDelay(1);
	lcdcommand(0x0E);
	ToDelay(1);
	lcdcommand(0x01);
	ToDelay(1);
	lcdcommand(0x0C);
	ToDelay(1);
	lcdcommand(0x80);  
	ToDelay(1);
	lcddisplay("Enter the pin:",14,1);
	ToDelay(10);
	lcdcommand(0xC0);
	ToDelay(10);
	while(1){
		r1 = 0; r2=1; r3=1; r4=1;
		if(c1 == 0){
			lcddat('1');  
			ToDelay(10);   
		}
		if(c2 == 0){
			lcddat('2');
			ToDelay(10);
		}
		if(c3 == 0){
			lcddat('3');
			ToDelay(10);
		}
		r1=1; r2=0;
		if(c1 == 0){
			lcddat('4');
			ToDelay(10);
		}
		if(c2 == 0){
			lcddat('5');
			ToDelay(10);
		}
		if(c3 == 0){
			lcddat('6');
			ToDelay(10);
		}
		r2=1; r3=0;
		if(c1 == 0){
			lcddat('7');
			ToDelay(10);
		}
		if(c2 == 0){
			lcddat('8');
			ToDelay(10);
		}
		if(c3 == 0){
			lcddat('9');
			ToDelay(10);
		}
		r3=1; r4=0;
		if(c1 == 0){
			lcddat('*');
			ToDelay(10);
		}
		if(c2 == 0){
			lcddat('0');
			ToDelay(10);
		}
		if(c3 == 0){ 
			lcddat('#');
			ToDelay(10);
		}
		r4=1;
	}
	
}


