NOTE (2023-09-04) - Due to the size of the audio file, it is too large to include to this repository (as I mainly upload through web browser, and unfamilliar with  committing changes through command lines). Apologies in advance for any issues this may cause.

Copyright (c) 2020-2023 Gabriel Martell <gabemartell@cmail.carleton.ca>, Nicholas Faubert <nicholasfaubert@cmail.carleton.ca>
 
// Project COMP 2501 //
Name: Gabe Martell, Nicholas Faubert
Student #: 10191857 & 101192850
OS: Windows

!! General Knowledge
Most of our comments are marked by //!/ or are headers. 
Controls are WASD with space to shoot and escape is now how to exit the game.

!! General Tester Knowledge

To test ending conditions:
	If needed, you can change the score cap in ScoreHandle to be lower.
	If needed, you can check HUD handle to test the distance ending condition.
	If needed you can spawn a technical immediately by making the technical's state -1

!! Libraries
The libraries used are:
	GLEW32s
	GLFW3
	SOIL
	OpenAL32
	alut
They are all provided via. Oliver's brightspace resources tab.
Please ensure that these libraries are available to you when compiling this game.

! Game Mechanics:

In our game, the player must escape from the state their in and cross the border into Canada. To do so they must drive on one highway which many have tried. The local Biker gang has foul blood with the player and is attempting to kill the player before they can reach Canada. For the player to escape from the state, they must either reach the border(The arrow above reaches the pixel maple leaf) or kill the remaining gang that is following them(Reach the score cap). They must survive occassional mini-bosses(Technicals), fight bikers and dodge traffic. Each enemy gives different score, however the player will lose score if they get hit. They can also attempt to play the game passively by making the bikers shoot each other and dodge all of the Technical's bullets in hopes to reach the border.

! Enemies:

In our game, there are three types of enemies: Bikers, Traffic, and Technical. Each enemy has different textures to help players distinguish between them. Here's a breakdown of each enemy's behavior:

	Bikers: When there are no Technicals present, Bikers will join the fight. They will fire at the player if they are close enough, and they will avoid crashing into any Oncoming or Incoming Traffic. Bikers are equipped with pistols, as they can only ride with one hand.

	Traffic: Traffic comes in two forms: Oncoming and Incoming. Oncoming Traffic moves slowly in the same direction as the player and is a potential danger if the player is distracted. Incoming Traffic is more dangerous, as it appears suddenly and can surprise the player. Both types of Traffic will kill the player instantly if they hit them, and they do not fire any weapons. However, if a bullet hits them, it can richochet and kill the sender or an unsuspecting enemy.

	Technical: The Technical is a mini-boss that is the most dangerous enemy on the screen. It can fire many bullets at a time and takes more hits than a Biker before it is taken down. When a Technical appears, no Bikers will join the fight because they know how unwieldy the Technical's machine gun is. The player must shoot the Technical three times to defeat it, and only then will the Bikers return to the fight.

! Weapons:

There are two weapons the player can use, a pistol and an UZI. Here is a brief breakdown:

	Pistol: The player begins with the pistol, as it has been their trusty weapon up to this point. Since it is pretty old, there is quite a delay in each shot. If the player drops another weapon, they will switch back to this one.

	UZI: The player picks up the random UZI from the ground and understands how to use it. They must get up close and personal as this weapon has high recoil, making harder to hit far targets. The player may shoot faster with this weapon but to kill, they need to make a bit more risky moves.

! Collectible items:

In our game, we have three collectibles, a new weapon, health and godmode. Each one moves downwards on the screen, in hopes that the player will be able to grab them. Here is a brief breakdown:

	UZI: The Uzi allows the player to fire faster but shorter distances. This weapon isn't permanent as it is hard to hold and the player will drop it to return to their pistol.
	
	Medkit: The Medkit fully heals the player as soon as they pick it up.

	Premium Biker Helmet: Once the player reaches low health, he will start to see biker helmets on the road, most likely from previous bikers trying to escape. Grabbing the helmet and putting it on will allow the player to disguise himself and confuse the other bikers and technical as they believe they are one of them.

! Movement and transformations:

In our game, all object movements are managed through transformations. This includes child-parent transformations. Our player character moves with physically based motion, while most other objects move with parametric motion. This is because they are often traveling at constant speeds to keep up with the player, who can slow down or speed up at any time.

! Collision detection:

All of our game objects share collision in some way. Mainly, the player can collide with collectibles and bullets, while the enemies can only collide with bullets. The enemy technical is in a tactical position to only ever be hit by bullets. Our traffic interacts with both the enemies and the player in their own special ways. The player, if hit, will always die because of how much metal just hit him. The enemies however always insure they move out of the way of traffic so they don't hit them and kill any one inside.

! Game world:

Our world is a top down scrolling game, that is larger than the screen. Since our player is in danger and hopes to leave, he only slows down and never fully stops, as the road below shows the continuous/derelict highway

! Patricles systems:

Our Game has 3 different particle systems, which all house different forms. When a bullet is fired, there is a yellow trail that follows it and cuts through wind to show where it came from. When an enemy is destroyed, they explode and a orange explosion system is played as the enemy dies and dissappears. Our third system is the exhausts, which output all the white smoke the bikes create.

! UI:

Our HUD displays useful information such as player health, invincibility, score and distance from Canada. It also displays the current weapon in the player's disposal. There are some console prints that can tell the TA, Prof or Us what happens on screen with words if that is ever needed.

! Advanced method:

The advanced method chosen was a system to allow bullets to richochet of Traffic, causing the bullet to return and kill the player or an enemy.

! Code readability and organization:

All of our personal comments have been marked with //!/ and our code has been sorted into functions to allow much better readability

! Bonus:

We added music, a cinematic intro and menu for our bonus concepts. 
The song is Roller Monster, made by Carpenter Brut, for Hotline Miami 2
(https://www.youtube.com/watch?v=qFfybn_W8Ak)

Source for explosion:
https://opengameart.org/content/pixel-explosion-12-frames
Assets attributed to JROB774 (https://opengameart.org/users/jrob774)

License(s):
CC-BY 3.0

--All other textures are made by Gabe Martell.

