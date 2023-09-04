#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "geometry.h"

namespace game {

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GLuint explosion, int type, GameObject* parent);

            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            virtual void addCollectible();
            virtual float GetBulletTime();
            virtual void AddBulletTime(float time);
            virtual void ResetBulletTime();

            // Getters
            inline glm::vec3& GetPosition(void) { return position_; }
            inline glm::vec2& GetScale(void) { return scale_; }
            inline glm::vec3& GetVelocity(void) { return velocity_; }
            inline float GetRotation(void) { return rotation_; }
            inline bool HasParent() { return (parent_ != NULL); }

            inline GameObject* GetParent() { return parent_; }
            inline GameObject* GetChild() { return child_; }

			inline int GetSide(void) { return side_; }
            inline bool BulletShot() { return shot_; }
            inline bool IsMoving() {return moving_;}
            inline bool HasNewWeapon(){ return new_weapon_; }
            inline int GetLane() { return lane_; }
            inline glm::vec3 GetInitialPos() { return initial_pos_; }
            inline bool IsExploding() {return exploding_;}
            inline int GetTextCount() { return text_count_; }
            inline bool IsReflectedBullet() { return reflected_; }
            inline bool IsInvincible() {return invincible_;}
            //!/ Collision/Status getters
            inline int getState() { return state_;  }
            inline float getStateTime(void) { return state_time_; }
            

            //!/ Type getter
            inline int getType() { return type_; }
            inline int getCollectType() {return collect_type_;}

            //!/ State getters
            inline glm::vec3 getTarget() { return target_; }

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(glm::vec2 scale) { scale_ = scale; };
            inline void SetVelocity(const glm::vec3& velocity) { velocity_ = velocity; }
            inline void SetRotation(float rotation) { rotation_ = rotation; }
            inline void SetSide(int side) { side_ = side; }
            inline void SetChild(GameObject* c) { child_ = c; }
            inline void SetBulletShot(bool b) { shot_ = b; }
            inline void SetMoving(bool m) { moving_ = m; }
            inline void SetLane(int l) { lane_ = l; }
            inline void SetInitialPos(glm::vec3 ip) { initial_pos_ = ip; }
            inline void SetTextCount(int tc) { text_count_ = tc; }
            inline void SetTexture(GLuint texture) { texture_ = texture; }
            inline void SetWeapon(bool b) { new_weapon_ = b; }
            inline void SetReflected(bool r) { reflected_ = r; }
            inline void SetInvincible(bool i) {invincible_ = i;}
            //!/ Collision/Status setters
            inline void SetState(int state) { state_ = state; }
            inline void resetStateTime(float state_time) { state_time_ = state_time; }
            inline void addStateTime(float added_time) { state_time_ += added_time; }

            //!/ State setters
            inline void SetTarget(const glm::vec3 target) { target_ = target; }

			GameObject* bullet_;
            GameObject* particle_;
            GameObject* explosion_particles_ = NULL;

            double explosion_timer = 0.0;

        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            glm::vec2 scale_;
            glm::vec3 velocity_;
            glm::vec3 initial_pos_;
            float rotation_;

            GameObject* parent_;
            GameObject* child_;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;
            GLuint explosion_;

            //!/ Collision/Status boolean variables
            int state_;

            float state_time_;

            int type_;
            int side_;
            int lane_;
            int text_count_ = 3;
            int collect_type_;

            bool shot_;
            bool invincible_;
            bool moving_;
            bool new_weapon_ = false;
            bool exploding_ = false;
            bool reflected_ = false;

            //!/ State variables
            glm::vec3 target_;

    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
