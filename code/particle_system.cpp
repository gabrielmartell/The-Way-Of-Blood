#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

#include "particle_system.h"
#include <iostream>


namespace game {

ParticleSystem::ParticleSystem(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, int type, GameObject *parent)
	: GameObject(position, geom, shader, texture, NULL, NULL, parent) {
    parent_ = parent;
    particle_type_ = type;
    particle_stop_ = 0;
    local_time = 0.0f;
}


void ParticleSystem::Update(double delta_time) {

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}


void ParticleSystem::Render(glm::mat4 view_matrix, double current_time){

    if (GameObject::HasParent() == true) {
        if (GameObject::GetParent()->getState() == 2) {
            return;
        }
    }
    

    local_time += 0.005f;
    // Set up the shader
    shader_->Enable();

    // Set up the view matrix
    shader_->SetUniformMat4("view_matrix", view_matrix);

    glm::mat4 dirangle = glm::rotate(glm::mat4(1.0f), glm::radians(GetRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 dir = glm::vec3(0.0, 1.0, 0.0); dir = glm::vec3(dirangle * glm::vec4(dir, 0.0f));

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(GetScale().x, GetScale().y, 1.0));
    glm::mat4 rotation_matrix;

    // Setup the rotation matrix for the shader
    if (parent_ != NULL) {
        rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(GameObject::GetRotation()), glm::vec3(0.0, 0.0, 1.0));
    } else {
        rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(0.0, 0.0, 1.0));

    }
    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix;

    //!/ If the particle is the exhaust of the bullet, position it so the particle is coming from the "exhaust" of the bullet.
    if (particle_type_ == 0) {
        translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.35,0.0f) - dir * 0.2f);
    }
    else if (particle_type_ == 2 || particle_type_ == 3) {
        translation_matrix = glm::translate(glm::mat4(1.0f), GameObject::GetPosition());
    }
    //!/ Otherwise, keep the position the same as the parent
    else {
        translation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    //!/ Set up the parent transformation matrix
    glm::mat4 transformation_matrix;
    if (parent_ != NULL) {
        glm::mat4 parent_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(parent_->GetRotation()), glm::vec3(0.0, 0.0, 1.0));
        glm::mat4 parent_translation_matrix = glm::translate(glm::mat4(1.0f), parent_->GetPosition());
        glm::mat4 parent_transformation_matrix = parent_translation_matrix * parent_rotation_matrix;

        //!/ Setup the transformation matrix for the shader
        transformation_matrix = parent_transformation_matrix * translation_matrix * rotation_matrix * scaling_matrix;
    } else {
        transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;
    }
    //!/ Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    /*
    To manipulate the particles to the fullest, I had to bring the glsl local variables to the C++ code so that I had full control of the particles and their capabilities
    */
    float cycle;
    float speed;
    float gravity;
    float acttime;

    /*Assign all cycle, speed, gravityand acttime.
      Particle Type 0 is the exhaust
      Particle Type 1 is the explosion
    * */
    if (particle_type_ == 0) {
        cycle = 5.0f;
        speed = 0.2f;
        gravity = 2.93f;
        acttime = fmod((local_time + local_time * cycle), cycle);
    } 
    else if (particle_type_ == 2) {
        cycle = 5.0f;
        speed = 0.0002f;
        gravity = 2.93f;
        acttime = fmod((local_time + local_time * cycle), cycle);
    }
	else if (particle_type_ == 3) {
		cycle = 7.0f;
		speed = 0.03f;
		gravity = 2.93f;
		acttime = fmod((local_time + local_time * cycle), cycle);
	}
    else {
        cycle = 3.0f;
        speed = 3.0f;
        gravity = 2.93f;
        acttime = fmod((local_time + local_time * cycle), cycle);
    }

    // We dont want the explosion to loop, so stop it once it reaches the end of the cycle.
    if (acttime >= cycle-0.2f && particle_stop_ == 0 && particle_type_ == -1) {
        particle_stop_ = 1;
        //std::cout << "here" << std::endl;
    }

    // Set the time in the shader
    shader_->SetUniform1f("time", current_time);

    shader_->SetUniform1f("acttime", acttime);
    shader_->SetUniform1f("speed", speed);
    shader_->SetUniform1f("gravity", gravity);
    shader_->SetUniform1f("cycle", cycle);

    shader_->SetUniform1i("stop", particle_stop_);
    shader_->SetUniform1i("type", particle_type_);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());

    // Bind the particle texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
}

} // namespace game
