#ifndef EMO_DRAWABLE_H
#define EMO_DRAWABLE_H

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <sys/timeb.h>
#include <string>
#include <hash_map>
#include <vector>
#include <squirrel.h>
#include "Image.h"
#include "Util.h"

namespace emo {
    class AnimationFrame {
    public:
        AnimationFrame();
        ~AnimationFrame();
        std::string name;
        int   start;
        int   count;
        int   loop;
        int32_t interval;

        int   currentLoopCount;
        int   currentCount;

        int32_t getLastOnAnimationDelta();
        timeb lastOnAnimationInterval;
        
        int getNextIndex(int frameCount, int currentIndex);
    };

    typedef std::hash_map <std::string, emo::AnimationFrame *> animations_t;

    class Drawable {
    public:

        Drawable();
        virtual ~Drawable();

        std::string name;

        virtual void load();
        virtual bool bindVertex();
        virtual void onDrawFrame();
        virtual void deleteBuffer();

        void setFrameCount(int count);
        int  getFrameCount();

        GLuint getCurrentBufferId();
        bool isCurrentTexBufferLoaded();

        bool setFrameIndex(int index);

        void setTexture(Image* image);


        bool hasSheet;
        bool animating;
        bool loaded;
        bool hasTexture;
        bool hasBuffer;
        bool independent;

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
        bool setAnimation(std::string name);
        AnimationFrame* getAnimation(std::string name);
        bool deleteAnimation(std::string name);

        bool enableAnimation(bool enable);

        virtual void addRow(int rowdata[], int count){}
        virtual bool clearTiles() { return false; }
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

        void deleteAnimations();
        animations_t* animations;

        bool frameIndexChanged;
        int  nextFrameIndex;

        std::string animationName;
        AnimationFrame* currentAnimation;
    };

    class TiledDrawable : public Drawable {
    public:
        TiledDrawable(Drawable* drawable);
        virtual ~TiledDrawable();

        virtual void load();
        virtual bool bindVertex();
        virtual void onDrawFrame();
        virtual void deleteBuffer();

        virtual void addRow(int rowdata[], int count);
        virtual bool clearTiles();
    protected:
        std::vector<std::vector<int>*>* tiles;
        Drawable* drawable;

        int columns;
        int rows;
    };
}
#endif