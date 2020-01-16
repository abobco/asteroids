
# ASTEROIDS

This is a recreation of the classic asteroids video game. Implemented in the C
programming language, using SDL 2 library to display graphics on the screen.

To compile to webassembly, you need to first install [emscripten](https://emscripten.org/docs/getting_started/downloads.html), then open an emscripten command prompt, navigate to your clone's directory and enter:
```
emcc -o index.html asteroids.c main.c player.c renderer.c vector.c stack.c -Wall -g -lm -s USE_SDL=2 --shell-file sdl_shell.html
```
    
## Controls
* left arrow to rotate left
* right arrow to rotate right
* up arrow to apply thrust in the direction you are pointing
* space to shoot a bullet
* ESC to exit game

## Images
![animation](https://i.imgur.com/sV164D6.gif)

![game play](http://i.imgur.com/vg8nlAO.png)

