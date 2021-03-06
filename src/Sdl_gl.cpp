/*
 Copyright © 2015 Teemu Lindborg
 SDAGE 1st year 2nd PPP Assignment
*/

#ifdef LINUX
  #include <GL/glut.h>
#endif
#ifdef DARWIN
  #include <glut/glut.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "NCCA/GLFunctions.h"
#include "Defs.h"
#include "Sdl_gl.h"

// ---------------------------------------------------------------------------------------
/// @file Sdl_gl.cpp
/// @brief Implementation of the SDL_GL class functions
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
SDL_GL::SDL_GL()
{
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 4096;

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) == -1)
    SDLErrorExit("Couldn't initialise SDL");

  // Initialise the OGG support for SDL mixer
  if(((Mix_Init(MIX_INIT_OGG))&MIX_INIT_OGG) != MIX_INIT_OGG)
    std::cerr << "Failed to initialise OGG support:" << Mix_GetError() << "\n";

  if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == -1)
    SDLErrorExit("Couldn't initialise SDL controller");

#ifdef LINUX
  int argc = 1;
  char *argv[1] = {(char*)"Something"};
  glutInit(&argc, argv);
#endif

  m_win = SDL_CreateWindow("SS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if(!m_win)
  {
    SDLErrorExit("Couldn't create SDL window");
  }

  m_gl = createOpenGLContext();
  if(!m_gl)
  {
    SDLErrorExit("Problem creating OpenGL context");
  }

  m_controller = NULL;

  // Look for possible controllers
  for(int i = 0; i < SDL_NumJoysticks(); i++)
  {
    if((m_controller = SDL_GameControllerOpen(i)))
      break;
  }

  // Open the audio device for the use of this program
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0)
  {
      std::cerr << "Unable to initialize audio: " << Mix_GetError() << "\n";
      exit(1);
  }

  // If everything initialised correctly we set the flag that allows the
  // program to run to be true
  m_act = true;
}
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
SDL_GL::~SDL_GL()
{
  // Clean up
  if(!m_controller)
    SDL_GameControllerClose(m_controller);

  Mix_CloseAudio();
  SDL_GL_DeleteContext(m_gl);
  SDL_Quit();
}
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
void SDL_GL::SDLErrorExit(const std::string &_msg)
{
  std::cerr << _msg <<"\n";
  std::cerr << SDL_GetError() <<"\n";
  SDL_Quit();
  exit(EXIT_FAILURE);
}
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
SDL_GLContext SDL_GL::createOpenGLContext()
{
  // Request an opengl 3.2 context first we setup our attributes, if you need any
  // more just add them here before the call to create the context
  // SDL doesn't have the ability to choose which profile at this time of writing,
  // but it should default to the core profile
  // for some reason we need this for mac but linux crashes on the latest nvidia drivers
  // under centos
  #ifdef DARWIN
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  #endif
  // set multi sampling else we get really bad graphics that alias
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,2);
  // Turn on double buffering with a 24bit Z buffer.
  // You may need to change this to 16 or 32 for your system
  // on mac up to 32 will work but under linux centos build only 16
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  // enable double buffering (should be on by default)
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //
  return SDL_GL_CreateContext(m_win);
}
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
void SDL_GL::enableLighting() const
{
  // Enable 2D textures, point sprites and blend functions
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_POINT_SPRITE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Setting up light details
  GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat mat_shininess[] = { 100.0 };
  GLfloat light_position[] = { 0.5, 0.75, 1.5, 0.0 };
  GLfloat light_position2[] = { 0.0, 0.0, -3, 0.0 };
  glShadeModel (GL_SMOOTH);

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glColor3f(1,1,1);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
}
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
bool SDL_GL::isActive() const
{
  return m_act;
}
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
void SDL_GL::handleInput(Player &io_p, Camera &io_cam)
{
  SDL_Event event;

  while( SDL_PollEvent(&event) )
  {
    switch (event.type)
    {
      // this is the window x being clicked.
      case SDL_QUIT : m_act = false; break;

      // now we look for a keydown event
      case SDL_KEYDOWN:
      {
        switch( event.key.keysym.sym )
        {
          // if it's the escape key act
          case SDLK_ESCAPE : m_act = false; break;
          case SDLK_k : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
          case SDLK_l : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
          default : break;
        } // end of key process
      } // end of keydown

      default : break;
    } // end of event switch
  } // end of poll events

  if(io_p.isAlive())
    io_p.handleMovement(m_controller, io_cam);
}
// ---------------------------------------------------------------------------------------
