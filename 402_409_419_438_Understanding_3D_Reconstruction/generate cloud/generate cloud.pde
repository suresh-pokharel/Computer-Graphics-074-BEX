import org.openkinect.freenect.*;
import org.openkinect.processing.*;

// Kinect Library object
Kinect kinect;

Table table;
boolean runcode = false;
boolean neg = false;
// Angle for rotation
float a = 0;
int ang = 0;

// We'll use a lookup table so that we don't have to repeat the math over and over
float[] depthLookUp = new float[2048];

void setup() {
   //Rendering in P3D
   size(800, 600, P3D);
   kinect = new Kinect(this);
   kinect.initDepth();
   kinect.initVideo();
   kinect.enableColorDepth(true);
  
   table = new Table();
  
   table.addColumn("x");
   table.addColumn("y");
   table.addColumn("z");
   table.addColumn("r");
   table.addColumn("g");
   table.addColumn("b");

  // Lookup table for all possible depth values (0 - 2047)
   for (int i = 0; i < depthLookUp.length; i++) {
      depthLookUp[i] = rawDepthToMeters(i);
   }
}

void draw() {
    if(runcode){
    background(0);

  // Get the raw depth as array of integers
    int[] depth = kinect.getRawDepth();
    PImage img = kinect.getVideoImage();
  // We're just going to calculate and draw every 3rd pixel
    int skip = 3;

  // Translate and rotate
    translate(width/2, height/2, -50);
  //rotateY(a);//radians(0));

    for (int x = 0; x < kinect.width; x += skip) {
      for (int y = 0; y < kinect.height; y += skip) {
        int offset = x + y*kinect.width;

      // Convert kinect data to camera xyz coordinate
        int rawDepth = depth[offset];
        PVector u = depthToWorld(x, y, rawDepth);
        PVector test = WorldToColor(u);
      
        PVector v;
        if(ang!=0)
          v = rotatey(u, ang);
        else  v = u;
     
      if(v.z>0.45 && v.z < 0.75){
        if(v.x > -0.3 && v.x < 0.3){
          if(v.y > -0.08 && v.y < 0.07){
            int l = img.get((int)test.x,(int)test.y);
            int red = (l >> 16) & 0xff;
            int green = (l >> 8) & 0xff;
            int blue = l & 0xff;
  
            TableRow newRow = table.addRow();
            newRow.setFloat("x",v.x);
            newRow.setFloat("y",v.y);
            newRow.setFloat("z",v.z);
            newRow.setFloat("r",red);
            newRow.setFloat("b",blue);
            newRow.setFloat("g",green);
         
            stroke(red,green,blue);
          //stroke(255);
            pushMatrix();
      
         // Scale up by 400
            float factor = 400;
      
            translate(v.x*factor, v.y*factor, factor-v.z*factor);
         // Draw a point
            point(0, 0);
       
            popMatrix();
          }
        }
      }
    }
  }
  a += 0.015f;
 
  saveTable(table,"data/point.csv");
  runcode = false;
  }
}

PVector rotatey(PVector p, float ang) {
  println(ang);
  float rad = radians(ang);
  PVector Q = new PVector();
  p.z = p.z-0.56;//0.54;//0.6;
  Q.x = ((p.x*cos(rad))+(p.z*sin(rad)));
  Q.z = ((p.z*cos(rad))-(p.x*sin(rad)));
  Q.y = p.y;
  Q.z = Q.z+0.56;//0.54;//0.6;
  return Q;
}

void keyPressed() {
  if (key == 'i') {
    runcode = true;
  }
  else if (key == 'r'){
    ang = 0;
    println(ang);
  }
  else{
    ang = ang*10+(key-48);
    println(ang);
  }
}

// These functions come from: http://graphics.stanford.edu/~mdfisher/Kinect.html
float rawDepthToMeters(int depthValue) {
  if (depthValue < 2047) {
    return (float)(1.0 / ((double)(depthValue) * -0.0030711016 + 3.3309495161));
  }
  return 0.0f;
}

PVector depthToWorld(int x, int y, int depthValue) {

  final double fx_d = 1.0 / 5.9421434211923247e+02;
  final double fy_d = 1.0 / 5.9104053696870778e+02;
  final double cx_d = 3.3930780975300314e+02;
  final double cy_d = 2.4273913761751615e+02;

  PVector result = new PVector();
  double depth =  depthLookUp[depthValue];//rawDepthToMeters(depthValue);
  result.x = (float)((x - cx_d) * depth * fx_d);
  result.y = (float)((y - cy_d) * depth * fy_d);
  result.z = (float)(depth);
  return result;
}

PVector WorldToColor(PVector point)
{
    final PVector row1 = new PVector(9.9984628826577793e-01f, 1.2635359098409581e-03f, -1.7487233004436643e-02f);
    final PVector row2 = new PVector(-1.4779096108364480e-03f, 9.9992385683542895e-01f, -1.2251380107679535e-02f);
    final PVector row3 = new PVector(1.7470421412464927e-02f, 1.2275341476520762e-02f, 9.9977202419716948e-01f);
    final PVector translation = new PVector(1.9985242312092553e-02f, -7.4423738761617583e-04f, -1.0916736334336222e-02f);
    
    PVector transformed = new PVector();
    transformed.x = (row1.x*point.x)+(row1.y*point.y)+(row1.z*point.z)+translation.x;
    transformed.y = (row2.x*point.x)+(row2.y*point.y)+(row2.z*point.z)+translation.y;
    transformed.z = (row3.x*point.x)+(row3.y*point.y)+(row3.z*point.z)+translation.z;
    
    final float fx_rgb = 5.2921508098293293e+02;
    final float fy_rgb = 5.2556393630057437e+02;
    final float cx_rgb = 3.2894272028759258e+02;
    final float cy_rgb = 2.6748068171871557e+02;

    PVector result = new PVector();
    result.x = ((transformed.x*fx_rgb)/transformed.z + cx_rgb);
    result.y = ((transformed.y*fy_rgb)/transformed.z + cy_rgb);
    result.x = constrain(result.x, 0, 639);
    result.y = constrain(result.y, 0, 479);
    return result;
}
