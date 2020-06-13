![](https://raw.githubusercontent.com/cumus/Juicy-Code-Games_Project-2/master/Build/Assets/textures/team-logo2.png)

We are Juicy Code Games, and we are working on an RTS called SQUARE UP!.

## Square Up

In this game you are calmly settled on your home planet, Orberus, when, suddently, the Kilian race attempts to invade your planet to expand their power across the universe. You'll have to fight the enemy forces before they destroy your base center, to avoid that, you'll have to build a base and fight against the evil. Good luck!

Our gold release is down below, so check it out!

## Trailer



## Team members

![Team Members](https://raw.githubusercontent.com/cumus/Juicy-Code-Games_Project-2/master/WikiResources/Home%20page%20picts/IMG_5071.JPG)
_`Team Members`_

#### Team Leader
  * Ruben Sardón: [cumus](https://github.com/cumus)
  
  I’m Rubén Sardón and was the team lead for Juicy Code’s Square Up. As part of my
responsibilities I was in charge of scheduling and documenting meetings, organizing tasks
and managing teamwork using Discord, Hacknplan and Google Drive’s tools. All of us took
part in deciding and designing the game and its features and together with my rol’s duties I
mainly worked programming the engines back-end systems.

Work log:

* Added C++ Libraries: SDL2 (window management and rendering), SDL_Mixer
(audio), SDL_TTF (fonts), PhysFS (file system), pugixml (.xlm parser), and mmgr
(memory leaks).

* Built rendering pipeline to draw sprites using layers, gradually update Fog of War
texture for Minimap and apply camera frustum culling to the scene.

* Set window resolution and fullscreen controls. Rendering viewport adapts to window
size or can preserve a target aspect ratio.

* Set an event system that allows sending events to any given listener. Data is held
using CVars, variables that adapt to the given data type.

* Added time management for fps control and scene playing and pausing. Spare time
between frames pulls queued events and updates pathfinding.

* Setup file system using PhysFS to read and load all assets: map, textures, fonts,
and audio samples from a compressed folder.

* Built an importer for each resource type and serialized module configuration for each
member to use the engine with their own preferred settings.

* Built gameobject hierarchy and component handling. Also added sprite, animation
and transform components using the event system to spread calculations throughout
different frames and.

* Added scene serialization to save and load the scene to an xml.

* Built UI components:

  * Canvas: adapts to the window’s aspect ratio and sets target area to draw
children UI components.

  * Image: adapts to viewport using pivots, normal coordinates.
  
  * Text: handles surface rendering on changing text.
  
  * Minimap: draws map and updates fog of war texture displayed.
  
* Added audio source components that receive transform events to measure the
spatial effect for each individual audio channel.

* Set up audio volume controls and a fade effect that triggers when pausing and
resuming the scene.
  
#### Team Manager
  * [Adrià Ávila](https://github.com/cumus/Juicy-Code-Games_Project-2/blob/gh-pages/Cv's/Adri_Web.pdf): [Avilgor](https://github.com/Avilgor) 
  
 
  
#### Code Leader
  * Victor Bosch: [victorbr2](https://github.com/victorbr2)
  
#### Game Designer
  * Jan Adell: [JanAdell](https://github.com/JanAdell)
  
 My name is Jan Adell and I was Lead Game Designer in Juicy Code’s Square Up project.
My workload has been divided in two groups mainly, these being Specific Design work,
where the concept for the game was developed and the game balance was made, and
General work, consisting of tasks pertaining to other areas, such as programming or art.

Specific work:

* Game Design Document assembly:

  * Game Pillar definition.
 
  * Basic idea analysis.
 
  * Gameplay features.
 
  * Campaign and quest definition.
 
  * Enemy and ally unit and building definition.
 
  * Map rough design and gameplay flow.
 
  * Proofreading and correcting of the Setting and Story documents.
 
  * Proofreading and correcting of any document pertaining to the design specific
parts of the project’s wiki.

* Game Balance:

  * Unit stat balancing.
 
  * Building stat balancing.
 
  * Edge, Gears and Gold currency/resource gain tweaking.
 
  * Economy systems utilization.
 
  * Unit and building price definition and balance.
 
General Work:

* Basic unit behaviour implementation
* Walkability system implementation
* Gears and Gold currencies implementation
* Capsule implementation
* Starting dialogue implementation
* Tutorial implementation and polish
* Scene preparation with all needed elements
* State machine implementation
* Final HUD implementation and polish
* Transition and pop-up notifications implementation (End Screen or tutorial related)
* Win/Lose screen polish
* Feedback elements polish (mainly pop-up buttons and tooltips on buildables)
  
#### Art Leader
  * Gerard Berenguer: [GerardBP77](https://github.com/GerardBP77)
  
#### Audio Leader
  * Oscar Reguera: [oscarrep](https://github.com/oscarrep)
  
#### UI Leader
  * Xavier Trillo: [xatrilu](https://github.com/xatrilu)
  
#### QA Leader
  * Pol Gannau: [PolGannau](https://github.com/PolGannau)

## Links
Here are some links to our GitHub, Wiki, etc.

* Gold Download [Gold Release](https://github.com/cumus/Juicy-Code-Games_Project-2/releases/download/v0.8/JuicyCode-SquareUp_v0.8.zip)

* Repository [Github](https://github.com/PolGannau/Juicy-Code-Games_Project-2)
* Game Research [Wiki](https://github.com/cumus/Juicy-Code-Games_Project-2/wiki)
* Tutor: [Marc Garrigó](https://github.com/markitus18)
* University: [CITM UPC](https://www.citm.upc.edu/)
* License: [GNU General Public License v3.0](https://github.com/PolGannau/Juicy-Code-Games_Project-2/blob/master/LICENSE)

## Social media

Facebook: [@juicycodegames](https://www.facebook.com/Juicy-Code-110251897235394/)

Twitter [@juicycodegames](https://twitter.com/JuicyCodeGames)

Instagram [@juicycodegames](https://www.instagram.com/juicycodegames/)

Youtube [@juicycodegames](https://www.youtube.com/channel/UCvtOzr0YiLtN2cmBA6WdB9Q?view_as=subscriber)

For a more detailed info feel free to contact us via e-mail at: JuicyCodeStudio@gmail.com

![University Logo](https://raw.githubusercontent.com/cumus/Juicy-Code-Games_Project-2/gh-pages/WikiResources/Home%20page%20picts/logocitm.png) 
