## 3D Labels experiments

### Line Labels

* Fit and repeat
![01](images/01.gif)

* Fade according to tilt angle
![02](images/02.gif)

* Curve path following
![07](images/07.gif)

### Point Labels

* Project top points 
![03](images/03.gif)

* Mutual occlusion based using depth sort and bounding boxes 
![04](images/04.gif)

* Occlude line labels with bounding box 
![06](images/06.gif)

* Street level point of view
![05](images/05.gif)

## Dependencies

* [glmTools](https://github.com/tangram-map/glmTools)
	-	[openGL](http://www.opengl.org/)
	- [glm](http://glm.g-truc.net/0.9.5/index.html)

* [glmFTGL](https://github.com/tangram-map/glmFTGL)
	- [FTGL](http://sourceforge.net/projects/ftgl/)

* [cURL](http://curl.haxx.se/libcurl/)
* [JsonCpp](https://github.com/open-source-parsers/jsoncpp)

## Notes

Compile with linker flags ```-lcurl```