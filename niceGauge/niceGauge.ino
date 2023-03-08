#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

//............INPUT PINS............switches and buttons
#define THROTTLE 43
#define BRAKE 44
#define LEFT  17  //direction pointer
#define RIGHT 18  //direction ponter
#define SHORT 16  //headlights
#define LONG  21 //headlights light
#define GEARUP 12
#define GEARDOWN 13
#define HORN 10
#define BRIGHTNESS 14

//..........OUTPUT PINS............lights, pointes, horn
#define left_pointer 1
#define right_pointer 11
#define head_lights  2
#define buzzer 3

//......................................colors
#define backColor 0x0026
#define gaugeColor 0x055D
#define dataColor 0x0311
#define purple 0xEA16
#define needleColor 0xF811

//............................dont edit this
int cx=75;
int cy=75;
int r=72;
int ir=70;
int n=0;
int angle=0;

float x[360]; //outer points of Speed gaouges
float y[360];
float px[360]; //ineer point of Speed gaouges
float py[360];
float lx[360]; //text of Speed gaouges
float ly[360];
float nx[360]; //needle low of Speed gaouges
float ny[360];
float x2[360]; //outer points of RPM gaouges
float y2[360];
float px2[360]; //ineer point of RPM gaouges
float py2[360];
float lx2[360]; //text of RPM gaouges
float ly2[360];
float nx2[360]; //needle low of RPM gaouges
float ny2[360];

double rad=0.01745;
unsigned short color1;
unsigned short color2;
float sA;
float rA;
int blinkPeriod=500;
unsigned long currentTimeL=0;
unsigned long currentTimeR=0;
int brightnesses[5]={40,80,120,150,240};
int selectedBrightness=3;
int deb1=0;
int deb2=0;
int debB=0;

int gearMaxSpeed[8]={12,0,60,90,120,150,190,236};
String gears[8]={"R","N","1","2","3","4","5","6"};
int selectedGear=1;

//........................................................colors
unsigned short blockColor[4]={0x0312,0x0290,0x01EC,0x016A};
unsigned short dirColor[2]={0x0312,TFT_ORANGE};
unsigned short lightColor[3]={0x01EC,0x0FA8,0xB79F};

// .........important variables
bool leftPointer=0; 
bool rightPointer=0;
bool braking;
int lights=0;  //0 is lights off, 1 is short light, 2 is long lights
float speedAngle=0; //...speed variable 0-240
float rpmAngle=5;  //.....RPM variable 0-9

void setup() {
  pinMode(THROTTLE,INPUT_PULLUP);
  pinMode(BRAKE,INPUT_PULLUP);
  pinMode(LEFT,INPUT_PULLUP);
  pinMode(RIGHT,INPUT_PULLUP);
  pinMode(GEARUP,INPUT_PULLUP);
  pinMode(GEARDOWN,INPUT_PULLUP);
  pinMode(SHORT,INPUT_PULLUP);
  pinMode(LONG,INPUT_PULLUP);
  pinMode(HORN,INPUT_PULLUP);
  pinMode(BRIGHTNESS,INPUT_PULLUP);

 pinMode(left_pointer,OUTPUT);
 pinMode(right_pointer,OUTPUT);
 pinMode(head_lights,OUTPUT);
 pinMode( buzzer,OUTPUT);


  tft.init();

    tft.setRotation(1);
    tft.fillScreen(backColor);
    sprite.createSprite(320,150);
    sprite.setSwapBytes(true);
    sprite.setTextDatum(4);
    sprite.setTextColor(TFT_WHITE,backColor);
    sprite.setTextDatum(4);

     ledcSetup(0, 10000, 8);
     ledcAttachPin(38, 0);
     ledcWrite(0, brightnesses[selectedBrightness]);  //brightnes of screen

   
      ledcSetup(1, 10000, 8);
     ledcAttachPin(head_lights, 1);
     //ledcWrite(1, 10); 

    

  int a=120;
  for(int i=0;i<360;i++)
    {
       x[i]=((r-10)*cos(rad*a))+cx;
       y[i]=((r-10)*sin(rad*a))+cy;
       px[i]=((r-14)*cos(rad*a))+cx;
       py[i]=((r-14)*sin(rad*a))+cy;
       lx[i]=((r-24)*cos(rad*a))+cx;
       ly[i]=((r-24)*sin(rad*a))+cy;
       nx[i]=((r-36)*cos(rad*a))+cx;
       ny[i]=((r-36)*sin(rad*a))+cy;
       x2[i]=((r-10)*cos(rad*a))+320-cx;
       y2[i]=((r-10)*sin(rad*a))+cy;
       px2[i]=((r-14)*cos(rad*a))+320-cx;
       py2[i]=((r-14)*sin(rad*a))+cy;
       lx2[i]=((r-24)*cos(rad*a))+320-cx;
       ly2[i]=((r-24)*sin(rad*a))+cy;
       nx2[i]=((r-36)*cos(rad*a))+320-cx;
       ny2[i]=((r-36)*sin(rad*a))+cy;

       a++;
       if(a==360)
       a=0;
    }
     
}



void draw()
{
  sprite.fillSprite(backColor);

   for(int i=0;i<4;i++)
   sprite.fillRect(120,28+i*24,80,22,blockColor[i]);  

   for(int i=0;i<selectedBrightness;i++)
   sprite.fillSmoothRoundRect(8+(i*4),6,2,9,1,TFT_ORANGE,backColor);   

   sprite.fillSmoothCircle(cx, cy, r+2, backColor);
   sprite.fillSmoothCircle(320-cx, cy, r+2, backColor);
   sprite.fillTriangle(126,14,136,7,136,21,dirColor[leftPointer]);   //dirction pointers
   sprite.fillRect(136,11,8,7,dirColor[leftPointer]);
   sprite.fillTriangle(126+68,14,136+48,7,136+48,21,dirColor[rightPointer]);
   sprite.fillRect(176,11,8,7,dirColor[rightPointer]);


   for(int i=0;i<5;i++)
   {
   if(i<=2)
   sprite.fillRect(144+(7*i),36,5,5,TFT_WHITE);   /// fuel  rect
   else
   sprite.drawRect(144+(7*i),36,5,5,TFT_WHITE); //empty fuel rect
   }   

    sprite.fillSmoothRoundRect(155,54,9,16,2,TFT_WHITE,blockColor[1]);
    sprite.fillSmoothRoundRect(166,56,2,14,2,TFT_WHITE,blockColor[1]);
    sprite.fillSmoothRoundRect(156,56,7,5,1,blockColor[1],TFT_WHITE);
    sprite.drawLine(153,69,166,69,TFT_WHITE);  //pumpimage
 
  sprite.drawSmoothArc(cx, cy, r, ir, 30, 330, gaugeColor, backColor);
  sprite.drawSmoothArc(320-cx, cy, r, ir, 30, 330, gaugeColor, backColor);

  sprite.drawSmoothArc(cx, cy, r-5, r-6, 30, 330, TFT_WHITE, backColor);
  sprite.drawSmoothArc(320-cx, cy, r-5, r-6, 30, 330, TFT_WHITE, backColor);

  sprite.drawSmoothArc(cx, cy, r-9, r-8, 270, 330, purple, backColor);
  
  sprite.drawSmoothArc(cx, cy, r-38, ir-37, 10, 350, gaugeColor, backColor);
  sprite.drawSmoothArc(320-cx, cy, r-38, ir-37, 10, 350, gaugeColor, backColor);
  
   //.....................................................draw GAUGES
     for(int i=0;i<26;i++){
     if(i<20) {color1=gaugeColor; color2=TFT_WHITE;} else {color1=purple; color2=purple;}

  if(i%2==0) {
  sprite.drawWedgeLine(x[i*12],y[i*12],px[i*12],py[i*12],2,1,color1);
  sprite.setTextColor(color2,backColor);
  sprite.drawString(String(i*10),lx[i*12],ly[i*12]);
  }else
  sprite.drawWedgeLine(x[i*12],y[i*12],px[i*12],py[i*12],1,1,color2);
  }

      for(int i=0;i<19;i++){
      if(i<20) {color1=gaugeColor; color2=TFT_WHITE;} else {color1=purple; color2=purple;}

  if(i%2==0) {
  sprite.drawWedgeLine(x2[i*16],y2[i*16],px2[i*16],py2[i*16],2,1,color1);
  sprite.setTextColor(color2,backColor);
  sprite.drawString(String(i/2),lx2[i*16],ly2[i*16]);
  }else
  sprite.drawWedgeLine(x2[i*16],y2[i*16],px2[i*16],py2[i*16],1,1,color2);
  }

  // ........................................needles draw
   sA=speedAngle*1.2;
   rA=2*rpmAngle*1.6;
   sprite.drawWedgeLine(px[(int)sA],py[(int)sA],nx[(int)sA],ny[(int)sA],2,2,needleColor);
   sprite.drawWedgeLine(px2[(int)rA],py2[(int)rA],nx2[(int)rA],ny2[(int)rA],2,2,needleColor);


  //.....................................drawing  TEXT
  sprite.setTextColor(TFT_WHITE,backColor);
  sprite.drawString(String((int)speedAngle),cx,cy,4);
  sprite.drawString(String(gears[selectedGear]),320-cx,cy,4);
  sprite.drawString("KM/H",cx,cy+16);
  sprite.drawString("GEAR",320-cx,cy+16);
  
  sprite.setTextColor(TFT_WHITE,blockColor[3]);
  sprite.drawString("14356",160,110);

  sprite.setTextColor(TFT_ORANGE,backColor);
  sprite.drawString("x1000",320-cx,136);
  sprite.drawString("RPM",320-cx,126);

  //.....................................drawing BRAKE
  if(braking==true){
  sprite.drawSmoothArc(160, 10, 9, 8, 10, 350, TFT_RED, backColor);
  sprite.drawSmoothArc(160, 10, 12, 11, 50, 130, TFT_RED, backColor);
  sprite.drawSmoothArc(160, 10, 12, 11, 230, 310, TFT_RED, backColor);
  //sprite.drawCircle(320-cx,cy-20,6,TFT_RED);
  sprite.setTextColor(TFT_RED,backColor);
  sprite.drawString("!",160,12,2);}

  //.....................................drawing LIGHTs
  sprite.fillSmoothRoundRect(152,82,14,10,7,lightColor[lights],lightColor[0]);
  sprite.fillRect(161,82,5,10,lightColor[0]);
  sprite.drawLine(163,82,167,84-lights,lightColor[lights]);
  sprite.drawLine(163,85,167,87-lights,lightColor[lights]);
  sprite.drawLine(163,88,167,90-lights,lightColor[lights]);
  sprite.drawLine(163,91,167,93-lights,lightColor[lights]);

   //...........................................draw DOT
     sprite.fillSmoothCircle(300, 10, 4,TFT_RED, backColor);
  
   //..............................................push Sprite to screen  
   sprite.pushSprite(0,10);
  
}

int blinking=1;

void loop() {

  if(digitalRead(SHORT)==0)
  lights=1;
  else if(digitalRead(LONG)==0)
  lights=2;
  else
  lights=0;

  ledcWrite(1, lights*4); 

  if(digitalRead(HORN)==0)
  digitalWrite(buzzer,1);
  else
  digitalWrite(buzzer,0);

  braking=!(digitalRead(BRAKE));



   if(digitalRead(BRIGHTNESS)==0)
  {
  if(debB==0)
    {
      debB=1;
      selectedBrightness++;
      if(selectedBrightness==5) selectedBrightness=0;
      ledcWrite(0, brightnesses[selectedBrightness]);  //brightnes of screen
     }
  }else debB=0;


  if(digitalRead(GEARUP)==0)
  {
  if(deb1==0)
    {
      deb1=1;
      if(selectedGear<7)
      selectedGear++;
      if(speedAngle>10)
      speedAngle=speedAngle-4;
    }
  }else deb1=0;

    if(digitalRead(GEARDOWN)==0)
  {
  if(deb2==0)
    {
      deb2=1;
      if(selectedGear>0)
      selectedGear--;
      if(speedAngle>10)
      speedAngle=speedAngle-4;
    }
  }else deb2=0;

  if(digitalRead(LEFT)==0){
  if(millis()>currentTimeL+blinkPeriod)
  {
    leftPointer=!leftPointer;
    digitalWrite(left_pointer,leftPointer);
    currentTimeL=millis();
  }}else {leftPointer=0; digitalWrite(left_pointer,leftPointer);}

  if(digitalRead(RIGHT)==0){
  if(millis()>currentTimeR+blinkPeriod)
  {
    rightPointer=!rightPointer;
    digitalWrite(right_pointer,rightPointer);
    currentTimeR=millis();
  }}else {rightPointer=0;   digitalWrite(right_pointer,rightPointer);}

   if(braking==true && speedAngle>4)
   speedAngle=speedAngle-4;  

   if(speedAngle<0)
   speedAngle=0;   
   
  
  draw();
  
  if(digitalRead(THROTTLE)==0 && speedAngle<gearMaxSpeed[selectedGear])
  {speedAngle=speedAngle+2-(0.24*selectedGear);} 

  if(digitalRead(THROTTLE)==1 && speedAngle>0)
  speedAngle--; 

   if(digitalRead(THROTTLE)==0 && rpmAngle<75)
  {rpmAngle=rpmAngle+1-(0.1*selectedGear);} 

   if(digitalRead(THROTTLE)==1 && rpmAngle>0)
  { if(rpmAngle>=3)
    rpmAngle=rpmAngle-3;
    else
    rpmAngle=0;
    } 

  

}
