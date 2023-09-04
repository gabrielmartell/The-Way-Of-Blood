#ifndef ENEMY_GAME_OBJECT_H_
#define ENEMY_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class EnemyGameObject : public GameObject {

        public:
            EnemyGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GLuint explosion, int type, int side, int lane, GameObject* parent);

            // Update function for moving the player object around
            void Update(double delta_time) override;
            inline float GetBulletTime() override { return bullet_cooldown_; }

            //!/ Setters
            inline void AddBulletTime(float time) { bullet_cooldown_ += time; }
			inline void ResetBulletTime() { bullet_cooldown_ = 0.0f; }

        private:
            float bullet_cooldown_;
            bool ver_dir;
    }; // class EnemyGameObject

} // namespace game

#endif // ENEMY_GAME_OBJECT_H_
