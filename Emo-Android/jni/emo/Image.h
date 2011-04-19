namespace emo {
    class Image {
    public:
        Image();
        ~Image();

        void genTextures();

        const char* filename;
        GLuint   textureId;
        int      width;
        int      height;

        int      glWidth;
        int      glHeight;

        GLubyte* data;
        bool     hasData;
        bool     hasAlpha;
        bool     loaded;
    };
}

bool loadPngSizeFromAsset(const char *fname, int *width, int *height);
bool loadPngFromAsset(const char *fname, emo::Image* image);
