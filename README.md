**Notes & Thoughts**
 - Why not implement parameterization stuff in Python? 
 - Seems that this would be easier and not have any real performance hit...

 - Missing functionality from Android app: sort based on order in one colour space 
     (e.g. HSV), but mix based on a different colour space (e.g. RGB). 
   This is partly how the "Antagone" filter works.
 - Missing functionality from Android app: combine pixels via bitwise operations 
   on uint8_t channel values (e.g. XOR, Multiply, etc.).
 - Extension to other non-linear ways to combine pixels:
   - Treat p1, p2 as unit quaternions w/ radius. Quaternions represent points in 
       the unit ball, colours are determined according to spherical coordinates.
     Combine pixels by multiplying/dividing the unit quaternions, and assigning 
       some radius to the resulting unit quaternion (e.g. min/max/avg of p1, p2 
       radii).
   - Geometric mean/function (i.e. mult, sum, sqrt)
 
 - Major meory leak somewhere: image data not being free'd somewhere in the 
   load process. No leaks apparent on save, so colour conversion and retrieval 
   of pixel data seem fine.

**Python API Changes**
 - ADD DOCUMENTATION!!!
 - Scrap C++ for segmentations, make it possible to initialize segments from
   Python API. Make Python Segmentation class.
 - Python Animation class: contains basic FPS/len params, and 
   ```compute_frame(frame, img)``` function that is provided as a lambda. 
   Supports keyframing: switch out ```compute_frame``` for a different 
     implementation at a given time.
 - Animation may use same source image for each frame, progressively sorted 
     frames, or a stream of video frames. 
   Can mix these with keyframing as well.
   Option with video frames to provide frames at reduced (e.g. 1/2) framerate, 
     and use sorting to interpolate frames (either use multiples of the same 
     source frame or progressively).
 - 

**Core Changes**
 - PImpl pattern (finish implementing this).
 - No more Segmentation class. Only Segments.
 - Rectangle segments: 4 "topologies" -- rectangle (no wrap), vertical cylinder 
     (column wrap), horizontal cylinder (row wrap), torus (row & column wrap)
 - New segment: Mask -> set of pixels. 
   - Supports intersection, union, set difference, complement operations.
   - Can be constructed from arbitrary image (e.g. by binning colours and making 
       masks for each bin -> see wiki: Color quantization, 8 bit color).
   - Can use graphics library to "paint" masks (e.g. for arbitrary shapes, curves, 
       patterns).
   - The "mask painting" should be implemented on the python side.
   - The linear ordering of pixels for segment traversal may be imposed on a 
       mask using any real-valued, function ```f: R^2 -> R```
   - Can skew be (efficiently) supported?
 - Move OpenMP concurrency down to the most granular levels (e.g. sorters, 
     segments). 
 - Implement parallel heapify. 
 - Experiment with parallel bubble passes (probably fine when doing multiple 
     passes)
   May require locking unless race conditions don't seriously impact how the 
     effect looks.
 - Asynchronous patterns: futures (submit all tasks that can be run in parallel, 
     then await their completion)
 - Executor pattern: 
 - Global context object containing 
 - 