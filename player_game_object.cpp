#include <iostream>

#include "player_game_object.h"

namespace game {

/*
	PlayerGameObject inherits from GameObject
	It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

PlayerGameObject::PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GLuint explosion, int type, GLuint inv, int health, GameObject* parent)
	: GameObject(position, geom, shader, texture, explosion, type, parent) { 
	scale_ = glm::vec2(1.3f, 2.0f);
	health_ = health;
	rotation_ = 0.0f;
	collected_ = 0;
	ori_texture_ = texture;
	inv_ = inv;
	side_ = 0;
	bullet_cooldown_ = 0.0f;
}

// Update function for moving the player object around
void PlayerGameObject::Update(double delta_time) {

	//!/ If the object is the player
	if (GameObject::HasParent() != true) {

		//!/ If the player is colliding, check the health
		if (GameObject::getState() == 1) {
			//!/ If the player has taken three hits, classify them as destroyed.
			if (gethealth() == 1) {
				//!/ They are destroyed, the game is over and they cannot collide or move anymore
				GameObject::SetState(2);
				GameObject::SetVelocity(glm::vec3(0.0f,0.0f,0.0f));
				GameObject::SetRotation(0);
				//std::cout << "Game Over! (You did not escape Ohio...)\n" << std::endl;
			}
			//!/ If not, damage them, so they learn the lesson. Also stop them from colliding.
			else {
				std::cout << "[BULLET] Player has Taken a Hit!" << std::endl;
				damage();
				GameObject::SetState(0);
			}
		}
		else if (GameObject::getState() == 4){
			health_ = 3;
            GameObject::SetState(0);
		}
	}
	else {
        glm::vec3 arm_pos = GameObject::GetPosition();
        if (GameObject::GetRotation() >= 0.0f) {
            GameObject::SetPosition(glm::vec3(GameObject::GetParent()->GetPosition().x - 0.1f, GameObject::GetParent()->GetPosition().y - 0.15f, 0.0f));
        }
        if (GameObject::GetRotation() < 0.0f) {
            GameObject::SetPosition(glm::vec3(GameObject::GetParent()->GetPosition().x + 0.1f, GameObject::GetParent()->GetPosition().y - 0.15f, 0.0f));
        }
	}

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

} // namespace game
