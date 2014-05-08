#include <IL/il.h>

/// Init DevIL library
void initDevIL();

/// Load image into memory
ILuint loadImage(const char* filename, int *status = 0);

/// Extracts filename from path
char* getFilename(const char* path);

/// Composes target filename from inputfilename
char* ComposeFilename(const char* filename);
