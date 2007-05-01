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
- Linux

Usage
-----

- XML config
- Explanation convert coordinates
- Explanation animation file
- Explanation materials (or lack thereof)
- Convert to binary Ogre mesh (+ edge lists, tangents, etc)

Disclaimer (don't keep in readme)
----------

The source code of this program isn't really good, I know that. It started as
a two-evening hack job, but as the number of features and configuration options
grew, things got constantly refactored and now there's not much of a clear 
design left. Anyway, the most important thing is that the thing does its job
and does it well enough.
