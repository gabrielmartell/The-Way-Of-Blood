    
#include <stdexcept>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>
#include <stdlib.h>

#include <path_config.h>

#include "sprite.h"
#include "shader.h"
#include "player_game_object.h"
#include "enemy_game_object.h"
#include "collectible_game_object.h"
#include "bullet_game_object.h"
#include "game.h"
#include "particle_system.h"
#include "particles.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "Game Demo";
int window_x = 800;
int window_y = 600;
const unsigned int window_width_g = window_x;
const unsigned int window_height_g = window_y;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 1.0);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;

Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}

void Game::Init(void)
{
    srand(time(0));
    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set window to not resizable
    // Required or else the calculation to get cursor pos to screenspace will be incorrect
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    exhaust_ = new Particles(0);exhaust_->CreateGeometry();
    blood_ = new Particles(1);blood_->CreateGeometry();
    explosion_ = new Particles(2);explosion_->CreateGeometry();
    bullet_particle_ = new Particles(3);bullet_particle_->CreateGeometry();

    // Initialize sprite shader
    particle_shader_.Init((resources_directory_g+std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/particle_fragment_shader.glsl")).c_str());

    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

	text_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/text_fragment_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;
}

Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Game::Setup(void)
{

    // Setup the game world

    // Load textures
    SetAllTextures();

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    GameObject* player = new PlayerGameObject(glm::vec3(0.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[2], tex_[1], 0, tex_[2], 3, NULL); 
	game_objects_.push_back(new PlayerGameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[10], tex_[1], 2, NULL, NULL, player)); //0 ARM!
    game_objects_.push_back(player); //1 PLAYER

    // Setup other objects
		//!/ Setup the 
	float pos = 1.5f;
	for (int i = 0; i < 2; ++i) { //2-3
		GameObject* oncoming_traffic = new EnemyGameObject(glm::vec3(pos, 12.0f, 0.0f), sprite_, &sprite_shader_, tex_[8], tex_[1], 9, 1, i+2, NULL);
		pos += 2.0f;
		oncoming_traffic_.push_back(oncoming_traffic);
		game_objects_.push_back(oncoming_traffic);
	}

	pos = -3.5f;
	for (int i = 0; i < 2; ++i) { //4-5
		GameObject* incoming_traffic = new EnemyGameObject(glm::vec3(pos, 12.0f, 0.0f), sprite_, &sprite_shader_, tex_[9], tex_[1], 10, 1, i, NULL);
		pos += 2.0f;
		incoming_traffic_.push_back(incoming_traffic);
		game_objects_.push_back(incoming_traffic);
	}

	/*
    GAME ENEMIES
    _____________________________________________________________________________
    */
    GameObject* enemy1 = new EnemyGameObject(glm::vec3(-2.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[4], tex_[1], 1, 0, 1, NULL);
    enemy1->SetState(-2);
	game_objects_.push_back(new EnemyGameObject(glm::vec3(-2.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[5], tex_[1], 2, 0, 1, enemy1));
    game_objects_.push_back(enemy1);

    GameObject* enemy2 = new EnemyGameObject(glm::vec3(2.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[3], tex_[1], 1, 1, 2, NULL);
    enemy2->SetState(-2);
	game_objects_.push_back(new EnemyGameObject(glm::vec3(2.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[5], tex_[1], 2, 1, 2, enemy2));
    game_objects_.push_back(enemy2);

    GameObject* enemy3 = new EnemyGameObject(glm::vec3(0.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[25], tex_[1], 11, 1, 2, NULL);
    enemy3->SetState(-2);
    game_objects_.push_back(new EnemyGameObject(glm::vec3(0.0f, -5.0f, 0.0f), sprite_, &sprite_shader_, tex_[26], tex_[1], 12, 1, 2, enemy3));
    game_objects_.push_back(enemy3);

	/*
	BACKGROUND
	_____________________________________________________________________________
	*/
    GameObject* background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[0], tex_[1], -1, NULL);
    background->SetScale(glm::vec2(50.0, 50.0));
	GameObject* background2 = new GameObject(glm::vec3(0.0f, 49.0f, 0.0f), sprite_, &sprite_shader_, tex_[12], tex_[1], -2, NULL);
	background2->SetScale(glm::vec2(50.0, 50.0));
	GameObject* grassright = new GameObject(glm::vec3(17.95f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[13], tex_[13], -3, NULL);
	grassright->SetScale(glm::vec2(25.0f, 150.0f));
	GameObject* grassleft = new GameObject(glm::vec3(-17.95f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[13], tex_[13], -4, NULL);
	grassleft->SetScale(glm::vec2(25.0f, 150.0f));

	game_objects_.push_back(grassleft);
	game_objects_.push_back(grassright);
    game_objects_.push_back(background2);
    game_objects_.push_back(background);
    backgr = grassleft;

	/*
    COLLECTIBLES
    _____________________________________________________________________________
    */
	GameObject* collectible_gun = new CollectibleGameObject(glm::vec3(-2.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[20], tex_[1], 3, 0);
	collectible_gun->SetState(2);
	game_objects_.push_back(collectible_gun);

	GameObject* collectible_heal = new CollectibleGameObject(glm::vec3(-4.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[21], tex_[1], 3, 1);
	collectible_heal->SetState(2);
	game_objects_.push_back(collectible_heal);

    GameObject* collectible_godmode = new CollectibleGameObject(glm::vec3(-4.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[22], tex_[1], 3, 2);
    collectible_godmode->SetState(2);
    game_objects_.push_back(collectible_godmode);

    /*
    HUD
    _____________________________________________________________________________
    */
    GameObject* distance_meter = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[14], tex_[1], 5, NULL);
    distance_meter->SetScale(glm::vec2(7.0f, 6.0f));
    game_objects_.push_back(distance_meter);

	GameObject* distance_indicator = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[18], tex_[1], 6, NULL);
	distance_indicator->SetScale(glm::vec2(0.2f, 0.2f));
	game_objects_.push_back(distance_indicator);

    GameObject* health_indicator = new GameObject(glm::vec3(-3.3f, -3.5f, 0.0f), sprite_, &sprite_shader_, tex_[15], tex_[1], 7, NULL);
    game_objects_.push_back(health_indicator);
    health_indi = health_indicator;

	GameObject* weapon_indicator = new GameObject(glm::vec3(-3.3f, -3.5f, 0.0f), sprite_, &sprite_shader_, tex_[19], tex_[1], 8, NULL);
	game_objects_.push_back(weapon_indicator);
	weapon_indi = weapon_indicator;

	TextGameObject* text = new TextGameObject(glm::vec3(0.0f, -3.5f, 0.0f), sprite_, &text_shader_, tex_[27], 20);
    text->SetScale(glm::vec2(6.0, 1.0));

    text->SetText("Press 'E' to Start");
	game_objects_.push_back(text);
    score_display_ = text;

    GameObject* title = new GameObject(glm::vec3(0.0f, 2.5f, 0.0f), sprite_, &sprite_shader_, tex_[23], tex_[1], 21, NULL);
    title->SetScale(glm::vec2(7.0, 1.0));
    game_objects_.push_back(title);
    title_ = title;
    //_____________________________________________________________________________

	//!/ Setup particle systems
	GameObject* exhaust_particle_1 = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), exhaust_, &particle_shader_, tex_[1], 0, game_objects_[1]);
	exhaust_particle_1->SetScale(glm::vec2(0.1f, 0.1f));
	game_objects_.push_back(exhaust_particle_1);

	GameObject* exhaust_particle_2 = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), exhaust_, &particle_shader_, tex_[1], 0, enemy1);
	exhaust_particle_2->SetScale(glm::vec2(0.1f, 0.1f));
	game_objects_.push_back(exhaust_particle_2);

	GameObject* exhaust_particle_3 = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), exhaust_, &particle_shader_, tex_[1], 0, enemy2);
	exhaust_particle_3->SetScale(glm::vec2(0.1f, 0.1f));
	game_objects_.push_back(exhaust_particle_3);
}

void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{
    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}

void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    //!/ Added explosion texture to tex_ and to all game objects
    glGenTextures(NUM_TEXTURES, tex_);

    //!/ Road, Explosion and Player Textures
    SetTexture(tex_[0], (resources_directory_g + std::string("/textures/road.png")).c_str());
    SetTexture(tex_[1], (resources_directory_g + std::string("/textures/explosion.png")).c_str());
    SetTexture(tex_[2], (resources_directory_g + std::string("/textures/playerleft.png")).c_str());

    SetTexture(tex_[3], (resources_directory_g + std::string("/textures/enemyleft.png")).c_str());
    SetTexture(tex_[4], (resources_directory_g + std::string("/textures/enemyright.png")).c_str());
    SetTexture(tex_[5], (resources_directory_g + std::string("/textures/enemyarm.png")).c_str());
    SetTexture(tex_[6], (resources_directory_g + std::string("/textures/bullet.png")).c_str());
    SetTexture(tex_[7], (resources_directory_g + std::string("/textures/orb.png")).c_str());

    //!/ Traffic Texture
	SetTexture(tex_[8], (resources_directory_g + std::string("/textures/oncoming_traffic.png")).c_str());
	SetTexture(tex_[9], (resources_directory_g + std::string("/textures/incoming_traffic.png")).c_str());

    //!/ Player Arm Textures
    SetTexture(tex_[10], (resources_directory_g + std::string("/textures/playerarm.png")).c_str());
    SetTexture(tex_[11], (resources_directory_g + std::string("/textures/playerright.png")).c_str());
	
    //!/ Extra Road and Texture
    SetTexture(tex_[12], (resources_directory_g + std::string("/textures/road2.png")).c_str());
	SetTexture(tex_[13], (resources_directory_g + std::string("/textures/grass.png")).c_str());

    //!/ Hud and Collectible Textures
    SetTexture(tex_[14], (resources_directory_g + std::string("/textures/distance_meter.png")).c_str());
    SetTexture(tex_[15], (resources_directory_g + std::string("/textures/health_indicator.png")).c_str());
    SetTexture(tex_[16], (resources_directory_g + std::string("/textures/health_indicator2.png")).c_str());
    SetTexture(tex_[17], (resources_directory_g + std::string("/textures/health_indicator3.png")).c_str());
	SetTexture(tex_[18], (resources_directory_g + std::string("/textures/distance_indicator.png")).c_str());
	SetTexture(tex_[19], (resources_directory_g + std::string("/textures/weapon_indicator_1.png")).c_str());
	SetTexture(tex_[20], (resources_directory_g + std::string("/textures/weapon_indicator_2.png")).c_str());
	SetTexture(tex_[21], (resources_directory_g + std::string("/textures/medkit.png")).c_str());
    SetTexture(tex_[22], (resources_directory_g + std::string("/textures/godmode.png")).c_str());
    SetTexture(tex_[23], (resources_directory_g + std::string("/textures/title.png")).c_str());

    //!/ Mini-Boss Textures
    SetTexture(tex_[25], (resources_directory_g + std::string("/textures/truck.png")).c_str());
    SetTexture(tex_[26], (resources_directory_g + std::string("/textures/turret.png")).c_str());

    //!/ Font Texture
	SetTexture(tex_[27], (resources_directory_g + std::string("/textures/font.png")).c_str());

    //!/ Intro Textures
    SetTexture(tex_[28], (resources_directory_g + std::string("/textures/player.png")).c_str());

    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}

void Game::MainLoop(void)
{

    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)) {

        // Clear background
        glClearColor(viewport_background_color_g.r,
            viewport_background_color_g.g,
            viewport_background_color_g.b, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view to zoom out, centered by default at 0,0
        float camera_zoom = 0.25f;
        glm::mat4 view_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));
        
        //!/ Change the poisition based on the player
        view_matrix /= glm::translate(glm::mat4(1.0f), glm::vec3(game_objects_[1]->GetPosition().x, 0.0f, 0.0f));

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update other events like input handling
        glfwPollEvents();

        // Update the game
        Update(view_matrix, delta_time);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);
    }

}

void Game::Update(glm::mat4 view_matrix, double delta_time)
{
    if (game_start) {
        // Update time
        current_time_ += delta_time;
        distance_ += distance_interval_;

        Controls(delta_time);

        // Update and render all game objects
        for (int i = 0; i < game_objects_.size(); i++) {
            // Get the current game object
            GameObject* current_game_object = game_objects_[i];

            // Update the current game object
            current_game_object->Update(delta_time);

            ExplosionHandle(current_game_object, current_time_, backgr);
            BackgroundHandle(current_game_object, delta_time);
            EnemyHandle(current_game_object);
            TrafficHandle(current_game_object);
            HUDHandle(current_game_object);
            BulletHandle(view_matrix, current_time_, game_objects_[1], game_objects_[0]->GetRotation());
            ScoreHandle(current_game_object);

            // Check for collision with other game objects
            // Note the loop bounds: we avoid testing the last object since
            // it's the background covering the whole game world
            for (int j = i + 1; j < (game_objects_.size() - 1); j++) {
                GameObject* other_game_object = game_objects_[j];

                if (game_objects_[1]->getState() != 2) {
                    CollectibleHandle(current_game_object, other_game_object);
                }

                // Compute distance between object i and object j
                float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());

                // If distance is below a threshold, we have a collision
                if (game_objects_[1]->getState() != 2) {
                    //!/ If an object is colliding under 0.6f
                    if (distance < 0.6f) {
                        CollisionHandle(current_game_object, other_game_object, 1, backgr);
                    }

                    //!/ If an object is colliding under 0.8f
                    else if (distance < 0.8f) {
                        CollisionHandle(current_game_object, other_game_object, 2, backgr);
                    }

                    //!/ If an object is colliding under 1.8f
                    else if (distance < 3.8f) {
                        CollisionHandle(current_game_object, other_game_object, 3, backgr);
                    }
                }

                //!/ If the current game object is the player and they are dead, exist
                if (current_game_object->getType() == 0) {
                    if (current_game_object->getState() != 3 && current_game_object->getStateTime() >= 1.0) {
                        glfwSetWindowShouldClose(window_, GLFW_TRUE);
                    }
                }

                //!/ Check the bullet again after everything else
                if (other_game_object->getType() == 4) {
                    CollisionHandle(current_game_object, other_game_object, 2, backgr);
                }
            }
        }

        //!/ Render each object depending on if its a child or not
        /*
        Layering is top to bottom, therefore we render the following from first to last

        -HUD
        -COLLECTIBLES
        -REMAINING
        */

        //!/ HUD
        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            if ((current_game_object->getType() >= 5 && current_game_object->getType() <= 8) || current_game_object->getType() == 20) {
                current_game_object->Render(view_matrix, current_time_);
            }
        }

        //!/ COLLECTIBLES
        for (int i = 0; i < game_objects_.size(); i++) {
            //!/ Get the current game object
            GameObject* current_game_object = game_objects_[i];
            if (current_game_object->getType() == 3) {
                current_game_object->Render(view_matrix, current_time_);
            }
        }

        //!/ REMAINING OBJECTS
        for (int i = 0; i < game_objects_.size(); i++) {
            //!/ Get the current game object
            GameObject* current_game_object = game_objects_[i];
            if (current_game_object->getType() == 4) {
                current_game_object->Render(view_matrix, current_time_);
            }
            else if (current_game_object->HasParent()) {
                if (current_game_object->GetParent()->getState() != 2 && current_game_object->GetParent()->getState() != -2) {
                    current_game_object->Render(view_matrix, current_time_);
                }
            }
            else {
                current_game_object->Render(view_matrix, current_time_);
            }
        }
    }

    //!/ MAIN MENU
    else if (!game_start && !intro_start){
        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            current_game_object->Update(delta_time);

            BackgroundHandle(current_game_object, delta_time);
        }
        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            if (current_game_object->getType() == 20 || current_game_object->getType() == 21) {
                current_game_object->Render(view_matrix, current_time_);
            }
        }
        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            if (current_game_object->getType() <= -1) {
                current_game_object->Render(view_matrix, current_time_);
            }
        }
        if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS) {
            intro_start = true;
            game_objects_[1]->SetTexture(tex_[28]);
            score_display_->SetScale(glm::vec2(3.0, 0.5));
        }
    }

    //!/ INTRO CUTSCENE
    else if (intro_start) {
        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            current_game_object->Update(delta_time);

            if (current_game_object->getType() == 0) {
                current_game_object->SetVelocity(glm::vec3(0.0f, 2.0f, 0.0f));
                if (current_game_object->GetPosition().y >= 6.0f) {
                    current_game_object->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                    game_objects_[7]->SetState(-1);
                    game_objects_[9]->SetState(-1);
                    game_start = true;
                    intro_start = false;
                }
            }
            BackgroundHandle(current_game_object, delta_time);
        }

        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            if (current_game_object->getType() == 0) {
                current_game_object->Render(view_matrix, current_time_);
            }
        }
        for (int i = 0; i < game_objects_.size(); i++) {
            GameObject* current_game_object = game_objects_[i];
            if (current_game_object->getType() <= -1) {
                current_game_object->Render(view_matrix, current_time_);
            }
        }
    }
}

//??// Method to display the score
void Game::ScoreHandle(GameObject* firstObject) {
    std::string str = "Score: " + std::to_string(score);
    score_display_->SetText(str);
    //!/ If the score is above 1000
    if (score >= 1000){
        std::cout << "[END] You killed them all, and the remaining ran... do you... do you have any regrets?\nScore: " << score << "\nKills: " << total_kills_ << "\nTime Played: " << current_time_ << "s" << std::endl;
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

//??// Method to handle explosions
void Game::ExplosionHandle(GameObject* firstObject, double current_time_, GameObject* backgr) {

    //!/ Handle if a object is exlpoding
    if (firstObject->IsExploding() && firstObject->explosion_particles_ == NULL) {
        GameObject* explosion_part_ = new ParticleSystem(firstObject->GetPosition(), explosion_, &particle_shader_, tex_[7], 2, NULL);
        explosion_part_->SetScale(glm::vec2(0.5f, 0.5f));
        game_objects_.push_back(explosion_part_);
        firstObject->explosion_particles_ = explosion_part_;
    }
    //!/ Handle if a object is done exploding
    if (!firstObject->IsExploding() && firstObject->explosion_particles_ != NULL) {
        game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), firstObject->explosion_particles_), game_objects_.end());
        firstObject->explosion_particles_ = NULL;
    }
}

//??// Method to handle background movement
void Game::BackgroundHandle(GameObject* firstObject, double delta_time) {
    //!/ Background road movement
    float mov = 5.0f;

    if (game_objects_[0]->GetVelocity().y > 1.0f) {
        distance_interval_ = 0.0007f;
        mov = 9.0f;
    }
    else if (game_objects_[0]->GetVelocity().y < -1.0f) {
		distance_interval_ = 0.0003f;
        mov = 3.5f;
    }
    
    //!/ Move the background vertically
    if (firstObject->getType() == -1 || firstObject->getType() == -2 || firstObject->getType() == -3 || firstObject->getType() == -4) {
        glm::vec3 pos = firstObject->GetPosition();
        int type = firstObject->getType();
        if (type == -1) { firstObject->SetPosition(glm::vec3(pos.x, pos.y -= mov * delta_time, 0.0f)); } //!/ Road 1 positioning
        if (type == -2) { firstObject->SetPosition(glm::vec3(pos.x, pos.y -= mov * delta_time, 0.0f)); } //!/ Right Grass positioning
        if (type == -3) { firstObject->SetPosition(glm::vec3(pos.x, pos.y -= mov * delta_time, 0.0f)); } //!/ Right Grass positioning
        if (type == -4) { firstObject->SetPosition(glm::vec3(pos.x, pos.y -= mov * delta_time, 0.0f)); } //!/ Left Grass positioning

        if (pos.y < -40.0f) {
            firstObject->SetPosition(glm::vec3(pos.x, 50.0f, 0.0f));
            
        }
    }
}

//??// Method to handle Collectible interaction
void Game::CollectibleHandle(GameObject* firstObject, GameObject* secondObject) {
    
    if (current_time_ >= timer_time_ && firstObject->getType() == 0 && firstObject->HasNewWeapon()){
        firstObject->SetWeapon(false);
        std::cout << "[PICKUP] You dropped the Uzi." <<std::endl;
		player_bullet_time_interval_ = 0.002f;
		weapon_indi->SetTexture(tex_[19]);
        timer_time_ = 0.0;
    }
    if (current_time_ >= invincible_timer_time_ && firstObject->getType() == 0 && firstObject->IsInvincible()){
        firstObject->SetInvincible(false);
        firstObject->SetState(0);
        firstObject->SetTextCount(3);
		health_indi->SetTexture(tex_[15]);
        invincible_timer_time_ = 0.0;
    }
    
    if (firstObject->getType() == 3 && firstObject->getState() == 0 && firstObject->getCollectType() != 2){

        glm::vec3 pos = firstObject->GetPosition();
        firstObject->SetPosition(glm::vec3(pos.x, pos.y - 0.0005f, pos.z));

        if (pos.y <= -6.0f) {
            firstObject->SetState(2);
        }
    }
    if (firstObject->getType() == 0 && firstObject->GetTextCount() == 1 && secondObject->getType() == 3 && secondObject->getCollectType() == 2 && secondObject->getState() == 0){
		glm::vec3 pos = secondObject->GetPosition();
        secondObject->SetPosition(glm::vec3(pos.x, pos.y - 0.005f, pos.z));

		if (pos.y <= -6.0f) {
			secondObject->SetState(2);
		}
    } else if (firstObject->getType() == 0 && firstObject->GetTextCount() >= 2 && secondObject->getType() == 3 && secondObject->getCollectType() == 2 && secondObject->getState() == 0){
        secondObject->SetState(2);
    }


}

//??// Method to handle Enemy interaction
void Game::EnemyHandle(GameObject* firstObject) {

    //!/ Enemy Gun Handling
    //! (If the game_object is a child (arm) or the object is the turret of the truck)
    if (firstObject->HasParent() && firstObject->getType() == 2 || firstObject->getType() == 12) {
        firstObject->SetTarget(game_objects_[0]->GetPosition());
    }

    //!/ If a boss is coming, prepare it. If a boss is dying erase him from existence.
    if (firstObject->getType() == 11 && firstObject->getState() == -2 && getBossStatus() == true) {
        std::cout << "[ENEMY] Technical Incoming!" << std::endl;
        Game::setBossHealth(3);
        firstObject->SetState(-1);
    }
    else if ((firstObject->getType() == 11 && firstObject->getState() == -1) && getBossStatus() == false) {
        firstObject->SetState(-2);
    }

    //!/ If a enemy is waiting for the boss car and boss car is gone, prepare the enemy.
    if ((firstObject->getType() == 1 && firstObject->getState() == -2) && getBossStatus() == false) {
        firstObject->SetPosition(glm::vec3(firstObject->GetPosition().x, -5.0, 0.0f));
        firstObject->SetState(2);
    }
    else if (firstObject->getType() == 1 && (firstObject->getState() == 2 && firstObject->getStateTime() > (firstObject->GetScale().x * 2)) && getBossStatus() == true) {
        firstObject->SetState(-2);
    }

	//!/ Enemy Sprite Arm to Player Position Handling
	if (firstObject->getType() == 2 && firstObject->GetParent()->getType() == 1 && firstObject->GetParent()->getState() != 2) {
		if (firstObject->GetSide() == 0) {
			if (firstObject->GetRotation() > 25.0f && firstObject->GetRotation() < 155.0f) {
				(firstObject->GetParent())->SetTexture(tex_[3]);
				firstObject->SetSide(1);
			}
		}
		if (firstObject->GetSide() == 1) {
			if (firstObject->GetRotation() < 335.0f && firstObject->GetRotation() > 205.0f) {
				(firstObject->GetParent())->SetTexture(tex_[4]);
				firstObject->SetSide(0);
			}
		}
	}

    //!/ Move enemy away from traffic
	if ((firstObject->getType() == 1) && firstObject->getState() != 2) {
		for (int j = 0; j < game_objects_.size(); ++j) {
			GameObject* other_game_object = game_objects_[j];
			if (other_game_object->getType() == 9 || other_game_object->getType() == 10) {
				if (other_game_object->IsMoving() == true && firstObject->GetLane() == other_game_object->GetLane()) {

					glm::vec3 pos = firstObject->GetPosition();
					int curlane = firstObject->GetLane();

                    //!/ Check the lane
					if (curlane == 0) {
						firstObject->SetVelocity(glm::vec3(0.5f, firstObject->GetVelocity().y, 0.0f));
					}
					else if (curlane == 1) {
						firstObject->SetVelocity(glm::vec3(-0.5f, firstObject->GetVelocity().y, 0.0f));
					}
					else if (curlane == 2) {
						firstObject->SetVelocity(glm::vec3(0.5f, firstObject->GetVelocity().y, 0.0f));
					}
					else if (curlane == 3) {
						firstObject->SetVelocity(glm::vec3(-0.5f, firstObject->GetVelocity().y, 0.0f));
					}

                    //!/ Alternate between lanes. 0 and 1 are incoming traffic, 2,3 are oncoming.
					if (firstObject->GetInitialPos().x > pos.x) {
						if (fabs(firstObject->GetInitialPos().x - pos.x) >= 1.0f) {
							firstObject->SetVelocity(glm::vec3(0.0f, firstObject->GetVelocity().y, 0.0f));
							if (curlane == 0) {
								firstObject->SetInitialPos(pos);
								firstObject->SetLane(1);
							}
							else if (curlane == 1) {
								firstObject->SetInitialPos(pos);
								firstObject->SetLane(0);
							}
							else if (curlane == 2) {
								firstObject->SetInitialPos(pos);
								firstObject->SetLane(3);
							}
							else if (curlane == 3) {
								firstObject->SetInitialPos(pos);
								firstObject->SetLane(2);
							}
						}
					}
					else if (fabs(pos.x - firstObject->GetInitialPos().x) >= 1.0f) {
						firstObject->SetVelocity(glm::vec3(0.0f, firstObject->GetVelocity().y, 0.0f));
						if (curlane == 0) {
							firstObject->SetInitialPos(pos);
							firstObject->SetLane(1);
						}
						else if (curlane == 1) {
							firstObject->SetInitialPos(pos);
							firstObject->SetLane(0);
						}
						else if (curlane == 2) {
							firstObject->SetInitialPos(pos);
							firstObject->SetLane(3);
						}
						else if (curlane == 3) {
							firstObject->SetInitialPos(pos);
							firstObject->SetLane(2);
						}
					}
				}
			}
		}
	}
}

//??// Method to handle Traffice interaction
void Game::TrafficHandle(GameObject* firstObject) {
	//!/ Controlling Oncoming Traffic
	if (firstObject->getType() == 9) {
		glm::vec3 pos;
		if (cur_oncoming_car_ == -1) {
			cur_oncoming_car_ = (rand() % 2 + 1);
			cur_oncoming_car_ += 1;
			game_objects_[cur_oncoming_car_]->SetMoving(true);
			//std::cout << "Car in Lane " << game_objects_[cur_oncoming_car_]->GetLane() << " oncoming" << std::endl;
		}
		else if (cur_oncoming_car_ > -1) {
			glm::vec3 vel = game_objects_[cur_oncoming_car_]->GetVelocity();
			pos = game_objects_[cur_oncoming_car_]->GetPosition();

			if (game_objects_[cur_oncoming_car_]->GetPosition().y <= -8.0f) {
				game_objects_[cur_oncoming_car_]->SetPosition(glm::vec3(pos.x, 12.0f, pos.z));
				game_objects_[cur_oncoming_car_]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
				game_objects_[cur_oncoming_car_]->SetMoving(false);
				cur_oncoming_car_ = -1;

			}
			else {
				game_objects_[cur_oncoming_car_]->SetVelocity(glm::vec3(vel.x, -1.0f, vel.z));
			}

		}
	}
	//!/ Controlling Incoming Traffic
	if (firstObject->getType() == 10) {
		glm::vec3 pos;
		if (cur_incoming_car_ == -1) {
			cur_incoming_car_ = (rand() % 2 + 1);
			cur_incoming_car_ += 3;
			//std::cout << "Car in Lane " << game_objects_[cur_incoming_car_]->GetLane() << " incoming" << std::endl;
			game_objects_[cur_incoming_car_]->SetMoving(true);
		}

		else if (cur_incoming_car_ > -1) {
			glm::vec3 vel = game_objects_[cur_incoming_car_]->GetVelocity();
			pos = game_objects_[cur_incoming_car_]->GetPosition();

			if (game_objects_[cur_incoming_car_]->GetPosition().y <= -8.0f) {
				game_objects_[cur_incoming_car_]->SetPosition(glm::vec3(pos.x, 12.0f, pos.z));
				game_objects_[cur_incoming_car_]->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
				game_objects_[cur_incoming_car_]->SetMoving(false);
				cur_incoming_car_ = -1;

			}
			else {
				//std::cout << "CAR " << cur_incoming_car_ << "MOVED." << std::endl;
				game_objects_[cur_incoming_car_]->SetVelocity(glm::vec3(vel.x, -40.0f, vel.z));
			}

		}
	}
}

//??// Method to display the HUD
void Game::HUDHandle(GameObject* firstObject) {

	if (firstObject->getType() == 5) {
		firstObject->SetPosition(glm::vec3(game_objects_[1]->GetPosition().x, 1.0f, 0.0f));
	}
	if (firstObject->getType() == 6) {
		firstObject->SetPosition(glm::vec3(game_objects_[1]->GetPosition().x + distance_, 3.2f, 0.0f));
        if (firstObject->GetPosition().x >= 4.0f){
            std::cout << "[END] You travelled all this way... to reach Canada!\nScore: "<< score <<"\nKills: " << total_kills_ << "\nTime Played: " << current_time_ << "s" << std::endl;
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
        }
	}
	if (firstObject->getType() == 7) {
		firstObject->SetPosition(glm::vec3(game_objects_[1]->GetPosition().x - 3.4f, -3.5f, 0.0f));
	}
	if (firstObject->getType() == 8) {
		firstObject->SetPosition(glm::vec3(game_objects_[1]->GetPosition().x + 3.4f, -3.5f, 0.0f));
	}
	if (firstObject->getType() == 20) {
		firstObject->SetPosition(glm::vec3(game_objects_[1]->GetPosition().x, -3.5f, 0.0f));
	}
}

//??// Method to handle Collision in between all objects
void Game::CollisionHandle(GameObject* firstObject, GameObject* secondObject, int version, GameObject* backgr) {

    //!/ If the player collects a collectible
    if (version == 1) {
        if (firstObject->getType() == 0 && secondObject->getType() == 3){
            //!/ UZI
            if (secondObject->getCollectType() == 0 && firstObject->HasNewWeapon() == false) {
                timer_time_ = current_time_ + 20.0;
				firstObject->SetWeapon(true);
                std::cout << "[PICKUP] Uzi Collected!" << std::endl;
                secondObject->SetState(1);
                weapon_indi->SetTexture(tex_[20]);
            }
            //!/ Medkit
            if (secondObject->getCollectType() == 1 && !firstObject->IsInvincible()){
                secondObject->SetState(1);
                firstObject->SetState(4);
                std::cout << "[PICKUP] Medkit Collected!" << std::endl;
                health_indi->SetTexture(tex_[15]);
				firstObject->SetTextCount(3);
            }
            //!/ Godmode
            if (secondObject->getCollectType() == 2) {
                firstObject->SetInvincible(true);
                invincible_timer_time_ = current_time_ + 10.0;
                secondObject->SetState(1);
                std::cout << "[PICKUP] Godmode Helmet ON!" << std::endl;
                health_indi->SetTexture(tex_[22]);
            }
        }
    }
    //!/ If the an object collides
    else if (version == 2) {

        //!/ If the player is in godmode you are invincible
        if (firstObject->getType() == 0 && firstObject->getState() == 3) {
            secondObject->SetState(1);
            return;
        }

        //!/ If the player is hit by a car they die instantly 
        if (firstObject->getType() == 0) {
            if ((secondObject->getType() == 9 || secondObject->getType() == 10) && secondObject->getState() != 2) {
                if (!firstObject->IsInvincible()) {
                    firstObject->SetState(2);
                    firstObject->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                    std::cout << "[END] You hit a car, maybe you weren't suited for the motorcycle life.\nScore: " << score << "\nKills: " << total_kills_ << "\nTime Played: " << current_time_ << "s" << std::endl;
                }
                secondObject->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
                secondObject->SetState(2);
            }
        }

        //!/ If the player or enemy is colliding with a bullet
        if ((firstObject->getType() == 0 || firstObject->getType() == 1 || firstObject->getType() == 11 || firstObject->getType() == 9 || firstObject->getType() == 10) && secondObject->getType() == 4) {
            if (firstObject->getType() == 0 && firstObject->IsInvincible()) { return; }
            if (firstObject->bullet_ != secondObject || (firstObject->bullet_ == secondObject && secondObject->IsReflectedBullet())) {
                
                //!/ Calculate the ray-circle collision using the quadratic formula.
                float x0 = secondObject->GetPosition().x;
                float y0 = secondObject->GetPosition().y;

                glm::vec3 nextpos = secondObject->GetPosition() + (secondObject->GetVelocity());
                
                float x1 = nextpos.x;
                float y1 = nextpos.y;

                float Cx = firstObject->GetPosition().x;
                float Cy = firstObject->GetPosition().y;

                float r = 0.5f;

                //!/ Calculate coefficients of quadratic equation
                float ax = pow((x1 - x0), 2);
                float bx = 2 * (x0 - Cx) * (x1 - x0);
                float cx = pow((x0 - Cx), 2) - r * r;
                float xdiscriminant = (pow(bx, 2) - 4 * (ax) * (cx));

                float ay = pow((y1 - y0), 2);
                float by = 2 * (y0 - Cy) * (y1 - y0);
                float cy = pow((y0 - Cy), 2) - r * r;
                float ydiscriminant = (pow(by, 2) - 4 * (ay) * (cy));

                float tx_pos = NULL;
                float tx_neg = NULL;
                float ty_pos = NULL;
                float ty_neg = NULL;

                float high_x = NULL;
                float low_x = NULL;
                float high_y = NULL;
                float low_y = NULL;

                if (xdiscriminant > 0.0f) {
                    float tx_pos = ((-1) * bx - sqrt(xdiscriminant)) / (2 * ax);
                    float tx_neg = ((-1) * bx + sqrt(xdiscriminant)) / (2 * ax);
                    tx_pos = x0 + tx_pos * (x1 - x0);
                    tx_neg = x0 + tx_neg * (x1 - x0);

                    if (tx_pos > tx_neg) { low_x = tx_neg; high_x = tx_pos; }
                    else { low_x = tx_pos; high_x = tx_neg; }
                }
                if (ydiscriminant > 0.0f){
                    float ty_pos = ((-1) * by - sqrt(ydiscriminant)) / (2 * ay);
                    float ty_neg = ((-1) * by + sqrt(ydiscriminant)) / (2 * ay);
                    ty_pos = y0 + ty_pos * (y1 - y0);
                    ty_neg = y0 + ty_neg * (y1 - y0);

                    if (ty_pos > ty_neg) { low_y = ty_neg; high_y = ty_pos; }
                    else { low_y = ty_pos; high_y = ty_neg; }
                }

                if (low_x != NULL && high_x != NULL && low_y != NULL && high_y != NULL) {
                    if (((secondObject->GetPosition().x >= low_x && secondObject->GetPosition().x <= high_x) && (secondObject->GetPosition().y >= low_y && secondObject->GetPosition().y <= high_y))) {
                       
                        //!/ If the gameobject is not colliding change the health indi
                        if (firstObject->getType() != 9 && firstObject->getType() != 10) {
                            if (firstObject->getType() == 0) {
                                //Texture Count helps keep track of the texture health indicator, but it is also used for determing the player's health at specific segments of the game.
                                if (firstObject->GetTextCount() == 3) {
                                    health_indi->SetTexture(tex_[16]);
                                    firstObject->SetTextCount(firstObject->GetTextCount() - 1);
                                    score -= 100;
                                }
                                else if (firstObject->GetTextCount() == 2) {
                                    health_indi->SetTexture(tex_[17]);
                                    firstObject->SetTextCount(firstObject->GetTextCount() - 1);
                                    score -= 100;
                                }
                                else if (firstObject->GetTextCount() == 1) {
                                    health_indi->SetTexture(tex_[17]);
                                    firstObject->SetTextCount(firstObject->GetTextCount() - 1);
                                    score -= 100;
                                }
                                else if (firstObject->GetTextCount() == 0){
                                    firstObject->SetTextCount(firstObject->GetTextCount() - 1);
                                    std::cout << "[END] You bleed out, full of bullet holes. You start to see white, acknowledging you have never fled Ohio.\nScore: " << score << "\nKills: " << total_kills_ << "\nTime Played: " << current_time_ << "s" << std::endl;
                                    glfwSetWindowShouldClose(window_, GLFW_TRUE);
                                }
                            }

                            //!/ If the bullet is colliding with a enemy
                            if (firstObject->getType() == 1 && getBossStatus() == false) {
                                srand(time(0));
                                int random_num = rand() % 100;
                                if (random_num < 10) {
                                    setBossStatus(true);
                                }
                            }

                            //!/ If the bullet is colliding with a boss
                            if (firstObject->getType() == 11 && getBossStatus() == true) {
                                if (Game::getBossHealth() == 1) {
                                    std::cout << "[BULLET] Boss Destroyed!" << std::endl;
                                    Game::setBossStatus(false);
                                    firstObject->SetState(1);
                                    score += 300;
                                    total_kills_ += 2;
                                }
                                else {
                                    std::cout << "[BULLET] Boss Hit!" << std::endl;
                                    Game::setBossHealth(Game::getBossHealth() - 1);
                                    score += 150;
                                }
                            }
                            else if (firstObject->getType() == 1) {
                                std::cout << "[BULLET] Enemy Destroyed!" << std::endl;
                                firstObject->SetState(1);
                                score += 100;
                                total_kills_ += 1;
                            }

                            //!/ Erase the bullets from the vector list
                            game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), secondObject), game_objects_.end());
                            game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), secondObject->particle_), game_objects_.end());

                            //If the bullet hits a car, the bullet ricochets off the car.
                        } else if (secondObject->IsReflectedBullet() == false && (firstObject->getType() == 9 || firstObject->getType() == 10)) {                            
                            glm::vec2 intersectionPoint1(low_x, low_y);
                            glm::vec2 intersectionPoint2(high_x, high_y);

                            //!/ Choose the intersection point closest to the bullet position
                            glm::vec2 closestIntersectionPoint;
                            if (glm::distance(glm::vec2(x0, y0), intersectionPoint1) < glm::distance(glm::vec2(x0, y0), intersectionPoint2)) {
                                closestIntersectionPoint = intersectionPoint1;
                            }
                            else {
                                closestIntersectionPoint = intersectionPoint2;
                            }

							//!/ Calculate the normal vector of the circle's surface at the intersection point
							glm::vec2 surface = glm::normalize(glm::vec2(2.0f * closestIntersectionPoint.x, 2.0f * closestIntersectionPoint.y));

                            glm::vec3 dir = glm::normalize(secondObject->getTarget() - secondObject->GetPosition());
							//!/ Calculate the reflection vector
							glm::vec3 ricochet = dir - 2.0f * glm::dot(dir, glm::vec3(surface, 0.0f)) * glm::vec3(surface, 0.0f);
                            glm::vec3 endpoint = secondObject->GetPosition() + glm::normalize(ricochet) * 10.0f;
							
                            //!/ Set everything for reflection
                            secondObject->SetTarget(endpoint);
                            secondObject->SetState(0);
							secondObject->SetReflected(true);

                        }
                    }
                }
            }
        }
    }
    else if (version == 3) {
		//!/ If an arm is 3.8f to the player, create, erase or just move the bullet
		if ((firstObject->getType() == 0 && firstObject->getState() == 0) && (secondObject->getType() == 2 || secondObject->getType() == 12) && secondObject->GetParent()->getState() != 2 && secondObject->GetParent()->getState() != -2) {
			if (secondObject->GetBulletTime() == 0.0f) {
				GameObject* bullet = new BulletGameObject(secondObject->GetPosition(), sprite_, &sprite_shader_, tex_[6], tex_[1], 4, game_objects_[1]->GetPosition());
				GameObject* bullet_part_ = new ParticleSystem(glm::vec3(0.0f,0.0f,0.0f), bullet_particle_, &particle_shader_, tex_[1], 3, bullet);

				secondObject->bullet_ = bullet;
                secondObject->GetParent()->bullet_ = bullet;

				auto it = find(game_objects_.begin(), game_objects_.end(), backgr);
				game_objects_.insert(it, bullet);

				bullet_part_->SetRotation(bullet->GetRotation());
				bullet_part_->SetScale(glm::vec2(0.1f, 0.1f));
                bullet->particle_ = bullet_part_;

				game_objects_.push_back(bullet_part_);

                if (secondObject->getType() == 12) {
                   secondObject->AddBulletTime(0.1);
                }
                else {
                    secondObject->AddBulletTime(0.027);
                }
				
			}
			else if (secondObject->GetBulletTime() >= 40.0f) {
                secondObject->bullet_ = NULL;
                secondObject->GetParent()->bullet_ = NULL;
				game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), secondObject->bullet_), game_objects_.end());
				game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), secondObject->particle_), game_objects_.end());
				secondObject->ResetBulletTime();
			}
			else {
                if (secondObject->getType() == 12) {
                    secondObject->AddBulletTime(0.1);
                }
                else {
                    secondObject->AddBulletTime(0.027);
                }
			}
		}
    }
}

//??// Method to handle player bullets
void Game::BulletHandle(glm::mat4& view_matrix, double time, GameObject* player, float angle) {
    //!/ KEY PRESS HANDLE
	if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (player->GetBulletTime() == 0.0f) {
			//!/ Create a bullet and it's exhaust particles
            glm::vec3 end_point = player->GetPosition() + glm::vec3(10.0f * cos(glm::radians(angle + 90)), 10.0f * sin(glm::radians(angle + 90)), 0.0f);

            GameObject* bullet = new BulletGameObject(glm::vec3(player->GetPosition().x - 0.1, player->GetPosition().y - 0.15, 0.0f), sprite_, &sprite_shader_, tex_[6], tex_[1], 4, end_point);
			GameObject* bullet_part_ = new ParticleSystem(glm::vec3(0.0f, 0.0f, 0.0f), bullet_particle_, &particle_shader_, tex_[7], 3, bullet);
			
            //!/ Set the player's bullet at the one created
			player->bullet_ = bullet;
            
			auto it = find(game_objects_.begin(), game_objects_.end(), backgr);
			game_objects_.insert(it, bullet);

			bullet_part_->SetRotation(bullet->GetRotation());
			bullet_part_->SetScale(glm::vec2(0.05f, 0.05f));
            bullet->particle_ = bullet_part_;

			game_objects_.push_back(bullet_part_);

            if (player->HasNewWeapon() == true) {
                player->AddBulletTime(0.06);
            }
            else {
                player->AddBulletTime(0.015);
            }
        }
    }
    else {
        //!/ If the Bullet State == 1 and the appointed timer is less than the current time, stop movement. This scenario occurs WHEN the rocket hit no one, 
        if (player->GetBulletTime() >= 400.0f) {
            game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), player->bullet_), game_objects_.end());
            game_objects_.erase(std::remove(game_objects_.begin(), game_objects_.end(), player->bullet_->particle_), game_objects_.end());
            player->ResetBulletTime();
        }
        else if (player->GetBulletTime() != 0.0f) {
            if (player->HasNewWeapon() == true) {
                player->AddBulletTime(0.06);
            }
            else {
                player->AddBulletTime(0.015);
            }
        }
    }
    
}

//??// Method to handle player controls
void Game::Controls(double delta_time)
{
    // Get player game object
    GameObject* player = game_objects_[1];

    //!/ If the user wants out, let them out
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        std::cout << "[END] You left the roads, what does your fate have in store?\nScore: " << score << "\nKills: " << total_kills_ << "\nTime Played: " << current_time_ << "s" << std::endl;
        glfwSetWindowShouldClose(window_, true);
    }

    //!/ Get current velocity and rotation
    glm::vec3 vel = player->GetVelocity();
    float rot = player->GetRotation();

    //!/ Calculate the acceleration based on the rotation
    glm::vec3 acc = glm::vec3(-sin(glm::radians(rot)), cos(glm::radians(rot)), 0.0f);

    // Adjust motion increment based on a given speed
    float speed = 3.0f;
    float rotation_speed = 250.0f;
    float motion_increment = speed * delta_time;

    //!/ If the player is destroyed, they cannot move
    if (player->getState() != 2 && player->GetParent() == NULL) {

        if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
            player->SetVelocity(vel + acc * motion_increment);
        }
        if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
            player->SetVelocity(vel - acc * motion_increment);
        }
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window_, GLFW_KEY_D) != GLFW_PRESS) {
            player->SetVelocity(vel - glm::vec3(1.0, 0.0, 0.0) * motion_increment);
            player->SetRotation(20.0f);
        }
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window_, GLFW_KEY_A) != GLFW_PRESS) {
            player->SetVelocity(vel + glm::vec3(1.0, 0.0, 0.0) * motion_increment);
            player->SetRotation(-20.0f);
        }
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_RELEASE && glfwGetKey(window_, GLFW_KEY_D) != GLFW_PRESS) {
            player->SetRotation(0.0f);
        }
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_RELEASE && glfwGetKey(window_, GLFW_KEY_A) != GLFW_PRESS) {
            player->SetRotation(0.0f);
        }
		if (glfwGetKey(window_, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (game_objects_[0]->GetRotation() < 120.0f) {
                game_objects_[0]->SetRotation(game_objects_[0]->GetRotation() + 0.2f);
            } else {
                game_objects_[0]->SetRotation(119.0f);
            }
		}
		if (glfwGetKey(window_, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			if (game_objects_[0]->GetRotation() > -120.0f) {
				game_objects_[0]->SetRotation(game_objects_[0]->GetRotation() - 0.2f);
			}
		}

		glm::vec3 arm_pos = game_objects_[0]->GetPosition();
		if (game_objects_[0]->GetRotation() >= 0.0f) {
			player->SetTexture(tex_[2]);
		} 
        else {
			player->SetTexture(tex_[11]);
		}



        //!/ Check if the player is within the bounds
        const float maxY = 1.5f, minY = -3.5f, maxX = 5.0f, minX = -5.0f;
        glm::vec3 pos = player->GetPosition();
        if (pos.y > maxY) {
            player->SetPosition(glm::vec3(pos.x, maxY, 0.0f));
        }
        else if (pos.y < minY) {
            player->SetPosition(glm::vec3(pos.x, minY, 0.0f));
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        if (pos.x > maxX) {
            player->SetPosition(glm::vec3(maxX, pos.y, 0.0f));
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        }
        else if (pos.x < minX) {
            player->SetPosition(glm::vec3(minX, pos.y, 0.0f));
            player->SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        }
    }
}
       
} // namespace game
