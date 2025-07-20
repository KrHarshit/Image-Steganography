# 🕵️‍♂️ Image LSB Steganography (BMP)

A C-based project for **hiding and retrieving secret messages or files** inside a BMP image using **Least Significant Bit (LSB)** steganography.

## 📌 Features

- Encode `.txt` files into 24-bit `.bmp` images.
- Decode and retrieve hidden text from stego images.
- Supports custom output filenames.
- Simple command-line interface.
- Magic string validation for integrity.

## 📁 Project Structure

├── encode.c
├── decode.c
├── main.c
├── encode.h
├── decode.h
├── types.h
├── common.h
├── README.md


---

## ⚙️ How It Works

### 🖼 Encoding Process

1. Copies 54-byte BMP header.
2. Encodes a magic string for file integrity.
3. Encodes:
   - Secret file extension length.
   - Secret file extension.
   - Secret file size.
   - Secret file data.
4. Fills in remaining BMP data unchanged.

### 🔓 Decoding Process

1. Skips BMP header.
2. Reads and validates magic string.
3. Extracts:
   - File extension length and extension.
   - Secret file size.
   - Secret file content.

---
