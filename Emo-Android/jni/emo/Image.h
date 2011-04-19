namespace emo {
    class Image {
    public:
        const char* filename;
        GLuint   textureId;
        int      width;
        int      height;

        int      glWidth;
        int      glHeight;

        GLubyte* data;
        bool     hasAlpha;
        bool     loaded;
    };
}

bool loadPngSizeFromAsset(const char *fname, int *width, int *height);
bool loadPngFromAsset(const char *fname, emo::Image* image);
