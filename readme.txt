Introduction
------------

In the catagory 'abusing something good for evil purposes', I present to you
the Quake To Ogre Mesh Converter. It can take a 3D model from either Quake 2
or Quake 3 and convert it to an equivalent Ogre mesh, including (a selection 
of) the original model's animations.

Features
--------

- Convert MD2 or MD3 files to Ogre Mesh XML
- Convert Quake's vertex animations to Ogre's morph animations
- Specify a selection of animations to convert (both MD2 and MD3), or load 
  them from a player model's animation file (MD3 only)
- Convert vertices and normals from Quake's coordinate system to Ogre's system
- 

Building
--------

- Windows (VC7.1)
- Linux (dependencies = G++ & libmath)

Usage
-----

The program is run from the command line as follows:

QuakeToOgre [configfile]

With 'configfile' being the name of a valid configuration XML file (more on
that later). The resulting mesh XML file can be converted to a binary Ogre mesh
using the standard OgreXmlConverter tool, with the usual options (generate edge
lists, compute tangent vectors, etc).

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
references within the final Ogre mesh. You still have to write the material
script yourself.

Disclaimer (don't keep in readme)
----------

This is something that has been asked for in the past, but has never actually
been made as far as I know. I decided not to ask, but to simply make it myself.

The source code of this program isn't really good. It started as a two-evening
hack job, but as the number of features and configuration options grew and
shrank again, things got constantly refactored and now it's just a mess without
clear design and with lots of naming inconsistencies.

Theoretically, the program could be expanded to allow conversions of complete
Quake 3 player models (legs, torso and head) to a single mesh with the tag
structure converted to a skeleton. It's a pretty complicated conversion though
and I don't have much use for it, so it's not likely that I'll ever add such
a feature.
