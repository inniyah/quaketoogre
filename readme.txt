------------
Introduction
------------

In the category 'why would you do that?', I present to you the Quake To Ogre 
Mesh Converter. It can take a 3D model from either Quake 2 or Quake 3 and 
convert it to an equivalent Ogre mesh, including (a selection of) the original
model's animations.

--------
Features
--------

- Convert MD2 or MD3 files to Ogre Mesh XML
- Convert Quake's vertex animations to Ogre's morph animations
- Specify a selection of animations to convert (both MD2 and MD3), or load 
  them from a player model's animation file (MD3 only)
- Convert vertices and normals from Quake's coordinate system to Ogre's system
- Specify material names for each submesh

--------
Building
--------

The program should compile on pretty much any platform, but it's only been 
tested on Windows and GNU/Linux. The only requirements are a decent C++ 
compiler and the standard C math library.

For Windows users, I've included a Visual C++ .NET 2003 (VC7.1) project file. 
It should also work for Visual C++ .NET 2005 (VC8), but that has not been 
tested.

For GNU/Linux, a Makefile has been included that uses 'g++' for compilation. 
To compile the code, just run "make" from the command line. The compiled binary
will be placed in the 'out' directory.

For users of other platforms or IDEs, just creating a new project and adding 
all source files should be enough.

-----
Usage
-----

The program is run from the command line as follows:

QuakeToOgre [config file]

With 'config file' being the name of a valid configuration XML file (more on
that later). The resulting mesh XML file can be converted to a binary Ogre mesh
using the standard 'OgreXmlConverter' tool, with the usual options (generate 
edge lists, compute tangent vectors, etc).

To aid in configuration, it is also possible to print some basic information
about a Quake mesh by running the program as follows:

QuakeToOgre -i [mesh file]

This will list the names of every frame, submesh, tag and shader contained in
the mesh file.

-------------
Configuration
-------------

The configuration file is an XML document containing conversion parameters as
defined by the Document Type Definition in the 'config.dtd' file. Most of the
specification from the DTD file should be pretty self-explanatory, and an 
example configuration file has been supplied to show you how to write a config
XML file. There are a couple of specific details I will explain here though:

- convertcoordinates
The Quake engines use a different coordinate system than Ogre uses. When this
tag is specified, every vertex and normal vector is converted from Quake's
coordinate system to Ogre's. This is useful when you want to use Quake assets
in a game that is otherwise filled with Ogre assets. If you're combining Quake
models with other Quake assets (e.g. you're using Quake 3 maps through the
BspSceneManager), then you will want to omit this tag.

- animationfile
Every Quake 3 player model has a text file containing the specification of
every animation. This file is usually called 'animation.cfg' and can be found
in the same directory as the player model's mesh and texture files. When 
converting MD3 meshes, one of these animation files can be loaded to specify a
series of animations. The names of these animations are fixed to be the same as
the names given in the animation file's comments.
Using either the 'convertlegs' or the 'converttorso' child tag, the full set of
animations for respectively the legs or the torso mesh can be converted. If the
'convertselection' tag is used instead, a specific selection of animations to
convert can be supplied.

- materials
Most (but not all) Quake models contain references to the materials (or shaders
in Quake jargon) they use. In many cases, these shader names will not suffice
for whatever project you're working on. To remedy this, an option has been 
added to specify new material names for every submesh. 
The important thing to note here is that this option only affects the material
references within the resulting Ogre mesh. You still have to write the material
scripts yourself.
