#include <EGL/egl.h>
#include <GLES/gl.h>

#include <hash_map>
#include <squirrel.h>
#include "Image.h"
#include "Util.h"

namespace emo {
    class AnimationFrame {
    public:
        AnimationFrame();
        ~AnimationFrame();
        const char* name;
        int   start;
        int   frameCount;
        int   loop;
        unsigned long interval;
    };

    typedef std::hash_map <const char *, emo::AnimationFrame *, std::hash<const char*>, char_comparator> animations_t;

    class Drawable {
    public:

        Drawable();
        ~Drawable();


        void load();

        bool bindVertex();

        void onDrawFrame();

        void setFrameCount(int count);
        int  getFrameCount();

        GLuint getCurrentBufferId();
        bool isCurrentTexBufferLoaded();

        void setFrameIndex(int index);

        void setTexture(Image* image);

        void deleteBuffer();

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

        void addAnimation(AnimationFrame* animation);
        void setAnimation(const char* name);
        AnimationFrame* getAnimation(const char* name);
        bool deleteAnimation(const char* name);

        void setName(const char* name);
        char* getName();

    protected:

        char* name;

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

        void deleteAnimations();
        animations_t* animations;
        const char*   animationName;
    };
}