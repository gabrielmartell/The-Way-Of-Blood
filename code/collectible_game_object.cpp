#include <iostream>

#include "collectible_game_object.h"

namespace game {

	/*
		CollectibleGameObject inherits from GameObject
	*/

	CollectibleGameObject::CollectibleGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GLuint explosion, int type, int collecttype)
		: GameObject(position, geom, shader, texture, explosion, type, NULL) {
		collect_type_ = collecttype;
		GameObject::SetScale(glm::vec2(0.45f,0.45f));
        srand(time(0));
	}

	// Update function for moving the player object around
	void CollectibleGameObject::Update(double delta_time) {

		//!/ If the collectible is colliding kill it so it can respawn
		if (GameObject::getState() == 1) {
			GameObject::SetPosition(glm::vec3(GetPosition().x, 6.0f, 0.0f));
			GameObject::SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			GameObject::SetState(2);
		}
		//!/ If the collectible is destroyed, wait to place it back on the window
		else if(GameObject::getState() == 2) {
			if (GameObject::getStateTime() > (scale_.x * 2.0f)) {
				GameObject::resetStateTime(0.0);
				GameObject::SetState(0);
				GameObject::SetPosition(glm::vec3((rand() % 4) - 2.33, 6.0f, 0.0f));
			}
			else {
				GameObject::addStateTime(0.0007);
				return;
			}
		}	
	}
} // namespace game
