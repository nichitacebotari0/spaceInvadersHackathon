#include <SDL3/SDL.h>

typedef struct InputState
{
    bool up;
    bool down;
    bool left;
    bool right;
} InputState;

typedef union Vector2
{
    float data[2];
    struct
    {
        float x, y;
    };
} Vector2;

typedef struct Entity
{
    bool isActive;
    // player only
    bool IsOverlap;
    Vector2 size;
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool isEnemy;
} Entity;

#define MAX_ENTITIES 100
typedef struct World
{
    float elapsedTime = 0; // modulates difficulty
    Vector2 screenSize = {};
    Entity entities[MAX_ENTITIES];
} World;

void SpawnEnemy(World *world);
void RenderEntity(SDL_Renderer *renderer, Entity *entity);
bool IsAABBOverlap(Vector2 position1, Vector2 size1, Vector2 position2, Vector2 size2);

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Log("start!");

    World world = {};
    (&world)->screenSize = {};
    (&world)->screenSize.x = 480;
    (&world)->screenSize.y = 800;

    SDL_Window *window = SDL_CreateWindow("My First Game", world.screenSize.x, world.screenSize.y, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    Entity player = {};
    player.speed = 5.0f;
    player.position.x = world.screenSize.x / 2;
    player.position.y = world.screenSize.y - world.screenSize.y / 10;
    player.size.x = 50;
    player.size.y = 50;

    float enemySpawnCooldown = 3000;
    float timeBeforeEnemy = enemySpawnCooldown;
    SpawnEnemy(&world);

    bool running = true;
    SDL_Event event;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;
    double fixedTickDuration = 20;
    SDL_Log("start!");

    InputState inputState = {};
    while (running) // fraME LOOP, RENDER LOOP
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        auto lastFrameDuration = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());
        deltaTime += lastFrameDuration;  // 22
        deltaTime = SDL_min(deltaTime, 500);

// Read Input
#pragma region InputRead
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }

            // key press
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                switch (event.key.key)
                {
                case SDLK_ESCAPE:
                    running = false;
                    SDL_Log("SDL_ESCAPE!");
                    break;
                case SDLK_UP:
                case SDLK_W:
                    inputState.up = true;
                    break;
                case SDLK_DOWN:
                case SDLK_S:
                    inputState.down = true;
                    break;
                case SDLK_LEFT:
                case SDLK_A:
                    inputState.left = true;
                    break;
                case SDLK_RIGHT:
                case SDLK_D:
                    inputState.right = true;
                    break;
                case SDLK_SPACE:
                    SDL_Log("Space pressed!");
                    break;
                }
            }

            // key release
            if (event.type == SDL_EVENT_KEY_UP)
            {
                switch (event.key.key)
                {
                case SDLK_UP:
                case SDLK_W:
                    inputState.up = false;
                    break;
                case SDLK_DOWN:
                case SDLK_S:
                    inputState.down = false;
                    break;
                case SDLK_LEFT:
                case SDLK_A:
                    inputState.left = false;
                    break;
                case SDLK_RIGHT:
                case SDLK_D:
                    inputState.right = false;
                    break;
                }
            }
        }

#pragma endregion InputRead

        while (deltaTime > fixedTickDuration) // fixed loop 
        {
            
            if (inputState.left)
                player.position.x -= player.speed;
            if (inputState.right)
                player.position.x += player.speed;

            deltaTime -= fixedTickDuration;

            // enforce bounds
            if (player.position.x < 0)
                player.position.x = 0;

            if (player.position.x > world.screenSize.x - player.size.x)
                player.position.x = world.screenSize.x - player.size.x;

            timeBeforeEnemy -= fixedTickDuration;
            if (timeBeforeEnemy < 0)
            {
                timeBeforeEnemy += enemySpawnCooldown;
                SpawnEnemy(&world);
            }

            player.IsOverlap = false;
            // simulate enemies going down
            for (int i = 0; i < MAX_ENTITIES; i++)
            {
                Entity *entity = &world.entities[i];
                if (!entity->isActive)
                    continue;

                entity->position.y += entity->speed;

                if (entity->isEnemy)
                {
                    // enemy direct hit detection
                    if (IsAABBOverlap(entity->position, entity->size, player.position, player.size))
                    {
                        player.IsOverlap = true;
                        entity->isActive = false;
                    }

                    // enemy going off screen detection
                    if (entity->position.y > world.screenSize.y)
                    {
                        entity->isActive = false;
                    }
                }
            }

            // set difficulty
            world.elapsedTime += fixedTickDuration;

            
            if (world.elapsedTime / 1000 / 60 > 3)
            {
                enemySpawnCooldown = 50;
            }
            else if (world.elapsedTime / 1000 / 60 > 3)
            {
                enemySpawnCooldown = 100;
            }
            else if (world.elapsedTime / 1000 / 60 > 2)
            {
                enemySpawnCooldown = 300;
            }
            else if (world.elapsedTime / 1000 / 60 > 1)
            {
                enemySpawnCooldown = 500;
            }
            else if (world.elapsedTime / 1000 > 30)
            {
                enemySpawnCooldown = 1000;
            }
            else if (world.elapsedTime / 1000 > 10)
            {
                enemySpawnCooldown = 2000;
            }
        }

#pragma region render
        // Clear screen (black)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw player (red square)
        if (player.IsOverlap)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        else
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_FRect rect = {player.position.x, player.position.y, player.size.x, player.size.y};
        SDL_RenderFillRect(renderer, &rect);

        // Draw entities
        for (int i = 0; i < MAX_ENTITIES; i++)
        {
            RenderEntity(renderer, &world.entities[i]);
        }

        // Show everything
        SDL_RenderPresent(renderer);
#pragma endregion render
    }

    SDL_Log("Exited!");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void RenderEntity(SDL_Renderer *renderer, Entity *entity)
{
    if (!entity->isActive)
        return;

    if (entity->isEnemy)
    {
        // Draw enemy (blue square)
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        SDL_FRect rect = {entity->position.x, entity->position.y, entity->size.x, entity->size.y};
        SDL_RenderFillRect(renderer, &rect);
        return;
    }
}

bool IsAABBOverlap(Vector2 positionA, Vector2 sizeA, Vector2 positionB, Vector2 sizeB)
{
    bool AIsRightOfB = positionA.x > positionB.x + sizeB.x;
    bool AIsLeftOfB = positionA.x + sizeA.x < positionB.x;
    bool AIsAboveB = positionA.y > positionB.y + sizeB.y;
    bool AIsBelowB = positionA.y + sizeA.y < positionB.y;

    return !(AIsRightOfB || AIsLeftOfB || AIsAboveB || AIsBelowB);
}

void SpawnEnemy(World *world)
{
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        Entity *entity = &world->entities[i];
        if (entity->isActive)
            continue;

        auto position = SDL_rand(world->screenSize.x - 50);

        world->entities[i].position.x = position;
        world->entities[i].position.y = -entity->size.x;
        world->entities[i].size.x = 50;
        world->entities[i].size.y = 50;
        world->entities[i].speed = 3.0f;
        world->entities[i].isActive = true;
        world->entities[i].isEnemy = true;
        break;
    }
}
