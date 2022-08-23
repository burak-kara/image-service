# Image Operator

Apply image operations to input image.

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
http://<`hostname`>:<`port`>/`v1`/<`operation`>:<`parameters`>/.../`url`:<`image url without the protocol`>

e.g. => http://localhost:8080/v1/resize:640x360/url:upload.wikimedia.org/wikipedia/en/a/a9/Example.jpg

## Run
Note that: The app was developed on Windows, therefore, Windows libraries used.

    ```bash
    cmake .
    cmake --build . --config Release
    ```
You can find the executables in the Release directory.

- `src/main.cpp` starts the Server
- `src/client/main.cpp` starts the test Client
- You can change the image, operation and parameters in the `Client`'s constructor.


