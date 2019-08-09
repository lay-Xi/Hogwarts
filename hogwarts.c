/*
 * Hogwarts - Final Project
 * Author - Xi Gao
 */

#include "CSCIx229.h"

int axes=0;               //  Display axes
int text=1;               //  Toggles texture mode modulate/replace
int view=1;               //  Toggle projections
int th=35;                //  Azimuth of view angle
int ph=30;                //  Elevation of view angle
int fov=112;              //  Field of view (for perspective)
int light=1;              //  Lighting
double asp=1;             //  Aspect ratio
double dim=4.0;           //  Size of world
unsigned int texture[8];  //  Texture names
int fog=1;                //  Fog


// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int emission  =   0;  // Emission intensity (%)
int ambient   =  15;  // Ambient intensity (%)
int diffuse   =  80;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  = 0.0;  // Elevation of light
int move      =   1;  // Move light
float dayNight=   0;  // Day/Night


// Draw vertex in polar coordinates
static void Vertex(double th,double ph,double r,double x,double y,double z) {
        glNormal3d(Sin(th)*Cos(ph)*r+x, Sin(ph)*r+y, Cos(th)*Cos(ph)*r+z);
        glTexCoord3d(Sin(th)*Cos(ph)*r+x, Sin(ph)*r+y, Cos(th)*Cos(ph)*r+z);
        glVertex3d(Sin(th)*Cos(ph)*r+x, Sin(ph)*r+y, Cos(th)*Cos(ph)*r+z);
}


// Rectangular ground
static void recGround(double width,double len,double x,double y,double z) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[3]);

        // Floor
        glBegin(GL_QUADS);
        glNormal3d(0, -1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(width, 0, len/2);
        glTexCoord2f(1, 0);
        glVertex3d(-width, 0, len/2);
        glTexCoord2f(1, 1);
        glVertex3d(-width, 0, -len/2);
        glTexCoord2f(0, 1);
        glVertex3d(width, 0, -len/2);
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Draw a ball for lighting at (x,y,z) with radius (r)
static void ball(double x,double y,double z,double r) {
        int th,ph;
        float yellow[] = {1.0,1.0,0.0,1.0};
        float Emission[]  = {0.0,0.0,0.01*emission,1.0};

        //  Save transformation
        glPushMatrix();

        //  Offset, scale and rotate
        glTranslated(x,y,z);
        glScaled(r,r,r);

        //  White ball
        glColor3f(1,1,1);
        glMaterialf(GL_FRONT,GL_SHININESS,shiny);
        glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
        glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

        //  Bands of latitude
        for (ph=-90; ph<90; ph+=inc) {
                glBegin(GL_QUAD_STRIP);
                for (th=0; th<=360; th+=2*inc) {
                        Vertex(th,ph, 1, 0,0,0);
                        Vertex(th,ph+inc, 1, 0,0,0);
                }
                glEnd();
        }

        //  Undo transofrmations
        glPopMatrix();
}


// Cone Roof
static void coneRoof(double radius,double ele,double x,double y,double z) {
        const int d = 20;
        double th;

        // Save transformation
        glPushMatrix();

        // Offsets
        glTranslated(0,y,0);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Build cone roof
        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[0]);
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
                Vertex(th, 0, 0, x, ele, z);
                Vertex(th, 0, radius, x, 0, z);
        }
        glEnd();

        // Build roof base
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
                Vertex(th, 0, 0, x, 0, z);
                Vertex(th, 0, radius, x, 0, z);
        }
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Shaved Roof
static void shavedRoof(double width,double len,double ele,
                       double x,double y,double z,double ry) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(ry, 0, 1, 0);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[0]);

        // Roof Top
        // Left
        glBegin(GL_QUADS);
        glNormal3f(-0.7, 0.7, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-width, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(-width, 0, len);
        glTexCoord2f(1, 1);
        glVertex3f(0, ele, len);
        glTexCoord2f(0, 1);
        glVertex3f(0, ele, 0);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(0.7, 0.7, 0);
        glTexCoord2f(1, 1);
        glVertex3f(0, ele, 0);
        glTexCoord2f(0, 1);
        glVertex3f(0, ele, len);
        glTexCoord2f(0, 0);
        glVertex3f(width, 0, len);
        glTexCoord2f(1, 0);
        glVertex3f(width, 0, 0);
        glEnd();

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[1]);

        // Roof Sides
        // Front
        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, 1);
        glTexCoord2f(1, 0);
        glVertex3f(width, 0, len);
        glTexCoord2f(0.5, 1);
        glVertex3f(0, ele, len);
        glTexCoord2f(0, 0);
        glVertex3f(-width, 0, len);
        glEnd();
        // Back
        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0,0);
        glVertex3f(width, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3f(-width, 0, 0);
        glTexCoord2f(0.5, 1);
        glVertex3f(0, ele, 0);
        glEnd();

        // Roof Bottom
        recGround(width, len, 0, 0, len/2);

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Pyramid roof
static void pyramidRoof(double width,double ele,
                        double x,double y,double z) {
        // Save transformations
        glPushMatrix();

        // Offset
        glTranslated(x, y, z);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[0]);

        // Roof
        // Front
        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0.7, 0.7);
        glTexCoord2f(0, 0);
        glVertex3f(-width, 0, width);
        glTexCoord2f(1, 0);
        glVertex3f(width, 0, width);
        glTexCoord2f(0.5, 1);
        glVertex3f(0, ele, 0);
        glEnd();
        // Right
        glBegin(GL_TRIANGLES);
        glNormal3f(0.7, 0.7, 0);
        glTexCoord2f(0, 0);
        glVertex3f(width, 0, width);
        glTexCoord2f(1, 0);
        glVertex3f(width, 0, -width);
        glTexCoord2f(0.5, 1);
        glVertex3f(0, ele, 0);
        glEnd();
        // Back
        glBegin(GL_TRIANGLES);
        glNormal3f(0, 0.7, -0.7);
        glTexCoord2f(0, 0);
        glVertex3f(width, 0, -width);
        glTexCoord2f(1, 0);
        glVertex3f(-width, 0, -width);
        glTexCoord2f(0.5, 1);
        glVertex3f(0, ele, 0);
        glEnd();
        // Left
        glBegin(GL_TRIANGLES);
        glNormal3f(-0.7, 0.7, 0);
        glTexCoord2f(1, 0);
        glVertex3f(-width, 0, -width);
        glTexCoord2f(0.5, 1);
        glVertex3f(0, ele, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-width, 0, width);
        glEnd();

        // Roof Base
        recGround(width, width*2, 0, 0, 0);

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Door
static void door(double x,double y,double z,double yr) {
        int d = 10;
        int ph;

        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(yr, 0, 1, 0);

        // Enable texture
        glEnable(GL_TEXTURE_2D);

        // Bind Texture
        glBindTexture(GL_TEXTURE_2D,texture[7]);

        // Door
        glBegin(GL_QUAD_STRIP);
        for (ph = 0; ph <= 180; ph += d) {
                glTexCoord2f(Sin(90)*Cos(ph)*0.4+0.5, Sin(ph)*0.4+1);
                glVertex3d(Sin(90)*Cos(ph)*0.4, Sin(ph)*0.4+1, Cos(90)*Cos(ph)*0.4);
                glTexCoord2f(Sin(90)*Cos(ph)*0.5+0.5, Sin(ph)*0.5+1);
                glVertex3d(Sin(90)*Cos(ph)*0.5, 0, Cos(90)*Cos(ph)*0.5);
        }
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Circle Wall
static void circleWall(double radius,double ele,double thk,
                       double x,double y,double z) {
        const int d = 20;
        double th;

        // Save transformation
        glPushMatrix();

        // Offsets
        glTranslated(0,y,0);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[1]);

        // Build outer wall
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
                Vertex(th, 0, radius, x, ele, z);
                Vertex(th, 0, radius, x, 0, z);
        }
        glEnd();

        // Build inner wall
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
                Vertex(th, 0, radius-thk, x, 0, z);
                Vertex(th, 0, radius-thk, x, ele, z);
        }
        glEnd();

        // Build bottom of wall
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
                Vertex(th, 0, radius-thk, x, 0, z);
                Vertex(th, 0, radius, x, 0, z);
        }
        glEnd();

        // Build top of wall
        glBegin(GL_QUAD_STRIP);
        for (th = 0; th <= 360; th += d) {
                Vertex(th, 0, radius-thk, x, ele, z);
                Vertex(th, 0, radius, x, ele, z);
        }
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Rectangular Wall
static void recWall(double width,double len,double ele,double thk,
                    double x,double y,double z,double col) {
        // Save transformation
        glPushMatrix();

        // Offsets
        glTranslated(x,y,z);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        if (col == 0) {
                glBindTexture(GL_TEXTURE_2D,texture[1]);
        }
        else if (col == 1) {
                glBindTexture(GL_TEXTURE_2D,texture[3]);
        }

        // Build outer wall
        // Front
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3d(-width, 0, len);
        glTexCoord2f(1, 0);
        glVertex3d(width, 0, len);
        glTexCoord2f(1, 1);
        glVertex3d(width, ele, len);
        glTexCoord2f(0, 1);
        glVertex3d(-width, ele, len);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(width, 0, len);
        glTexCoord2f(1, 0);
        glVertex3d(width, 0, -len);
        glTexCoord2f(1, 1);
        glVertex3d(width, ele, -len);
        glTexCoord2f(0, 1);
        glVertex3d(width, ele, len);
        glEnd();
        // Back
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0);
        glVertex3d(width, 0, -len);
        glTexCoord2f(1, 0);
        glVertex3d(-width, 0, -len);
        glTexCoord2f(1, 1);
        glVertex3d(-width, ele, -len);
        glTexCoord2f(0, 1);
        glVertex3d(width, ele, -len);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(-width, 0, -len);
        glTexCoord2f(1, 0);
        glVertex3d(-width, 0, len);
        glTexCoord2f(1, 1);
        glVertex3d(-width, ele, len);
        glTexCoord2f(0, 1);
        glVertex3d(-width, ele, -len);
        glEnd();

        // Build inner wall
        // Front
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glVertex3d(-width+thk, 0, len-thk);
        glVertex3d(width-thk, 0, len-thk);
        glVertex3d(width-thk, ele, len-thk);
        glVertex3d(-width+thk, ele, len-thk);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3d(width-thk, 0, len-thk);
        glVertex3d(width-thk, 0, -len+thk);
        glVertex3d(width-thk, ele, -len+thk);
        glVertex3d(width-thk, ele, len-thk);
        glEnd();
        // Back
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3d(width-thk, 0, -len+thk);
        glVertex3d(width-thk, ele, -len+thk);
        glVertex3d(-width+thk, ele, -len+thk);
        glVertex3d(-width+thk, 0, -len+thk);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3d(-width+thk, 0, -len+thk);
        glVertex3d(-width+thk, 0, len-thk);
        glVertex3d(-width+thk, ele, len-thk);
        glVertex3d(-width+thk, ele, -len+thk);
        glEnd();

        // Top wall
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        // Front
        glTexCoord2f(0, 0);
        glVertex3f(-width, ele, len);
        glTexCoord2f(0.5, 0);
        glVertex3f(width-thk, ele, len);
        glTexCoord2f(0.5, 0.5);
        glVertex3f(width-thk, ele, len-thk);
        glTexCoord2f(0, 0.5);
        glVertex3f(-width, ele, len-thk);
        // Right
        glTexCoord2f(0, 0);
        glVertex3f(width-thk, ele, len);
        glTexCoord2f(1, 0);
        glVertex3f(width, ele, len);
        glTexCoord2f(1, 1);
        glVertex3f(width, ele, -len+thk);
        glTexCoord2f(0, 1);
        glVertex3f(width-thk, ele, -len+thk);
        // Back
        glTexCoord2f(0.5, 0);
        glVertex3f(width, ele, -len+thk);
        glTexCoord2f(0.5, 0.5);
        glVertex3f(width, ele, -len);
        glTexCoord2f(0, 0.5);
        glVertex3f(-width+thk, ele, -len);
        glTexCoord2f(0, 0);
        glVertex3f(-width+thk, ele, -len+thk);
        //Left
        glTexCoord2f(1, 1);
        glVertex3f(-width+thk, ele, -len);
        glTexCoord2f(0, 1);
        glVertex3f(-width, ele, -len);
        glTexCoord2f(0, 0);
        glVertex3f(-width, ele, len-thk);
        glTexCoord2f(1, 0);
        glVertex3f(-width+thk, ele, len-thk);
        glEnd();

        // Bottom wall
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        // Front
        glVertex3f(-width, 0, len);
        glVertex3f(width-thk, 0, len);
        glVertex3f(width, 0, len-thk);
        glVertex3f(-width, 0, len-thk);
        // Right
        glVertex3f(width-thk, 0, len);
        glVertex3f(width, 0, len);
        glVertex3f(width, 0, -len+thk);
        glVertex3f(width-thk, 0, -len+thk);
        // Back
        glVertex3f(width, 0, -len+thk);
        glVertex3f(width, 0, -len);
        glVertex3f(-width+thk, 0, -len);
        glVertex3f(-width+thk, 0, -len+thk);
        //Left
        glVertex3f(-width+thk, 0, -len);
        glVertex3f(-width, 0, -len);
        glVertex3f(-width, 0, len-thk);
        glVertex3f(-width+thk, 0, len-thk);
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Square Windows
static void squareWindow(double x,double y,double z,
                         double dx,double dy,double dz,int yr) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x,y,z);
        glRotated(yr, 0, 1, 0);
        glScaled(dx,dy,dz);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[2]);

        // Back Frame
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        // Left
        glTexCoord2f(0, 1);
        glVertex3d(0, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0, 1, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0, 0);
        // Top
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 0.9, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0.9, 0);
        // Right
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 1, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(1, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(1, 1, 0);
        // Bottom
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 0.1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 0.1, 0);
        glEnd();

        // Front Frame
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        // Left
        glTexCoord2f(0, 0);
        glVertex3d(0, 0, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 0, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 1, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0, 1, 0.05);
        // Top
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.9, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.9, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 1, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 1, 0.05);
        // Right
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(1, 1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(1, 0, 0.05);
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0, 0.05);
        // Bottom
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.1, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.1, 0.05);
        glEnd();

        // Outer Frame
        // Bottom
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0, 0, 0);
        glTexCoord2f(0, 1);
        glVertex3d(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(1, 0, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0, 0, 0.05);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(1, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(1, 1, 0.05);
        glTexCoord2f(0, 0);
        glVertex3d(1, 0, 0.05);
        glEnd();
        // Top
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0, 1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(1, 1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(1, 1, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0, 1, 0);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0, 0, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0, 1, 0.05);
        glEnd();

        // Inner Frame
        // Bottom
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.1, 0);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0.1, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.9, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 0.9, 0);
        glEnd();
        // Top
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.9, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.9, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.9, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.9, 0);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 0.9, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 0.9, 0.05);
        glEnd();


        // Inside window
        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[7]);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.9, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.9, 0.05);
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Arch Windows
static void archWindow(double x,double y,double z,
                       double dx,double dy,double dz, int yr) {
        const int d = 10;
        double ph;

        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x,y,z);
        glRotated(yr, 0, 1, 0);
        glScaled(dx,dy,dz);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[2]);

        // Back Frame
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        // Left
        glTexCoord2f(1, 0);
        glVertex3d(0, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0, 0);
        // Right
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(1, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(1, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0, 0);
        // Bottom
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 0.1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 0.1, 0);
        glEnd();

        // Front Frame
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        // Left
        glTexCoord2f(0, 0);
        glVertex3d(0, 0, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.1, 0, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 1, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0, 1, 0.05);
        // Right
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(1, 1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(1, 0, 0.05);
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0, 0.05);
        // Bottom
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0, 0.05);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.1, 0.05);
        glEnd();

        // Outer Frame
        // Bottom
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0, 0, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0, 0, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(1, 0, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(1, 0, 0);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(1, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(1, 1, 0.05);
        glTexCoord2f(0, 0);
        glVertex3d(1, 0, 0.05);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0, 0, 0);
        glTexCoord2f(1, 0);
        glVertex3d(0, 0, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0, 1, 0.05);
        glEnd();

        // Inner Frame
        // Bottom
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.1, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.1, 0.05);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.1, 0);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.9, 0.1, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.9, 0.9, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0.9, 0.9, 0.05);
        glTexCoord2f(1, 0);
        glVertex3d(0.9, 0.1, 0.05);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(1, 1);
        glVertex3d(0.1, 0.9, 0);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(0.1, 0.1, 0.05);
        glTexCoord2f(0, 1);
        glVertex3d(0.1, 0.9, 0.05);
        glEnd();

        // Back Arch
        glBegin(GL_QUAD_STRIP);
        glNormal3f(0, 0, -1);
        for (ph = 0; ph <= 180; ph += d) {
                glTexCoord2f(Sin(90)*Cos(ph)*0.4+0.5, Sin(ph)*0.4+1);
                glVertex3d(Sin(90)*Cos(ph)*0.4+0.5, Sin(ph)*0.4+1, Cos(90)*Cos(ph)*0.4);
                glTexCoord2f(Sin(90)*Cos(ph)*0.5+0.5, Sin(ph)*0.5+1);
                glVertex3d(Sin(90)*Cos(ph)*0.5+0.5, Sin(ph)*0.5+1, Cos(90)*Cos(ph)*0.5);
        }
        glEnd();

        // Front Arch
        glBegin(GL_QUAD_STRIP);
        glNormal3f(0, 0, 1);
        for (ph = 0; ph <= 180; ph += d) {
                glTexCoord2f(Sin(90)*Cos(ph)*0.4+0.5, Sin(ph)*0.4+1);
                glVertex3d(Sin(90)*Cos(ph)*0.4+0.5, Sin(ph)*0.4+1, Cos(90)*Cos(ph)*0.4+0.05);
                glTexCoord2f(Sin(90)*Cos(ph)*0.5+0.5, Sin(ph)*0.5+1);
                glVertex3d(Sin(90)*Cos(ph)*0.5+0.5, Sin(ph)*0.5+1, Cos(90)*Cos(ph)*0.5+0.05);
        }
        glEnd();

        // Inner Arch
        glBegin(GL_QUAD_STRIP);
        for (ph = 0; ph <= 180; ph += d) {
                Vertex(90, ph, 0.4, 0.5, 1, 0);
                Vertex(90, ph, 0.4, 0.5, 1, 0.05);
        }
        glEnd();

        // Outer Arch
        glBegin(GL_QUAD_STRIP);
        for (ph = 0; ph <= 180; ph += d) {
                Vertex(90, ph, 0.5, 0.5, 1, 0);
                Vertex(90, ph, 0.5, 0.5, 1, 0.05);
        }
        glEnd();

        // Inside of my window
        door(0.5, 0, 0.025, 0);

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Balcony Window
static void balconyWindow(double x,double y,double z,double dx,
                          double dy,double dz,double yr) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(yr, 0, 1, 0);
        glScaled(dx, dy, dz);

        // Floor
        recGround(0.9, 0.9, 0, 0, 0);

        // Walls
        recWall(1, 1, 1, 0.1, 0, 0, 0, 0);

        // Roof
        shavedRoof(1, 2, 1, 0, 1, -1, 0);

        // Undo transformations
        glPopMatrix();
}


// Circle towers
static void circleTower(double radius,double tEle,double rEle,double thk,
                        double x,double y,double z) {
        int th;

        // Saves transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);

        // Wall of tower
        circleWall(radius, tEle, thk, 0, 0, 0);

        // Roof of tower
        coneRoof(radius+(radius*0.1), rEle, 0, tEle, 0);

        // Floor of tower
        glBegin(GL_POLYGON);
        for (th = 0; th <= 360; th += 10) {
                Vertex(th, 0, radius-thk, 0, 0, 0);
        }
        glEnd();

        // Undo transformations
        glPopMatrix();
}


// Rectangular building
static void recBuilding(double width,double len, double thk,
                        double wEle,double rEle,double x,double y,double z,double yr) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(yr, 0, 1, 0);

        // Walls
        recWall(width, len, wEle, thk, 0, 0, 0, 0);

        // Roof
        shavedRoof(width, len*2, rEle, 0, wEle, -len, 0);

        // Ground
        recGround(width-thk, len*2-thk, 0, 0, 0);

        // Undo transformations
        glPopMatrix();
}


// Square Building
static void squareBuilding(double width,double thk,double wEle,double tEle,
                           double x,double y,double z,double col) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);

        // Walls
        if (col == 1) {
                recWall(width, width, wEle, thk, 0, 0, 0, 1);
        }
        else {
                recWall(width, width, wEle, thk, 0, 0, 0, 0);
        }

        // Roof
        if (tEle != 0) {
                pyramidRoof(width, tEle, 0, wEle, 0);
        }
        // Floor
        recGround(width, width*2, 0, 0, 0);

        // Undo transformations
        glPopMatrix();
}


// Columns
static void column(double x,double y,double z,double dx,double dy,double dz) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glScaled(dx, dy, dz);

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[3]);

        // Column
        glBegin(GL_QUAD_STRIP);
        for (int th = 0; th <= 360; th += 10) {
                Vertex(th, 0, 0.5, 0, 0.6, 0);
                Vertex(th, 0, 0.5, 0, 3.6, 0);
        }
        glEnd();
        // Base
        squareBuilding(0.8, 0.001, 0.4, 0, 0, 0, 0, 1);
        squareBuilding(0.6, 0.001, 0.2, 0, 0, 0.4, 0, 1);
        squareBuilding(0.6, 0.001, 0.2, 0, 0, 3.6, 0, 1);
        squareBuilding(0.8, 0.001, 0.4, 0, 0, 3.8, 0, 1);

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Watchtower
static void watchtower(double x,double y,double z,
                       double dx,double dy,double dz,double yr) {
        double rd, height;

        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(yr, 0, 1, 0);
        glScaled(dx, dy, dz);

        // Tower portion
        circleWall(1.015, 3, 0.2, 0, 0, 0);
        // Cone roof
        coneRoof(1.1, 1, 0, 3, 0);

        // Windows
        for (rd = 0; rd <= 270; rd += 90) {
                for (height = 0.5; height <= 2.5; height += 0.5) {
                        squareWindow(Sin(rd), height, Cos(rd), 0.15, 0.15, 0.3, rd);
                }
        }

        // Undo transformations
        glPopMatrix();
}


// Main big tower in middle
static void bigWatchTower(double x,double y,double z,
                          double dx,double dy,double dz,double yr) {
        double rd, height;

        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(yr, 0, 1, 0);
        glScaled(dx, dy, dz);

        // Tower portion
        circleWall(1.015, 3, 0.2, 0, 0, 0);
        // Tower bump portion
        circleWall(1.1, 0.5, 0.3, 0, 3, 0);
        // Cone roof
        coneRoof(1.1, 1, 0, 3.5, 0);

        // Windows for tower
        for (rd = 0; rd <= 360; rd += 45) {
                for (height = 0.5; height <= 3.0; height += 0.5) {
                        squareWindow(Sin(rd), height, Cos(rd), 0.15, 0.15, 0.3, rd);
                }
        }

        // Balcony windows for roof
        for (rd = 0; rd <= 180; rd += 90) {
                double cosR = 0.26;
                double sinR = 0.26;
                for (height = 3.8; height <= 4.2; height += 0.2) {
                        if (rd == 0) {
                                balconyWindow(Sin(rd), height, Cos(rd)-cosR, 0.05, 0.05, 0.05, rd);
                                cosR += 0.22;
                        }
                        else if (rd == 90) {
                                balconyWindow(Sin(rd)-sinR, height, Cos(rd), 0.05, 0.05, 0.05, rd);
                                sinR += 0.22;
                        }
                        else if (rd == 180) {
                                balconyWindow(Sin(rd), height, Cos(rd)+cosR,  0.05, 0.05, 0.05, rd);
                                cosR += 0.22;
                        }
                }
        }

        // Circle towers on roof
        double tx = 0;
        for (height = 3.8; height <= 3.9; height += 0.05) {
                circleTower(0.05, 0.15, 0.075, 0.05, -0.6-tx, height, 0);
                tx += 0.08;
        }

        // Undo transformations
        glPopMatrix();
}


// Cathedral
static void cathedral(double x,double y,double z,
                      double dx,double dy,double dz,double yr) {
        double width = 0.8;
        double len = 2;
        double thk = 0.1;
        double wEle = 2;
        double rEle = 0.5;

        // Save transformations
        glPushMatrix();

        // Offset
        glTranslated(x, y, z);
        glRotated(yr, 0, 1, 0);

        // Base building
        recBuilding(width, len, thk, wEle, rEle, 0, 0, 0, 0);
        // Base Floor
        recGround(width-thk, len-thk, 0, 0, 0);
        // Outer square towers
        for (double x = -1.95; x <= 1.96; x += 0.65) {
                if (x > 1.3 && x < 1.4) {
                        squareBuilding(0.1, 0.05, 0.5, 0.3, -0.8, 1.4, x, 0);
                }
                else {
                        squareBuilding(0.1, 0.05, 1.9, 0.3, -0.8, 0, x, 0);
                }
        }
        for (double x = -1.95; x <= 1.96; x += 0.65) {
                squareBuilding(0.1, 0.05, 1.9, 0.3, 0.8, 0, x, 0);
        }
        // Middle Tower
        circleTower(0.2, 0.35, 0.6, 0.05, 0, 2.25, 0);
        // Front tower
        circleTower(0.2, 0.35, 0.4, 0.05, 0, 2.25, 1.8);
        // Back tower
        circleTower(0.2, 0.35, 0.4, 0.05, 0, 2.25, -2);
        // Tower support
        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[1]);
        glBegin(GL_QUAD_STRIP);
        for (int ph = 0; ph >= -90; ph -= 10) {
                for (int th = 0; th <= 360; th += 10) {
                        Vertex(th, ph, 0.2, 0, 2.25, -2);
                        Vertex(th, ph+10, 0.2, 0, 2.25, -2);
                }
        }
        glEnd();

        // Balcony Windows
        for (double z = -1.5; z < 1.5; z += 0.5) {
                balconyWindow(-0.35, 2.2, z, 0.1, 0.1, 0.1, 90);
                balconyWindow(0.35, 2.2, z, 0.1, 0.1, 0.1, 90);
        }
        // Arch Windows
        for (double z = -1.45; z < 1; z +=0.65) {
                archWindow(-0.83, 0.6, z, 0.35, 0.8, 0.8, 90);
                archWindow(0.8, 0.6, z, 0.35, 0.8, 0.8, 90);
        }
        archWindow(-0.83, 0.3, 1.6, 0.6, 0.6, 1, 90);

        // Entrance
        recBuilding(width-0.2, len/6, thk, wEle-0.2, rEle-0.1, 0, 0, 2, 0);
        // Square towers
        squareBuilding(0.1, 0.05, 1.9, 0.3, -0.65, 0, 2.25, 0);
        squareBuilding(0.1, 0.05, 1.9, 0.3, 0.65, 0, 2.25, 0);
        // Tower
        circleTower(0.2, 0.2, 0.2, 0.05, 0, 2, 2.25);

        // Tower support
        // Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[1]);
        glBegin(GL_QUAD_STRIP);
        for (int ph = 0; ph >= -90; ph -= 10) {
                for (int th = 0; th <= 360; th += 10) {
                        Vertex(th, ph, 0.2, 0, 2, 2.25);
                        Vertex(th, ph+10, 0.2, 0, 2, 2.25);
                }
        }
        glEnd();

        // Door
        door(0, 0, 2.335, 0);

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Courtyard
static void courtyard(double x,double y,double z,
                      double dx,double dy,double dz) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glScaled(dx, dy, dz);

        // Cathedral
        cathedral(0, 0, 0, 1, 1, 1, 0);

        // Connector building
        recBuilding(0.6, 0.8, 0.01, 1.5, 0.2, 1.2, 0, 1.35, 90);
        circleTower(0.2, 0.5, 0.4, 0.01, 1.5, 1.5, 1.35);
        for (double x = 1.2; x < 1.8; x += 0.55) {
                squareBuilding(0.1, 0.01, 1.5, 0.2, x, 0, 2, 0);
                squareBuilding(0.1, 0.01, 1.5, 0.2, x, 0, 0.68, 0);
        }

        // Columns
        for (double x = -1; x >= -1.4; x -= 0.4) {
                column(x, 0, 2.1, 0.1, 0.1, 0.1);
        }
        for (double z = 2.5; z <= 4.1; z += 0.4) {
                //column(double x, double y, double z, double dx, double dy, double dz)
                column(-1.4, 0, z, 0.1, 0.1, 0.1);
        }
        for (double x = -1; x <= 1.4; x += 0.4) {
                column(x, 0, 4.1, 0.1, 0.1, 0.1);
        }
        for (double z = 4.1; z >= 2.5; z -= 0.4) {
                column(1.4, 0, z, 0.1, 0.1, 0.1);
        }

        //  Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[3]);

        // Build outer wall
        // Front
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3d(-1.5, 0.4, 4.2);
        glTexCoord2f(1, 0);
        glVertex3d(1.5, 0.4, 4.2);
        glTexCoord2f(1, 1);
        glVertex3d(1.5, 0.6, 4.2);
        glTexCoord2f(0, 1);
        glVertex3d(-1.5, 0.6, 4.2);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(1.5, 0.4, 4.2);
        glTexCoord2f(1, 0);
        glVertex3d(1.5, 0.4, 1);
        glTexCoord2f(1, 1);
        glVertex3d(1.5, 0.6, 1);
        glTexCoord2f(0, 1);
        glVertex3d(1.5, 0.6, 4.2);
        glEnd();
        // Back
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0);
        glVertex3d(-0.5, 0.4, 2);
        glTexCoord2f(1, 0);
        glVertex3d(-1.5, 0.4, 2);
        glTexCoord2f(1, 1);
        glVertex3d(-1.5, 0.6, 2);
        glTexCoord2f(0, 1);
        glVertex3d(-0.5, 0.6, 2);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(-1.5, 0.4, 2);
        glTexCoord2f(0, 1);
        glVertex3d(-1.5, 0.6, 2);
        glTexCoord2f(1, 1);
        glVertex3d(-1.5, 0.6, 4.2);
        glTexCoord2f(1, 0);
        glVertex3d(-1.5, 0.4, 4.2);
        glEnd();

        // Build inner wall
        // Front
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(1, 0);
        glVertex3d(-1.3, 0.4, 4);
        glTexCoord2f(1, 1);
        glVertex3d(-1.3, 0.6, 4);
        glTexCoord2f(0, 1);
        glVertex3d(1.3, 0.6, 4);
        glTexCoord2f(0, 0);
        glVertex3d(1.3, 0.4, 4);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(1.3, 0.4, 1);
        glTexCoord2f(0, 1);
        glVertex3d(1.3, 0.4, 4.2);
        glTexCoord2f(1, 1);
        glVertex3d(1.3, 0.6, 4.2);
        glTexCoord2f(1, 0);
        glVertex3d(1.3, 0.6, 1);
        glEnd();
        // Back
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3d(-1.3, 0.4, 2.2);
        glTexCoord2f(0, 1);
        glVertex3d(-0.5, 0.4, 2.2);
        glTexCoord2f(1, 1);
        glVertex3d(-0.5, 0.6, 2.2);
        glTexCoord2f(1, 0);
        glVertex3d(-1.3, 0.6, 2.2);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3d(-1.3, 0.4, 4.2);
        glTexCoord2f(0, 1);
        glVertex3d(-1.3, 0.4, 2.2);
        glTexCoord2f(1, 1);
        glVertex3d(-1.3, 0.6, 2.2);
        glTexCoord2f(0, 1);
        glVertex3d(-1.3, 0.6, 4.2);
        glEnd();

        // Top wall
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        // Front
        glTexCoord2f(0, 0);
        glVertex3f(-1.5, 0.6, 4.2);
        glTexCoord2f(1, 0);
        glVertex3f(1.5, 0.6, 4.2);
        glTexCoord2f(1, 1);
        glVertex3f(1.5, 0.6, 4);
        glTexCoord2f(0, 1);
        glVertex3f(-1.5, 0.6, 4);
        // Right
        glTexCoord2f(1, 1);
        glVertex3f(1.5, 0.6, 4);
        glTexCoord2f(1, 0);
        glVertex3f(1.5, 0.6, 1);
        glTexCoord2f(0, 0);
        glVertex3f(1.3, 0.6, 1);
        glTexCoord2f(0, 1);
        glVertex3f(1.3, 0.6, 4);
        // Back
        glTexCoord2f(1, 0);
        glVertex3f(-0.5, 0.6, 2.2);
        glTexCoord2f(1, 1);
        glVertex3f(-0.5, 0.6, 2);
        glTexCoord2f(0, 1);
        glVertex3f(-1.3, 0.6, 2);
        glTexCoord2f(0, 0);
        glVertex3f(-1.3, 0.6, 2.2);
        // Left
        glTexCoord2f(1, 0);
        glVertex3f(-1.3, 0.6, 4);
        glTexCoord2f(1, 1);
        glVertex3f(-1.3, 0.6, 2);
        glTexCoord2f(0, 1);
        glVertex3f(-1.5, 0.6, 2);
        glTexCoord2f(0, 0);
        glVertex3f(-1.5, 0.6, 4);
        glEnd();

        // Bottom wall
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        // Front
        glTexCoord2f(0, 1);
        glVertex3f(-1.5, 0.4, 4.2);
        glTexCoord2f(1, 1);
        glVertex3f(1.5, 0.4, 4.2);
        glTexCoord2f(1, 0);
        glVertex3f(1.5, 0.4, 4);
        glTexCoord2f(0, 0);
        glVertex3f(-1.5, 0.4, 4);
        // Right
        glTexCoord2f(1, 1);
        glVertex3f(1.5, 0.4, 4);
        glTexCoord2f(1, 0);
        glVertex3f(1.5, 0.4, 1);
        glTexCoord2f(0, 0);
        glVertex3f(1.3, 0.4, 1);
        glTexCoord2f(0, 1);
        glVertex3f(1.3, 0.4, 4);
        // Back
        glTexCoord2f(1, 1);
        glVertex3f(-0.5, 0.4, 2.2);
        glTexCoord2f(1, 0);
        glVertex3f(-0.5, 0.4, 2);
        glTexCoord2f(0, 0);
        glVertex3f(-1.3, 0.4, 2);
        glTexCoord2f(0, 1);
        glVertex3f(-1.3, 0.4, 2.2);
        //Left
        glTexCoord2f(1, 1);
        glVertex3f(-1.3, 0.4, 4);
        glTexCoord2f(1, 0);
        glVertex3f(-1.3, 0.4, 2);
        glTexCoord2f(0, 0);
        glVertex3f(-1.5, 0.4, 2);
        glTexCoord2f(0, 1);
        glVertex3f(-1.5, 0.4, 4);
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// Main Campus
static void campus(double x,double y,double z,
                   double dx,double dy,double dz) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glScaled(dx, dy, dz);

        // Big tower
        bigWatchTower(0.8, 0, 1.4, 1, 1, 1, 0);
        // Side buildings
        recBuilding(0.6, 2, 0.01, 1.5, 0.5, 1.45, 0, -1, -15);
        recBuilding(0.6, 1.5, 0.01, 1.5, 0.5, 3.4, 0, -2.6, 75);
        recBuilding(0.6, 2, 0.01, 1.5, 0.5, 4.3, 0, -0.1, -15);
        recBuilding(0.6, 1.5, 0.01, 1.5, 0.5, 2.5, 0, 1.8, 75);
        // Side towers
        watchtower(1.9, 0, -3, 0.7, 0.8, 0.7, 0);
        watchtower(4.9, 0, -2.2, 0.7, 0.8, 0.7, 0);
        watchtower(3.7, 0, 2.1, 0.7, 0.8, 0.7, 0);

        // Door
        door(2.3, 0, 2.4, -15);

        glPopMatrix();
}


// Landscape
static void landscape() {
        int zh1 = zh;
        float a = 0;
        if (dayNight == 0.0) {
                a = zh1/359.0;
                if (a == 1) {
                        dayNight = 359.0;
                }
        }
        else {
                zh1 += dayNight;
                a = zh1/359.0;
                dayNight--;
        }
        float Ambient[] = {0.01, a, a, 1};

        // Save transformations
        glPushMatrix();

        // Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[4]);

        // Floor
        glBegin(GL_QUADS);
        glNormal3d(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3d(10, 0, 10);
        glTexCoord2f(1, 0);
        glVertex3d(-10, 0, 10);
        glTexCoord2f(1, 1);
        glVertex3d(-10, 0, -10);
        glTexCoord2f(0, 1);
        glVertex3d(10, 0, -10);
        glEnd();

        // Disable Z-buffering
        glDisable(GL_DEPTH_TEST);

        // Ambient
        glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
        glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);

        glBindTexture(GL_TEXTURE_2D,texture[5]);

        // Background
        // Right
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(10, 0, -10);
        glTexCoord2f(1, 0);
        glVertex3f(10, 0, 10);
        glTexCoord2f(1, 1);
        glVertex3f(10, 10, 10);
        glTexCoord2f(0, 1);
        glVertex3f(10, 10, -10);
        glEnd();
        // Back
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(-10, 0, -10);
        glTexCoord2f(1, 0);
        glVertex3f(10, 0, -10);
        glTexCoord2f(1, 1);
        glVertex3f(10, 10, -10);
        glTexCoord2f(0, 1);
        glVertex3f(-10, 10, -10);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(-10, 0, 10);
        glTexCoord2f(1, 0);
        glVertex3f(-10, 0, -10);
        glTexCoord2f(1, 1);
        glVertex3f(-10, 10, -10);
        glTexCoord2f(0, 1);
        glVertex3f(-10, 10, 10);
        glEnd();
        // Front
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex3f(10, 0, 10);
        glTexCoord2f(1, 0);
        glVertex3f(-10, 0, 10);
        glTexCoord2f(1, 1);
        glVertex3f(-10, 10, 10);
        glTexCoord2f(0, 1);
        glVertex3f(10, 10, 10);
        glEnd();
        // Top
        glBegin(GL_QUADS);
        glTexCoord2f(0.9, 0.9);
        glVertex3f(-10, 10, -10);
        glTexCoord2f(1, 0.9);
        glVertex3f(10, 10, -10);
        glTexCoord2f(1, 1);
        glVertex3f(10, 10, 10);
        glTexCoord2f(0.9, 1);
        glVertex3f(-10, 10, 10);
        glEnd();

        // Enable z-buffering
        glEnable(GL_DEPTH_TEST);

        // End texture
        glDisable(GL_TEXTURE_2D);

        // End lighting
        glDisable(GL_LIGHTING);

        // Undo transformations
        glPopMatrix();
}


// Hogwarts
static void hogwarts(double x,double y,double z,
                     double dx,double dy,double dz, double ry) {
        // Save transformations
        glPushMatrix();

        // Offsets
        glTranslated(x, y, z);
        glRotated(ry, 0, 1, 0);
        glScaled(dx, dy, dz);

        // Howart areas
        courtyard(-2, 0.05, 0, 1, 1, 1);
        campus(0, 0.05, 0, 1, 1, 1);

        // Enable textures
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D,texture[6]);

        // Elevation
        // Top
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-6, 0.05, 6);
        glTexCoord2f(1, 0);
        glVertex3f(6, 0.05, 6);
        glTexCoord2f(1, 1);
        glVertex3f(6, 0.05, -6);
        glTexCoord2f(0, 1);
        glVertex3f(-6, 0.05, -6);
        glEnd();

        // Front
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glTexCoord2f(0, 0);
        glVertex3f(-6, 0, 6);
        glTexCoord2f(1, 0);
        glVertex3f(6, 0, 6);
        glTexCoord2f(1, 1);
        glVertex3f(6, 0.05, 6);
        glTexCoord2f(0, 1);
        glVertex3f(-6, 0.05, 6);
        glEnd();
        // Right
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(6, 0, 6);
        glTexCoord2f(1, 0);
        glVertex3f(6, 0, -6);
        glTexCoord2f(1, 1);
        glVertex3f(6, 0.05, -6);
        glTexCoord2f(0, 1);
        glVertex3f(6, 0.05, 6);
        glEnd();
        // Back
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glTexCoord2f(0, 0);
        glVertex3f(6, 0, -6);
        glTexCoord2f(1, 0);
        glVertex3f(-6, 0, -6);
        glTexCoord2f(1, 1);
        glVertex3f(-6, 0.05, -6);
        glTexCoord2f(0, 1);
        glVertex3f(6, 0.05, -6);
        glEnd();
        // Left
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glTexCoord2f(0, 0);
        glVertex3f(-6, 0, -6);
        glTexCoord2f(1, 0);
        glVertex3f(-6, 0, 6);
        glTexCoord2f(1, 1);
        glVertex3f(-6, 0.05, 6);
        glTexCoord2f(0, 1);
        glVertex3f(-6, 0.05, -6);
        glEnd();

        // End texture
        glDisable(GL_TEXTURE_2D);

        // Undo transformations
        glPopMatrix();
}


// OpenGL (GLUT) calls this routine to display the scene
void display() {
        // Length of axes
        const double len = 2.0;

        // Erase the window and the depth buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        // Enable Z-buffering in OpenGL
        glEnable(GL_DEPTH_TEST);
        // Undo previous transformations
        glLoadIdentity();

        //  Perspective - set eye position
        if (view) {
                double Ex = -2*dim*Sin(th)*Cos(ph);
                double Ey = +2*dim        *Sin(ph);
                double Ez = +2*dim*Cos(th)*Cos(ph);
                gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);
        }
        //  Orthogonal - set world orientation
        else {
                glRotatef(ph,1,0,0);
                glRotatef(th,0,1,0);
        }

        // Smooth shading
        glShadeModel(GL_SMOOTH);

        if (light) {
                // Enable lighting
                glEnable(GL_LIGHTING);
        }

        // Skybox
        landscape();

        // Lights
        if (light) {
                // Translate intensity to color vectors
                float Ambient[]   = {0.02*ambient,0.02*ambient,0.02*ambient,1.0};
                float Diffuse[]   = {0.02*diffuse,0.02*diffuse,0.02*diffuse,1.0};
                float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
                // Light position
                float Position[] = {distance*Sin(zh),distance*Cos(zh), ylight, 1.0};
                // Draw light position as ball (still no lighting here)
                glColor3f(1,1,1);
                ball(Position[0]*2.5, (Position[1]-0.8)*2.5, Position[2]*2.5, 0.8);
                // OpenGL should normalize normal vectors
                glEnable(GL_NORMALIZE);
                // Enable lighting
                glEnable(GL_LIGHTING);
                // glColor sets ambient and diffuse color materials
                glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
                glEnable(GL_COLOR_MATERIAL);
                // Enable light 0
                glEnable(GL_LIGHT0);
                // Set ambient, diffuse, specular components and position of light 0
                glLightfv(GL_LIGHT0,GL_AMBIENT,Ambient);
                glLightfv(GL_LIGHT0,GL_DIFFUSE,Diffuse);
                glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
                glLightfv(GL_LIGHT0,GL_POSITION,Position);
        }
        else {
                glDisable(GL_LIGHTING);
        }

        // Draw Hogwarts
        hogwarts(-0.5, 0, 0, 1, 1, 1, 0);

        float fogColor[4] = {0.2, 0.2, 0.2, 0.2};
        // Fog
        if (fog) {
          glEnable(GL_FOG);
          glFogi(GL_FOG_MODE, GL_EXP);
          glFogfv(GL_FOG_COLOR, fogColor);
          glFogf(GL_FOG_DENSITY, 0.2);
        }
        else {
          glDisable(GL_FOG);
        }

        //  Draw axes - no lighting from here on
        glDisable(GL_LIGHTING);
        // Color of axes
        glColor3f(1,1,1);
        if (axes) {
                glBegin(GL_LINES);
                glVertex3d(0.0,0.0,0.0);
                glVertex3d(len,0.0,0.0);
                glVertex3d(0.0,0.0,0.0);
                glVertex3d(0.0,len,0.0);
                glVertex3d(0.0,0.0,0.0);
                glVertex3d(0.0,0.0,len);
                glEnd();
                //  Label axes
                glRasterPos3d(len,0.0,0.0);
                Print("X");
                glRasterPos3d(0.0,len,0.0);
                Print("Y");
                glRasterPos3d(0.0,0.0,len);
                Print("Z");
        }

        //  Pixels from the lower left corner of the window
        glWindowPos2i(5,25);
        //  Print the text string
        Print("Angle=%d,%d Dim=%.1f FOV=%d Projection=%s",
              th,ph,dim,fov,view ? "Perpective" : "Orthogonal");
        glWindowPos2i(5,5);
        Print("Light=%s Fog=%s", light ? "On" : "Off", fog ? "On" : "Off");
        if (light) {
                glWindowPos2i(5,65);
                Print("Model=Smooth Distance=%d Elevation=%.1f",distance,ylight);
                glWindowPos2i(5,45);
                Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",
                      ambient,diffuse,specular,emission,shiny);
        }

        // Render the scene and make it visible
        ErrCheck("display");
        glFlush();
        glutSwapBuffers();
}


// GLUT calls this routine when the window is resized
void idle() {
        // Elapsed time in seconds
        double t = glutGet(GLUT_ELAPSED_TIME)/2000.0;
        zh = fmod(90 * t, 360.0);
        // Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
}


// GLUT calls this routine when an arrow key is pressed
void special(int key,int x,int y) {
        // Right arrow key - increase angle by 5 degrees
        if (key == GLUT_KEY_RIGHT)
                th += 5;
        // Left arrow key - decrease angle by 5 degrees
        else if (key == GLUT_KEY_LEFT)
                th -= 5;
        // Up arrow key - increase elevation by 5 degrees
        else if (key == GLUT_KEY_UP) {
                if (ph < 175) {
                        ph += 5;
                }
        }
        // Down arrow key - decrease elevation by 5 degrees
        else if (key == GLUT_KEY_DOWN) {
                if (ph > 5) {
                        ph -= 5;
                }
        }
        // PageUp key - increase dim
        else if (key == GLUT_KEY_PAGE_DOWN && dim<5.1)
                dim += 0.1;
        // PageDown key - decrease dim
        else if (key == GLUT_KEY_PAGE_UP && dim>0.1)
            dim -= 0.1;
        // Keep angles to +/-360 degrees
        th %= 360;
        ph %= 360;
        // Update projection
        Project(view ? fov : 0, asp, dim);
        // Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
}


// GLUT calls this routine when a key is pressed
void key(unsigned char ch,int x,int y) {
        // Exit on ESC
        if (ch == 27)
                exit(0);
        // Reset view angle
        else if (ch == '0') {
                th = 0;
                ph = 15;
        }
        // Front view
        else if (ch == '1') {
          th = 15;
          ph = 25;
        }
        // Cathedral view
        else if (ch == '2') {
          th = 60;
          ph = 30;
          dim = 3.2;
          fog = 112;
        }
        // Campus view
        else if (ch == '3') {
          th = 265;
          ph = 40;
          dim = 3.9;
          fov = 112;
        }
        // Toggle axes
        else if (ch == 'x' || ch == 'X')
                axes = 1-axes;
        // Toggle lighting
        else if (ch == 'l' || ch == 'L')
                light = 1-light;
        // Switch projection view
        else if (ch == 'p' || ch == 'P')
                view = 1-view;
        // Change field of view angle
        else if (ch == '-' && ch>1) {
                if (fov > 1) {
                        fov--;
                }
        }
        else if (ch == '+' && ch<179) {
                if (fov < 150) {
                        fov++;
                }
        }
        // Ambient level
        else if (ch=='a' && ambient>0)
                ambient -= 5;
        else if (ch=='A' && ambient<100)
                ambient += 5;
        // Diffuse level
        else if (ch=='d' && diffuse>0)
                diffuse -= 5;
        else if (ch=='D' && diffuse<100)
                diffuse += 5;
        // Specular level
        else if (ch=='s' && specular>0)
                specular -= 5;
        else if (ch=='S' && specular<100)
                specular += 5;
        // Emission level
        else if (ch=='e' && emission>0)
                emission -= 5;
        else if (ch=='E' && emission<100)
                emission += 5;
        // Shininess level
        else if (ch=='n' && shininess>-1)
                shininess -= 1;
        else if (ch=='N' && shininess<7)
                shininess += 1;
        // Toggle light movement
        else if (ch == 'm' || ch == 'M')
                move = 1-move;
        // Toggle fog
        else if (ch == 'f' || ch == 'F')
                fog = 1-fog;

        // Translate shininess power to value (-1 => 0)
        shiny = shininess<0 ? 0 : pow(2.0, shininess);
        // Reproject
        Project(view ? fov : 0,asp,dim);
        // Animate if requested
        glutIdleFunc(move ? idle : NULL);
        // Tell GLUT it is necessary to redisplay the scene
        glutPostRedisplay();
}


// GLUT calls this routine when the window is resized
void reshape(int width,int height) {
        // Ratio of the width to the height of the window
        asp = (height>0) ? (double)width/height : 1;
        // Set the viewport to the entire window
        glViewport(0,0, width,height);
        // Set projection
        Project(view ? fov : 0,asp,dim);
}


// Start up GLUT and tell it what to do
int main(int argc,char* argv[]) {
        // Initialize GLUT
        glutInit(&argc,argv);
        // Request double buffered, true color window with Z buffering at 600x600
        glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
        glutInitWindowSize(600,600);
        glutCreateWindow("Hogwarts: Xi Gao");

        // Set callbacks
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutSpecialFunc(special);
        glutKeyboardFunc(key);
        glutIdleFunc(idle);

        // Load texture
        texture[0] = LoadTexBMP("roof.bmp");
        texture[1] = LoadTexBMP("brick.bmp");
        texture[2] = LoadTexBMP("window.bmp");
        texture[3] = LoadTexBMP("column.bmp");
        texture[4] = LoadTexBMP("ground.bmp");
        texture[5] = LoadTexBMP("sky.bmp");
        texture[6] = LoadTexBMP("landscape.bmp");
        texture[7] = LoadTexBMP("door.bmp");

        // Pass control to GLUT so it can interact with the user
        ErrCheck("init");
        glutMainLoop();
        return 0;
}
