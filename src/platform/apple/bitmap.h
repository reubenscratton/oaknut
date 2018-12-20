//
// Copyright © 2018 Sandcastle Software Ltd. All rights reserved.
//
// This file is part of 'Oaknut' which is released under the MIT License.
// See the LICENSE file in the root of this installation for details.
//

class BitmapApple : public Bitmap {
public:
    CGContextRef _context; // nil if bitmap is texture-only
    CVImageBufferRef _cvImageBuffer;
    CFDataRef _cfData;
    
    BitmapApple();
    BitmapApple(int width, int height, int format);
    BitmapApple(int width, int height, int format, void* pixels, int stride);
    BitmapApple(CVPixelBufferRef cvImageBuffer, bool fromCamera);
    ~BitmapApple();
    
    // Overrides
    void lock(oak::PIXELDATA* pixelData, bool forWriting) override;
    void unlock(oak::PIXELDATA* pixelData, bool pixelDataChanged) override;
    void fromVariant(const oak::variant& v) override;
    void toVariant(oak::variant& v) override;
    
private:
    void createBitmapContext();

};


