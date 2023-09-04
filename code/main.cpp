/*
 *
 * A skeleton for a 2D game
 *
 * Copyright (c) 2020-2022 Oliver van Kaick <Oliver.vanKaick@carleton.ca>, David Mould <mould@scs.carleton.ca>
 *
 */

#include <iostream>
#include <exception>
#include "game.h"
#include "audio_manager.h"
#include "bin/path_config.h"

// Macro for printing exceptions
#define PrintException(exception_object)\
    std::cerr << exception_object.what() << std::endl

// Main function that builds and runs the game

int main(void){
    game::Game the_game;
    audio_manager::AudioManager am;

    // Reference index of each file to be played
    int music;

    try {
        // Initialize audio manager
        am.Init(NULL);

        // Set position of listener
        am.SetListenerPosition(0.0, 0.0, 0.0);

        // Load first sound to be played
        std::string filename = std::string(AUDIO_DIRECTORY).append(std::string("/").append(std::string("RollerMobster.wav")));
        music = am.AddSound(filename.c_str());
        // Set sound properties
        am.SetSoundPosition(music, -10.0, 0.0, 0.0);
        am.SetLoop(music, true);
        // Play the sound
        am.PlaySound(music);

    }
    catch (std::exception& e) {
        PrintException(e);
    }

    try {
        // Initialize graphics libraries and main window
        the_game.Init();
        // Setup the game (scene, game objects, etc.)
        the_game.Setup();
        // Run the game
        the_game.MainLoop();
    }
    catch (std::exception &e){
        // Catch and print any errors
        PrintException(e);
    }

    return 0;
}
