//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

#if USE_WORKER_FaceDetector

// Workers are built separately for web, they shouldn't be compiled into the main .js
#if !(PLATFORM_WEB && !BUILD_AS_WORKER)


#include <oaknut.h>


struct Stage {
    float threshold;
    int numNodes;
};
static float* nodeData;
static uint8_t* nodesNumRectsData;
static int8_t* rectData;
static int baseBlockWidth = 20;
static int baseBlockHeight = 20;

static void init_nodes_data();
static void init_rects_data();
static bool s_initialised = false;
static vector<RECT> detect(const bytearray& pixels, int width, int height, RECT roi);
static bool evalStages(const int32_t* integralImage, const int32_t* integralImageSquare, int i, int j, int width, int blockWidth, int blockHeight, float scale);

class FaceDetectorWorker : public WorkerImpl {
public:

    void start_(const variant& config) override {
        app->log("start_()! %X", this);
        if (!s_initialised) {
            s_initialised = true;
            init_nodes_data();
            init_rects_data();
        }
    }
    variant process_(const variant& data_in) override {
        int width = data_in.intVal("width");
        int height = data_in.intVal("height");
        const bytearray& pixels = data_in.bytearrayRef("data");
        RECT roi;
        roi.origin.x = data_in.intVal("roiLeft");
        roi.origin.y = data_in.intVal("roiTop");
        roi.size.width = data_in.intVal("roiWidth");
        roi.size.height = data_in.intVal("roiHeight");
        vector<RECT> result = detect(pixels, width, height, roi);
        variant retval;
        retval.setType(variant::ARRAY);
        for (int i=0 ; i<result.size() ; i++) {
            RECT& rect = result[i];
            variant vrect;
            vrect.set("x", rect.origin.x);
            vrect.set("y", rect.origin.y);
            vrect.set("width", rect.size.width);
            vrect.set("height", rect.size.height);
            retval.appendVal(vrect);
        }
        return retval;
    }
    void stop_() override {
        
    }

};

DECLARE_WORKER_IMPL(FaceDetectorWorker);



static vector<RECT> detect(const bytearray& pixels, int width, int height, RECT roi) {
    vector<RECT> rects;
    
    // Convert just the ROI to a new greyscale image
    int roiLeft = (int)roi.origin.x;
    int roiTop = (int)roi.origin.y;
    int roiWidth = (int)roi.size.width;
    int roiHeight = (int)roi.size.height;
    uint8_t* gray = (uint8_t*)malloc(roiWidth * roiHeight);
    uint32_t* pixels32 = (uint32_t*)pixels.data();
    int i=0;
    for (int y=roiTop; y<roiTop+roiHeight ; y++) {
        int yy = y*width;
        for (int x=roiLeft; x<roiLeft+roiWidth ; x++) {
            uint32_t c = pixels32[yy+x];
            uint8_t luma = ((c>>16&0xff) * 13933 + (c>>8&0xff) * 46871 + (c&0xff) * 4732)>>16;
            gray[i++] = luma;
        }
    }
    
    // From this point we disregard anything outside the ROI
    width = roiWidth;
    height = roiHeight;
    
    // Create the integral images
    int32_t* integralImage = (int32_t*) malloc(width * height * 4);
    int32_t* integralImageSquare = (int32_t*) malloc(width * height * 4);
    // top row
    int32_t pixAcc = 0;
    int32_t pixAccSq = 0;
    for (int j = 0; j < width; j++) {
        int32_t pixel = gray[j];
        pixAcc += pixel;
        pixAccSq += pixel*pixel;
        integralImage[j] = pixAcc;
        integralImageSquare[j] = pixAccSq;
    }
    // left column
    for (int i = 1; i < height; i++) {
        int w = i * width;
        int32_t pixel = gray[w];
        int32_t pixelSq = pixel*pixel;
        integralImage[w] = pixel + integralImage[w-width];
        integralImageSquare[w] = pixelSq + integralImageSquare[w-width];
    }
    // Remainder of image
    for (int i = 1; i < height; i++) {
        for (int j = 1; j < width; j++) {
            int w = i * width + j;
            int32_t pixel = gray[w];
            int32_t pixelSq = pixel * pixel;
            integralImage[w] = integralImage[w - width]
            + integralImage[w - 1]
            + pixel
            - integralImage[w - width - 1];
            integralImageSquare[w] = integralImageSquare[w - width]
            + integralImageSquare[w - 1]
            + pixelSq
            - integralImageSquare[w - width - 1];
        }
    }
    
    // Iterate from 50% of ROI width up to 100% in increments of 10%
    for (float scale=0.5; scale<=1.0; scale+=0.1) {
        float blockScale = (scale * width) / baseBlockWidth;
        int blockWidth = (blockScale * baseBlockWidth);
        int blockHeight = (blockScale * baseBlockHeight);
        
        float step = (blockWidth/8);
        
        // Scan the image a block at a time top->bottom, left->right. Blocks overlap a lot.
        for (int i = 0; i < (height - blockHeight); i += step) {
            for (int j = 0; j < (width - blockWidth); j += step) {
                
                // Test block for presence of a face
                if (evalStages(integralImage, integralImageSquare, i, j, width, blockWidth, blockHeight, blockScale)) {
                    
                    // Found a face. See if the rect overlaps by more than 50% with any existing
                    // face rect and if so, merge them together.
                    RECT rect = {roi.left()+j, roi.top()+i, (float)blockWidth, (float)blockHeight};
                    bool merged = false;
                    for (int k=0 ; k<rects.size(); k++) {
                        int r1x0=rect.origin.x;
                        int r1x1=r1x0 + rect.size.width;
                        int r2x0=rects[k].origin.x;
                        int r2x1=r2x0 + rects[k].size.width;
                        int intersectWidth = MIN(r1x1, r2x1) - MAX(r1x0, r2x0);
                        if (intersectWidth <= 0) {
                            continue;
                        }
                        int r1y0=rect.origin.y;
                        int r1y1=r1y0 + rect.size.height;
                        int r2y0=rects[k].origin.y;
                        int r2y1=r2y0 + rects[k].size.height;
                        int intersectHeight = MIN(r1y1, r2y1) - MAX(r1y0, r2y0);
                        if (intersectHeight<=0) {
                            continue;
                        }
                        int intersectArea = intersectWidth * intersectHeight;
                        int area1 = (rect.size.width * rect.size.height);
                        int area2 = (rects[k].size.width * rects[k].size.height);
                        if ((intersectArea / area1) >= 0.5 || (intersectArea / area2) >= 0.5) {
                            rects[k].origin.x = MIN(r1x0, r2x0);
                            rects[k].origin.y = MIN(r1y0, r2y0);
                            rects[k].size.width = MAX(r1x1, r2x1) - rects[k].origin.x;
                            rects[k].size.height = MAX(r1y1, r2y1) - rects[k].origin.y;
                            merged = true;
                            break;
                        }
                    }
                    
                    // A new face rect, add it to the results array
                    if (!merged) {
                        rects.push_back(rect);
                    }
                }
            }
        }
    }
    
    free(gray);
    free(integralImage);
    free(integralImageSquare);
    return rects;
};


static Stage stageData[] = {
    {0.822689, 3},
    {6.956609, 16},
    {9.498543, 21},
    {18.412970, 39},
    {15.324140, 33},
    {21.010639, 44},
    {23.918791, 50},
    {24.527880, 51},
    {27.153351, 56},
    {34.554111, 71},
    {39.107288, 80},
    {50.610481, 103},
    {54.620071, 111},
    {50.169731, 102},
    {66.669121, 135},
    {67.698921, 137},
    {69.229874, 140},
    {79.249077, 160},
    {87.696030, 177},
    {90.253349, 182},
    {104.749199, 211},
    {105.761101, 213},
};

/**
 * Runs a HAAR cascade on a block of image.
 * @param i Left edge of block within image
 * @param j Top edge of block within image
 * @param width The image width.
 * @param blockWidth The block width.
 * @param blockHeight The block height.
 * @param scale The scale factor between base block size and block size
 * @return {boolean} Whether the region passes all the stage tests.
 */
static bool evalStages(const int32_t* integralImage, const int32_t* integralImageSquare, int i, int j, int width, int blockWidth, int blockHeight, float scale) {
    float inverseArea = 1.0 / (blockWidth * blockHeight);
    int wbA = i * width + j;
    int wbB = wbA + blockWidth;
    int wbD = wbA + blockHeight * width;
    int wbC = wbD + blockWidth;
    float mean = (integralImage[wbA] - integralImage[wbB] - integralImage[wbD] + integralImage[wbC]) * inverseArea;
    float variance = (integralImageSquare[wbA] - integralImageSquare[wbB] - integralImageSquare[wbD] + integralImageSquare[wbC]) * inverseArea - mean * mean;
    
    float standardDeviation = 1;
    if (variance > 0) {
        standardDeviation = sqrt(variance);
    }
    
    int n=0;
    int ri=0;
    int nr=0;
    for (int s=0 ; s<sizeof(stageData)/sizeof(Stage) ; s++) {
        Stage& stage = stageData[s];
        float stageSum = 0;
        int nodeLength = stage.numNodes;
        
        while (nodeLength--) {
            float nodeThreshold = nodeData[n++];
            float nodeLeft = nodeData[n++];
            float nodeRight = nodeData[n++];
            int rectsLength = nodesNumRectsData[nr++];
            
            float rectsSum = 0;
            
            for (int r = 0; r < rectsLength; r++) {
                int rectLeft = (j + rectData[ri++] * scale + 0.5);
                int rectTop = (i + rectData[ri++] * scale + 0.5);
                int rectWidth = (rectData[ri++] * scale + 0.5);
                int rectHeight = (rectData[ri++] * scale + 0.5);
                int rectWeight = rectData[ri++];
                
                // RectSum(r) = SAT(x-1, y-1) + SAT(x+w-1, y+h-1) - SAT(x-1, y+h-1) - SAT(x+w-1, y-1)
                int  w1 = rectTop * width + rectLeft;
                int  w2 = w1 + rectWidth;
                int  w3 = w1 + rectHeight * width;
                int  w4 = w3 + rectWidth;
                rectsSum += (integralImage[w1] - integralImage[w2] - integralImage[w3] + integralImage[w4]) * rectWeight;
            }
            
            if (rectsSum * inverseArea < nodeThreshold * standardDeviation) {
                stageSum += nodeLeft;
            } else {
                stageSum += nodeRight;
            }
        }
        
        if (stageSum < stage.threshold) {
            return false;
        }
    }
    return true;
}





/**
 
 HAAR cascade data begins here. This originated in OpenCV, see   https://github.com/opencv/opencv/blob/master/data/haarcascades/haarcascade_frontalface_alt.xml
 
 This cascade has 22 stages, ~2500 nodes, and ~4500 rects. The data was converted to a reasonably
 optimal binary format and then base64'd for use in Javascript. Notes about the binary format are
 in the decoding code below.
 
 
 Stump-based 20x20 gentle adaboost frontal face detector.
 Created by Rainer Lienhart.
 ////////////////////////////////////////////////////////////////////////////////////////
 IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 By downloading, copying, installing or using the software you agree to this license.
 If you do not agree to this license, do not download, install,
 copy or use the software.
 Intel License Agreement
 For Open Source Computer Vision Library
 Copyright (C) 2000, Intel Corporation, all rights reserved.
 Third party copyrights are property of their respective owners.
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 * Redistribution's of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 * Redistribution's in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 * The name of Intel Corporation may not be used to endorse or promote products
 derived from this software without specific prior written permission.
 This software is provided by the copyright holders and contributors "as is" and
 any express or implied warranties, including, but not limited to, the implied
 warranties of merchantability and fitness for a particular purpose are disclaimed.
 In no event shall the Intel Corporation or contributors be liable for any direct,
 indirect, incidental, special, exemplary, or consequential damages
 (including, but not limited to, procurement of substitute goods or services;
 loss of use, data, or profits; or business interruption) however caused
 and on any theory of liability, whether in contract, strict liability,
 or tort (including negligence or otherwise) arising in any way out of
 the use of this software, even if advised of the possibility of such damage. */



const string nodeDataEncoded = "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";

static void init_nodes_data() {
    // Decode the compressed node data into a float32 array and a separate nodeNumRects array.
    // The compressed data is 3 floats per node (threshold, left, right).
    // The number of rects per node is only ever 1, 2, or 3 and is encoded in the two
    // unused sign bits of the 'left' and 'right' floats which are always >=0. This
    // data is extracted into a separate array, nodesNumRectsData.
    bytearray binarydata = oak::base64_decode(nodeDataEncoded);
    int numFloats = binarydata.size() / sizeof(float);
    int numNodes = numFloats / 3;
    nodeData = (float*)binarydata.data();
    binarydata.detach();
    nodesNumRectsData = (uint8_t*)malloc(numNodes);
    int n=0;
    for (int i=0 ; i<numFloats; i+=3) {
        uint8_t numRects = 1;
        float nodeLeft =  nodeData[i+1];
        float nodeRight =  nodeData[i+2];
        if (nodeLeft < 0 || (nodeLeft==-0.f)) {
            numRects=2;
            nodeData[i+1] = -nodeLeft;
        }
        if (nodeRight < 0 || (nodeRight==-0.f)) {
            numRects=3;
            nodeData[i+2] = -nodeRight;
        }
        nodesNumRectsData[n++] = numRects;
    }
}

static const string rectDataEncoded = "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";



static void init_rects_data() {
    
    // Decode rect data. A rect has 5 integer properties: top, left, width, height, and weight.
    // Each encoded rect is just 20 bits, thanks to the limited range of each property.
    // Left and top are always 0 to 19, width and height are always 1 to 20, so we simply
    // subtract 1 from those so they are also 0 to 19. The four fields can then be trivially
    // arithmetic-encoded. The last field, weight, has an even more limited range, and takes
    // up just two bits.
    // The decoded form uses one byte per property, i.e. 5 bytes per rect.
    bytearray binarydata = base64_decode(rectDataEncoded);
    uint8_t* bytes = (uint8_t*)binarydata.data();
    int len = binarydata.size();
    rectData = (int8_t*)malloc(len * 2);
    int o=0;
    bool oddrect = false;
    uint32_t prev = 0;
    for (int i = 0; i < len; ) {
        uint32_t rectCoded=0;
        if (!oddrect) {
            rectCoded = bytes[i++];
            rectCoded |= (bytes[i++]<<8);
            prev = bytes[i++];
            rectCoded |= ((prev>>4)<<16);
            prev = (prev & 0xf);
        } else {
            rectCoded = prev;
            rectCoded |= (bytes[i++]<<4);
            rectCoded |= (bytes[i++]<<12);
        }
        oddrect = !oddrect;
        
        // Decoding
        int8_t rectWeight = rectCoded&3;
        if (rectWeight==0) rectWeight=-1;
        rectCoded>>=2;
        int8_t rectHeight = 1 + rectCoded % 20;
        rectCoded = rectCoded / 20;
        int8_t rectWidth = 1 + rectCoded % 20;
        rectCoded = rectCoded / 20;
        int8_t rectTop = rectCoded % 20;
        rectCoded = rectCoded / 20;
        int8_t rectLeft = rectCoded % 20;
        
        // Write out the five rect properties as bytes
        rectData[o++] = rectLeft;
        rectData[o++] = rectTop;
        rectData[o++] = rectWidth;
        rectData[o++] = rectHeight;
        rectData[o++] = rectWeight;
    }
}

#endif
#endif
