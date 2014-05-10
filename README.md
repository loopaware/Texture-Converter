Texture Converter - Manual
=========================
*Bakes together 2 images into the resulting image RGB=(A:rgb) + A=(B:avg(rgb))*

**Usage**

In windows explorer, select 2 images and drag&drop them onto the executable.
When selecting, make sure to select the RGB source image first.

**What it does**

Takes the RGB channels from image B.
Calculates the greyscale of them, ie averages them into one value.
The final image exported has the RGB-values from image A, and as its alpha value is the calculated greyscale value of image B.
In short:
RGB=(A:rgb) 
A=(B:avg(rgb))

**Why?**

In the current version of the Oyster rendering engine there is no support for transparent objects, aside from particles. This makes the alpha-channel of the diffuse texture redundant. We made the design choice to use that free alpha channel to store our glow-factor.
The same goes for our normal map, we use the alpha value slot to store our specular factor. The specular light factor has been reduced to one value; as opposed to two values.

MADE FOR the indie game NO EDGE, http://noedge.nerdcavestudio.com/

