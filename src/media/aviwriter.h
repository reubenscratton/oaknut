//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//


class AVIWriter {
public:
    AVIWriter(Stream* stream);
    ~AVIWriter();

    enum StreamType { db, dc, pc, wb };
    
    struct FOURCC {
        FOURCC(const char*);
        FOURCC(char,char,char,char);
        char _ch[4];
    };

    void init(double fps, SIZEI size);
    void startWriteAVI(int stream_count);
    void writeStreamHeader(FOURCC cc);
    void writeChunk(const bytearray& bytes);
    void startWriteChunk(FOURCC fourcc);
    void endWriteChunk();
    void close();
    
    FOURCC getAVIIndex(int stream_number, StreamType strm_type);
    void writeIndex(int stream_number, StreamType strm_type);
    void finishWriteAVI();
    
    bool isEmptyFrameOffset() const { return frameOffset.empty(); }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }
    size_t getMoviPointer() const { return moviPointer; }
    size_t getStreamPos() const;
    
    void pushFrameOffset(size_t elem) { frameOffset.push_back(elem); }
    void pushFrameSize(size_t elem) { frameSize.push_back(elem); }
    bool isEmptyFrameSize() const { return frameSize.empty(); }
    size_t atFrameSize(size_t i) const { return frameSize[i]; }
    size_t countFrameSize() const { return frameSize.size(); }
    void putStreamBytes(const uint8_t* buf, int count);
    void putStreamByte(int val);
    
    // Chunk codes
    static const FOURCC MJPG_CC;

private:
    Stream* strm;
    int outfps;
    int width, height, channels;
    size_t moviPointer;
    std::vector<size_t> frameOffset, frameSize, chunkSizeIndex, frameNumIndexes;

    void writeFourCC(FOURCC cc);
    void patchInt(uint32_t val, size_t pos);
};
