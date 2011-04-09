#import "EmoStage.h"
#import "emo_engine_func.h"

@implementation EmoStage
-(BOOL)loadBuffer {
    loaded = false;
	
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;
	
    positions[0] = 0;
    positions[1] = 0;
    positions[2] = 0;
	
    positions[3] = 0;
    positions[4] = 1;
    positions[5] = 0;
	
    positions[6] = 1;
    positions[7] = 1;
    positions[8] = 0;
	
    positions[9]  = 1;
    positions[10] = 0;
    positions[11] = 0;
	
    clearGLErrors();
	
    glGenBuffers(2, vbo);
	
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
    GLint error;
    if ((error = glGetError()) != GL_NO_ERROR) {
        char str[128];
        sprintf(str, "Could not create OpenGL buffers: code=0x%x", error);
        LOGE(str);
        return FALSE;
    }
	
    loaded = TRUE;
	return TRUE;
}

-(void)unloadBuffer{
    if (!loaded) return;
	
    glDeleteBuffers(2, vbo);
    loaded = FALSE;
}
@end
