#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.h"

namespace game {

GameObject::GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GLuint explosion, int type, GameObject* parent) 
{
    // Initialize all attributes
    position_ = position;
    initial_pos_ = position;
    scale_ = glm::vec2(1.0f,1.0f);
    velocity_ = glm::vec3(0.0f, 0.0f, 0.0f); // Starts out stationary
    rotation_ = 0.0;
    geometry_ = geom;
    shader_ = shader;
    texture_ = texture;
    explosion_ = explosion;
    state_ = 0;
    state_time_ = 0.0;
    type_ = type;
    parent_ = parent;
    new_weapon_ = false;
    if (parent_ != NULL){
        parent_->SetChild(this);
    }
}


void GameObject::Update(double delta_time) {
    
    if (type_ != 3 && type_ != 4) {
        //!/ Calculations for velocity management
        glm::vec3 norm = glm::normalize(velocity_);
        glm::vec3 vecChangeAcc = norm * 4.9f;
        glm::vec3 vecChangeDec = norm * (float)(1.0f * delta_time);

        //!/ If the velocity is bigger than 5, make the velocity the set Acceleration
        if (glm::length(velocity_) > 5.0f) {
            velocity_ = vecChangeAcc;
        }

        //!/ If the velocity is bigger than 5, make the velocity the set Deceleration
        if (glm::length(velocity_) > 0.0f) {
            velocity_ -= vecChangeDec;
        }
    }
    // Update object position with Euler integration
    position_ += velocity_ * ((float)delta_time);     
}

void GameObject::Render(glm::mat4 view_matrix, double current_time){

    if ((getType() == 3 || getType() == 4) && getState() == 2) {
        return;
    }
    //!/ Configuring the render to scale the explosion after it exploded
    else if(getStateTime() < (scale_.x * 2)){

        glm::vec2 reducer_;
        GLint changer_;

        //!/ If the object is destroyed but not a coin
        if (getState() == 2 && type_ != 3) {
            //!/ If the explosion already happened
            if (getStateTime() > scale_.x) {
                shader_->SetUniform1i("object_deceased", 1);
                reducer_ = scale_;
                changer_ = texture_;
            }
            //!/ Make it grayscale after the explosion
            else {
                shader_->SetUniform1i("object_deceased", 0);
                reducer_.x = state_time_;
                reducer_.y = state_time_;
                changer_ = explosion_;
            }
            addStateTime(0.002);
        }
        //!/ If its normal than don't make it grayscale and act normally
        else {
            shader_->SetUniform1i("object_deceased", 0);
            reducer_ = scale_;
            changer_ = texture_;
        }
        
        // Set up the shader
        shader_->Enable();

        // Set up the view matrix
        shader_->SetUniformMat4("view_matrix", view_matrix);

        //!/ Send in the object type and state to the shader and sprite
        if (getType() <= -1) {
            shader_->SetUniform1i("isBackground", 1);
        }
        else {
            shader_->SetUniform1i("isBackground", 0);
        }

        // Setup the scaling matrix for the shader
        glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(reducer_, 1.0));

        // Set up the rotation matrix for the shader
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_), glm::vec3(0.0f, 0.0f, 1.0f));

        // Set up the translation matrix for the shader
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

        glm::mat4 parent_matrix;

        // Setup the transformation matrix for the shader
        glm::mat4 transformation_matrix = translation_matrix * scaling_matrix * rotation_matrix;

        if (HasParent()) {
            parent_matrix = (glm::translate(glm::mat4(1.0f), parent_->GetPosition())) * (glm::scale(glm::mat4(1.0f), glm::vec3(parent_->GetScale(), 1.0))) * (glm::rotate(glm::mat4(1.0f), glm::radians(parent_->GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f)));
            glm::mat4 transformation_matrix = parent_matrix * rotation_matrix * scaling_matrix;
        }

        // Set the transformation matrix in the shader
        shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

        // Set up the geometry
        geometry_->SetGeometry(shader_->GetShaderProgram());

        // Bind the entity's texture
        glBindTexture(GL_TEXTURE_2D, changer_);

        // Draw the entity
        glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
    }
    else {
        addStateTime(0.0027);
    }
    
}

void GameObject::addCollectible() { }

float GameObject::GetBulletTime() { return 0.0f; }

void GameObject::AddBulletTime(float time) {};

void GameObject::ResetBulletTime(void) { };
} // namespace game


