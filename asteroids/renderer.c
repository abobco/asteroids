
//renderer.c

#include "renderer.h"

int draw_line(uint32_t* pixel_buffer, int x1, int y1, int x2, int y2, uint32_t colour) {


	//plot the first point
	draw_pixel(pixel_buffer, x1, y1, colour);

	//make sure we draw the line always from left to right
	if (x1 > x2) {

		int temp_x = x1;
		int temp_y = y1;

		x1 = x2;
		y1 = y2;

		x2 = temp_x;
		y2 = temp_y;
	}
	
	int dx = x2 - x1;
	int dy = y2 - y1;

	//the length of the line is greater along the X axis
	if (dx >= fabs(dy)) {
		
		float slope = (float) dy / dx;
	
		//line travels from top to bottom
		if (y1 <= y2) {

			float ideal_y = y1 + slope;
			int y = (int) round(ideal_y);
			float error = ideal_y - y;

			int i = 0;
			
			//loop through all the X values
			for(i = 1; i <= dx; i++) {
				
				int x = x1 + i;
				
				draw_pixel(pixel_buffer, x, y, colour);
				
				error += slope;

				if (error  >= 0.5) {
				
					y++;
					error -= 1;
				}
			}
		}
		
		//line travels from bottom to top
		if (y1 > y2) {
			
			float ideal_y = y1 + slope;
			int y = (int) round(ideal_y);
			float error = ideal_y - y;

			int i = 0;
			
			//loop through all the x values
			for(i = 1; i <= dx; i++) {
				
				int x = x1 + i;
				
				draw_pixel(pixel_buffer, x, y, colour);
				
				error += slope;

				if (error  <= -0.5) {
				
					y--;
					error += 1;
				}
			}
		}


	}

	//the length of the line is greater along the Y axis
	if (fabs(dy) > dx) {
		
		float slope = (float) dx / dy;
		
		//line travels from top to bottom
		if (y1 < y2) {
			
			float ideal_x = x1 + slope;
			int x = (int) round(ideal_x);
			float error = ideal_x - x;

			int i = 0;
			
			//loop through all the y values
			for(i = 1; i <= dy; i++) {
				
				int y = y1 + i;
				
				draw_pixel(pixel_buffer, x, y, colour);
				
				error += slope;

				if (error  >= 0.5) {
				
					x++;
					error -= 1;
				}
			}
		}
		
		//draw line from bottom to top
		if (y1 > y2) {
			
			float ideal_x = x1 - slope;
			int x = (int) round(ideal_x);
			float error = ideal_x - x;

			int i = 0;
			
			//loop through all the y values
			for(i = 1; i <= fabs(dy); i++) {
				
				int y = y1 - i;
				
				draw_pixel(pixel_buffer, x, y, colour);
				
				error += slope;

				if (error  <= -0.5) {
				
					x++;
					error += 1;
				}
			}
		}
	}

	return 0;	
}

int draw_pixel(uint32_t* pixel_buffer, int x, int y, uint32_t colour) {
	
	//dont draw any pixels that are outside of the pixel buffer
	if (x < 0 || y < 0) {
			
		return 1;
	}
	
	//dont draw any pixels that are outside of the pixel buffer
	if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
			
		return 1;
	}

	uint32_t position = y * SCREEN_WIDTH + x;
	pixel_buffer[position] = colour;

	return 0;
}

int draw_box(uint32_t* pixel_buffer, int x, int y, uint32_t colour){
	//dont draw any pixels that are outside of the pixel buffer
	int bullet_size = 4;
	x -= bullet_size/2;
	y -= bullet_size/2;
	if (x < 0 || y < 0) {		
		return 1;
	}
	if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {		
		return 1;
	}

	for ( int i = 0; i < bullet_size; i++){
		for (int j = 0; j < bullet_size; j++){
			uint32_t position = (y+j) * SCREEN_WIDTH + (x+i);
			pixel_buffer[position] = colour;
		}
	}
}

int draw_button(uint32_t* pixel_buffer, int x, int y, int width, int height, int direction, uint32_t color){

	// draw the button outline
	draw_line(pixel_buffer, x, y, x+width, y, color);
	draw_line(pixel_buffer, x+width, y, x+width, y+height, color);
	draw_line(pixel_buffer, x+width, y+height, x, y+height, color);
	draw_line(pixel_buffer, x, y, x, y+height, color);

	int xOffset = width/10;
	int yOffset = height/10;

	// draw a triangle indicating direction
	switch (direction){
		case 0:	// forward arrow
			draw_line(pixel_buffer, x+width/2, y + yOffset, x + width - xOffset, y + height - yOffset, color );
			draw_line(pixel_buffer, x + width - xOffset, y + height - yOffset, x + xOffset, y + height - yOffset, color );
			draw_line(pixel_buffer, x + xOffset, y + height - yOffset, x+width/2, y + yOffset, color );
			break;
		case 1:	// left button
			draw_line(pixel_buffer, x + xOffset, y + height/2, x + width - xOffset, y + yOffset, color );
			draw_line(pixel_buffer, x + width - xOffset, y + yOffset, x + width - yOffset, y +  height - yOffset, color );
			draw_line(pixel_buffer, x + width - xOffset, y + height - yOffset, x + xOffset, y + height/2, color );
			break;
		case 2:	// right button
			draw_line(pixel_buffer, x + xOffset, y + yOffset, x + width - xOffset, y + height/2, color );
			draw_line(pixel_buffer, x + width - xOffset, y + height/2, x + xOffset, y +  height - yOffset, color );
			draw_line(pixel_buffer,x + xOffset, y +  height - yOffset, x + xOffset, y + yOffset, color );
			break;
	}
	return 0;
}

void clear_pixels(uint32_t* pixel_buffer, uint32_t colour) {

	int i = 0;
	int buffer_size = SCREEN_WIDTH * SCREEN_HEIGHT;

	for (i = 0; i < buffer_size; i++) {
		
		pixel_buffer[i] = colour;
	}
}

