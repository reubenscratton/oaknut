//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class RIFFWriter {
public:
    RIFFWriter(Stream* stream);
    ~RIFFWriter();

    enum StreamType { db, dc, pc, wb };
    
    struct FOURCC {
        FOURCC();
        FOURCC(const char*);
        FOURCC(char,char,char,char);
        char _ch[4];
    };

    void startWriteAVI(int stream_count, int width, int height, double fps);
    void writeWavFile(const AudioFormat& audioFormat, const bytearray& wavdata);
    void writeStreamHeader(FOURCC cc);
    void writeChunk(const bytearray& bytes);
    void startWriteChunk(FOURCC fourcc);
    void endWriteChunk();
    void close();
    
    FOURCC getAVIIndex(int stream_number, StreamType strm_type);
    void writeIndex(int stream_number, StreamType strm_type);
    
    bool isEmptyFrameOffset() const { return frameOffset.empty(); }
    
    void pushFrameOffset(size_t elem) { frameOffset.push_back(elem); }
    void pushFrameSize(size_t elem) { frameSize.push_back(elem); }
    
    // Chunk codes
    static const FOURCC MJPG_CC;

private:
    Stream* strm;
    int outfps;
    int _width, _height, channels;
    size_t moviPointer;
    std::vector<size_t> frameOffset, frameSize, chunkSizeIndex, frameNumIndexes;

    void writeFourCC(FOURCC cc);
    void patchInt(uint32_t val, size_t pos);
    
};
