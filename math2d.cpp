#include "math2d.h"

/*
 * Texture manipulation.
 */
/*
 * Flip a texture across its centerline, vertically.
 *   tex_data: Texture data; poitner to x * y * n bytes.
 *          x: X-dimension of the texture.
 *          y: Y-dimension of the texture.
 *          n: # of channels, i.e. 4 for RGBA.
 */
void flip_tex_V(unsigned char* tex_data, int x, int y, int n) {
	// Use a buffer. We could swap with XOR, but it'd be awful to debug
	// and this memory is freed quickly.
	char temp_row;

	for (int i=0; i<y/2; i++) {
		// Pointers to the top and bottom rows.
		unsigned char* top = tex_data + (sizeof(char) * x * i * n);;
		unsigned char* bottom = tex_data + (sizeof(char) * x * (y-(i+1)) * n);

		// Account for odd y-dimension, but images -should- have y%2==0.
		if (top == bottom) { break; }
		// top -> temp, bottom -> top, temp -> bottom
		for (int j=0; j<x*n; j++) {
			temp_row = *top;
			*top = *bottom;
			*bottom = temp_row;
			top++;
			bottom++;
		}
	}
}
