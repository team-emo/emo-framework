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

#include "SquirrelGlue.h"
#include "Engine.h"
#include "Runtime.h"
#include "Physics.h"

extern emo::Engine* engine;

using namespace Sqrat;

namespace emo {
    void SquirrelGlue::initScriptFunctions() {

        // Set default VM
        DefaultVM::Set(engine->sqvm);

        // bind functions to "emo::Android"
        Sqrat::Table androidSpace(engine->sqvm);
        androidSpace.Func("toast", sqToast)
                    .Func("getOSVersion", sqGetOSVersion)
                    .Func("getSDKVersion", sqGetSDKVersion)
                    .Func("getNativeOrientation",sqGetNativeOrientation)
                    .Func("transit",sqTransit);

        // bind functions to "emo::Database"
        Sqrat::Table databaseSpace(engine->sqvm);
        databaseSpace.Func("openOrCreate", sqOpenOrCreate)
                     .Func("open", sqOpen)
                     .Func("close", sqClose)
                     .Func("remove", sqRemove)
                     .Func("createTable", sqCreateTable)
                     .Func("dropTable", sqDropTable)
                     .Func("select", sqSelect)
                     .Func("selectAll", sqSelectAll)
                     .Func("selectWhere", sqSelectWhere)
                     .Func("selectAllWhere", sqSelectAllWhere)
                     .Func("count", sqCount)
                     .Func("countWhere", sqCountWhere)
                     .Func("insert", sqInsert)
                     .Func("update", sqUpdate)
                     .Func("updateWhere", sqUpdateWhere)
                     .Func("truncateTable", sqTruncateTable)
                     .Func("deleteWhere", sqDeleteWhere)
                     .Func("vacuum", sqVacuum)
                     .Func("getPath", sqGetPath)
                     .Func("getLastError", sqGetLastError)
                     .Func("getLastErrorMessage", sqGetLastErrorMessage);

        // bind functions to "emo::Preference"
        Sqrat::Table preferenceSpace(engine->sqvm);
        preferenceSpace.Func("openOrCreate", sqOpenOrCreatePreference)
                       .Func("open", sqOpen)
                       .Func("close", sqClose)
                       .Func("get", sqGetPreference)
                       .Func("set", sqSetPreference)
                       .Func("del", sqDeletePreference)
                       .Func("keys", sqGetPreferenceKeys);

        // bind tables to "emo"
        engine->emoSpace->Bind(EMO_ANDROID_TABLE, androidSpace);
        engine->emoSpace->Bind(EMO_DATABASE_TABLE, databaseSpace);
        engine->emoSpace->Bind(EMO_PREFERENCE_TABLE, preferenceSpace);

        // @TODO refactoring with Sqrat
        initRuntimeFunctions();
        initDrawableFunctions();
        initAudioFunctions();
        initJavaGlueFunctions();
        initPhysicsFunctions();
    }
}
