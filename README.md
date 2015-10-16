# POF Constructor Suite 2.1 - Official Release
==================================================

Contents:
1. Changes from 1.x
2. What is PMF?
3. Changelog
4. Reporting Bugs
5. Document History

---------------------------------------------------
# 1.0 Changes from 1.x
---------------------------------------------------
A) New Internal file format
B) New BSP Compiler
C) New Render Window design + capabilities
D) Moment of Interia (MOI)
E) Support for multiple-polygon leaf nodes in the BSP
F) Support for diffuse, glow and shine in render window
G) Renderer Support for Vertex Buffer Objects

---------------------------------------------------
# 2. What is PMF?
---------------------------------------------------
PMF is PCS Model File, it's POF Constructor Suite 2's internal model format.

It is a flexible model format that is easy to convert others into, and stores all the data needed for a .pof.

This makes it so that PCS2 only has to have one place in the entire code where any model format is converted to .pof.
All supported formats go through PMF as an intermediary and as the active model editor.

---------------------------------------------------
# 3. Changelog
---------------------------------------------------
2.1:
	* Collada import and export
	* Affine transformations
	* Insignia editor and generator
	* Bounding box and radius editor
	* Many performance improvements and bug fixes

2.0.3:
	* Resolved OpenGL VBO related crash on model unload
	* Fixed issue with Geometry filter on/off flag being ignored (oops)
2.0.2:
	* Resolved #35 (graphics error when selecting textures with VBOs enabled)
	* Added "Use OpenGL VBOs (if able)" Option and enabled Vertex Buffer rendering
	* Resolved #64 "Sub-object rotation failure in PCS2.0.1 and 2d" reported by Water
	* Possibly Resolved #68 [and dups] "Inverted bounding box on submodel still showing up".
	* Possibly resolved #61 by adding Option to use or not use the geometry filter (default to on)
2.0.1:
	* (Installer) now write Conversion Options setting COB Scaling
	* BSPGEN's BSP Caching validation has been reokved - models not produced by BSPGEN had BSPGEN tags
	* Resolved an issue causing INSG (Insignia) Corruption (Mantis #59)
	* Resolved an issue with PCS2 not reading COB smoothing data correctly (Mantis #51)
2.0: Initial Release

---------------------------------------------------
# 4. Reporting bugs
---------------------------------------------------
To report a bug register an account at http://www.ferrium.org/mantis/ and then check and make sure your
bug is not already reported - if it is add a note to the existing bug report, otherwise create a new report.

---------------------------------------------------
# 5. Document History
---------------------------------------------------
Jan 26. 2014 - Updated for 2.1
May 15. 2008 - Updated for 2.0.2 added Reporting bugs
Feb 11, 2008 - Updated for 2.0.1
Feb 9, 2008 - Initial revision for PCS2.0 Final release
