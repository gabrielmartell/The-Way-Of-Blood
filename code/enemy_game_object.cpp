#include <iostream>

#include "enemy_game_object.h"

namespace game {

	/*
		EnemyGameObject inherits from GameObject
		It overrides GameObject's update method, so that you can check for input to change the velocity of the enemy
	*/

	EnemyGameObject::EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GLuint explosion, int type, int side, int lane, GameObject* parent)
		: GameObject(position, geom, shader, texture, explosion, type, parent) { 
		srand(time(NULL));
		scale_ = glm::vec2(1.3f, 2.0f);
		side_ = side;
		lane_ = lane;
		bullet_cooldown_ = 0.0f;
		moving_ = false;
		ver_dir = false;
	}

	// Update function for moving the player object around
	void EnemyGameObject::Update(double delta_time) {

		//!/ If a boss is present, the enemy can't join the fight
		if (GameObject::getState() == -2) {
			SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
		}
		//!/ If the enemy is just spawning, move him up.
		else if (GameObject::getState() == -1) {
			
			if (GameObject::getType() != 9 && GameObject::getType() != 10) {

				GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.8, 0.0f));

				if (GetSide() == 0) {

					if (GetPosition().y > 1.55) {
						GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.0f, 0.0f));
						ver_dir = true;
						GameObject::SetState(0);
					}
				}
				else if (GetSide() == 1) {

					if (GameObject::GetPosition().y > -1.55) {
						GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.0f, 0.0f));
						ver_dir = false;
						GameObject::SetState(0);
					}
				}
			}
			else {
				GameObject::SetState(0);
				GameObject::SetPosition(glm::vec3(GameObject::GetPosition().x, -8.1f, 0.0f));
			}
		}
		//!/ If the enemy is in driving state, make it drive down or up
		else if (GameObject::getState() == 0) {

			//!/ Check if the car is on the left or right
			if (GameObject::GetSide() == 0) {
				//!/ Set the velocity and rotation of left traffic
				GameObject::SetRotation(0);
				if (GameObject::HasParent()) {
					GameObject::SetPosition(glm::vec3(GameObject::GetParent()->GetPosition().x + 0.1f, GameObject::GetParent()->GetPosition().y - 0.15f, 0.0f));
					float x = target_.x - position_.x;
					float y = target_.y - position_.y;
					float angle = glm::degrees(glm::atan(y / x));

					if (x < 0 && y < 0) {
						angle = 270 + angle - 180;
					}
					else if (x < 0) {
						angle += 90;
					}
					else if (y < 0) {
						if (angle < 0) {
							angle = 360 + angle - 90;
						}
					}
					else {
						angle += 270;
					}

					//std::cout << angle << std::endl;
					GameObject::SetRotation(angle);
				}
				else {
					//GameObject::SetVelocity(glm::vec3(0.0f, -1.0f, 0.0f));
				}
			}
			else if(GetSide() == 1) {
				//!/ Set the velocity and rotation of right traffic
				GameObject::SetRotation(0.0);
				if (GameObject::HasParent()) {
					GameObject::SetPosition(glm::vec3(GameObject::GetParent()->GetPosition().x - 0.1f, GameObject::GetParent()->GetPosition().y, 0.0f));

					float x = target_.x - position_.x;
					float y = target_.y - position_.y;
					float angle = glm::degrees(glm::atan(y / x));

					if (x < 0 && y < 0) {
						angle = 270 + angle - 180;
					}
					else if (x < 0) {
						angle += 90;
					}
					else if (y < 0) {
						if (angle < 0) {
							angle = 360 + angle - 90;
						}
					}
					else {
						angle += 270;
					}
					//std::cout << angle << std::endl;
					GameObject::SetRotation(angle);
				}
				else {
					//GameObject::SetVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
				}
			}

			//!/ Move the enemy depending on where they are
			if (GameObject::getType() == 1 && GameObject::getState() == 0) {
				if (ver_dir) {
					if (GameObject::GetPosition().y > -1.5) {
						if (GameObject::GetSide() == 0) {
							GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, -0.8, 0.0f));
						}
						else {
							GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, -0.5, 0.0f));
						}
					}
					else {
						ver_dir = false;
						GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.0, 0.0f));
					}
				}
				else {
					if (GameObject::GetPosition().y < 1.5) {
						if (GameObject::GetSide() == 0) {
							GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.8, 0.0f));
						}
						else {
							GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.5, 0.0f));
						}
					}
					else {
						ver_dir = true;
						GameObject::SetVelocity(glm::vec3(GameObject::GetVelocity().x, 0.0, 0.0f));
					}
				}
			}
		}
		//!/ If the Enemy, it has been destroyed, set it as so and it can't collide anymore
		else if (GameObject::getState() == 1) {
			GameObject::SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			exploding_ = true;
			GameObject::SetState(2);
		}
		//!/ If the enemy is in destroyed and it's already exploded, reset the enemy
		else if (GameObject::getState() == 2) {
			if (GameObject::getStateTime() > (GetScale().x * 2) && GameObject::getStateTime() < (GetScale().x * 3)) {
				GameObject::SetPosition(glm::vec3(GameObject::GetPosition().x, -5.0, 0.0f));
				exploding_ = false;
			}
			else if (GameObject::getStateTime() > (GetScale().x * 5)) {
				GameObject::resetStateTime(0.0);
				GameObject::SetState(-1);
			}
		}
		//!/ If the enemy is in targeting state, adjust the velocity to track
		else if (GameObject::getState() == 4) {

			//!/ Calculate distances and angle
			float x = target_.x - position_.x;
			float y = target_.y - position_.y;
			float angle = glm::degrees(glm::atan(y / x));

			//!/ Take care of angles so the ship maintains the angle while it's patrolling
			if (x < 0 && y < 0) {
				angle = 270 + angle - 180;
			}
			else if (x < 0) {
				angle += 90;
			}
			else if (y < 0) {
				if (angle < 0) {
					angle = 360 + angle - 90;
				}
			}
			else {
				angle += 270;
			}
			
			//!/ Set the ship's rotation with angle
			GameObject::SetRotation(angle);

			//!/ Normalize x and y
			float norm = sqrt(x * x + y * y);
			x /= norm;
			y /= norm;

			//!/ Set the velocity with the calculated distances
			GameObject::SetVelocity(glm::vec3(x*0.7, y*0.7, 0.0f));
		}
		
		if (GameObject::getState() != -2) {
			// Call the parent's update method to move the object in standard way, if desired
			GameObject::Update(delta_time);
		}

	}

} // namespace game
