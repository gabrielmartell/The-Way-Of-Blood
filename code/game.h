#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <exception>

#include "shader.h"
#include "game_object.h"
#include "text_game_object.h"


namespace game {

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

            void BulletHandle(glm::mat4&, double, GameObject*, float);
			void BackgroundHandle(GameObject*, double);
            void CollisionHandle(GameObject*, GameObject*, int, GameObject*);
            void EnemyHandle(GameObject*);
			void ExplosionHandle(GameObject*, double, GameObject*);
			void TrafficHandle(GameObject*);
            void HUDHandle(GameObject*);
			void CollectibleHandle(GameObject*, GameObject*);
            void ScoreHandle(GameObject*);

            inline bool getBossStatus() { return boss_car; }
            inline void setBossStatus(bool status) { boss_car = status; }
            inline int getBossHealth() { return boss_health; }
            inline void setBossHealth(int health) { boss_health = health; }
            

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Sprite geometry
            Geometry *sprite_;

            //!/ Particle geometry
            Geometry *exhaust_;
            Geometry *blood_;
            Geometry *explosion_;
            Geometry *bullet_particle_;


            // Shader for rendering sprites in the scene
            Shader sprite_shader_;
            Shader particle_shader_;
            Shader text_shader_;

            // References to textures
#define NUM_TEXTURES 30
            GLuint tex_[NUM_TEXTURES];

            // List of game objects
            std::vector<GameObject*> game_objects_;
            std::vector<GameObject*> oncoming_traffic_;
            std::vector<GameObject*> incoming_traffic_;
            int cur_oncoming_car_ = -1;
            int cur_incoming_car_ = -1;
            int boss_health;
            int total_kills_ = 0;

            bool boss_car = false;
            bool game_start = false;
            bool intro_start = false;
            bool backgroundClean = true;

            GameObject* backgr;
            GameObject* health_indi;
            GameObject* weapon_indi;
            GameObject* title_;
			TextGameObject* score_display_;

            // Keep track of time
            double current_time_ = 0.0;
            double timer_time_ = 0.0;
            double invincible_timer_time_ = 0.0;
            float player_bullet_cooldown_ = 100.0f;
            float player_bullet_time_interval_ = 0.027f;
           

            float distance_ = -3.2f;
            float distance_interval_ = 0.0003f;

            // Score
            int score = 0;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void Controls(double delta_time);

            // Update the game based on user input and simulation
            void Update(glm::mat4 view_matrix, double delta_time);

    }; // class Game

} // namespace game

#endif // GAME_H_
