#include <EGL/egl.h>
#include <GLES/gl.h>

namespace emo {
    class Stage {
    public:
        Stage();
        ~Stage();
        bool onLoad();
        void onDrawFrame();
        void deleteBuffer();
        void rebindBuffer();
        bool dirty;

        int width;
        int height;
        int viewport_width;
        int viewport_height;
        GLuint vbo[2];
    protected:
        bool loaded;

        short  indices[4];
        float  positions[12];

    };
}