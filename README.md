# disparity
Rough tests for finding disparity maps between multiple (e.g. stereo) images

https://vimeo.com/168115690

Included openFrameworks project using openCV:
* Click on left image to find corresponding point in right image
* Drag a box, to track corresponding points for all pixels in the box

Disparity pixels map gets drawn on top, and is saved to disk periodically.

Since this version of openFrameworks doesn't support float images, displacement is in 8bit, centred on value 128:
* R: 128 + X pixel displacement
* G: 128 + Y pixel displacement
* B: 'weight' - estimate of how good the track is
