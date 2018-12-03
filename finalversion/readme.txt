Our project contains two runnable files:
 - One that detects dartboards on all given images
 - One that detects dartboards on a single image.

Compile all images program with: 
	make all
then run with:
	./output

Compile single images program with:
	make single
then run with:
	./output <dartboard image number>
i.e
	./image 0

Both programs output an image called:
	everythingdart<number>.jpg Containing hough lines and circles and detected 				     dartboards
	filtereddart<number>.jpg Containing just detected dartboards.


To clean workspace and remove generated images, run ./clean.sh
