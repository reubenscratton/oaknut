//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#include <oaknut.h>

typedef RIFFWriter::FOURCC FOURCC;

FOURCC::FOURCC() {
}
FOURCC::FOURCC(const char* sz) {
    _ch[0] = sz[0];
    _ch[1] = sz[1];
    _ch[2] = sz[2];
    _ch[3] = sz[3];
}
FOURCC::FOURCC(char c1,char c2,char c3,char c4) {
    _ch[0] = c1;
    _ch[1] = c2;
    _ch[2] = c3;
    _ch[3] = c4;
}

const FOURCC RIFF_CC("RIFF");
const FOURCC LIST_CC("LIST");
const FOURCC HDRL_CC("hdrl");
const FOURCC AVIH_CC("avih");
const FOURCC STRL_CC("strl");
const FOURCC STRH_CC("strh");
const FOURCC STRF_CC("strf");
const FOURCC VIDS_CC("vids");
const FOURCC RIFFWriter::MJPG_CC = {'M','J','P','G'};
const FOURCC MOVI_CC("movi");
const FOURCC IDX1_CC("idx1");
const FOURCC AVI_CC("AVI ");
const FOURCC AVIX_CC("AVIX");
const FOURCC JUNK_CC("JUNK");
const FOURCC INFO_CC("INFO");
const FOURCC ODML_CC("odml");
const FOURCC DMLH_CC("dmlh");
const FOURCC WAVE_CC("WAVE");


    /*
     AVI struct:
     
     RIFF ('AVI '
     LIST ('hdrl'
     'avih'(<Main AVI Header>)
     LIST ('strl'
     'strh'(<Stream header>)
     'strf'(<Stream format>)
     [ 'strd'(<Additional header data>) ]
     [ 'strn'(<Stream name>) ]
     [ 'indx'(<Odml index data>) ]
     ...
     )
     [LIST ('strl' ...)]
     [LIST ('strl' ...)]
     ...
     [LIST ('odml'
     'dmlh'(<ODML header data>)
     ...
     )
     ]
     ...
     )
     [LIST ('INFO' ...)]
     [JUNK]
     LIST ('movi'
     {{xxdb|xxdc|xxpc|xxwb}(<Data>) | LIST ('rec '
     {xxdb|xxdc|xxpc|xxwb}(<Data>)
     {xxdb|xxdc|xxpc|xxwb}(<Data>)
     ...
     )
     ...
     }
     ...
     )
     ['idx1' (<AVI Index>) ]
     )
     
     {xxdb|xxdc|xxpc|xxwb}
     xx - stream number: 00, 01, 02, ...
     db - uncompressed video frame
     dc - commpressed video frame
     pc - palette change
     wb - audio frame
     
     JUNK section may pad any data section and must be ignored
     */
    

    // {xxdb|xxdc|xxpc|xxwb}
    // xx - stream number: 00, 01, 02, ...
    // db - uncompressed video frame
    // dc - commpressed video frame
    // pc - palette change
    // wb - audio frame
    

    

#pragma pack(push, 1)
    struct AviMainHeader
    {
        uint32_t dwMicroSecPerFrame;    //  The period between video frames
        uint32_t dwMaxBytesPerSec;      //  Maximum data rate of the file
        uint32_t dwReserved1;           // 0
        uint32_t dwFlags;               //  0x10 AVIF_HASINDEX: The AVI file has an idx1 chunk containing an index at the end of the file.
        uint32_t dwTotalFrames;         // Field of the main header specifies the total number of frames of data in file.
        uint32_t dwInitialFrames;       // Is used for interleaved files
        uint32_t dwStreams;             // Specifies the number of streams in the file.
        uint32_t dwSuggestedBufferSize; // Field specifies the suggested buffer size forreading the file
        uint32_t dwWidth;               // Fields specify the width of the AVIfile in pixels.
        uint32_t dwHeight;              // Fields specify the height of the AVIfile in pixels.
        uint32_t dwReserved[4];         // 0, 0, 0, 0
    };
    
    struct AviStreamHeader
    {
        FOURCC fccType;              // 'vids', 'auds', 'txts'...
        FOURCC fccHandler;           // "cvid", "DIB "
        uint32_t dwFlags;               // 0
        uint32_t dwPriority;            // 0
        uint32_t dwInitialFrames;       // 0
        uint32_t dwScale;               // 1
        uint32_t dwRate;                // Fps (dwRate - frame rate for video streams)
        uint32_t dwStart;               // 0
        uint32_t dwLength;              // Frames number (playing time of AVI file as defined by scale and rate)
        uint32_t dwSuggestedBufferSize; // For reading the stream
        uint32_t dwQuality;             // -1 (encoding quality. If set to -1, drivers use the default quality value)
        uint32_t dwSampleSize;          // 0 means that each frame is in its own chunk
        struct {
            short int left;
            short int top;
            short int right;
            short int bottom;
        } rcFrame;                // If stream has a different size than dwWidth*dwHeight(unused)
    };
    
    struct AviIndex
    {
        uint32_t ckid;
        uint32_t dwFlags;
        uint32_t dwChunkOffset;
        uint32_t dwChunkLength;
    };
    
    struct BitmapInfoHeader
    {
        uint32_t biSize;                // Write header size of BITMAPINFO header structure
        int32_t  biWidth;               // width in pixels
        int32_t  biHeight;              // height in pixels
        uint16_t  biPlanes;              // Number of color planes in which the data is stored
        uint16_t  biBitCount;            // Number of bits per pixel
        uint32_t biCompression;         // Type of compression used (uncompressed: NO_COMPRESSION=0)
        uint32_t biSizeImage;           // Image Buffer. Quicktime needs 3 bytes also for 8-bit png
        //   (biCompression==NO_COMPRESSION)?0:xDim*yDim*bytesPerPixel;
        int32_t  biXPelsPerMeter;       // Horizontal resolution in pixels per meter
        int32_t  biYPelsPerMeter;       // Vertical resolution in pixels per meter
        uint32_t biClrUsed;             // 256 (color table size; for 8-bit only)
        uint32_t biClrImportant;        // Specifies that the first x colors of the color table. Are important to the DIB.
    };
    

#pragma pack(pop)
    
static const int STRF_SIZE = 40;
static const int AVI_DWQUALITY = -1;
static const int JUNK_SEEK = 2048;
static const int AVIIF_KEYFRAME = 0x10;
static const int MAX_BYTES_PER_SEC = 99999999;
static const int SUG_BUFFER_SIZE = 1048576;



RIFFWriter::RIFFWriter(Stream* stream) : strm(stream) {
    channels = 3; //iscolor ? 3 : 1;
    moviPointer = 0;
    channels = 0;
    moviPointer = 0;
}

RIFFWriter::~RIFFWriter() {
    frameOffset.clear();
    frameSize.clear();
    chunkSizeIndex.clear();
    frameNumIndexes.clear();
}

void RIFFWriter::close() {
    if(isEmptyFrameOffset()) {
        return;
    }
    endWriteChunk(); // end LIST 'movi'
    writeIndex(0, RIFFWriter::dc);
    uint32_t nframes = (uint32_t)frameOffset.size();
    
    // Record frames numbers to AVI Header
    while (!frameNumIndexes.empty()) {
        size_t ppos = frameNumIndexes.back();
        frameNumIndexes.pop_back();
        patchInt(nframes, ppos);
    }
    endWriteChunk(); // end RIFF
}

void RIFFWriter::writeFourCC(FOURCC cc) {
    strm->writeBytes(4, &cc);
}

void RIFFWriter::startWriteAVI(int stream_count, int width, int height, double fps) {
    outfps = round(fps);
    _width = width;
    _height = height;
    startWriteChunk(RIFF_CC);
    writeFourCC(AVI_CC);
    startWriteChunk(LIST_CC);
    writeFourCC(HDRL_CC);
    
    // Main avi header chunk
    startWriteChunk(AVIH_CC);
    AviMainHeader mainHeader;
    memset(&mainHeader, 0, sizeof(mainHeader));
    mainHeader.dwMicroSecPerFrame = round(1e6 / outfps);
    mainHeader.dwMaxBytesPerSec = MAX_BYTES_PER_SEC;
    mainHeader.dwFlags = 0x00010000 | 0x10; //0x910
    frameNumIndexes.push_back(strm->offsetWrite + offsetof(AviMainHeader, dwTotalFrames));
    mainHeader.dwStreams = stream_count;
    mainHeader.dwSuggestedBufferSize = 0;//_width*_height*4*2; // i.e. enough RAM for 2 decompressed frames
    mainHeader.dwWidth = _width;
    mainHeader.dwHeight = _height;
    strm->writeBytes(sizeof(AviMainHeader), &mainHeader);
    endWriteChunk();
}

void RIFFWriter::writeWavFile(const AudioFormat& audioFormat, const bytearray& wavdata) {
    startWriteChunk(RIFF_CC);
    writeFourCC(WAVE_CC);
    writeFourCC(FOURCC("fmt "));
    strm->writeInt32(0x10); // sizeof(fmt chunk)
    int16_t formatTag;
    int16_t bitsPerSample;
    if (audioFormat.sampleType == AudioFormat::Float32) {
        formatTag = 0x0003; // WAVE_FORMAT_IEEE_FLOAT;
        bitsPerSample = 32;
    } else {
        formatTag = 0x0001; // WAVE_FORMAT_PCM
        bitsPerSample = 16;
    }
    strm->writeInt16(formatTag);
    strm->writeInt16(audioFormat.numChannels);
    strm->writeInt32(audioFormat.sampleRate);
    strm->writeInt32(audioFormat.numChannels * audioFormat.sampleRate * bitsPerSample/8);
    strm->writeInt16(audioFormat.numChannels * bitsPerSample/8);
    strm->writeInt16(bitsPerSample);
    
    writeFourCC(FOURCC("data"));
    strm->writeInt32(wavdata.size());
    strm->writeBytes(wavdata.size(), wavdata.data());
    endWriteChunk();
}

//static const int AVIH_STRH_SIZE = 56;

void RIFFWriter::writeStreamHeader(FOURCC cc) {
    // strh
    startWriteChunk(LIST_CC);
    
    writeFourCC(STRL_CC);
    
    // Stream header
    writeFourCC(STRH_CC);
    strm->writeInt32(sizeof(AviStreamHeader));
    AviStreamHeader header;
    header.fccType = VIDS_CC;
    header.fccHandler = cc;
    header.dwScale = 1;
    header.dwRate = outfps;
    frameNumIndexes.push_back(strm->offsetWrite + offsetof(AviStreamHeader, dwLength));
    header.dwSuggestedBufferSize = _width * _height * 4;
    header.dwQuality = 0;//AVI_DWQUALITY;
    //header.rcFrame.right = _width;
    //header.rcFrame.bottom = _height;
    strm->writeBytes(sizeof(AviStreamHeader), &header);


    /*writeFourCC(VIDS_CC);
    writeFourCC(cc);
    strm->writeInt32(0);
    strm->writeInt32(0);
    strm->writeInt32(0);
    strm->writeInt32(AVI_DWSCALE);
    strm->writeInt32(outfps);
    strm->writeInt32(0);
    frameNumIndexes.push_back(strm->offsetWrite);
    strm->writeInt32(0);
    strm->writeInt32(SUG_BUFFER_SIZE);
    strm->writeInt32(static_cast<uint32_t>(AVI_DWQUALITY));
    strm->writeInt32(0);
    strm->writeInt16(0);
    strm->writeInt16(0);
    strm->writeInt16(_width);
    strm->writeInt16(_height);*/
    
    // strf (use the BITMAPINFOHEADER for video)
    startWriteChunk(STRF_CC);
    strm->writeInt32(STRF_SIZE);
    strm->writeInt32(_width);
    strm->writeInt32(_height);
    strm->writeInt16(1); // planes (1 means interleaved data (after decompression))
    strm->writeInt16(0);//8 * channels); // bits per pixel
    writeFourCC(cc);
    strm->writeInt32(_width * _height * channels);
    strm->writeInt32(0);
    strm->writeInt32(0);
    strm->writeInt32(0);
    strm->writeInt32(0);
    // Must be indx chunk
    endWriteChunk(); // end strf
    
    endWriteChunk(); // end strl
    
    // odml
    //startWriteChunk(LIST_CC);
    //writeFourCC(ODML_CC);
    //startWriteChunk(DMLH_CC);
    //frameNumIndexes.push_back(strm->offsetWrite);
    //strm->writeInt32(0);
    //strm->writeInt32(0);
    //endWriteChunk(); // end dmlh
    //endWriteChunk(); // end odml
    
    endWriteChunk(); // end hdrl
    
    // JUNK
    startWriteChunk(JUNK_CC);
    size_t pos = strm->offsetWrite;
    for( ; pos < (size_t)JUNK_SEEK; pos += 4 )
        strm->writeInt32(0);
    endWriteChunk(); // end JUNK
    
    // movi
    startWriteChunk(LIST_CC);
    moviPointer = strm->offsetWrite;
    writeFourCC(MOVI_CC);
}

void RIFFWriter::startWriteChunk(FOURCC fourcc) {
    writeFourCC(fourcc);
    chunkSizeIndex.push_back(strm->offsetWrite);
    strm->writeInt32(0);
}

void RIFFWriter::writeChunk(const bytearray& bytes) {
    size_t chunkPointer = strm->offsetWrite;
    
    FOURCC avi_index = getAVIIndex(0, RIFFWriter::dc);
    startWriteChunk(avi_index);
    
    strm->writeBytes(bytes.size(), bytes.data());
    
    // Align to multiple of 4
    size_t pos = strm->offsetWrite;
    size_t pos1 = (pos + 3) & ~3;
    for( ; pos < pos1; pos++ ) {
        uint8_t val = 0;
        strm->writeBytes(1, &val);
    }
    
    size_t tempChunkPointer = strm->offsetWrite;
    size_t moviPointer = this->moviPointer;
    pushFrameOffset(chunkPointer - moviPointer);
    pushFrameSize(tempChunkPointer - chunkPointer - 8); // Size excludes fourcc and size field
    endWriteChunk();
}

void RIFFWriter::endWriteChunk() {
    if (!chunkSizeIndex.empty()) {
        size_t currpos = strm->offsetWrite;
        assert(currpos > 4);
        currpos -= 4;
        size_t pospos = chunkSizeIndex.back();
        chunkSizeIndex.pop_back();
        assert(currpos >= pospos);
        uint32_t chunksz = (uint32_t)(currpos - pospos);
        patchInt(chunksz, pospos);
    }
}

FOURCC RIFFWriter::getAVIIndex(int stream_number, StreamType strm_type) {
    char strm_indx[2];
    strm_indx[0] = '0' + static_cast<char>(stream_number / 10);
    strm_indx[1] = '0' + static_cast<char>(stream_number % 10);
    
    switch (strm_type) {
        case db: return FOURCC(strm_indx[0], strm_indx[1], 'd', 'b');
        case dc: return FOURCC(strm_indx[0], strm_indx[1], 'd', 'c');
        case pc: return FOURCC(strm_indx[0], strm_indx[1], 'p', 'c');
        case wb: return FOURCC(strm_indx[0], strm_indx[1], 'w', 'b');
    }
    return FOURCC(strm_indx[0], strm_indx[1], 'd', 'b');
}

void RIFFWriter::writeIndex(int stream_number, StreamType strm_type) {
    // old style AVI index. Must be Open-DML index
    startWriteChunk(IDX1_CC);
    int nframes = (int)frameOffset.size();
    for( int i = 0; i < nframes; i++ ) {
        writeFourCC(getAVIIndex(stream_number, strm_type));
        strm->writeInt32(AVIIF_KEYFRAME);
        strm->writeInt32((int)frameOffset[i]);
        strm->writeInt32((int)frameSize[i]);
    }
    endWriteChunk(); // End idx1
}

void RIFFWriter::patchInt(uint32_t val, size_t pos) {
    auto saved_pos = strm->offsetWrite;
    strm->setWriteOffset(pos);
    strm->writeInt32(val);
    strm->setWriteOffset(saved_pos);
}




