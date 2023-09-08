/*
 *
This project contains code from the COMP2501 demos: 

Copyright (c) 2020-2023 Oliver van Kaick <Oliver.vanKaick@carleton.ca>, David Mould <mould@scs.carleton.ca>

under the MIT License.


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
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
