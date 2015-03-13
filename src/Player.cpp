#include <OpenGl/gl.h>
#include <iostream>

#include "Defs.h"
#include "NCCA/GLFunctions.h"
#include "Camera.h"
#include "Player.h"
#include "World.h"

void Player::drawPlayer()
{
  glPushMatrix();

    glMultMatrixf(orientation.m_openGL);
    glLoadIdentity();

    glPointSize(15.0f);

    // Move player to the correct position and rotate
    glTranslatef(xMov, yMov, -pos.length()+WORLDRADIUS+PLAYEROFFSET);
    glRotatef(rot, 0, 0, 1);

    glBegin(GL_POINTS);
      glColor3f(0, 0, 0);
      glVertex3f(0.1*cosf(aimDir), 0.1*sinf(aimDir), 0);
    glEnd();

    glColor3f(1.0f, 0.0, 0.0f);
    cube();

  glPopMatrix();
}

void Player::handleMovement(SDL_GameController *_c, Camera &_cam)
{
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);

  // Variables for controlling & smoothing the player movement
  /*
   * dir holds the actual angle of the direction where top is 0 degrees
   * localRadius restricts the movement around the sphere
   * retSteps used for speed calculations
   * ud/lr hold the movement direction coordinates
   */
  static float dir;
  const float localRadius = 0.5f;
  const int retSteps = 25;
  const int angleStep = 8;
  float ud = 0, lr = 0;
  float xDest, xMove;
  float yDest, yMove;

  // Using the cross product we calculate the new up and side vectors for the camera
  // to control the correct movement of the camera and the player
  _cam.eye.normalize();

  _cam.w = _cam.eye;
  _cam.w = _cam.w.cross(_cam.up);
  _cam.up = _cam.w.cross(_cam.eye);
  _cam.w.normalize();
  _cam.up.normalize();


  /*
   * Handle keyboard direction (trying to imitate controller stick coordinates
   * Y-axis (U/D):
   * Top: 1
   * Top left/right: sin(45)
   * Bottom: -1
   * Bottom left/right: -sin(45)
   *
   * X-axis (L/R):
   * Left: -1
   * Left top/bottom: -sin(45)
   * Right: 1
   * Right top/bottom: sin(45)
   */
  if(_c != NULL)
  {
    ud = -SDL_GameControllerGetAxis(_c, SDL_CONTROLLER_AXIS_LEFTY);
    lr = SDL_GameControllerGetAxis(_c, SDL_CONTROLLER_AXIS_LEFTX);

    ud = (fabs(ud) < sensitivity ? 0 : ud/32767.0);
    lr = (fabs(lr) < sensitivity ? 0 : lr/32767.0);
  }
  else
  {
    if(keystate[SDL_SCANCODE_W])
      ud = (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D] ? sinf(PI4) : 1);
    else if(keystate[SDL_SCANCODE_S])
      ud = (keystate[SDL_SCANCODE_A] || keystate[SDL_SCANCODE_D] ? -sinf(PI4) : -1);

    if(keystate[SDL_SCANCODE_A])
      lr = (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_S] ? -sinf(PI4) : -1);
    else if(keystate[SDL_SCANCODE_D])
      lr = (keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_S] ? sinf(PI4) : 1);
  }

  // Calculate the player movement "inside a circle"
  // in front of the camera
  xDest = sinf(atan2f(lr, ud)) * localRadius;
  yDest = sinf(atan2f(ud, lr)) * localRadius;
  yMove = (yDest - yMov)/(float)retSteps;
  xMove = (xDest - xMov)/(float)retSteps;

  yMov = (yMov > yDest ? yMov + yMove :
          (yMov < yDest ? yMov + yMove : yMov));
  xMov = (xMov > xDest ? xMov + xMove :
          (xMov < xDest ? xMov + xMove : xMov));

  // Move the camera to the direction of the up and the left vector
  // based on the player's desired direction
  // Also when check whether the player's smoothing towards the center is happening
  // and smooth the camera's movement as well
  if(ud)
  {
    _cam.eye.m_x += ud * _cam.up.m_x * MOVESPEED;
    _cam.eye.m_y += ud * _cam.up.m_y * MOVESPEED;
    _cam.eye.m_z += ud * _cam.up.m_z * MOVESPEED;
  }
  else
  {
    _cam.eye.m_x -= _cam.up.m_x * yMove;
    _cam.eye.m_y -= _cam.up.m_y * yMove;
    _cam.eye.m_z -= _cam.up.m_z * yMove;
  }
  if(lr)
  {
    _cam.eye.m_x -= lr * _cam.w.m_x * MOVESPEED;
    _cam.eye.m_y -= lr * _cam.w.m_y * MOVESPEED;
    _cam.eye.m_z -= lr * _cam.w.m_z * MOVESPEED;
  }
  else
  {
    _cam.eye.m_x += _cam.w.m_x * xMove;
    _cam.eye.m_y += _cam.w.m_y * xMove;
    _cam.eye.m_z += _cam.w.m_z * xMove;
  }

  //
  _cam.eye.normalize();
  norm = _cam.eye;
  norm.normalize();

  _cam.eye *= 4;
  pos = _cam.eye;

  orientation = GLFunctions::orientation(_cam.eye, _cam.look, _cam.up);

  // Check whether player is moving somewhere and
    // handle the possible rotation required
    if(ud != 0 || lr != 0)
    {
      // Map the ud & lr coords to angles and substract 90 degrees to have top be 0
      dir = atan2f(ud, lr) * 180/PI - 90;
      wrapRotation(dir);

      float distance = dir - rot;
      float aStep = angleStep;
      if(fabs(distance) < angleStep)
        aStep = fabs(distance);
      rot = ((distance > 0 && distance <= 180) || distance < -180 ? rot + aStep :
            ((distance >= -180 && distance < 0) || distance > 180 ? rot - aStep : rot));
    }

    wrapRotation(rot);

  shoot(_c);
}

void Player::shoot(SDL_GameController *_c)
{
  bool shoot = false;
  float x = 0, y = 0;

  if(_c != NULL)
  {
    x = SDL_GameControllerGetAxis(_c, SDL_CONTROLLER_AXIS_RIGHTX);
    y = -SDL_GameControllerGetAxis(_c, SDL_CONTROLLER_AXIS_RIGHTY);

    x = (fabs(x) < sensitivity ? 0 : x/32767.0);
    y = (fabs(y) < sensitivity ? 0 : y/32767.0);

    if(x != 0 || y != 0)
      shoot = true;
  }
  else
  {
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);

    aimDir = fmod((keystate[SDL_SCANCODE_LEFT] ? aimDir + 0.1f :
             (keystate[SDL_SCANCODE_RIGHT] ? aimDir - 0.1f : aimDir)), TWO_PI);

    if(keystate[SDL_SCANCODE_SPACE])
      shoot = true;
  }

  if((x != 0 || y != 0) && _c)
    aimDir = atan2f(y, x) - (rot * PI / 180);

  if(shoot)
  {
    /*p.push_back(Projectile(pos.m_x + 0.1*cosf(aimDir),
                       pos.m_y + 0.1*sinf(aimDir),
                       WORLDRADIUS+PLAYEROFFSET,
                       cosf(aimDir), sinf(aimDir), 0));*/
    p.push_back(Projectile(pos.m_x, pos.m_y, pos.m_z,
                           cosf(aimDir), sinf(aimDir), 0));
  }

  glPushMatrix();
    glPointSize(25);
    glColor3f(1, 0, 0);
    glBegin(GL_POINTS);
    for(int i = 0; i < (int)p.size(); ++i)
    {
      if(p[i].life > 0)
      {
        p[i].pos += p[i].dir*p[i].speed;
        if(p[i].pos.length() > WORLDRADIUS*ASPHERERADIUS)
        {
          p[i].pos.normalize();
          p[i].pos *= (WORLDRADIUS*ASPHERERADIUS);
        }
        p[i].pos.vertexGL();
        --p[i].life;
      }
      else
        p.erase(p.begin() + i);
    }
    glEnd();
  glPopMatrix();
}

void Player::cube()
{
  float r = 0.05f;

  glBegin(GL_QUADS);
    norm.normalGL();
    // Side 1
    glVertex3f(-r, r, r/2);
    glVertex3f(r, r, r/2);
    glVertex3f(r, -r, r/2);
    glVertex3f(-r, -r, r/2);

    // Side 2
    glVertex3f(-r, r, -r/2);
    glVertex3f(r, r, -r/2);
    glVertex3f(r, -r, -r/2);
    glVertex3f(-r, -r, -r/2);

    // Side 3
    glVertex3f(-r, r, r/2);
    glVertex3f(-r, r, -r/2);
    glVertex3f(-r, -r, -r/2);
    glVertex3f(-r, -r, r/2);

    // Side 4
    glVertex3f(r, r, r/2);
    glVertex3f(r, r, -r/2);
    glVertex3f(r, -r, -r/2);
    glVertex3f(r, -r, r/2);

    // Side 5
    glVertex3f(-r, r, r/2);
    glVertex3f(-r, r, -r/2);
    glVertex3f(r, r, -r/2);
    glVertex3f(r, r, r/2);

    // Side 6
    glVertex3f(-r, -r, r/2);
    glVertex3f(-r, -r, -r/2);
    glVertex3f(r, -r, -r/2);
    glVertex3f(r, -r, r/2);

  glEnd();
  glBegin(GL_LINE_STRIP);
    glColor3f(1,0,0);
    glVertex3f(0,0,0);
    glVertex3f(0,1,0);
  glEnd();
}

void Player::wrapRotation(float &io_a)
{
  if(io_a < 0)
    io_a = 360 - fabs(fmod(io_a, 360));
  else
    io_a = fmod(io_a, 360);
}
