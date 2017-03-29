# POF Constructor Suite 2.1 - Official Release
==================================================

Contents:
1. Changes from 1.x
2. What is PMF?
3. Changelog
4. Reporting Bugs
5. Document History

---------------------------------------------------
# 1. Changes from 1.x
---------------------------------------------------
+ New Internal file format
+ New BSP Compiler
+ New Render Window design + capabilities
+ Moment of Interia (MOI)
+ Support for multiple-polygon leaf nodes in the BSP
+ Support for diffuse, glow and shine in render window
+ Renderer Support for Vertex Buffer Objects

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
Since the loss of http://www.ferrium.org/mantis/ all bugs should be reported here via the Github issue tracker.
If a bug is already reported, add a note to the existing issue, otherwise create a new issue.

---------------------------------------------------
# 5. Document History
---------------------------------------------------
  * Mar 29, 2017 - Updated bug reporting
  * Jan 26. 2014 - Updated for 2.1
  * May 15. 2008 - Updated for 2.0.2 added Reporting bugs
  * Feb 11, 2008 - Updated for 2.0.1
  * Feb 9, 2008 - Initial revision for PCS2.0 Final release
