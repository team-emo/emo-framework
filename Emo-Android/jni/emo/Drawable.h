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
        void unload();

        void genTextures();
        bool bindVertex();

        void onDrawFrame();

        void setFrameCount(int count);
        int  getFrameCount();

        GLuint getCurrentBufferId();
        bool isCurrentTexBufferLoaded();

        void setFrameIndex(int index);

        void setTexture(Image* image);

        void deleteBuffer();

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

        int tex_coord_frame_startX();
        int tex_coord_frame_startY();
        float getTexCoordStartX();
        float getTexCoordEndX();
        float getTexCoordStartY();
        float getTexCoordEndY();

        bool frameCountLoaded;
    };
}