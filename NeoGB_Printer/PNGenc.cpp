//
// PNG Decoder
//
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 5/3/2021
// Original PNG code written 20+ years ago :)
// The goal of this code is to decode PNG images on embedded systems
//
// Copyright 2021 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#include "./includes/image/PNGenc/PNGenc.h"

// Include the C code which does the actual work
#include "./includes/image/PNGenc/png.h"

//
// File (SD/MMC) based initialization
//
int PNG::open(const char *szFilename, PNG_OPEN_CALLBACK *pfnOpen, PNG_CLOSE_CALLBACK *pfnClose, PNG_READ_CALLBACK *pfnRead, PNG_WRITE_CALLBACK *pfnWrite, PNG_SEEK_CALLBACK *pfnSeek)
{
    memset(&_png, 0, sizeof(PNGIMAGE));
    _png.iTransparent = -1;
    _png.pfnRead = pfnRead;
    _png.pfnWrite = pfnWrite;
    _png.pfnSeek = pfnSeek;
    _png.pfnOpen = pfnOpen;
    _png.pfnClose = pfnClose;
    _png.PNGFile.fHandle = (*pfnOpen)(szFilename);
    if (_png.PNGFile.fHandle == NULL) {
        _png.iError = PNG_INVALID_FILE;
       return PNG_INVALID_FILE;
    }
    return PNG_SUCCESS;

} /* open() */

int PNG::open(uint8_t *pOutput, int iBufferSize)
{
    memset(&_png, 0, sizeof(PNGIMAGE));
    _png.iTransparent = -1;
    _png.pOutput = pOutput;
    _png.iBufferSize = iBufferSize;
    return PNG_SUCCESS;
} /* open() */

//
// return the last error (if any)
//
int PNG::getLastError()
{
    return _png.iError;
} /* getLastError() */
//
// Close the file - not needed when decoding from memory
//
int PNG::close()
{
    if (_png.pfnClose)
        (*_png.pfnClose)(&_png.PNGFile);
    return _png.iDataSize;
} /* close() */

int PNG::encodeBegin(int iWidth, int iHeight, uint8_t ucPixelType, uint8_t ucBpp, uint8_t *pPalette, uint8_t ucCompLevel)
{
    _png.iWidth = iWidth;
    _png.iHeight = iHeight;
    _png.ucPixelType = ucPixelType;
    _png.ucBpp = ucBpp;
    if (pPalette != NULL)
        memcpy(_png.ucPalette, pPalette, 768); // save 256 color entries
    _png.ucCompLevel = ucCompLevel;
    _png.y = 0;
    return PNG_SUCCESS;
} /* encodeBegin() */

int PNG::addLine(uint8_t *pPixels)
{
    int rc;
    rc = PNGAddLine(&_png, pPixels, _png.y);
    _png.y++;
    return rc;
} /* addLine() */

int PNG::setTransparentColor(uint32_t u32Color)
{
    if (_png.ucPixelType == PNG_PIXEL_GRAYSCALE || _png.ucPixelType == PNG_PIXEL_TRUECOLOR) {
        _png.iTransparent = u32Color;
        return PNG_SUCCESS;
    }
    else
        return PNG_INVALID_PARAMETER; // indexed image must have palette alpha values
} /* setTransparentColor() */

int PNG::setAlphaPalette(uint8_t *pPalette)
{
    if (pPalette != NULL && _png.ucPixelType == PNG_PIXEL_INDEXED) {
        _png.ucHasAlphaPalette = 1;
        memcpy(&_png.ucPalette[768], pPalette, 256); // capture up to 256 alpha values
        return PNG_SUCCESS;
    }
    return PNG_INVALID_PARAMETER;
} /* setAlphaPalette() */
