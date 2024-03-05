#include "hw2.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief A structure for a pixel
 *
 * A structure representing a pixel. Each pixel has a red, green, and blue component.
 *
 * @param r The red component of the pixel
 * @param g The green component of the pixel
 * @param b The blue component of the pixel
 *
 */
typedef struct Pixel {
	unsigned char r, g, b;
} Pixel;

/**
 * @brief A structure for an image
 *
 * A structure representing an image. The image has a width and a height, and an array of pixels.
 *
 * @param width The width of the image
 * @param height The height of the image
 * @param pixels The array of pixels in the image
 */
typedef struct Image {
	int width, height;
	Pixel **pixels;
} Image;

/**
 * @brief A structure for copying a region of an image
 *
 * A structure representing the parameters for copying a region of an image. The region to be copied
 * is specified by the row and column of the source image where the copying will start, and the width
 * and height of the region to be copied.
 *
 * @param row The row of the source image where the copying will start
 * @param column The column of the source image where the copying will start
 * @param width The width of the region to be copied
 * @param height The height of the region to be copied
 *
 *
 */
typedef struct CopyParams {
	int row, column, width, height;
} CopyParams;

/**
 * @brief A structure for pasting an image onto another image
 *
 * A structure representing the parameters for pasting an image onto another image. The image to be pasted
 * is specified by the row and column of the source image where the pasting will start, and the width and height
 * of the region to be pasted.
 *
 * @param row The row of the source image where the pasting will start
 * @param column The column of the source image where the pasting will start
 *
 */
typedef struct PasteParams {
	int row, column;
} PasteParams;

/**
 * @brief A structure for rendering a message on an image
 *
 * A structure representing the parameters for rendering a message on an image. The message is a string
 * that will be rendered on the image using a font. The font is specified by the path to a file containing
 * the font data, and the font size. The message will be rendered starting at the specified row and column
 * of the image.
 *
 * @param message The message to be rendered
 * @param fontPath The path to the file containing the font data
 * @param fontSize The size of the font
 * @param row The row of the image where the message will be rendered
 * @param col The column of the image where the message will be rendered
 *
 */
typedef struct RenderParams {
	char *message;
	char *fontPath;
	int fontSize;
	int row;
	int col;
} RenderParams;

/**
 * @brief A structure for a single font character
 *
 * A structure representing a single character in a font. Each character has a key (the character itself),
 * a graphical representation (a grid of '*' and ' '), and the number of rows and columns in the grid.
 *
 * @param key The character itself
 * @param data The graphical representation of the character (a grid of '*' and ' ')
 * @param rows Number of rows in the character's graphical representation
 * @param cols Number of columns in the character's graphical representation
 */
typedef struct FontChar {
	char key;           // The character itself
	char **data;        // The graphical representation of the character (a grid of '*' and ' ')
	int rows;           // Number of rows in the character's graphical representation
	int cols;           // Number of columns in the character's graphical representation
} FontChar;

/**
 * @brief A structure for a font containing multiple characters
 *
 * A structure representing a font containing multiple characters. The font is represented as a dynamic array
 * of FontChar structures, each representing a different character.
 *
 * @param characters Dynamic array of FontChar, each representing a different character
 * @param numChars Number of characters in the font
 */
typedef struct Font {
	FontChar *characters;   // Dynamic array of FontChar, each representing a different character
	int numChars;           // Number of characters in the font
} Font;

/**
 * @brief Check if a character is in a string
 * @param option the character to check
 * @param string the string to check
 * @return true if the character is in the string, false otherwise
 */
bool charIn(int option, const char *string);


/**
 * @brief Check if a string starts with another string
 * @param filename the string to check
 * @param string the string to check for
 * @return true if the string starts with the other string, false otherwise
 */
bool startWith(const char *filename, const char *string);

/**
 * @brief Check if the parameters for the -c argument are valid
 * @param params the parameters for the -c argument
 * @return true if the parameters are valid, false otherwise
 */
bool checkCopyParams(char *params);

/**
 * @brief Split a string into an array of strings using a delimiter
 * @param str the string to split
 * @param delim the delimiter to split the string by
 * @return
 */
char **split(const char *str, const char *delim);

/**
 * @brief Count the number of words in a string
 * @param str the string to count the words in
 * @param delim the delimiter to split the string by
 * @param numwords the number of words in the string
 * @return
 */
int count_words(const char *str, const char *delim, int numwords);

/**
 * @brief Get the length of an array of strings
 * @param pString the array of strings
 * @return the length of the array
 */
int arrayLength(char **pString);

/**
 * @brief Free an array of strings
 * @param pString the array of strings to free
 */
void freeArr(char **pString);

/**
 * @brief Check if the parameters for the -p argument are valid
 * @param params the parameters for the -p argument
 * @return true if the parameters are valid, false otherwise
 */
bool checkPasteParams(char *params);

/**
 * @brief Check if the parameters for the -r argument are valid
 * @param params the parameters for the -r argument
 * @return true if the parameters are valid, false otherwise
 */
bool checkRenderParams(char *params);

/**
 * @brief Get the file extension from a filename
 * @param filename the filename to get the extension from
 * @return the file extension
 */
const char *getExt(const char *filename);

/**
 * @brief Load an image from a file
 *
 * Load an image from a file. The file format is determined by the file extension. Supported file formats
 * are PPM and SBU. If the file format is not supported, the function returns an image with width and height
 * set to 0, and pixels set to NULL. If the file extension is PPM, the function calls load_ppm to load the image.
 * If the file extension is SBU, the function calls load_sbu to load the image. Images are converted into a single unified structure
 * for further processing.
 *
 * @param filepath the path to the file containing the image
 * @return the loaded image
 */
Image load_image(const char *filepath);

/**
 * @brief Load an image from a PPM file
 *
 * Load an image from a PPM file. The PPM file format is a simple, portable format for storing images. The file format
 * consists of a header followed by the pixel data. The header contains the magic number, the width and height of the image,
 * and the maximum value for the pixel data. The pixel data is a sequence of RGB values for each pixel in the image.
 *
 * @param filepath the path to the file containing the image
 * @return the loaded image
 */
Image load_ppm(const char *filepath);

/**
 * @brief Load an image from a SBU file
 *
 * Load an image from a SBU file. The SBU file format is a simple, portable format for storing images. The file format
 * consists of a header followed by the pixel data. The header contains the magic number, the width and height of the image,
 * and the maximum value for the pixel data. The pixel data is a sequence of RGB values for each pixel in the image.
 *
 * @param filepath the path to the file containing the image
 * @return the loaded image
 */
Image load_sbu(const char *filepath);

/**
 * @brief Save an image to a file
 *
 * Save an image to a file. The file format is determined by the file extension. Supported file formats
 * are PPM and SBU. If the file format is not supported, the function does nothing. If the file extension is PPM,
 * the function calls save_as_ppm to save the image. If the file extension is SBU, the function calls save_as_sbu to save the image.
 *
 * @param img the image to save
 * @param filepath the path to the file to save the image to
 */
void save_image(const Image *img, const char *filepath);

/**
 * @brief Save an image to a PPM file
 *
 * Save an image to a PPM file. The PPM file format is a simple, portable format for storing images. The file format
 * consists of a header followed by the pixel data. The header contains the magic number, the width and height of the image,
 * and the maximum value for the pixel data. The pixel data is a sequence of RGB values for each pixel in the image.
 *
 * @param img the image to save
 * @param filepath the path to the file to save the image to
 */
void save_as_ppm(const Image *img, const char *filepath);

/**
 * @brief Save an image to a SBU file
 *
 * Save an image to a SBU file. The SBU file format is a simple, portable format for storing images. The file format
 * consists of a header followed by the pixel data. The header contains the magic number, the width and height of the image,
 * and the maximum value for the pixel data. The pixel data is a sequence of RGB values for each pixel in the image.
 *
 * @param img the image to save
 * @param filepath the path to the file to save the image to
 */
void save_as_sbu(const Image *img, const char *filepath);

/**
 * @brief Allocate memory for an array of pixels
 *
 * Allocate memory for an array of pixels. The array is a 2D array of pixels, with the first dimension
 * representing the rows of the image, and the second dimension representing the columns of the image.
 *
 * @param width the width of the image
 * @param height the height of the image
 * @return the allocated array of pixels
 */
Pixel **allocate_pixels(int width, int height);

/**
 * @brief Free memory for an array of pixels
 *
 * Free memory for an array of pixels. The array is a 2D array of pixels, with the first dimension
 * representing the rows of the image, and the second dimension representing the columns of the image.
 *
 * @param pixels the array of pixels to free
 * @param height the height of the image
 */
void free_pixels(Pixel **pixels, int height);

/**
 * @brief Free memory for an image
 *
 * Free memory for an image. The image has a width and a height, and an array of pixels.
 *
 * @param img the image to free
 */
void free_image(Image img);

/**
 * @brief Check if a pixel array contains a color
 *
 * Check if a pixel array contains a color. The pixel array is a 2D array of pixels, with the first dimension
 * representing the rows of the image, and the second dimension representing the columns of the image. The color
 * is specified by the red, green, and blue components of the pixel.
 *
 * @param table the array of pixels to check
 * @param param the color to check for
 * @param size the size of the array
 * @return true if the color is in the array, false otherwise
 */
bool pixelArrayContainsColor(Pixel *table, Pixel param, int size);

/**
 * @brief Get the position of a pixel in a pixel array
 *
 * Get the position of a pixel in a pixel array. The pixel array is a 2D array of pixels, with the first dimension
 * representing the rows of the image, and the second dimension representing the columns of the image. The position
 * is the index of the pixel in the array.
 *
 * @param table the array of pixels to check
 * @param pixel the pixel to check for
 * @param colors the size of the array
 * @return the position of the pixel in the array
 */
int getPosition(Pixel *table, Pixel pixel, int colors);

/**
 * @brief Copy a region of an image and paste it onto another image
 *
 * Copy a region of an image and paste it onto another image. The region to be copied is specified by the row and column
 * of the source image where the copying will start, and the width and height of the region to be copied. The image to be pasted
 * is specified by the row and column of the source image where the pasting will start.
 *
 * @param ptr the image to copy and paste
 * @param copy the parameters for copying the region
 * @param paste the parameters for pasting the image
 * @return 0 if the operation was successful, -1 otherwise
 */
int copy_paste(Image *ptr, CopyParams copy, PasteParams paste);

/**
 * @brief Print a message on an image using a font
 *
 * Print a message on an image using a font. The message is a string that will be rendered on the image using a font.
 * The font is specified by the path to a file containing the font data, and the font size. The message will be rendered
 * starting at the specified row and column of the image.
 *
 * @param ptr the image to print the message on
 * @param render the parameters for rendering the message
 * @return 0 if the operation was successful, -1 otherwise
 *
 */
int print_message(Image *ptr, RenderParams render);

/**
 * @brief Load a font from a file
 *
 * Load a font from a file. The font is represented as a dynamic array of FontChar structures, each representing a different character.
 * The font data is stored in a text file, with each line representing a different character. The character itself is the first
 * character on the line, followed by a grid of '*' and ' ' representing the graphical representation of the character.
 *
 * @param filename the path to the file containing the font data
 * @return the loaded font
 */
char **loadFontsRaw(const char *filename);

/**
 * @brief Get a font character from a font
 *
 * Get a font character from a font. The font is represented as a dynamic array of FontChar structures, each representing a different character.
 * The character is specified by the key, which is the character itself.
 *
 * @param font the font to get the character from
 * @param key the character to get
 * @return the font character
 */
FontChar *getFontChar(Font *font, char key);

/**
 * @brief print raw font data
 * @param fonts the raw font data
 */
void printFontRaw(char **fonts);

/**
 * @brief free raw font data
 * @param fonts the raw font data
 */
void freeFontsRaw(char **fonts);

/**
 * Initialize a font. The font is represented as a dynamic array of FontChar structures, each representing a different character.
 * The number of characters in the font is specified by the numChars parameter.
 *
 * @param font the font to initialize
 * @param numChars the number of characters in the font
 */
void initFont(Font *font, int numChars);

/**
 * @brief Add a font character to a font
 *
 * Add a font character to a font. The font is represented as a dynamic array of FontChar structures, each representing a different character.
 * The character is specified by the key, which is the character itself. The graphical representation of the character is specified by the data,
 * which is a grid of '*' and ' '. The number of rows and columns in the grid is specified by the rows and cols parameters. The index is the position
 * in the font where the character will be added.
 *
 * @param font the font to add the character to
 * @param key the character to add
 * @param data the graphical representation of the character
 * @param rows the number of rows in the character's graphical representation
 * @param cols the number of columns in the character's graphical representation
 * @param index the position in the font where the character will be added
 */
void addFontChar(Font *font, char key, char **data, int rows, int cols, int index);

/**
 * @brief Free memory for a font
 *
 * Free memory for a font. The font is represented as a dynamic array of FontChar structures, each representing a different character.
 *
 * @param font the font to free
 */
void freeFont(Font *font);

/**
 * @brief Load a font from a raw font data
 *
 * This function loads a font from a raw font data. The font is represented as a dynamic array of FontChar structures, each representing a different character.
 *
 * @param rawFont the raw font data
 * @return the loaded font
 */
Font *loadFont(char **rawFont);

/**
 * @brief Scale a font
 *
 * Scale a font. The font is represented as a dynamic array of FontChar structures, each representing a different character. The font size is specified
 * by the fontSize parameter. The font will be scaled to the specified font size.
 *
 * @param font the font to scale
 * @param fontSize the size of the font
 * @return 0 if the operation was successful, -1 otherwise
 */
int scaleFont(Font *font, int fontSize);

/**
 * @brief Check if a column is empty
 *
 * Check if a column is empty. The column is specified by the j parameter. The column is empty if all the characters in the column are ' '.
 *
 * @param pString the array of strings to check
 * @param j the column to check
 * @return true if the column is empty, false otherwise
 */
bool isEmptyCol(char **pString, size_t j);

/**
 * @brief Free memory for a font character
 *
 * Free memory for a font character. The font character has a key (the character itself), a graphical representation (a grid of '*' and ' '), and the number of rows and columns in the grid.
 *
 * @param font the font to free
 * @param i the index of the font character to free
 */
void freeFontChar(const Font *font, int i);

/**
 * @brief Check if a font character overlaps with an image
 *
 * Check if a font character overlaps with an image. The font character is specified by the pChar parameter. The image is specified by the ptr parameter. The position of the font character on the image is specified by the row and col parameters.
 *
 * @param ptr the image to check
 * @param row the row of the image where the font character will be rendered
 * @param col the column of the image where the font character will be rendered
 * @param pChar the font character to check
 * @return true if the font character overlaps with the image, false otherwise
 */
bool fontOverlaps(Image *ptr, int row, int col, FontChar *pChar);

int main(int argc, char **argv) {
	if (argc < 2) {
		return MISSING_ARGUMENT;
	}

	int o;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	int flag4 = 0;
	int flag5 = 0;
	char *input_filename = NULL, *output_filename = NULL;
	char *copyParams = NULL;
	char *pasteParams = NULL;
	char *renderParams = NULL;
	while ((o = getopt(argc, argv, "i:o:c:p:r:")) != -1) {
		switch (o) {
			case 'i':
				if (flag1) return DUPLICATE_ARGUMENT;
				flag1 = true;
				input_filename = optarg;
				if (input_filename == NULL) return MISSING_ARGUMENT;
				if (startWith(input_filename, "-")) return MISSING_ARGUMENT;
				break;
			case 'o':
				if (flag2) return DUPLICATE_ARGUMENT;
				flag2 = true;
				output_filename = optarg;
				if (startWith(output_filename, "-")) return MISSING_ARGUMENT;
				break;
			case 'c':
				if (flag3) return DUPLICATE_ARGUMENT;
				flag3 = 1;
				copyParams = optarg;
				if (startWith(copyParams, "-")) return MISSING_ARGUMENT;
				break;
			case 'p':
				if (flag4) return DUPLICATE_ARGUMENT;
				flag4 = 1;
				pasteParams = optarg;
				if (startWith(pasteParams, "-")) return MISSING_ARGUMENT;
				break;
			case 'r':
				if (flag5) return DUPLICATE_ARGUMENT;
				flag5 = 1;
				renderParams = optarg;
				if (startWith(renderParams, "-")) return MISSING_ARGUMENT;
				break;
			default:
				if (charIn(optopt, "iocpr") == false) {
					return UNRECOGNIZED_ARGUMENT;
				} else {
					return MISSING_ARGUMENT;
				}
		}
	}

	if (!flag1 || !flag2) return MISSING_ARGUMENT;
	if (access(input_filename, F_OK) == -1) return INPUT_FILE_MISSING;
	FILE *file = fopen(output_filename, "w");
	if (file == NULL) return OUTPUT_FILE_UNWRITABLE;
	fclose(file);

	//The -p argument has been provided at least once, but the
	//-c argument was not provided.

	if (flag4 && !flag3) return C_ARGUMENT_MISSING;
	if (flag3 && checkCopyParams(copyParams) == false) return C_ARGUMENT_INVALID;
	if (flag4 && checkPasteParams(pasteParams) == false) return P_ARGUMENT_INVALID;
	if (flag5 && checkRenderParams(renderParams) == false) return R_ARGUMENT_INVALID;

	Image img = load_image(input_filename);

	if (flag3) {
		char **pString = split(copyParams, ",");
		CopyParams copy = {atoi(pString[0]), atoi(pString[1]), atoi(pString[2]), atoi(pString[3])};
		freeArr(pString);

		if (flag4) {
			pString = split(pasteParams, ",");
			PasteParams paste = {atoi(pString[0]), atoi(pString[1])};
			freeArr(pString);

			copy_paste(&img, copy, paste);
		}

	}

	if (flag5) {
		char **pString = split(renderParams, ",");

		RenderParams render = {pString[0], pString[1], atoi(pString[2]), atoi(pString[3]), atoi(pString[4])};
		int result = print_message(&img, render);
		if (result != 0) {
			freeArr(pString);
			free_image(img);
			return result;
		}
		freeArr(pString);

	}


	// Test save_as_ppm
	save_image(&img, output_filename);

	free_image(img);

	(void) argc;
	(void) argv;

	return 0;
}

int copy_paste(Image *ptr, CopyParams copy, PasteParams paste) {
	//Ignore out of bounds copy and paste operations
	int srcRow = copy.row;
	int srcCol = copy.column;
	int destRow = paste.row;
	int destCol = paste.column;
	int width = copy.width;
	int height = copy.height;

	Pixel **temp = allocate_pixels(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (srcRow + i >= ptr->height || srcCol + j >= ptr->width) {
				continue;
			}
			temp[i][j] = ptr->pixels[srcRow + i][srcCol + j];
		}
	}

	for (int i = 0; i < height; i++) {
		if (destRow + i >= ptr->height || srcRow + i >= ptr->height) {
			continue;
		}
		for (int j = 0; j < width; j++) {
			if (destCol + j >= ptr->width || srcCol + j >= ptr->width) {
				continue;
			}
			ptr->pixels[destRow + i][destCol + j] = temp[i][j];
		}
	}

	free_pixels(temp, height);

	return 0;
}

bool checkRenderParams(char *params) {
	char **pString = split(params, ",");
	if (pString == NULL) {
		return false;
	}
	if (arrayLength(pString) != 5) {
		freeArr(pString);
		return false;
	}

	char *path = pString[1];
	if (access(path, F_OK) == -1) {
		freeArr(pString);
		return false;
	}

	for (int i = 2; i < 5; i++) {
		for (int j = 0; pString[i][j]; j++) {
			if (!isdigit(pString[i][j])) {
				freeArr(pString);
				return false;
			}
		}
	}
	freeArr(pString);
	return true;
}

bool checkPasteParams(char *params) {
	char **pString = split(params, ",");
	if (pString == NULL) {
		return false;
	}

	if (arrayLength(pString) != 2) {
		freeArr(pString);
		return false;
	}

	for (int i = 0; i < 2; i++) {
		for (int j = 0; pString[i][j]; j++) {
			if (!isdigit(pString[i][j])) {
				freeArr(pString);
				return false;
			}
		}
	}
	freeArr(pString);
	return true;
}

int count_words(const char *str, const char *delim, int numwords) {
	int i;

	for (i = 0; str[i] != '\0'; i++) {
		if (!charIn(str[i], delim) &&
			(charIn(str[i + 1], delim) || !str[i + 1])) {
			numwords++;
		}
	}
	return (numwords);
}

char **split(const char *str, const char *delim) {
	int i, j, k, m, numwords = 0;
	char **s;

	if (str == NULL || str[0] == 0)
		return (NULL);
	if (!delim)
		delim = " ";

	numwords = count_words(str, delim, numwords);
	if (numwords == 0) {
		return (NULL);
	}
	s = malloc((1 + numwords) * sizeof(char *));
	if (!s)
		return (NULL);
	for (i = 0, j = 0; j < numwords; j++) {
		while (charIn(str[i], delim) == 1)
			i++;

		k = 0;
		while (!charIn(str[i + k], delim) && str[i + k])
			k++;

		s[j] = malloc((k + 1) * sizeof(char));
		if (!s[j]) {
			for (k = 0; k < j; k++)
				free(s[k]);

			free(s);
			return (NULL);
		}
		for (m = 0; m < k; m++)
			s[j][m] = str[i++];
		s[j][m] = 0;
	}
	s[j] = NULL;
	return (s);
}

int arrayLength(char **pString) {
	int length = 0;
	while (pString[length] != NULL) {
		length++;
	}
	return length;
}

void freeArr(char **pString) {
	for (int i = 0; pString[i] != NULL; i++) {
		free(pString[i]);
	}
	free(pString);
}

bool checkCopyParams(char *params) {
	char **pString = split(params, ",");
	if (pString == NULL) {
		return false;
	}
	if (arrayLength(pString) != 4) {
		freeArr(pString);
		return false;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; pString[i][j]; j++) {
			if (!isdigit(pString[i][j])) {
				freeArr(pString);
				return false;
			}
		}
	}
	freeArr(pString);
	return true;
}

bool startWith(const char *string1, const char *string) {
	for (size_t i = 0; i < strlen(string); i++) {
		if (string1[i] != string[i]) {
			return false;
		}
	}
	return true;
}

bool charIn(int option, const char *string) {
	for (size_t i = 0; i < strlen(string); i++) {
		if (option == string[i]) {
			return true;
		}
	}
	return false;
}

const char *getExt(const char *filename) {
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) return ""; // No extension found
	return dot + 1; // Skip the dot itself
}

Pixel **allocate_pixels(int width, int height) {
	Pixel **pixels = malloc(height * sizeof(Pixel *));
	for (int i = 0; i < height; i++) {
		pixels[i] = malloc(width * sizeof(Pixel));
		pixels[i] = memset(pixels[i], 0, width * sizeof(Pixel));


	}
	return pixels;
}

void free_pixels(Pixel **pixels, int height) {
	for (int i = 0; i < height; i++) {
		free(pixels[i]);
	}
	free(pixels);
}

void free_image(Image img) {
	if (img.pixels == NULL) return;
	free_pixels(img.pixels, img.height);
}

Image load_image(const char *filepath) {
	const char *extension = getExt(filepath);
	Image img;

	if (strcmp(extension, "ppm") == 0) {
		img = load_ppm(filepath);
	} else if (strcmp(extension, "sbu") == 0) {
		img = load_sbu(filepath);
	} else {
		// Handle unsupported file types
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
	}

	return img;
}

Image load_ppm(const char *filepath) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		// Handle file not found
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		return img;
	}

	char magic[3];
	fscanf(file, "%s", magic);
	if (strcmp(magic, "P3") != 0) {
		// Handle unsupported file types
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		fclose(file);
		return img;
	}

	int width, height, max;
	fscanf(file, "%d %d %d", &width, &height, &max);
	Pixel **pixels = allocate_pixels(width, height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			fscanf(file, "%hhu %hhu %hhu", &pixels[i][j].r, &pixels[i][j].g, &pixels[i][j].b);
		}
	}
	fclose(file);

	Image img;
	img.width = width;
	img.height = height;
	img.pixels = pixels;
	return img;
}

void save_image(const Image *img, const char *filepath) {
	const char *extension = getExt(filepath);

	if (strcmp(extension, "ppm") == 0) {
		save_as_ppm(img, filepath);
	} else if (strcmp(extension, "sbu") == 0) {
		save_as_sbu(img, filepath);
	} else {
		// Handle unsupported file types
		perror("Unsupported file type");
		exit(EXIT_FAILURE);
	}
}

void save_as_ppm(const Image *img, const char *filepath) {
	FILE *file = fopen(filepath, "w");
	if (!file) {
		// Handle file not found
		return;
	}

	fprintf(file, "P3\n");
	fprintf(file, "%d %d\n", img->width, img->height);
	fprintf(file, "255\n");
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			fprintf(file, "%hhu %hhu %hhu ", img->pixels[i][j].r, img->pixels[i][j].g, img->pixels[i][j].b);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

Image load_sbu(const char *filepath) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		// Handle file not found
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		return img;
	}

	char magic[4];
	fscanf(file, "%s", magic);
	if (strcmp(magic, "SBU") != 0) {
		// Handle unsupported file types
		Image img;
		img.width = 0;
		img.height = 0;
		img.pixels = NULL;
		fclose(file);
		return img;
	}

	int width, height;
	fscanf(file, "%d %d", &width, &height);

	int num_colors;
	fscanf(file, "%d", &num_colors);

	Pixel color_table[num_colors];
	for (int i = 0; i < num_colors; i++) {
		fscanf(file, "%hhu %hhu %hhu", &color_table[i].r, &color_table[i].g, &color_table[i].b);
	}

	Pixel **pixels = allocate_pixels(width, height);

	// read the next lines word by word and check if the word is a number or a run length encoding
	// if it is a number, then it is a single pixel, if it is a run length encoding, then it is a run length encoding
	// and we need to decode it and fill the pixels array
	int i = 0;
	int j = 0;
	int color_index;
	int run_length;

	// go to the next line

	char c;

	while ((c = fgetc(file)) != EOF) {
		if (isdigit(c)) {
			ungetc(c, file);
			fscanf(file, "%d", &color_index);
			// fill the pixels array
			pixels[i][j] = color_table[color_index];
			j++;
			if (j == width) {
				j = 0;
				i++;
			}
		} else if (c == '*') {
			fscanf(file, "%d %d", &run_length, &color_index);

			// fill the pixels array
			for (int k = 0; k < run_length; k++) {
				pixels[i][j] = color_table[color_index];
				j++;
				if (j == width) {
					j = 0;
					i++;
				}

				if (i == height) {
					break;
				}
			}
		}
	}

	fclose(file);

	Image img;
	img.width = width;
	img.height = height;
	img.pixels = pixels;
	return img;
}

void save_as_sbu(const Image *img, const char *filepath) {
	FILE *file = fopen(filepath, "w");
	if (!file) {
		// Handle file not found
		return;
	}

	fprintf(file, "SBU\n");
	fprintf(file, "%d %d\n", img->width, img->height);
	// create a color table use malloc start with 1
	int num_colors = 0;
	Pixel *color_table = malloc(1 * sizeof(Pixel));
	Pixel *temp = NULL;
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			if (!pixelArrayContainsColor(color_table, img->pixels[i][j], num_colors)) {
				num_colors++;
				temp = reallocarray(color_table, num_colors, sizeof(Pixel));
				if (temp == NULL) {
					free(color_table);
					fprintf(stderr, "Failed to allocate memory.\n");
					exit(EXIT_FAILURE);
				}
				color_table = temp;
				color_table[num_colors - 1] = img->pixels[i][j];
			}
		}
	}

	fprintf(file, "%d ", num_colors);
	for (int i = 0; i < num_colors; i++) {
		fprintf(file, "%hhu %hhu %hhu ", color_table[i].r, color_table[i].g, color_table[i].b);
	}

	fprintf(file, "\n");
	int run_length;
	int j = 0;
	int i = 0;
	while (i < img->height) {
		run_length = 1;
		Pixel current_pixel = img->pixels[i][j];
		while (true) {
			j++;
			if (j == img->width) {
				j = 0;
				i++;
			}
			if (i == img->height) {
				break;
			}
			if (img->pixels[i][j].r == current_pixel.r && img->pixels[i][j].g == current_pixel.g &&
				img->pixels[i][j].b == current_pixel.b) {
				run_length++;
			} else {
				break;
			}
		}


		int pos = getPosition(color_table, current_pixel, num_colors);
		if (pos == -1) {
			fprintf(stderr, "Failed to find color in color table.\n");
			exit(EXIT_FAILURE);
		}
		if (run_length == 1) {
			fprintf(file, "%d ", pos);
		} else {
			fprintf(file, "*%d %d ", run_length, pos);
		}

	}

	free(color_table);
	color_table = NULL;
	fclose(file);
}

int getPosition(Pixel *table, Pixel pixel, int colors) {
	for (int i = 0; i < colors; i++) {
		if (table[i].r == pixel.r && table[i].g == pixel.g && table[i].b == pixel.b) {
			return i;
		}
	}
	return -1;
}

bool pixelArrayContainsColor(Pixel *table, Pixel param, int size) {
	for (int i = 0; i < size; i++) {
		if (table[i].r == param.r && table[i].g == param.g && table[i].b == param.b) {
			return true;
		}
	}
	return false;
}

char **loadFontsRaw(const char *filename) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		// Handle file not found
		return NULL;
	}

	char **fonts = NULL;
	size_t rowsAllocated = 0;
	size_t currentRowLength = 0;
	size_t rows = 0;
	int ch;
	char *lineBuffer = NULL;
	size_t lineBufferSize = 0;

	while ((ch = fgetc(file)) != EOF) {
		// Check if we need to start a new row
		if (ch == '\n' || ch == '\r') {
			// Add a new row to fonts
			if (rows == rowsAllocated) {
				// Need more space
				size_t newSize = rowsAllocated == 0 ? 1 : rowsAllocated * 2;
				char **newFonts = realloc(fonts, newSize * sizeof(char *));
				if (!newFonts) {
					// Handle memory allocation failure
					fclose(file);
					return NULL; // In real code, you'd also free the already allocated memory
				}

				// Initialize new elements only if the size has increased
				if (newSize > rowsAllocated) {
					for (size_t i = rowsAllocated; i < newSize; i++) {
						newFonts[i] = NULL; // Initialize new pointers to NULL
					}
				}

				fonts = newFonts;
				rowsAllocated = newSize;
			}
			// Null-terminate the current line
			lineBuffer = realloc(lineBuffer, currentRowLength + 1);
			lineBuffer[currentRowLength] = '\0';
			fonts[rows++] = lineBuffer;
			// Reset for the next line
			lineBuffer = NULL;
			lineBufferSize = 0;
			currentRowLength = 0;
		} else {
			if (currentRowLength == lineBufferSize) {
				size_t newSize = lineBufferSize == 0 ? 1 : lineBufferSize * 2;
				char *newLineBuffer = realloc(lineBuffer, newSize);
				if (!newLineBuffer) {
					// Handle memory allocation failure
					fclose(file);
					return NULL;
				}
				lineBuffer = newLineBuffer;
				lineBufferSize = newSize;
			}
			lineBuffer[currentRowLength++] = (char) ch;
		}
	}
	// Handle last line if it doesn't end with a newline
	if (currentRowLength > 0) {
		// Finish the last line
		lineBuffer = realloc(lineBuffer, currentRowLength + 1);
		lineBuffer[currentRowLength] = '\0';
		if (rows == rowsAllocated) {
			char **newFonts = realloc(fonts, (rowsAllocated + 1) * sizeof(char *));
			if (!newFonts) {
				// Handle memory allocation failure
				fclose(file);
				return NULL;
			}
			fonts = newFonts;
		}
		fonts[rows++] = lineBuffer;
	} else if (lineBuffer != NULL) {
		// Free the unused buffer if the file ends with a newline
		free(lineBuffer);
	}

	fclose(file);
	return fonts;
}

void printFontRaw(char **fonts) {
	int rows = 0;
	int cols = 0;
	for (size_t i = 0; fonts[i] != NULL; i++) {
		char *row = fonts[i];
		cols = 0;
		for (size_t j = 0; row[j] != '\0'; j++) {
			if (isEmptyCol(fonts, j)) {
				printf("%s", "  ");
			} else {
				printf("%c", row[j]);
			}

			cols++;
		}
		printf("\n");
		rows++;
	}

	printf("Number of rows: %d\n", rows);
	printf("Number of cols: %d\n", cols);
}

bool isEmptyCol(char **pString, size_t j) {
	for (size_t i = 0; pString[i] != NULL; i++) {
		char character = pString[i][j];
		if (character != ' ') {
			return false;
		}
	}
	return true;
}

void freeFontsRaw(char **fonts) {
	if (!fonts) {
		return; // Nothing to free
	}

	// Free each row
	for (int i = 0; fonts[i] != NULL; i++) {
		free(fonts[i]);
	}

	// Free the array of rows
	free(fonts);
}

void initFont(Font *font, int numChars) {
	font->numChars = numChars;
	font->characters = (FontChar *) malloc(numChars * sizeof(FontChar));
}

void addFontChar(Font *font, char key, char **data, int rows, int cols, int index) {
	if (index >= font->numChars) return; // Safety check

	font->characters[index].key = key;
	font->characters[index].rows = rows;
	font->characters[index].cols = cols;
	font->characters[index].data = data;

}

void freeFont(Font *font) {
	for (int i = 0; i < font->numChars; i++) {
		freeFontChar(font, i);
	}
	free(font->characters);
}

void freeFontChar(const Font *font, int i) {
	for (int j = 0; j < font->characters[i].rows; j++) {
		free(font->characters[i].data[j]);
	}
	free(font->characters[i].data);
}

Font *loadFont(char **rawFont) {
	// Count the number of characters in the font

	int numChars = 0;
	int rows = 0;
	for (int i = 0; rawFont[i] != NULL; i++) {
		char *row = rawFont[i];
		numChars = 0;
		for (size_t j = 0; row[j] != '\0'; j++) {
			//trim first and last empty columns
			if (j == 0) {
				while (isEmptyCol(rawFont, j)) {
					j++;
				}
			}

			if (isEmptyCol(rawFont, j)) {
				while (isEmptyCol(rawFont, (j + 1))) {
					j++;
				}
				numChars++;
			}

		}
		rows++;
	}

	// Initialize the font
	Font *font = (Font *) malloc(sizeof(Font));
	initFont(font, numChars);

	int startCol = 0;
	char *keys = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	for (int i = 0; i < numChars; i++) {
		char key = keys[i];

		if (i == 0) {
			while (isEmptyCol(rawFont, startCol)) {
				startCol++;
			}
		}


		int endCol = startCol;
		while (isEmptyCol(rawFont, endCol) == 0) {
			//check if there are any other consecutive empty columns
			endCol++;
		}
		int cols = endCol - startCol;
		char **data = (char **) malloc(rows * sizeof(char *));
		for (int j = 0; j < rows; j++) {
			data[j] = (char *) malloc((cols + 1) * sizeof(char));
			strncpy(data[j], rawFont[j] + startCol, cols);
			data[j][cols] = '\0';
		}
		addFontChar(font, key, data, rows, cols, i);
		startCol = endCol + 1;

		while (isEmptyCol(rawFont, startCol)) {
			startCol++;
		}

	}

	return font;
}

FontChar *getFontChar(Font *font, char key) {
	key = (char) toupper((int) key);
	for (int i = 0; i < font->numChars; i++) {
		if (font->characters[i].key == key) {
			return &font->characters[i];
		}
	}
	return NULL;
}

void printFontChar(FontChar *fontChar) {
	printf("Character: %c\n", fontChar->key);
	printf("Rows: %d\n", fontChar->rows);
	printf("Cols: %d\n", fontChar->cols);
	for (int i = 0; i < fontChar->rows; i++) {
		printf("%s\n", fontChar->data[i]);
	}
}

int scaleFont(Font *font, int fontSize) {
	for (int i = 0; i < font->numChars; i++) {
		FontChar *fontChar = &font->characters[i];
		int rows = fontChar->rows;
		int cols = fontChar->cols;
		char **data = (char **) malloc(fontSize * rows * sizeof(char *));
		memset(data, 0, fontSize * rows * sizeof(char *));
		for (int j = 0; j < fontSize * rows; j++) {
			data[j] = (char *) malloc((1 + (fontSize * cols)) * sizeof(char));
			memset(data[j], 0, (1 + (fontSize * cols)) * sizeof(char));
		}


		for (int j = 0; j < rows; j++) {
			for (int k = 0; k < cols; k++) {
				for (int l = 0; l < fontSize; l++) {
					for (int m = 0; m < fontSize; m++) {
						data[j * fontSize + l][k * fontSize + m] = fontChar->data[j][k];
					}
				}
			}
		}

		freeFontChar(font, i);

		fontChar->data = data;
		fontChar->rows = fontSize * rows;
		fontChar->cols = fontSize * cols;
	}
	return 0;
}

int print_message(Image *ptr, RenderParams render) {
	char *fontsPath = render.fontPath;
	char **fonts = loadFontsRaw(fontsPath);
	int fontSize = render.fontSize;
	char *message = render.message;
	if (fonts == NULL) {
		(void) ptr;
		return MISSING_ARGUMENT;
	}

	Font *font = loadFont(fonts);
	scaleFont(font, fontSize);


	for (size_t i = 0; i < strlen(message); i++) {
		//a space character in the input message should
		//still be rendered as five blank columns of pixels

		if (message[i] == ' ') {
			render.col += 5;
			continue;
		}

		FontChar *fontChar = getFontChar(font, message[i]);

		if (fontChar == NULL) {
			freeFontsRaw(fonts);
			freeFont(font);
			free(font);
			(void) ptr;
			return MISSING_ARGUMENT;
		}

		if (!fontOverlaps(ptr, render.row, render.col, fontChar)) {
			for (int j = 0; j < fontChar->rows; j++) {
				for (int k = 0; k < fontChar->cols; k++) {
					if (fontChar->data[j][k] == '*') {
						if (render.row + j >= ptr->height || render.col + k >= ptr->width) {
							continue;
						}
						ptr->pixels[render.row + j][render.col + k].r = 255;
						ptr->pixels[render.row + j][render.col + k].g = 255;
						ptr->pixels[render.row + j][render.col + k].b = 255;
					}
				}
			}
		}


		//o leave only one space between adjacent letters
		render.col += fontChar->cols + 1;
	}


	freeFontsRaw(fonts);
	freeFont(font);
	free(font);
	(void) ptr;
	return 0;
}

bool fontOverlaps(Image *ptr, int row, int col, FontChar *pChar) {
	for (int i = 0; i < pChar->rows; i++) {
		for (int j = 0; j < pChar->cols; j++) {
			if (pChar->data[i][j] == '*' && (row + i >= ptr->height || col + j >= ptr->width)) {
				return true;
			}
		}
	}
	return false;
}

