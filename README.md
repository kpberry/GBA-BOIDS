# BOIDS

## Installation:
### Linux:
    git clone https://github.com/kpberry/gba-boids.git
    sudo apt-get install gcc-arm-none-eabi
    sudo apt-get install libnewlib-arm-none-eabi
    sudo apt-get install cs2110-vbam-sdl
    cd ./gba-boids
    make run

### Windows/Mac:
	¯\_(ツ)_/¯    

## Objective:

    Eat food! The large blue dots on the screen are pieces of food; you need to
    eat 30 pieces of food before the Boids (bird-oid objects) can eat 100. You
    may eat Boids by passing over them on your way to the food, but they will
    all respawn as soon as you eat any food.
    Good Luck.


## Controls:

    From Any Context:
        Select:   Return to main menu

    In Options Menu:
        Up, Down: Move the cursor up or down
        Left:     Decrement the value at the cursor position
        Right:    Increment the value at the cursor position

    During Gameplay:
        Up:       Move the player (the blue arrow) up
        Down:     Move the player down
        Left:     Move the player left
        Right:    Move the player
        Start:    Open options menu

    All other necessary controls are listed on the screen.

## Options:

    Number of Boids: This changes how many boids can appear at once. This runs
    fine for 0 to 20 boids, but substantially slows down beyond that.

    Cohesion: This changes how much the boids will attempt to clump together.
    Smaller values mean that they will clump more.
    Negative values are valid, but lead to repulsion.

    Sync: This changes how much boids will try to fly in the same direction.
    Lower values mean that they will attempt to have more similar velocities.
    Negative values are valid, but lead to chaos.

    Repulsion: This changes how much boids will try to space themselves apart.
    Lower values mean that they will attempt to space out more.
    Negative values are valid, but lead to extreme clumping.

    Player Speed: This changes how fast the player can move.
    Higher values mean a faster player.

    Viewing Radius: This changes how far the boids can see.
    Higher values mean they can see farther.
