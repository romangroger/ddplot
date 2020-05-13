# ddplot

Visualization of distortions of crystals containing dislocations using the differential displacement maps. Both screw and edge view are implemented, comparison of two different distortions, geometrical transforms, user-defined styles. Imports data in a variety of formats, exports to Postscript, bitmap and internal .dd formats. The change in the separations of neighboring atoms is depicted by arrows whose lengths correspond to the magnitudes of the changes of their separation relative to their distance in the ideal crystal.

The main features implemented in ddplot are:
- plotting of atomic structure
- visualization of screw and edge components around a dislocation line

projection of relative displacements into an arbitrary direction
scaling of arrows (for the plotting of edge components)
automatic finding of the first to fifth nearest neighbors
unrelaxed/relaxed configuration
coordinate transformations
output to Postscript, .XYZ (JMol), standard .CFG (AtomEye)
group loading of a set of plot files
internal .DD format containing the coordinate system, position of the dislocation, lattice parameter, etc.
creating a .GIF animation from the uploaded blocks
changing colors, line thickness and radii of atoms and arrows
distinguishing atomic layers or atomic types
user-defined settings can be stored in ~/.ddplot or loaded at the start
