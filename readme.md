# QRay Engine

A WIP raycasting game engine.

## Usage

Build both the QRayEditor.exe and the QRayRuntime.exe and make sure they are in the same folder.

Execute the QRayEditor.exe to open the editor.

Add your own block type (texture) at the right side of the editor, by clicking the + icon.

A pop-up appears. Give a name for your block and click Browse to select a texture for your block. There are some example textures included in this repository. (Note that textures must be 32x32 pngs)

Once you've created a block type, select it from the block type selection. (Right side of the screen). 

Now you may click around the viewport (the grid) to place walls. Right click to remove a wall.

Add a spawnpoint to your map, by selecting "Spawnpoint" on the block type selection and clicking on the grid.

Set a name for your game by going File -> Settings.

Build your game by clicking File -> Build, and choosing a location for you game.

## Features

You can change the resolution and title of your game from settings menu (File -> Settings)

## TODO

Here's a list of features that I consider are needed before I can release the first "official" version

- Doors
- Entities
- Saving or loading editor projects
- Start direction editable
- Some kind of ui support
- Weapons
- Better visualizing of the map in the editor

Things below are not necessarily needed but would be nice to have

- Multiple map support? (Load new map after getting to a specific point in the previous one)
- HTML build target (using HTML5 Canvas or WebGL)