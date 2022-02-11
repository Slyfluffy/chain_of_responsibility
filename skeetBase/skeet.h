/***********************************************************************
 * Header File:
 *    SKEET
 * Author:
 *    Br. Helfrich
 * Summary:
 *    The game class
 ************************************************************************/

#pragma once

#include "bird.h"
#include "bullet.h"
#include "effect.h"
#include "gun.h"
#include "point.h"
#include "score.h"
#include "time.h"
#include "uiInteract.h"

#include <list>

/*************************************************************************
 * Skeet
 * The game class
 *************************************************************************/
class Skeet
{
public:
    Skeet(Point & dimensions) : dimensions(dimensions),
        gun(Point(800.0, 0.0)), time(), score(), hitRatio() {}

    // handle all user input
    void interact(const UserInput& ui);

    // move the gameplay by one unit of time
    void animate();

    // output everything on the screen
    void drawLevel()  const;    // output the game
    void drawStatus() const;    // output the status information

    // is the game currently playing right now?
    bool isPlaying() const { return time.isPlaying();  }
private:
   
   /**************************************
    * SKEET :: HANDLER
    * Abstract handler to handle messages.
    *************************************/
   class Handler {
   public:
      virtual bool handleRequest(const UserInput * ui, Skeet * s) = 0;
   };

   /************************************
    * SKEET :: HANDLERGAMEOVER
    * Handles the game over request from
    * user.
    ***********************************/
   class HandlerGameOver: public Handler {
   public:
      bool handleRequest(const UserInput * ui, Skeet * s);
   };

   /**********************************
    * SKEET :: HANDLERPELLET
    * Handles the creation of pellets.
    *********************************/
   class HandlerPellet: public Handler {
   public:
      bool handleRequest(const UserInput * ui, Skeet * s);
   };

   /************************************
    * SKEET :: HANDLERMISSILE
    * Handles the creation of a missile.
    ***********************************/
   class HandlerMissile: public Handler {
   public:
      bool handleRequest(const UserInput * ui, Skeet * s);
   };

   /*****************************
    * SKEET :: HANDLERBOMB
    * Handles creation of a bomb.
    ****************************/
   class HandlerBomb: public Handler {
   public:
      bool handleRequest(const UserInput * ui, Skeet * s);
   };
   
   /**********************************
    * SKEET :: HANDLERMOVEGUN
    * Handles the movement of the gun.
    *********************************/
   class HandlerMoveGun: public Handler {
   public:
      bool handleRequest(const UserInput * ui, Skeet * s);
   };
   
   /***********************************
    * SKEET :: HANDLERGUIDEMISSILE
    * Handles the guidance of missiles!
    **********************************/
   class HandlerGuideMissile: public Handler {
   public:
      bool handleRequest(const UserInput * ui, Skeet * s);
   };
   
    // setLevel determines the chain for Handlers.
    void setLevel(int level);
   
    // generate new birds
    void spawn();                  
    void drawBackground(double redBack, double greenBack, double blueBack) const;
    void drawTimer(double percent,
                   double redFore, double greenFore, double blueFore,
                   double redBack, double greenBack, double blueBack) const;
    
    Gun gun;                        // the gun
    std::list<Bird*>    birds;      // all the shootable birds
    std::list<Bullet*>  bullets;    // the bullets
    std::list<Effect*>  effects;    // the fragments of a dead bird.
    std::list<Handler*> handlers;   // List of handlers.
    Time time;                      // how many frames have transpired since the beginning
    Score score;                    // the player's score
    HitRatio hitRatio;              // the hit ratio for the birds
    Point dimensions;               // size of the screen
};
