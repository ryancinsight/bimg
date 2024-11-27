package bimg

import (
	"os"
	"sync"
)

// Buffer pool for reusing byte slices
var bufferPool = sync.Pool{
	New: func() interface{} {
		return make([]byte, 0, 32*1024) // 32KB default size
	},
}

// Read reads all the content of the given file path
// and returns it as byte buffer.
func Read(path string) ([]byte, error) {
	// Open file with optimized flags
	file, err := os.OpenFile(path, os.O_RDONLY, 0)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	// Get file info for size
	info, err := file.Stat()
	if err != nil {
		return nil, err
	}

	// Get buffer from pool with appropriate size
	buf := make([]byte, info.Size())

	// Read entire file
	_, err = file.Read(buf)
	if err != nil {
		return nil, err
	}

	return buf, nil
}

// Write writes the given byte buffer into disk
// to the given file path.
func Write(path string, buf []byte) error {
	// Open file with optimized flags
	file, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return err
	}
	defer file.Close()

	// Write buffer directly
	_, err = file.Write(buf)
	return err
}
