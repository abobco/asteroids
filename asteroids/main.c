//main.c

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "renderer.h"
#include "player.h"
#include "asteroids.h"
#include "stack.h"
#include <stdbool.h>

// Import "emscripten_set_main_loop()" and "emscripten_cancel_main_loop()"
#include "emscripten.h"

#define ASTEROIDS 27
#define LIVES 3
#define BUTTON_HELD_OFFSET 5	// how much the buttons will move down to indicate they are being pressed
#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 86

int init(int width, int height);

void checkButtons(SDL_Event* event, bool motionevent);	// check the location of touch events, push button and touch ids to a stack

void popButtonEvent(struct StackNode* buttonStack, struct StackNode* temp_buttons, SDL_Event* event); // remove an old button event from the button/touch id stack

SDL_Window* window = NULL;			//The window we'll be rendering to
SDL_Renderer *renderer;				//The renderer SDL will use to draw to the screen
SDL_Texture *screen;				//The texture representing the screen	
uint32_t* pixels = NULL;			//The pixel buffer to draw to
struct asteroid asteroids[ASTEROIDS];		//The asteroids
struct player p;				//The player
struct player lives[LIVES];			//Player lives left

bool forward_held = false;
bool right_held = false;
bool left_held = false;

struct button_location {
	int x;
	int y;
};

enum button_ids {forwardID, leftID, rightID};

// struct Stack* button_presses = createStack()
struct StackNode* button_presses = NULL;
int64_t f = 16;

struct button_location leftButton = {20, SCREEN_HEIGHT-BUTTON_HEIGHT-20};
struct button_location rightButton = {30+BUTTON_WIDTH, SCREEN_HEIGHT-BUTTON_HEIGHT-20};
struct button_location forwardButton = {25+BUTTON_WIDTH/2, SCREEN_HEIGHT-BUTTON_HEIGHT*2-30};

// Main game loop
void mainloop()
{
	// Moved these variables
	int sleep = 0;
	int quit = 0;
	SDL_Event event;
	Uint32 next_game_tick = SDL_GetTicks();
	bool stop_push = false;
	

	// Main loop code
	//check for new events every frame
	SDL_PumpEvents();

	const Uint8 *state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_ESCAPE]) {
		quit = 1;
	}
		
	if (state[SDL_SCANCODE_UP]) {
		struct vector2d thrust = get_direction(&p);
		multiply_vector(&thrust, .06);
		apply_force(&p.velocity, thrust);
	}
	
	if (state[SDL_SCANCODE_LEFT]) {
		rotate_player(&p, -4);
	}

	if (state[SDL_SCANCODE_RIGHT]) {
		rotate_player(&p, 4);
	}

	while (SDL_PollEvent(&event)) {	
		struct StackNode* new_list = NULL;
		switch(event.type) {
			case SDL_KEYDOWN:
				switch( event.key.keysym.sym ) {
					case SDLK_SPACE:
						if (p.lives > 0) {
							shoot_bullet(&p);
						}
						break; 
				}
				break;
			// austin's very important touch event handling
			case SDL_FINGERDOWN:
				// printf("%" PRId64 " DOWN\n", event.tfinger.fingerId);
				checkButtons(&event, false);
				break;
			case SDL_FINGERUP:		
				// check which finger was lifted
				while ( !isEmpty(button_presses) ){
					struct button_event temp = pop(&button_presses);
					printf("%" PRId64 " UP\n", temp.finger_id);
					if (temp.finger_id == event.tfinger.fingerId){
						switch (temp.button_id){
							case leftID:	
								left_held = false;
								leftButton.y -= BUTTON_HELD_OFFSET;
								break;
							case rightID:
								right_held = false;
								rightButton.y -= BUTTON_HELD_OFFSET;
								break;
							case forwardID:
								forward_held = false;
								forwardButton.y -= BUTTON_HELD_OFFSET;
								break;
						}
					}
					else {
						push(&new_list, temp.button_id, temp.finger_id);
					}	
				}
				// fill global button event stack with new values
				while (!isEmpty(new_list)){
					struct button_event temp = pop(&new_list);
					push(&button_presses, temp.button_id, temp.finger_id);
				}
				free(new_list);
				break;
			case SDL_FINGERMOTION:
				// check which finger was moved, remove from stack
				while ( !isEmpty(button_presses) ){
					struct button_event temp = pop(&button_presses);
					printf("%" PRId64 " UP\n", temp.finger_id);
					if (temp.finger_id == event.tfinger.fingerId){
						switch (temp.button_id){
							case leftID:	
								left_held = false;
								leftButton.y -= BUTTON_HELD_OFFSET;
								break;
							case rightID:
								right_held = false;
								rightButton.y -= BUTTON_HELD_OFFSET;
								break;
							case forwardID:
								forward_held = false;
								forwardButton.y -= BUTTON_HELD_OFFSET;
								break;
						}
					}
					else {
						push(&new_list, temp.button_id, temp.finger_id);
					}	
				}
				// fill global button event stack with new values
				while (!isEmpty(new_list)){
					struct button_event temp = pop(&new_list);
					push(&button_presses, temp.button_id, temp.finger_id);
				}
				free(new_list);
				checkButtons(&event, true);
				break;

		}
	}

	if (forward_held){
		struct vector2d thrust = get_direction(&p);
		multiply_vector(&thrust, .06);
		apply_force(&p.velocity, thrust);
	}
	if (right_held) {
		rotate_player(&p, 4);
	}
	if ( left_held) {
		rotate_player(&p, -4);
	}

	//draw to the pixel buffer
	clear_pixels(pixels, 0x00000000);
	draw_asteroids(pixels, asteroids, ASTEROIDS);
	update_player(&p);
	bounds_player(&p);
	bounds_asteroids(asteroids, ASTEROIDS);

	int res = collision_asteroids(asteroids, ASTEROIDS, &p.location, p.hit_radius);

	if (res != -1) {
		p.lives--;
		p.location.x = 0;
		p.location.y = 0;
		p.velocity.x = 0;
		p.velocity.y = 0;

		int i = LIVES - 1;
		for ( i = LIVES; i >= 0; i--) {
			if(lives[i].lives > 0) {
				lives[i].lives = 0;
				break;
			}
		}
	}
	draw_player(pixels, &p);
	// draw_player(pixels, &lives[0]);
	// draw_player(pixels, &lives[1]);
	// draw_player(pixels, &lives[2]);
	
	int i = 0;
	struct vector2d translation = {-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2};

	for (i = 0; i < BULLETS; i++) {
		//only check for collision for bullets that are shown on screen
		if (p.bullets[i].alive == TRUE) {
			//convert bullet screen space location to world space to compare
			//with asteroids world space to detect a collision
			struct vector2d world = add_vector_new(&p.bullets[i].location, &translation);
			int index = collision_asteroids(asteroids, ASTEROIDS, &world, 1);
			
			//collision occured
			if (index != -1) {
				asteroids[index].alive = 0;
				p.bullets[i].alive = FALSE;

				if (asteroids[index].size != SMALL) {
					spawn_asteroids(asteroids, ASTEROIDS, asteroids[index].size, asteroids[index].location);
				}
			}
		}
	}
	
	update_asteroids(asteroids, ASTEROIDS);

	draw_button(pixels, forwardButton.x, forwardButton.y, BUTTON_WIDTH, BUTTON_HEIGHT, 0, 0xffffffff);
	draw_button(pixels, leftButton.x, leftButton.y, BUTTON_WIDTH, BUTTON_HEIGHT, 1, 0xffffffff);
	draw_button(pixels, rightButton.x, rightButton.y, BUTTON_WIDTH, BUTTON_HEIGHT, 2, 0xffffffff);

	//draw buffer to the texture representing the screen
	SDL_UpdateTexture(screen, NULL, pixels, SCREEN_WIDTH * sizeof (Uint32));

	//draw to the screen
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screen, NULL, NULL);
	SDL_RenderPresent(renderer);

	if(quit == 1) {
		emscripten_cancel_main_loop();

		//free the screen buffer
		free(pixels);
		
		//Destroy window 
		SDL_DestroyWindow(window);

		//Quit SDL subsystems 
		SDL_Quit(); 
	}
}

int main (int argc, char* args[]) {

	//SDL Window setup
	if (init(SCREEN_WIDTH, SCREEN_HEIGHT) == 1) {
		
		return 0;
	}

	int i = 0;
	int j = 0;
	int offset = 0;
	struct vector2d translation = {-SCREEN_WIDTH / 2, -SCREEN_HEIGHT / 2};

	//set up icons used to represent player lives
	for (i = 0; i < LIVES; i++) {
			
		init_player(&lives[i]);
		lives[i].lives = 1;

		//shrink lives
		for (j = 0; j < P_VERTS; j++) {
		
			divide_vector(&lives[i].obj_vert[j], 2);
		}

		//convert screen space vector into world space
		struct vector2d top_left = {20 + offset, 20};
		add_vector(&top_left, &translation);
		lives[i].location = top_left;
		update_player(&lives[i]);
		offset += 20;
	}

	//set up player and asteroids in world space
	init_player(&p);
	init_asteroids(asteroids, ASTEROIDS);

	// give emscripten a fixed update loop to use in the browser
	emscripten_set_main_loop(mainloop, 0, 1);

	return 0;
}

int init(int width, int height) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	} 
	
	//Create window	
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
	
	//set up screen texture
	screen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	//allocate pixel buffer
	pixels = (Uint32*) malloc((SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(Uint32));


	if (window == NULL) { 
		
		printf ("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	}

	if (screen == NULL) { 
		
		printf ("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	}
	
	if (pixels == NULL) {
	
		printf ("Error allocating pixel buffer");
		
		return 1;
	}

	return 0;
}

void checkButtons(SDL_Event* event, bool motionevent){
	// printf("%" PRId64 " DOWN\n", event.tfinger.fingerId);
	int fingerx = event->tfinger.x * SCREEN_WIDTH;
	int fingery = event->tfinger.y * SCREEN_HEIGHT;
	// forward button
	if (fingerx < forwardButton.x + BUTTON_WIDTH && fingerx > forwardButton.x && fingery < forwardButton.y + BUTTON_HEIGHT  && fingery > forwardButton.y ){
		push(&button_presses, forwardID, event->tfinger.fingerId);
		forwardButton.y+= BUTTON_HELD_OFFSET;
		forward_held = true;
	}
	// left button
	else if (fingerx < leftButton.x + BUTTON_WIDTH && fingerx > leftButton.x && fingery < leftButton.y + BUTTON_HEIGHT  && fingery > leftButton.y ){
		push(&button_presses, leftID, event->tfinger.fingerId);
		leftButton.y+= BUTTON_HELD_OFFSET;
		left_held = true;
	}
	// right button
	else if (fingerx < rightButton.x + BUTTON_WIDTH && fingerx > rightButton.x && fingery < rightButton.y + BUTTON_HEIGHT  && fingery > rightButton.y ){
		push(&button_presses, rightID, event->tfinger.fingerId);
		rightButton.y+= BUTTON_HELD_OFFSET;
		right_held = true;
	}
	else if (!motionevent){
		shoot_bullet(&p);
	}
}

void popButtonEvent(struct StackNode* buttonStack, struct StackNode* temp_buttons, SDL_Event* event){
	// check which finger was lifted
	while ( !isEmpty(buttonStack) ){
		struct button_event temp = pop(&buttonStack);
		printf("%" PRId64 " UP\n", temp.finger_id);
		if (temp.finger_id == event->tfinger.fingerId){
			switch (temp.button_id){
				case leftID:	
					left_held = false;
					break;
				case rightID:
					right_held = false;
					break;
				case forwardID:
					forward_held = false;
					break;
			}
		}
		else {
			push(&temp_buttons, temp.button_id, temp.finger_id);
		}	
	}
	// fill button id stack with new values
	while (!isEmpty(temp_buttons)){
		struct button_event temp = pop(&temp_buttons);
		push(&button_presses, temp.button_id, temp.finger_id);
	}
	free(temp_buttons);
}