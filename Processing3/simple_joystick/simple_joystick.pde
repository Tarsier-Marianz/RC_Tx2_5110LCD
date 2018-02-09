import processing.opengl.*;
import processing.serial.*;
Serial serial;

String dir ="";
int oldX, oldY;
int x;   
int y;  
int z;
int b;
int step= 5; //the bigger the value the faster its movement
int bigSize = 50;
int smallSize = 25;

boolean enablePath = true;

void setup() {
  PFont font;
  size(600, 600, OPENGL);
  smooth();
  font =createFont("verdana", 14); // loadFont("Monospaced.bold-12.vlw");
  textFont(font);

  x=  (width/2)-25;
  y=  height/2;
  println(Serial.list()); // Use this to print connected serial devices
  //serial = new Serial(this, Serial.list()[0], 115200); // Set this to your serial port obtained using the line above
  serial = new Serial(this, "COM5", 9600); // Set this to your serial port obtained using the line above
  serial.bufferUntil('\n'); // Buffer until line feed
}
void draw()
{
  fill(0);
  clear();
  smooth();
  noStroke();


  if (b == 1) {
    fill(0, z, 0); 
    ellipse(x, y, bigSize, bigSize);
  } else {
    fill(255); 
    ellipse(x, y, smallSize, smallSize);
  }  
  if(enablePath){
    strokeWeight(2);
    stroke(255,0,0,100);
    line(oldX, oldY, x,y);
  }    
  oldX = x;
  oldY = y;
  fill(0, 255, 0); 
  text("X: "+(x), 10, 20);
  text("Y: "+(y), 10, 40);
  text("Z: "+z, 10, 60);
  text("Dir: "+dir, 10, 80);
  text("Button: "+b, 10, 100);
}



void serialEvent (Serial serial) {

  try {
    String data = serial.readStringUntil('\n'); 
    if (data != null) {
      //print(data);
      String[] list = split(data, '\t');
      for (int i=0; i < list.length; i++) {
        if (i==0) {
          command(trim(list[i]));
        } else if (i==1) {
          z = int(trim(list[i]));
        } else {
          b = int(trim(list[i]));
        }
      }
    }
  }
  catch (Exception e) {
    println(e);
    //closePort();
  }
  finally {
    serial.clear(); // Clear buffer
  }
}

void command(String cmd) {
  if (cmd != null) {
    dir= cmd;
    if (cmd.equals("F") == true) {
      y-= step;
    } else if (cmd.equals("B") == true) {
      y+= step;
    } else if (cmd.equals("L")  == true) {
      x-= step;
    } else if (cmd.equals("R") == true) {
      x+= step;
    } else if (cmd.equals("FR")  == true) {
      y-= step;
      x+= step;
    } else if (cmd.equals("FL") == true) {
      y-= step;
      x-= step;
    } else if (cmd.equals( "BL") == true) {
      y+= step;
      x-= step;
    } else if (cmd.equals("BR")  == true) {
      y+= step;
      x+= step;
    } else {
      //x=  width/2;
      //y=  height/2;
    }
  }
}
void mouseWheel(MouseEvent event) {
  float e = event.getCount();
  if (event.isShiftDown()) {
  } else {
  }
}

void stop() {
  closePort();
}
void closePort() {
  serial.stop();
  serial = null;
}