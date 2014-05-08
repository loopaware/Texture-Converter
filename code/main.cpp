#include <iostream>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <glm/glm.hpp>
#include "main.h"
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    cout << "Initializing..." << endl;

    // init DevIL
    initDevIL();

////////////////////

    // Fetch file args
    if (argc < 3)
    {
        cerr << "ERROR: must provide 2 filepaths." << std::endl;
        system("PAUSE");
        return 1;
    }

    char* diffuseFilePath   = argv[1];
    char* glowFilePath      = argv[2];

    cout << "Diffuse map:\t"    <<diffuseFilePath   << endl;
    cout << "Glow map:\t"       << glowFilePath     << endl;

/*
    char* diffuseFilePath   = "jumppad_pointy_md.png";
    char* glowFilePath      = "jumppad_pointy_mg.png";*/
////////////////////

////////////////////
    cout << "Converting..." << endl;

    ILuint imageA;
    ILuint imageB;
    ILuint finalImage;
    int status;

    ilGenImages(1, &finalImage); 		// Generate the image ID
	ilBindImage(finalImage); 			// Bind the image

    ////////////////
    // Load RGB source image (diffusemap)
    imageA = loadImage(diffuseFilePath, &status);
    ilBindImage(imageA); 			        // Bind the image
    ILubyte *imageA_data = ilGetData();

    // Get width from imageA
    ILuint widthA  = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint heightA = ilGetInteger(IL_IMAGE_HEIGHT);

    // Load A source image ( glowmap )
    imageB = loadImage(glowFilePath, &status);
    ilBindImage(imageB); 			        // Bind the image
    ILubyte *imageB_data = ilGetData();

    // Get width from imageB
    ILuint widthB  = ilGetInteger(IL_IMAGE_WIDTH);
    ILuint heightB = ilGetInteger(IL_IMAGE_HEIGHT);

    // Check íf same size ( exit if not )
    if ((widthA*heightA) != (widthB*heightB))
    {
        cerr << "ERROR: Sizes of A and B does not match!" << endl;
        system("PAUSE");
        return 1;
    }

    //
    ILuint   finalImage_size = (widthA*heightA)*4;
    ILubyte *finalImage_data = new ILubyte[finalImage_size];
    memset(finalImage_data, (unsigned char)255, finalImage_size);

    // Loop through data
    ILuint width  = widthA;
    ILuint height = heightA;
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            // Image A
            //cout << "A-rgba: " << (unsigned int)imageA_data[(i*width + j)*4 + 0] << " " << (unsigned int)imageA_data[(i*width + j)*4 + 1] << " " << (unsigned int)imageA_data[(i*width + j)*4 + 2] << " " << (unsigned int)imageA_data[(i*width + j)*4 + 3] << endl;

            // Image B
            //cout << "B-rgba: " << j << " " << (unsigned int)imageB_data[(i*width + j)*4 + 0] << " " << (unsigned int)imageB_data[(i*width + j)*4 + 1] << " " << (unsigned int)imageB_data[(i*width + j)*4 + 2] << " " << (unsigned int)imageB_data[(i*width + j)*4 + 3] << endl;

            // Calculate glow (Alpha channel)
            float glowFactor = (imageB_data[(i*width + j)*4 + 0] + imageB_data[(i*width + j)*4 + 1] + imageB_data[(i*width + j)*4 + 2]) / 3.0f;

            // Create final pixel
            finalImage_data[(i*width + j)*4 + 0] = imageA_data[(i*width + j)*4 + 0];
            finalImage_data[(i*width + j)*4 + 1] = imageA_data[(i*width + j)*4 + 1];
            finalImage_data[(i*width + j)*4 + 2] = imageA_data[(i*width + j)*4 + 2];
            finalImage_data[(i*width + j)*4 + 3] = (unsigned char)(unsigned int)glm::floor(glowFactor);

            // Altered
            //cout << "C-rgba: " << (unsigned int)finalImage_data[(i*width + j)*4 + 0] << " " << (unsigned int)finalImage_data[(i*width + j)*4 + 1] << " " << (unsigned int)finalImage_data[(i*width + j)*4 + 2] << " " << (unsigned int)finalImage_data[(i*width + j)*4 + 3] << endl;
            //cout << endl;
        }
        //cout << "B-rgba: " << i << " " << (unsigned int)imageB_data[(i*width)*4 + 0] << " " << (unsigned int)imageB_data[(i*width)*4 + 1] << " " << (unsigned int)imageB_data[(i*width)*4 + 2] << " " << (unsigned int)imageB_data[(i*width)*4 + 3] << endl;
    }
    ////////////////

	ilBindImage(finalImage); 			// Bind the image
    bool texSet = ilTexImage( width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, finalImage_data);
    if (!texSet)
    {
        cerr << "ERROR: Could not pass final texture data into image." << endl;
        return 1;
    }

    ILinfo ImageInfo;
    iluGetImageInfo(&ImageInfo);
    if (ImageInfo.Origin == IL_ORIGIN_LOWER_LEFT)
    {
        iluFlipImage();
    }

    ilEnable(IL_FILE_OVERWRITE);
    ilSave(IL_PNG, ComposeFilename(getFilename(diffuseFilePath)));

    // Cleanup
 	//ilDeleteImages(1, &imageA);
 	//ilDeleteImages(1, &imageB);
 	//ilDeleteImages(1, &finalImage);
////////////////////

    //system("PAUSE");
    cout << "Success!" << endl;
    return 0;
}

void initDevIL()
{
    // Init DevIL (IL, ILU, ILUT)
    ilInit();                   // IL
    iluInit();                  // ILU
    //ilutRenderer(ILUT_OPENGL);  // ILUT with opengl
}

ILuint loadImage(const char* filename, int *status)
{
    ILuint imageID;				// Create an image ID as a ULuint
	ILboolean success;			// Create a flag to keep track of success/failure
	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID
	ilBindImage(imageID); 			// Bind the image
	success = ilLoadImage(filename); 	// Load the image file

	// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_LOWER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			*status = 1;
			return 0;
		}
 	}
  	else // If we failed to open the image file in the first place...
  	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		*status = 2;
        return 0;
  	}
    *status = 0;
  	return imageID;
}

char* getFilename(const char* path)
{
    string s = path;

    char sep = '/';

    #ifdef _WIN32
    sep = '\\';
    #endif

    size_t i = s.rfind(sep, s.length( ));
    if (i != string::npos)
    {
        std::string sFilename = s.substr(i+1, s.length( ) - i);
        char* cFilename = new char[sFilename.size()];
        strcpy(cFilename, sFilename.c_str());

        return cFilename;
    }

   return "";
}

char* ComposeFilename(const char* filename)
{
    std::string oldFilename = filename;
    std::string newFilename = "";

    if (oldFilename.find("_md") != std::string::npos)
    {
        std::cout << "found _md!" << '\n';

        newFilename = oldFilename.substr(0, oldFilename.find("_md"));
        //oldFilename.copy(newFilename, oldFilename.find("_md"), 0);
        newFilename.append("_mdg.png");

        std::cout << newFilename << '\n';

        char* cFilename = new char[newFilename.size()];
        strcpy(cFilename, newFilename.c_str());

        return cFilename;
    }

    if (oldFilename.find("_mn") != std::string::npos)
    {
        std::cout << "found _mn!" << '\n';

        newFilename = oldFilename.substr(0, oldFilename.find("_mn"));
        //oldFilename.copy(newFilename, oldFilename.find("_mn"), 0);
        newFilename.append("_mns.png");

        std::cout << newFilename << '\n';

        char* cFilename = new char[newFilename.size()];
        strcpy(cFilename, newFilename.c_str());

        return cFilename;
    }

   return "";
}
