#include <EGL/egl.h>
#include <GLES/gl.h>

#include <squirrel.h>
#include "Image.h"

namespace emo {
    class Drawable {
    public:

        Drawable();
        ~Drawable();

        void init();
        void load();

        void genTextures();
        bool bindVertex();

        void onDrawFrame();

        void setFrameCount(int count);
        int  getFrameCount();

        GLuint getCurrentBufferId();
        void setFrameIndex(int index);

        void setTexture(Image* image);

        const char* name;

        bool  hasSheet;
        bool  animating;
        bool  loaded;
        bool  hasTexture;
        bool  hasBuffer;

        int border;
        int margin;

        float      x;
        float      y;
        float      z;

        int        width;
        int        height;

        float      param_rotate[4];
        float      param_scale[4];
        float      param_color[4];
    protected:

        float      vertex_tex_coords[8];

        int        frame_index;
        GLuint*    frames_vbos;
        int        frameCount;

        Image*     texture;
    };
}