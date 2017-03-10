In order to detect the object, we first make sure that the boolean value of trackObjects to be True in the main function of Object_Tracking_and_Filtering.cpp file. (it is true by default). We call the trackFilteredObject function.
We declare a vector of points, then we run the findcontours function from OpenCv library and pass in the filtered image. This will track the object. To track the object, we used the drawing functions from the OpenCv library such as "circle()". It will draw around the object to track it.


In order to take care of the lightning conditions we have created track bars in Object_Tracking_and_Filtering.cpp, so it will change the Image from RGB  to HSV (Hue, saturation Value).
This helps us depict the object in bad lightning conditions. To further accomodate this issue, we decided to use the Erode or Dilate function. Firstly, in main, we have to set the boolean variable of useMorphOps to be true. (It is set to be false by default, we can change it as per need)
Lets say that we decide to use threshold window. Meaning, we black out the entire background, but just keep the object of interest white (we achieve this by using the track bars).
There might be some noise left that could mess up with the object. Therefore we use Erode, which will make the white spaces smaller and clear up the object. The Dilate will make the white spaces in the Object of interest
bigger, this way we can depict the object nicely even in bad lightning conditions. This will also help with the object being distant. Using these techniques, the stabliziation will be the same when the object gets smaller in size (gets farther from the camera).

