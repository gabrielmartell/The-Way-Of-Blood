#ifndef BULLET_GAME_OBJECT_H_
#define BULLET_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class BulletGameObject : public GameObject {

    public:
        BulletGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GLuint explosion, int type, glm::vec3& target);

        // Update function for moving the player object around
        void Update(double delta_time) override;

    private:
        float x_;
        float y_;

    }; // class CollectibleGameObject

} // namespace game

#endif // BULLET_GAME_OBJECT_H_
