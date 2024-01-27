#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BLOCK_SIZE = 18;
const int NUM_OBSTACLES = 10;
const int BONUS_FOOD_INTERVAL = 5;  
const int BONUS_FOOD_DURATION = 40; 

 struct Obstacle
{ 
    int x,y;
};

struct SnakeSegment {
    int x, y;
};


struct Food {
    SDL_Point position;
    bool isBonus;
};


void render(SDL_Renderer* renderer, const std::vector<SnakeSegment>& snake, const Food& food, const std::vector<Obstacle>& obstacles, int score, TTF_Font* font);
void renderGameOver(SDL_Renderer* renderer, TTF_Font* font, int score);
void update(std::vector<SnakeSegment>& snake, Food& food, std::vector<Obstacle>& obstacles, SDL_Keycode direction, bool& running, int& score, int& foodCount, int& bonusFoodTimer);
bool checkCollision(const std::vector<SnakeSegment>& snake, const std::vector<Obstacle>& obstacles, int x, int y);

int main(int argc, char* argv[]) {
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    
    if (TTF_Init() != 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

   
    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

   
    TTF_Font* font = TTF_OpenFont("Zebulon Italic.otf", 24);  

     std::vector<SnakeSegment> snake{{5, 5}};

    Food food{{10, 10}, false};  
    std::vector<Obstacle> obstacles {{20,20}};
    int foodCount = 0;
    int bonusFoodTimer = BONUS_FOOD_DURATION; 

    
    for (int i = 7; i < NUM_OBSTACLES; ++i) {
        Obstacle obstacle = {rand() % (SCREEN_WIDTH / BLOCK_SIZE), rand() % (SCREEN_HEIGHT / BLOCK_SIZE)};
        obstacles.push_back(obstacle);
        
    }

    SDL_Keycode direction = SDLK_RIGHT;
    bool running = true;
    int score = 0;

   
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch ( event.key.keysym.sym ) {
                    case SDLK_UP:
                        if (direction != SDLK_DOWN)
                            direction = SDLK_UP;
                        break;
                    case SDLK_DOWN:
                        if (direction != SDLK_UP)
                            direction = SDLK_DOWN;
                        break;
                    case SDLK_LEFT:
                        if (direction != SDLK_RIGHT)
                            direction = SDLK_LEFT;
                        break;
                    case SDLK_RIGHT:
                        if (direction != SDLK_LEFT)
                            direction = SDLK_RIGHT;
                        break;
                    default:
                        break;
                }
            }
        }

        
        update(snake, food, obstacles, direction, running, score, foodCount, bonusFoodTimer);

        
        SDL_SetRenderDrawColor(renderer, 153, 255, 153, 255);
        SDL_RenderClear(renderer);

        
        render(renderer, snake, food, obstacles, score, font);
        
        SDL_RenderPresent(renderer);

       
        SDL_Delay(150);
    }

   
    renderGameOver(renderer, font, score);

    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}

void drawObstackle(SDL_Renderer* renderer, const std::vector<Obstacle>& obstacles ){
    for (const auto& obstacle : obstacles) {
        SDL_Rect obstacleRect = {obstacle.x * BLOCK_SIZE, obstacle.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &obstacleRect);
    }
}

void render(SDL_Renderer* renderer, const std::vector<SnakeSegment>& snake, const Food& food, const std::vector<Obstacle>& obstacles, int score, TTF_Font* font) {
    
    drawObstackle(renderer, obstacles);
   
    for (const auto& segment : snake) {
        SDL_Rect blockRect = {segment.x * BLOCK_SIZE, segment.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_SetRenderDrawColor(renderer, 255, 128, 128, 255);
        SDL_RenderFillRect(renderer, &blockRect);
    }

    
    SDL_Rect foodRect = {food.position.x * BLOCK_SIZE, food.position.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
    if (food.isBonus) {
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer,255, 0, 0 , 255);
    }
    SDL_RenderFillRect(renderer, &foodRect);

     
    SDL_Color textColor = {255, 153, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);


    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int textWidth, textHeight;
    SDL_QueryTexture(textTexture, nullptr, nullptr, &textWidth, &textHeight);

    SDL_Rect scoreRect = {10, 10, textWidth, textHeight};
    SDL_RenderCopy(renderer, textTexture, nullptr, &scoreRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void renderGameOver(SDL_Renderer* renderer, TTF_Font* font, int score) {
   
    SDL_SetRenderDrawColor(renderer, 153, 255, 153, 255);
    SDL_RenderClear(renderer);

   
    SDL_Color textColor = {255, 0, 0, 255};
    std::string gameOverText = "Game Over";
    SDL_Surface* gameOverSurface = TTF_RenderText_Solid(font, gameOverText.c_str(), textColor);
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);

    int textWidth, textHeight;
    SDL_QueryTexture(gameOverTexture, nullptr, nullptr, &textWidth, &textHeight);

    SDL_Rect gameOverRect = {(SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 3, textWidth, textHeight};
    SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);

    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);

    
    SDL_Color scoreColor = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), scoreColor);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    SDL_QueryTexture(scoreTexture, nullptr, nullptr, &textWidth, &textHeight);

    SDL_Rect scoreRect = {(SCREEN_WIDTH - textWidth) / 2, SCREEN_HEIGHT / 2, textWidth, textHeight};
    SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

 
    SDL_RenderPresent(renderer);

   
    Uint32 startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < 7000) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
               
                return;
            }
        }
        
        SDL_Delay(10);
    }
}

void update(std::vector<SnakeSegment>& snake, Food& food, std::vector<Obstacle>& obstacles, SDL_Keycode direction, bool& running, int& score, int& foodCount, int& bonusFoodTimer) {
    
    int headX = snake.front().x;
    int headY = snake.front().y;

    switch (direction) {
        case SDLK_UP:
            headY--;
            break;
        case SDLK_DOWN:
            headY++;
            break;
        case SDLK_LEFT:
            headX--;
            break;
        case SDLK_RIGHT:
            headX++;
            break;
        default:
            break;
    }

   
    headX = (headX + SCREEN_WIDTH / BLOCK_SIZE) % (SCREEN_WIDTH / BLOCK_SIZE);
    headY = (headY + SCREEN_HEIGHT / BLOCK_SIZE) % (SCREEN_HEIGHT / BLOCK_SIZE);

   
    if (headX == food.position.x && headY == food.position.y) {
      
        SnakeSegment newSegment = {headX, headY};
        snake.insert(snake.begin(), newSegment);
        
        if (food.isBonus) {
           
            score += 20;
            food.isBonus = false;  
            bonusFoodTimer = BONUS_FOOD_DURATION; 
        } else {
           
            score += 10;
            foodCount++;

            
            if (foodCount % BONUS_FOOD_INTERVAL == 0) {
               
                food.position.x = rand() % (SCREEN_WIDTH / BLOCK_SIZE);
                food.position.y = rand() % (SCREEN_HEIGHT / BLOCK_SIZE);
                food.isBonus = true;
            } else {
               
                food.position.x = rand() % (SCREEN_WIDTH / BLOCK_SIZE);
                food.position.y = rand() % (SCREEN_HEIGHT / BLOCK_SIZE);
                food.isBonus = false;
            }
        }
    } else {
        
        if (checkCollision(snake, obstacles, headX, headY)) {
           
            running = false;
            return;
        }

        
        SnakeSegment newHead = {headX, headY};
        snake.insert(snake.begin(), newHead);
        snake.pop_back();

        
        if (food.isBonus) {
            bonusFoodTimer--;

            if (bonusFoodTimer <= 0) {
               
                food.isBonus = false;
                bonusFoodTimer = BONUS_FOOD_DURATION;

                food.position.x = rand() % (SCREEN_WIDTH / BLOCK_SIZE);
                food.position.y = rand() % (SCREEN_HEIGHT / BLOCK_SIZE);
            }
        }
    }
}

bool checkCollision(const std::vector<SnakeSegment>& snake, const std::vector<Obstacle>& obstacles, int x, int y) {
  
    for (const auto& obstacle : obstacles) {
        if (obstacle.x == x && obstacle.y == y) {
            return true;
        }
    }

   
    for (int i=0; i<snake.size(); ++i) {
        if (snake[i].x == x && snake[i].y == y) {
            return true;
        }
    }

    return false; 
}