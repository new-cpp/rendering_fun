#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cmath>

#define  ASSET_DIR  "asset/";

/*window configs*/
constexpr int WIDTH{800};
constexpr int HEIGHT{450};
const char* TITLE {"Running"};
constexpr int WINDOW_POS{SDL_WINDOWPOS_UNDEFINED};
constexpr const int SCREEN_FPS = 60;
constexpr const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

using i32 = int;
using i64 = long long int;
using ui32 = unsigned int;

struct Vect2
{

  int magnitude()
    {
      return std::sqrt(m_pos.x*m_pos.x + m_pos.y+m_pos.y);
    }

  Vect2 normalise()
    {
      return Vect2{ .m_pos = { m_pos.x/magnitude(), m_pos.y/magnitude()}};
    }

  SDL_Point m_pos{0,0};
  float  m_angle{0.0f};
};




struct TextureManager
{
  TextureManager()=default;
  ~TextureManager()=default;

  bool loadFromFile(const std::string& t_path,SDL_Renderer* t_render);
  void render(SDL_Renderer* t_render, i32 x , i32 y,float& angle);
  //void render_rotate(SDL_Renderer* t_render,const double t_angle,const std_Point* center);
  void free();

  SDL_Texture* m_texture{nullptr};
  i32 m_width,m_height;
};

bool TextureManager::loadFromFile(const std::string& t_path, SDL_Renderer* t_render)
  {
    // free preexiste Texture; ??

    printf("%s\n", t_path.c_str());
    SDL_Surface* loadedSurface = IMG_Load(t_path.c_str());
    if(loadedSurface== nullptr)
      {
        puts("can't load image to surface!!");
        return false;
      }
    else
      {
        m_texture = SDL_CreateTextureFromSurface(t_render,loadedSurface);
        SDL_FreeSurface(loadedSurface);

        if(m_texture == nullptr)
          {
            puts("can't create Texture from surface!!!");
            return false;
          }
        else
          {
            i32 access;
            ui32 format;
            SDL_QueryTexture(m_texture,&format,&access,&m_width,&m_height);
          }
      }

  }

void TextureManager::render(SDL_Renderer* t_render, i32 x ,i32 y,float& t_angle)
  {
    x;
    y;
    int frameWidth = m_width/6;
    int frameHeight = m_height;
    SDL_Rect src {0,0,frameWidth,frameHeight};
    SDL_Rect dst { x ,  y , frameWidth*2, frameHeight*2};

    SDL_Point center {dst.w/2,dst.h/2};
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    SDL_RenderCopyEx(t_render,m_texture,&src,&dst,t_angle,&center,flip);
   // SDL_RenderCopy(t_render,m_texture,&src,&dst);
  }

void TextureManager::free()
  {
    if(m_texture)
      {
        SDL_DestroyTexture(m_texture);
        m_width = 0;
        m_height = 0;
      }
  }

struct Entity
{
  Entity()
  {

  };

  ~Entity()=default;

  void load(const std::string& t_path, SDL_Renderer* t_render);
  void update();
  void render(SDL_Renderer* t_render);

  TextureManager m_texture;
  Vect2 m_position;
};

void Entity::load(const std::string& t_path, SDL_Renderer* t_render)
  {
    m_texture.loadFromFile(t_path, t_render);
     m_position.m_pos.x = WIDTH/2 - m_texture.m_width/6;
     m_position.m_pos.y = HEIGHT/2 - m_texture.m_height;
  }

void Entity::update()
  {
    m_position.m_angle = (static_cast<int>(m_position.m_angle) + 1)%360;
  }

void Entity::render(SDL_Renderer* t_render)
  {
    m_texture.render(t_render ,m_position.m_pos.x, m_position.m_pos.y, m_position.m_angle);
  }

struct Game
{
  SDL_Window* m_window{nullptr};
  SDL_Renderer* m_render{nullptr};
  bool m_running {true};
  ui32 m_next_game_step{0};

  Entity m_entity;/*should be vector*/

  void init();
  void input();
  void update();
  void render();
  void close();

  void controlFPS();
  bool isRunning()const { return m_running;}

};

void Game::init()
  {
    if(SDL_Init(SDL_INIT_VIDEO)<0)
      {
        puts("SDL can't init subsystem");
        m_running = false;
        return;
      }

    if(!(m_window = SDL_CreateWindow(TITLE,WINDOW_POS,WINDOW_POS,WIDTH, HEIGHT, SDL_WINDOW_SHOWN)))
      {
        puts("can't create window");
        m_running = false;
        return;
      }

    if(!(m_render = SDL_CreateRenderer(m_window,-1,SDL_RENDERER_ACCELERATED)))
      {
        puts("can't create render");
        m_running = false;
        return;
      }

    if(!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG))
      {
        puts("can't init image subsystem");
        m_running = false;
        return ;
      }
    const char*  str = "asset/" "scarfy.png";
    printf("str : [%s]\n", str);
      m_entity.load(str, m_render);
  }

void Game::input()
  {
    SDL_Event e;
    while( SDL_PollEvent(&e) )
      {
        if(e.type == SDL_QUIT)
          m_running = false;
        else if (e.type == SDL_KEYDOWN)
          {
            if(e.key.keysym.sym == SDLK_UP)
              {
      //          m_entity.update();
              }
            else if(e.key.keysym.sym== SDLK_RIGHT)
              {
        //        ++m_entity.m_position.m_angle;
              }
          }
      }
  }

void Game::update()
{
  m_entity.update();
}

void Game::render()
  {

        SDL_SetRenderDrawColor(m_render,0xff,0xff,0xff,SDL_ALPHA_OPAQUE);
        SDL_RenderClear(m_render);

        //draw
        m_entity.render(m_render);
        SDL_SetRenderDrawColor(m_render, 0,0,0,SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(m_render,0,HEIGHT/2, WIDTH, HEIGHT/2);
        SDL_RenderDrawLine(m_render,WIDTH/2,0, WIDTH/2, HEIGHT);

        SDL_RenderPresent(m_render);


    
  }

void Game::close()
  {
    m_entity.m_texture.free();

    SDL_DestroyRenderer(m_render);
    SDL_DestroyWindow(m_window);

    m_render = nullptr;
    m_window = nullptr;

    SDL_Quit();
    IMG_Quit();
  }


void Game::controlFPS()
  {
    //fps check
    auto now = SDL_GetTicks();
    if(m_next_game_step>now)
      {
        SDL_Delay(m_next_game_step - now);
      }
    else
      {
         m_next_game_step += SCREEN_TICKS_PER_FRAME;
      }


  }
auto main(int argc, char *argv[]) -> int {


  Game game;

  game.init();
  while(game.isRunning())
    {
      game.controlFPS();
      game.input();
      game.update();
      game.render();
    }

  game.close();

  return 0;
}
