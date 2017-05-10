In order to detect the objects, we first make sure that the boolean value of trackObjects to be True in the main function of Object_Tracking_and_Filtering.cpp file. (it is true by default). We call the trackFilteredObject function.
We declare a vector of points, then we run the findcontours function from OpenCv library and pass in the filtered image. This will track the object. To track the object, we used the drawing functions from the OpenCv library such as "circle()". It will draw around the object to track it.


In order to take care of the lightning conditions we have created track bars in Object_Tracking_and_Filtering.cpp, so it will change the Image from RGB  to HSV (Hue, saturation Value).
This helps us depict the object in bad lightning conditions. To further accomodate this issue, we decided to use the Erode or Dilate function. Firstly, in main, we have to set the boolean variable of useMorphOps to be true. (It is set to be false by default, we can change it as per need)
Lets say that we decide to use threshold window. Meaning, we black out the entire background, but just keep the object of interest white (we achieve this by using the track bars).
There might be some noise left that could mess up with the object. Therefore we use Erode, which will make the white spaces smaller and clear up the object. The Dilate will make the white spaces in the Object of interest
bigger, this way we can depict the object nicely even in bad lightning conditions. This will also help with the object being distant. Using these techniques, the stabliziation will be the same when the object gets smaller in size (gets farther from the camera).

Now, dealing with the Video stabilization algorithm, it uses any video input file. The algorithm then detects any shaking or blurring within the video. The Algorithm counts the frame of the input video file, and save them in a vector. Then we prepare the previously initiazlized motion estimation builders which were defined in the header file. Then in the code we determine if we need to use
one pass or two pass stabilizers, these are the built in functions from opencv. In the stabilizers we pass in the radius of the motion. Then we check if we need to use wobble supress in the video. we cast stabilizer to simple frame source interface to read stabilized frames. We initialize the Deblur function, then we set up trimmig parameters which zoooms in the video. Finally we use the inpainter, which restores the selected region in an image using the region neighborhood. So the final result would be a stabilized video in .avi format.

Dealing with the implementation of the web server in Raspberry Pi:
Our raspberry pi is connected to a cloud server, which consists of an apache web server,  a redis server instance and a flask script executed by the apache server. When a client connects to our web service. Their browser will request the most recent frame every second. Meanwhile, in the background, the raspberry pi continuously posts images to our webserver. The web server will then combine the most recent frame with the other frames, analyze them, and return a stabilized image. This is the image the web browser will receive at the end. 
We introduced a cloud server, which also ran the web service. This was done so that we could regulate the operations which go on in the raspberry pi because of the limited amount of resources in it. 
Furthermore, we had to regulate the burden of Latency. For, in some conditions the raspberry pi would not be able to communicate with the server effectively if it does not have enough bandwidth. To solve the problem we used only black and white images. This uses 1/3 of the Bandwidth as compared to the regular colored images. This will allow the Pi to function effectively in remote environment where we have limited amount of facilities.  


Final video: https://drive.google.com/open?id=0Bxl76hN_K8DzYkZGZm5mU25zM00
