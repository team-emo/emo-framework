// Copyright (c) 2011 emo-framework project
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of the project nor the names of its contributors may be
//   used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
#include "native_app_glue.h"

#include "emo/Runtime.h"
#include "emo/Engine.h"

emo::Engine* engine;

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

void android_main(struct android_app* state) {

    app_dummy();

    engine = new emo::Engine();

    state->userData = engine;
    state->onAppCmd = app_handle_cmd;
    state->onInputEvent = app_handle_input;
    engine->app = state;

    engine->onInitEngine();

    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(engine->animating ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // Process the user queue.
            if (ident == LOOPER_ID_USER) {
                if (engine->getSensorEventQueue() != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(
                            engine->getSensorEventQueue(), &event, 1) > 0) {
                        engine->onSensorEvent(&event);
                    }
                }
            }

            if (unlikely(state->destroyRequested != 0)) {
                engine->onDispose();
                delete engine;
                return;
            }
        }

        if (likely(engine->animating)) {
            engine->onDrawFrame();
        }
        
        // invoke ANR (Application Not Responding) dialog
        if (unlikely(engine->useANR)) {
            break;
        }
    }
}
