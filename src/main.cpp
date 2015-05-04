#include <iostream>
#include <cstdlib>

#include "Sdl_gl.h"
#include "Camera.h"
#include "World.h"
#include "Player.h"
#include "Asteroids.h"
#include "frames.h"
#include "OrthoText.h"

int main()
{
  SDL_GL sdlgl;
  Camera cam;
  World world;
  Player player(0.0f, 0.0f, WORLDRADIUS+PLAYEROFFSET);

  cam.initCamera();
  world.initStars(1500);
  sdlgl.enableLighting();
  glClearColor (0.4, 0.4, 0.4, 0.4);

  int start = SDL_GetTicks();
  int lastTime = 0;

  while(sdlgl.isActive())
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    frameStart();

    world.drawWorld();

    if(player.isAlive())
      player.drawPlayer();
    else
      Mix_PauseMusic();

    cam.updateCamera();

    if(player.m_particles.size() > 0)
      player.drawParticles();

    if(player.isAlive())
      player.checkCollisions(world.m_asteroids, world.m_aColIndices);
    else
      OrthoText(GLUT_BITMAP_HELVETICA_18, "GAME OVER!", 0.5f, 0.5f);

    sdlgl.handleInput(player, cam);

    if(lastTime != (int)(SDL_GetTicks() - start)/1000 && world.m_maxAsteroids < 300)
    {
      world.m_maxAsteroids += world.m_maxAsteroids/50 + 1;
      lastTime = (SDL_GetTicks() - start)/1000;
    }

    frameEnd(GLUT_BITMAP_HELVETICA_18, 1.0, 1.0, 1.0, 0.05, 0.95, player.m_score);
    SDL_GL_SwapWindow(sdlgl.m_win);
  }

  return 0;
}
