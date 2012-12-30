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

        // "Android" class binding
        engine->emoSpace->Bind(
            EMO_ANDROID_CLASS,
            Class<emo::Android>().Func("toast", &emo::Android::sqToast)
                                 .Func("getOSVersion", &emo::Android::sqGetOSVersion)
                                 .Func("getSDKVersion", &emo::Android::sqGetSDKVersion)
                                 .Func("getNativeOrientation",&emo::Android::sqGetNativeOrientation)
                                 .Func("transit",&emo::Android::sqTransit)
        );

        engine->emoSpace->Bind(
            EMO_DATABASE_CLASS,
            Class<emo::Database>().Func("openOrCreate", &emo::Database::sqOpenOrCreate)
                                  .Func("open", &emo::Database::sqOpen)
                                  .Func("close", &emo::Database::sqClose)
                                  .Func("remove", &emo::Database::sqRemove)
                                  .Func("createTable", &emo::Database::sqCreateTable)
                                  .Func("dropTable", &emo::Database::sqDropTable)
                                  .Func("select", &emo::Database::sqSelect)
                                  .Func("selectAll", &emo::Database::sqSelectAll)
                                  .Func("selectWhere", &emo::Database::sqSelectWhere)
                                  .Func("selectAllWhere", &emo::Database::sqSelectAllWhere)
                                  .Func("count", &emo::Database::sqCount)
                                  .Func("countWhere", &emo::Database::sqCountWhere)
                                  .Func("insert", &emo::Database::sqInsert)
                                  .Func("update", &emo::Database::sqUpdate)
                                  .Func("updateWhere", &emo::Database::sqUpdateWhere)
                                  .Func("truncateTable", &emo::Database::sqTruncateTable)
                                  .Func("deleteWhere", &emo::Database::sqDeleteWhere)
                                  .Func("vacuum", &emo::Database::sqVacuum)
                                  .Func("getPath", &emo::Database::sqGetPath)
                                  .Func("getLastError", &emo::Database::sqGetLastError)
                                  .Func("getLastErrorMessage", &emo::Database::sqGetLastErrorMessage)
        );

        engine->emoSpace->Bind(
            EMO_PREFERENCE_CLASS,
            Class<emo::Preference>().Func("openOrCreate", &emo::Preference::sqOpenOrCreatePreference)
                                    .Func("open", &emo::Database::sqOpen)
                                    .Func("close", &emo::Database::sqClose)
                                    .Func("get", &emo::Preference::sqGetPreference)
                                    .Func("set", &emo::Preference::sqSetPreference)
                                    .Func("del", &emo::Preference::sqDeletePreference)
                                    .Func("keys", &emo::Preference::sqGetPreferenceKeys)
        );

        // @TODO refactoring with Sqrat
        initRuntimeFunctions();
        initDrawableFunctions();
        initAudioFunctions();
        initJavaGlueFunctions();
        initPhysicsFunctions();
    }
}
