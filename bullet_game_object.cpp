#include <iostream>

#include "bullet_game_object.h"

namespace game {

	/*
		BulletGameObject inherits from GameObject
	*/

	BulletGameObject::BulletGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GLuint explosion, int type, glm::vec3& target)
		: GameObject(position, geom, shader, texture, explosion, type, NULL) {
		GameObject::SetScale(glm::vec2(0.5f, 0.5f));
		//target_ = target;
		SetTarget(target);
		x_ = 0.0f;
		y_ = 0.0f;
	}

	// Update function for moving the player object around
	void BulletGameObject::Update(double delta_time) {

		if (getState() == 0) {
			//!/ Calculate distances and angle
			float x = (target_.x) - position_.x;
			float y = (target_.y) - position_.y;
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

			x_ = x;
			y_ = y;

			//!/ Set the velocity with the calculated distances
			GameObject::SetVelocity(glm::vec3(x_ * 1.7, y_ * 1.7, 0.0f));
			GameObject::SetState(4);
			
		} 
		else if (getState() == 1) {
			GameObject::SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
			GameObject::SetState(2);
		}
		else if (getState() == 4){
			GameObject::SetVelocity(glm::vec3(x_ * 2.0, y_ * 2.0, 0.0f));
		}

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);

	}

} // namespace game