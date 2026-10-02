// gcc bmpshow.c -lglfw -lGL -lm -o bmpshow

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char* data;
    int width, height;
} BMPImage;

BMPImage loadBMP(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) { printf("Error: File not found\n"); exit(1); }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, f);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    
    int row_padded = (width * 3 + 3) & (~3);
    int data_size = row_padded * height;
    unsigned char* data = (unsigned char*)malloc(data_size);
    
    fread(data, sizeof(unsigned char), data_size, f);
    fclose(f);

    return (BMPImage){data, width, height};
}

int main(int argc, char** argv) {

		int FONT_WIDTH=9;
		int FONT_HEIGHT=15;
int Canvas_Width_Columns;
int Canvas_Height_Lines;
int WINDOW_WIDTH=960;
int WINDOW_HEIGHT=15*42;

    if (argc < 2) return -1;

    if (!glfwInit()) return -1;

    BMPImage img = loadBMP(argv[1]);

// since 2024-Dec-03 [
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
// since 2024-Dec-03 ]

    // Get the desktop dimensions and monitor refresh rate
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int refresh_rate = mode->refreshRate;

    // Get the desktop dimensions
    int desktop_width = mode->width;
    int desktop_height = mode->height;
    //printf("Desktop dimensions: %dx%d\n", desktop_width, desktop_height);
    //printf("Monitor Refresh Rate: %d Hz\n", refresh_rate);

    //if (desktop_height >= 1800) WINDOW_HEIGHT = 16*40*2;

// Commented out since r.8++ [
/*
	#ifdef _Wrap_
		#ifdef _9x15_
			Canvas_Width_Columns = 114;
		#else // 

		#ifdef _16x32_
			Canvas_Width_Columns = 128/2;
		#else // 
			Canvas_Width_Columns = 128;
		#endif

		#endif
		int canvas_width = FONT_WIDTH*Canvas_Width_Columns+2+2; // 128 columns 8bits wide in FHD rotated
		int canvas_height = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT + 0*2 + 0*2;
		Canvas_Height_Lines = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT;
		Canvas_Height_Lines =Canvas_Height_Lines/FONT_HEIGHT;
	#else // 
		int canvas_width = (int)( (float)(desktop_width * 10)/11.0f/FONT_WIDTH )*FONT_WIDTH + 1*2 + 1*2;
		int canvas_height = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT + 0*2 + 0*2;
		Canvas_Width_Columns = (int)( (float)(desktop_width * 10)/11.0f/FONT_WIDTH )*FONT_WIDTH;
		Canvas_Width_Columns = Canvas_Width_Columns/FONT_WIDTH;
		Canvas_Height_Lines = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT;
		Canvas_Height_Lines =Canvas_Height_Lines/FONT_HEIGHT;
	#endif
*/
// Commented out since r.8++ ]

// Unify since r.8++ [
		int canvas_width = (int)( (float)(desktop_width * 10)/11.0f/FONT_WIDTH )*FONT_WIDTH + 1*2 + 1*2;
		int canvas_height = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT + 0*2 + 0*2;
		Canvas_Width_Columns = (int)( (float)(desktop_width * 10)/11.0f/FONT_WIDTH )*FONT_WIDTH;
		Canvas_Width_Columns = Canvas_Width_Columns/FONT_WIDTH;
		Canvas_Height_Lines = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT;
		Canvas_Height_Lines =Canvas_Height_Lines/FONT_HEIGHT;
// Unify since r.8++ ]

	WINDOW_WIDTH = canvas_width;
	WINDOW_HEIGHT = canvas_height;

    // Create a windowed mode window and its OpenGL context
// since 2024-Dec-03 [
#if defined(Windowed)
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(img.width, img.height, "0% Opacity Viewer", NULL, NULL);
#else
    // Create a fullscreen window
	WINDOW_WIDTH = mode->width;
	WINDOW_HEIGHT = mode->height;

		canvas_width = (int)( (float)(desktop_width * 10)/11.0f/FONT_WIDTH )*FONT_WIDTH; //+ 1*2 + 1*2;
		canvas_height = (int)( (float)(desktop_height * 10)/11.0f/FONT_HEIGHT )*FONT_HEIGHT + 0*2 + 0*2;
		Canvas_Width_Columns = (int)( (float)(desktop_width)/FONT_WIDTH )*FONT_WIDTH;
		Canvas_Width_Columns = Canvas_Width_Columns/FONT_WIDTH;
		Canvas_Height_Lines = (int)( (float)(desktop_height)/FONT_HEIGHT )*FONT_HEIGHT;
		Canvas_Height_Lines =Canvas_Height_Lines/FONT_HEIGHT;

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Reading and parsing the file...", monitor, NULL);
#endif
// since 2024-Dec-03 ]

    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // --- SETUP BLENDING ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

    while (!glfwWindowShouldClose(window)) {
        // Clearing to a dark gray so you can actually see the transparency effect
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, img.width, 0, img.height, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        // --- APPLY 30% OPACITY ---
        // (Red, Green, Blue, Alpha) -> 0.3f = 30%
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2i(0, 0);
            glTexCoord2f(1, 0); glVertex2i(img.width, 0);
            glTexCoord2f(1, 1); glVertex2i(img.width, img.height);
            glTexCoord2f(0, 1); glVertex2i(0, img.height);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(img.data);
    glfwTerminate();
    return 0;
}

/*
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    unsigned char* data;
    int width, height;
} BMPImage;

BMPImage loadBMP(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) { printf("Error: File not found\n"); exit(1); }

    unsigned char header[54];
    fread(header, sizeof(unsigned char), 54, f);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    
    // BMP rows are padded to multiples of 4 bytes
    int row_padded = (width * 3 + 3) & (~3);
    int data_size = row_padded * height;
    unsigned char* data = (unsigned char*)malloc(data_size);
    
    fread(data, sizeof(unsigned char), data_size, f);
    fclose(f);

    return (BMPImage){data, width, height};
}

int main(int argc, char** argv) {
    if (argc < 2) { printf("Usage: %s <file.bmp>\n", argv[0]); return -1; }

    if (!glfwInit()) return -1;

    BMPImage img = loadBMP(argv[1]);

    // Force the window to be non-resizable to maintain 1:1
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(img.width, img.height, "1:1 Integer Setup", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // Texture Setup
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Standard BMP alignment
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload as BGR (Standard BMP format)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.width, img.height, 0, GL_BGR, GL_UNSIGNED_BYTE, img.data);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // --- SETUP INTEGER COORDINATES ---
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        // Sets 0,0 to bottom-left and width,height to top-right
        glOrtho(0, img.width, 0, img.height, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        // --- DRAW USING PIXEL INTEGERS ---
        glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex2i(0, 0);
            glTexCoord2f(1, 0); glVertex2i(img.width, 0);
            glTexCoord2f(1, 1); glVertex2i(img.width, img.height);
            glTexCoord2f(0, 1); glVertex2i(0, img.height);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(img.data);
    glfwTerminate();
    return 0;
}
*/
