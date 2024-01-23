# Overview
This is an implementation of Least Significant Bit (LSB) steganography in C. LSB steganography is a technique that involves hiding information within the least significant bits of pixel values in an image. This implementation focuses on hiding text messages within image files.
## How LSB Steganography Works
### Hiding a Message:

For each pixel in the image, the least significant bit of each color channel (RGB) is replaced with a bit from the binary representation of the message.
The message length is encoded in the first few pixels to facilitate retrieval.
### Retrieving a Message:

Iterate through the pixels of the image and extract the least significant bits.
Assemble the bits into bytes until the length of the hidden message is reached.
### Contributing
Contributions are welcome! Feel free to open issues for bug reports or feature requests. If you'd like to contribute code, please follow the typical pull request process.
#### Happy Steganography! 🕵️‍♂️📸
