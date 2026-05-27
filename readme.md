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

Here's a list of things yet to be implemented in the engine:

- Saving or loading previously saved editor projects
- In the editor, show the textures in the map
- Making the starting direction and other data editable in the editor