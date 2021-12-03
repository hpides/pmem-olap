# Copyright 2010-2018 Google LLC
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Code sample to demonstrates how to build an interval."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from ortools.sat.python import cp_model


def IntervalSampleSat():
    model = cp_model.CpModel()

    horizon = 100
    start_var = model.NewIntVar(0, horizon, 'start')
    duration = 10  # Python cp/sat code accept integer variables or constants.
    end_var = model.NewIntVar(0, horizon, 'end')
    interval_var = model.NewIntervalVar(start_var, duration, end_var,
                                        'interval')

    print('start = %s, duration = %i, end = %s, interval = %s' %
          (start_var, duration, end_var, interval_var))


IntervalSampleSat()
