#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

        public:
            PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GLuint explosion, int type, GLuint invincible, int health, GameObject* parent);

            // Update function for moving the player object around
            void Update(double delta_time) override;
            
            //!/ Health system methods
            inline int gethealth() { return health_; };
            inline void damage() { health_ -= 1; };

            //!/ Collectible methods
            inline int getCollected() { return collected_; };
            inline void resetCollection() { collected_ = 0; }
            inline void addCollectible() override { collected_++; };
			inline float GetBulletTime() override { return bullet_cooldown_; }

			//!/ Setters
			inline void AddBulletTime(float time) { bullet_cooldown_ += time; }
			inline void ResetBulletTime() { bullet_cooldown_ = 0.0f; }

    private:
            int health_;
            int collected_;
			float bullet_cooldown_;
            GLuint ori_texture_;
            GLuint inv_;


    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
