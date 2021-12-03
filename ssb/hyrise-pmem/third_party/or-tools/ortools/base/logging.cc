// Copyright 2010-2018 Google LLC
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ortools/base/logging.h"

#include <mutex>  // NOLINT

#include "ortools/base/commandlineflags.h"

DECLARE_bool(log_prefix);
DECLARE_bool(logtostderr);

namespace {
std::once_flag init_done;
}  // namespace

void FixFlagsAndEnvironmentForSwig() {
  std::call_once(init_done, [] { google::InitGoogleLogging("swig_helper"); });
  FLAGS_logtostderr = true;
  FLAGS_log_prefix = false;
}
