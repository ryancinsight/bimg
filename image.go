package bimg

import (
	"sync"
)

// Image provides a method DSL to transform a given image as byte buffer
type Image struct {
	buffer []byte
}

// Buffer pool for reusing byte slices
var imageBufferPool = sync.Pool{
	New: func() interface{} {
		return make([]byte, 0, 32*1024)
	},
}

// NewImage creates a new Image struct with method DSL
func NewImage(buf []byte) *Image {
	return &Image{buf}
}

// Resize resizes the image to fixed width and height
func (i *Image) Resize(width, height int) ([]byte, error) {
	return i.Process(Options{
		Width:  width,
		Height: height,
		Embed:  true,
	})
}

// ForceResize resizes with custom size (aspect ratio won't be maintained)
func (i *Image) ForceResize(width, height int) ([]byte, error) {
	return i.Process(Options{
		Width:  width,
		Height: height,
		Force:  true,
	})
}

// ResizeAndCrop resizes the image to fixed width and height with additional crop
func (i *Image) ResizeAndCrop(width, height int) ([]byte, error) {
	return i.Process(Options{
		Width:  width,
		Height: height,
		Embed:  true,
		Crop:   true,
	})
}

// SmartCrop produces a thumbnail aiming at focus on the interesting part
func (i *Image) SmartCrop(width, height int) ([]byte, error) {
	return i.Process(Options{
		Width:   width,
		Height:  height,
		Crop:    true,
		Gravity: GravitySmart,
	})
}

// Extract area from the by X/Y axis in the current image
func (i *Image) Extract(top, left, width, height int) ([]byte, error) {
	options := Options{
		Top:        top,
		Left:       left,
		AreaWidth:  width,
		AreaHeight: height,
	}
	if top == 0 && left == 0 {
		options.Top = -1
	}
	return i.Process(options)
}

// Enlarge enlarges the image by width and height
func (i *Image) Enlarge(width, height int) ([]byte, error) {
	return i.Process(Options{
		Width:   width,
		Height:  height,
		Enlarge: true,
	})
}

// EnlargeAndCrop enlarges the image by width and height with additional crop
func (i *Image) EnlargeAndCrop(width, height int) ([]byte, error) {
	return i.Process(Options{
		Width:   width,
		Height:  height,
		Enlarge: true,
		Crop:    true,
	})
}

// Crop crops the image to the exact size specified
func (i *Image) Crop(width, height int, gravity Gravity) ([]byte, error) {
	return i.Process(Options{
		Width:   width,
		Height:  height,
		Gravity: gravity,
		Crop:    true,
	})
}

// CropByWidth crops an image by width only param
func (i *Image) CropByWidth(width int) ([]byte, error) {
	return i.Process(Options{
		Width: width,
		Crop:  true,
	})
}

// CropByHeight crops an image by height
func (i *Image) CropByHeight(height int) ([]byte, error) {
	return i.Process(Options{
		Height: height,
		Crop:   true,
	})
}

// Thumbnail creates a thumbnail with aspect ratio 4:4
func (i *Image) Thumbnail(pixels int) ([]byte, error) {
	return i.Process(Options{
		Width:   pixels,
		Height:  pixels,
		Crop:    true,
		Quality: 95,
	})
}

// Process processes the image with given options
func (i *Image) Process(o Options) ([]byte, error) {
	image, err := Resize(i.buffer, o)
	if err != nil {
		return nil, err
	}
	i.buffer = image
	return image, nil
}

// Image returns the current resultant image buffer
func (i *Image) Image() []byte {
	return i.buffer
}

// Length returns the size in bytes of the image buffer
func (i *Image) Length() int {
	return len(i.buffer)
}

// Metadata returns the image metadata
func (i *Image) Metadata() (ImageMetadata, error) {
	return Metadata(i.buffer)
}

// Interpretation gets the image interpretation type
func (i *Image) Interpretation() (Interpretation, error) {
	return ImageInterpretation(i.buffer)
}

// ColourspaceIsSupported checks if the current image color space is supported
func (i *Image) ColourspaceIsSupported() (bool, error) {
	return ColourspaceIsSupported(i.buffer)
}

// Type returns the image type format
func (i *Image) Type() string {
	return DetermineImageTypeName(i.buffer)
}

// Size returns the image size as form of width and height pixels
func (i *Image) Size() (ImageSize, error) {
	return Size(i.buffer)
}

// AutoRotate automatically rotates the image with no additional transformation 
// based on the EXIF orientation metadata, if available.
func (i *Image) AutoRotate() ([]byte, error) {
    return i.Process(Options{autoRotateOnly: true})
}