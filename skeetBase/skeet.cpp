/**********************************************************************
 * Skeet
 * No actual birds were killed in the making of this game.
 **********************************************************************/

#include <string>
#include <sstream>
#include "skeet.h"
using namespace std;


#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <openGL/gl.h>    // Main OpenGL library
#include <GLUT/glut.h>    // Second OpenGL library
#define GLUT_TEXT GLUT_BITMAP_HELVETICA_18
#endif // __APPLE__

#ifdef __linux__
#include <GL/gl.h>        // Main OpenGL library
#include <GL/glut.h>      // Second OpenGL library
#define GLUT_TEXT GLUT_BITMAP_HELVETICA_12
#endif // __linux__

#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>         // OpenGL library we copied 
#define _USE_MATH_DEFINES
#include <math.h>
#define GLUT_TEXT GLUT_BITMAP_HELVETICA_12
#endif // _WIN32

/************************
 * SKEET ANIMATE
 * move the gameplay by one unit of time
 ************************/
void Skeet::animate()
{
   time++;
   
   // if status, then do not move the game
   if (time.isStatus())
   {
      // get rid of the bullets and the birds without changing the score
      birds.clear();
      bullets.clear();
      effects.clear();
      return;
   }
   
   // spawn
   spawn();
   
   // move the birds and the bullets
   for (auto element : birds)
   {
      element->advance();
      hitRatio.adjust(element->isDead() ? -1 : 0);
   }
   for (auto bullet : bullets)
      bullet->move(effects);
   for (auto effect : effects)
      effect->fly();
      
   // hit detection
   for (auto element : birds)
      for (auto bullet : bullets)
         if (!element->isDead() && !bullet->isDead() &&
             element->getRadius() + bullet->getRadius() >
             minimumDistance(element->getPosition(), element->getVelocity(),
                             bullet->getPosition(),  bullet->getVelocity()))
         {
            for (int i = 0; i < 25; i++)
               effects.push_back(new Fragment(bullet->getPosition(), bullet->getVelocity()));
            element->kill();
            bullet->kill();
            hitRatio.adjust(1);
         }
   
   // remove the zombie birds
   for (auto it = birds.begin(); it != birds.end();)
      if ((*it)->isDead())
      {
         score.adjust((*it)->getPoints());
         it = birds.erase(it);
      }
      else
         ++it;
       
   // remove zombie bullets
   for (auto it = bullets.begin(); it != bullets.end(); )
      if ((*it)->isDead())
      {
         (*it)->death(bullets);
         it = bullets.erase(it);
      }
      else
         ++it;
   
   // remove zombie fragments
   for (auto it = effects.begin(); it != effects.end();)
      if ((*it)->isDead())
         it = effects.erase(it);
      else
         ++it;
}

/************************************************************************
 * DRAW Background
 * Fill in the background
 *  INPUT color   Background color
 *************************************************************************/
void Skeet::drawBackground(double redBack, double greenBack, double blueBack) const
{
   glBegin(GL_TRIANGLE_FAN);

   // two rectangles is the fastest way to fill the screen.
   glColor3f((GLfloat)redBack /* red % */, (GLfloat)greenBack /* green % */, (GLfloat)blueBack /* blue % */);
   glVertex2f((GLfloat)0.0, (GLfloat)0.0);
   glVertex2f((GLfloat)dimensions.getX(), (GLfloat)0.0);
   glVertex2f((GLfloat)dimensions.getX(), (GLfloat)dimensions.getY());
   glVertex2f((GLfloat)0.0, (GLfloat)dimensions.getY());

   glEnd();
}

/************************************************************************
 * DRAW Timer
 * Draw a large timer on the screen
 *  INPUT percent     Amount of time left
 *        Foreground  Foreground color
 *        Background  Background color
 *************************************************************************/
void Skeet::drawTimer(double percent,
                     double redFore, double greenFore, double blueFore,
                     double redBack, double greenBack, double blueBack) const
{
   double radians;

   GLfloat length = (GLfloat)dimensions.getX();
   GLfloat half = length / (GLfloat)2.0;

   // do the background stuff
   drawBackground(redBack, greenBack, blueBack);

   // foreground stuff
   radians = percent * M_PI * 2.0;
   GLfloat x_extent = half + length * (GLfloat)sin(radians);
   GLfloat y_extent = half + length * (GLfloat)cos(radians);

   // get read to draw the triangles
   glBegin(GL_TRIANGLE_FAN);
   glColor3f((GLfloat)redFore /* red % */, (GLfloat)greenFore /* green % */, (GLfloat)blueFore /* blue % */);
   glVertex2f(half, half);

   // fill in the triangles, one eight at a time
   switch ((int)(percent * 8.0))
   {
   case 7: // 315 - 360
      glVertex2f(half, length);
      glVertex2f(length, length);
      glVertex2f(length, 0.0);
      glVertex2f(0.0, 0.0);
      glVertex2f(0.0, length);
      break;
   case 6: // 270 - 315
      glVertex2f(half, length);
      glVertex2f(length, length);
      glVertex2f(length, 0.0);
      glVertex2f(0.0, 0.0);
      glVertex2f(0.0, half);
      break;
   case 5: // 225 - 270
      glVertex2f(half, length);
      glVertex2f(length, length);
      glVertex2f(length, 0.0);
      glVertex2f(0.0, 0.0);
      break;
   case 4: // 180 - 225
      glVertex2f(half, length);
      glVertex2f(length, length);
      glVertex2f(length, 0.0);
      glVertex2f(half, 0.0);
      break;
   case 3: // 135 - 180
      glVertex2f(half, length);
      glVertex2f(length, length);
      glVertex2f(length, half);
      glVertex2f(length, 0.0);
      break;
   case 2: // 90 - 135 degrees
      glVertex2f(half, length);
      glVertex2f(length, length);
      glVertex2f(length, half);
      break;
   case 1: // 45 - 90 degrees
      glVertex2f(half, length);
      glVertex2f(length, length);
      break;
   case 0: // 0 - 45 degrees
      glVertex2f(half, length);
      break;
   }
   glVertex2f(x_extent, y_extent);

   // complete drawing
   glEnd();

   // draw the red line now
   glBegin(GL_LINES);
   glColor3f((GLfloat)0.6, (GLfloat)0.0, (GLfloat)0.0);
   glVertex2f(half, half);
   glVertex2f(x_extent, y_extent);
   glColor3f((GLfloat)1.0 /* red % */, (GLfloat)1.0 /* green % */, (GLfloat)1.0 /* blue % */);
   glEnd();
}

/*************************************************************************
 * DRAW TEXT
 * Draw text using a simple bitmap font
 *   INPUT  topLeft   The top left corner of the text
 *          text      The text to be displayed
 ************************************************************************/
void drawText(const Point& topLeft, const char* text) 
{
   void* pFont = GLUT_TEXT;
   glColor3f((GLfloat)1.0 /* red % */, (GLfloat)1.0 /* green % */, (GLfloat)1.0 /* blue % */);

   // prepare to output the text from the top-left corner
   glRasterPos2f((GLfloat)topLeft.getX(), (GLfloat)topLeft.getY());

   // loop through the text
   for (const char* p = text; *p; p++)
      glutBitmapCharacter(pFont, *p);
}
void drawText(const Point & topLeft, const string & text)
{
   drawText(topLeft, text.c_str());
}

/************************
 * SKEET DRAW LEVEL
 * output everything that will be on the screen
 ************************/
void Skeet::drawLevel() const
{
   // output the background
   drawBackground(time.level() * .1, 0.0, 0.0);
   
   // output the gun
   gun.display();
         
   // output the birds, bullets, and fragments
   for (auto effect : effects)
      effect->render();
   for (auto bullet : bullets)
      bullet->output();
   for (auto element : birds)
      element->draw();
   
   // status
   drawText(Point(10,                         dimensions.getY() - 30), score.getText()  );
   drawText(Point(dimensions.getX() / 2 - 30, dimensions.getY() - 30), time.getText()   );
   drawText(Point(dimensions.getX() - 110,    dimensions.getY() - 30), hitRatio.getText());
}

/************************
 * SKEET DRAW STATUS
 * place the status message on the center of the screen
 ************************/
void Skeet::drawStatus() const
{
   // output the text information
   ostringstream sout;
   if (time.isGameOver())
   {
      // draw the end of game message
      drawText(Point(dimensions.getX() / 2 - 30, dimensions.getY() / 2 + 10),
               "Game Over");

      // draw end of game status
      drawText(Point(dimensions.getX() / 2 - 30, dimensions.getY() / 2 - 10),
               score.getText());
   }
   else
   {
      // output the status timer
      drawTimer(1.0 - time.percentLeft(),
                     (time.level() - 0.0) * .1, 0.0, 0.0,
                     (time.level() - 1.0) * .1, 0.0, 0.0);

      // draw the message giving a countdown
      sout << "Level " << time.level()
           << " begins in " << time.secondsLeft() << " seconds";
      drawText(Point(dimensions.getX() / 2 - 110, dimensions.getY() / 2 - 10),
         sout.str());
   }
}

/************************
 * SKEET INTERACT
 * handle all user input
 ************************/
void Skeet::interact(const UserInput & ui)
{
   setLevel(time.level());
   for (auto handler : handlers)
      if (handler->handleRequest(&ui, this))
         break;
}

/******************************************************************
 * RANDOM
 * This function generates a random number.
 *
 *    INPUT:   min, max : The number of values (min <= num <= max)
 *    OUTPUT   <return> : Return the integer
 ****************************************************************/
int random(int min, int max)
{
   assert(min < max);
   int num = (rand() % (max - min)) + min;
   assert(min <= num && num <= max);

   return num;
}

/************************
 * SKEET SPAWN
 * lanuch new birds
 ************************/
void Skeet::spawn()
{
   double size;
   switch (time.level())
   {
      // in level 1 spawn big birds occasionally
      case 1:
         size = 30.0;
         // spawns when there is nothing on the screen
         if (birds.size() == 0 && random(0, 15) == 1)
            birds.push_back(new Standard(size, 7.0));
         
         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Standard(size, 7.0));
         break;
         
      // two kinds of birds in level 2
      case 2:
         size = 25.0;
         // spawns when there is nothing on the screen
         if (birds.size() == 0 && random(0, 15) == 1)
            birds.push_back(new Standard(size, 7.0, 12));

         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Standard(size, 5.0, 12));
         // spawn every 3 seconds
         if (random(0, 3 * 30) == 1)
            birds.push_back(new Sinker(size));
         break;
      
      // three kinds of birds in level 3
      case 3:
         size = 20.0;
         // spawns when there is nothing on the screen
         if (birds.size() == 0 && random(0, 15) == 1)
            birds.push_back(new Standard(size, 5.0, 15));

         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Standard(size, 5.0, 15));
         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Sinker(size, 4.0, 22));
         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Floater(size));
         break;
         
      // three kinds of birds in level 4
      case 4:
         size = 15.0;
         // spawns when there is nothing on the screen
         if (birds.size() == 0 && random(0, 15) == 1)
            birds.push_back(new Standard(size, 4.0, 18));

         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Standard(size, 4.0, 18));
         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Sinker(size, 3.5, 25));
         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Floater(size, 4.0, 25));
         // spawn every 4 seconds
         if (random(0, 4 * 30) == 1)
            birds.push_back(new Crazy(size));
         break;
         
      default:
         break;
   }
}

/*******************************************
 * SKEET :: HANDLERGAMEOVER :: HANDLEREQUEST
 * INPUTS  :: *ui, *skeet
 * OUTPUTS :: true or false
 * official method that handles a reset
 * from the game over menu.
 ******************************************/
bool Skeet::HandlerGameOver::handleRequest(const UserInput * ui, Skeet * s) {
   if (ui->isSpace() && s->time.isGameOver()) {
      s->time.reset();
      s->score.reset();
      s->hitRatio.reset();
      return true;
   }
   
   return false;
}

/*******************************************
 * SKEET :: HANDLERPELLET :: HANDLEREQUEST
 * INPUTS  :: *ui, *skeet
 * OUTPUTS :: true or false
 * official method that handles the creation
 * of a pellet!
 ******************************************/
bool Skeet::HandlerPellet::handleRequest(const UserInput * ui, Skeet * s) {
   if (ui->isSpace()) {
      Bullet * p = new Pellet(s->gun.getAngle());
      s->bullets.push_back(p);
      s->score.adjust(0 - p->getValue());
      return true;
   }
   
   return false;
}

/*******************************************
 * SKEET :: HANDLERMISSILE :: HANDLEREQUEST
 * INPUTS  :: *ui, *skeet
 * OUTPUTS :: true or false
 * official method that handles the creation
 * of a missile!
 ******************************************/
bool Skeet::HandlerMissile::handleRequest(const UserInput * ui, Skeet * s) {
   if (ui->isM()) {
      Bullet * p = new Missile(s->gun.getAngle());
      s->bullets.push_back(p);
      s->score.adjust(0 - p->getValue());
      return true;
   }
   
   return false;
}

/*******************************************
 * SKEET :: HANDLERBOMB :: HANDLEREQUEST
 * INPUTS  :: *ui, *skeet
 * OUTPUTS :: true or false
 * official method that handles the creation
 * of a bomb!
 ******************************************/
bool Skeet::HandlerBomb::handleRequest(const UserInput * ui, Skeet * s) {
   if (ui->isB()) {
      Bullet * p = new Bomb(s->gun.getAngle());
      s->bullets.push_back(p);
      s->score.adjust(0 - p->getValue());
      return true;
   }
   
   return false;
}

/*******************************************
 * SKEET :: HANDLERMOVEGUN :: HANDLEREQUEST
 * INPUTS  :: *ui, *skeet
 * OUTPUTS :: true or false
 * official method that handles the movement
 * of the gun.
 ******************************************/
bool Skeet::HandlerMoveGun::handleRequest(const UserInput * ui, Skeet * s) {
   s->gun.interact(ui->isUp() + ui->isRight(), ui->isDown() + ui->isLeft());
   return false;
}

/************************************************
 * SKEET :: HANDLERGUIDEMISSILES :: HANDLEREQUEST
 * INPUTS  :: *ui, *skeet
 * OUTPUTS :: true or false
 * official method that handles the guidance of
 * the missiles.
 ***********************************************/
bool Skeet::HandlerGuideMissile::handleRequest(const UserInput * ui, Skeet * s) {
   for (auto bullet : s->bullets) // Only missiles care about input.
      bullet->input(ui->isUp() + ui->isRight(), ui->isDown() + ui->isLeft(), ui->isB());
   return false;
}

/************************************
 * SKEET :: SETLEVEL
 * INPUTS  :: level
 * OUTPUTS :: NONE
 * Creates the chain needed for the
 * current level (or game over menu).
 ***********************************/
void Skeet::setLevel(int level) {
   handlers.clear(); // Clear the chain for use
   switch (level) {
      case 0:
         handlers.push_back(new HandlerGameOver());
         break;
      case 1:
         handlers.push_back(new HandlerMoveGun);
         handlers.push_back(new HandlerPellet);
         break;
      case 2:
         handlers.push_back(new HandlerMoveGun);
         handlers.push_back(new HandlerGuideMissile);
         handlers.push_back(new HandlerMissile);
         handlers.push_back(new HandlerPellet);
         break;
      case 3:
      case 4:
         handlers.push_back(new HandlerMoveGun);
         handlers.push_back(new HandlerGuideMissile);
         handlers.push_back(new HandlerMissile);
         handlers.push_back(new HandlerPellet);
         handlers.push_back(new HandlerBomb);
   }
}
