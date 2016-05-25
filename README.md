# disparity
Rough tests for finding disparity maps between multiple (e.g. stereo) images

Included openFrameworks project using openCV:
* Click on left image to find corresponding point in right image
* Drag a box, to track corresponding points for all pixels in the box

Disparity pixels map gets drawn on top, and is saved to disk periodically.

Since this version of openFrameworks doesn't support float images, disparity is in 8bit, centred on value 128:
* R: 128 + X pixel offset
* G: 128 + Y pixel offset
* B: 'weight' - estimate of how good the track is
