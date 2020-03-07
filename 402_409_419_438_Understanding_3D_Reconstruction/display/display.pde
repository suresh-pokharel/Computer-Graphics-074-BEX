import peasy.*;


Table table;
float a;
PeasyCam camera;
void setup() {
 // Rendering in P3D
  size(800, 600, P3D);
  //rotateY(-30*(3.1415/180));
  table = loadTable("E:/Processing/graphics/data/CUBE.csv","header");
  camera = new PeasyCam(this, 400, 300, 100, 100);

 

}

void draw(){
    background(0);
    translate(width/2, height/2, -50);
    //rotateY(radians(-30));
    //rotateY(a);
  stroke(90,240,130);
    point(0, 0, 0.6*400);
    //line(0,0,0.5*400, 0,0,0.54*400);
    //line(0,0,*400, 0,0,0.53*400);
     for (TableRow row : table.rows()) {
  stroke(row.getFloat("r"),row.getFloat("g"),row.getFloat("b"));
      //stroke(255);
      pushMatrix();
      //rotateY(radians(90));
      // Scale up by 400
      float factor = 400;
      translate(row.getFloat("x")*factor, row.getFloat("y")*factor, row.getFloat("z")*factor);
      // Draw a point
      point(0, 0);
      popMatrix();
  }
  a += 0.015f;
}
