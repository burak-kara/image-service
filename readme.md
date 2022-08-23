# Image Operator
HTTP webserver that performs on-the-fly image resizing from the provided source image url.

###### Notes: 
- It is just a basic HTTP server(ish) implemented with pure socket programming since any third party HTTP server is not allowed to use.
- Additional operations in Bonus 1 are implemented

## Supported operations
- Resize
- Crop
- Rotate
- GrayScale

## Supported operation parameters
- Resize:
  - width
  - height
- Crop:
  - width
  - height
  - xOffset (optional)
  - yOffset (optional)
- Rotate:
  - angle
- GrayScale:

## Input format
http://<`hostname`>:<`port`>/`v1`/<`operation`>:<`parameters`>/`url`:<`image url without the protocol`>

e.g. => http://localhost:8080/v1/resize:640x360/url:upload.wikimedia.org/wikipedia/en/a/a9/Example.jpg

## Run
Note that: The app was developed on Windows, therefore, Windows libraries used.

    ```bash
    cmake .
    cmake --build . --config Release
    ```
You can find the executables in the Release directory.

_PS: ImageMagick should be used in Release mode to perform the operations._

- `src/main.cpp` starts the Server
- Go to browser: [http://localhost:8080/v1/resize:640x360/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg](http://localhost:8080/v1/resize:640x360/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg)
- Sometimes, the server exists after the first request but gives the modified image correctly.
- You can also test with socket client: `src/client/main.cpp` starts the test Client
- Change the image, operation and parameters in the `src/client/main.cpp` file.
- Provide the query as an argument to the ImageServiceClient such as:
  - `GET /v1/resize:640x360/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg`
  - `GET /v1/rotate:90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg`
  - `GET /v1/grayscale/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg`
  - `GET /v1/crop:100x100/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg`
  - `GET /v1/crop:100x100x90x90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg`
  - `GET /v1/WRONG:100x100x90x90/url:upload.wikimedia.org/wikipedia/commons/6/64/Fabritius-vink.jpg`
- Output image `out.jpg`  can be found in root directory. Note that the modified image is returned to the client as a base64 response and written to the file there.

## TODO
- Add more error handling
- Not thread safe
- Bonus 2
- Bonus 3
