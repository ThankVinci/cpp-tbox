/*
 *     .============.
 *    //  M A K E  / \
 *   //  C++ DEV  /   \
 *  //  E A S Y  /  \/ \
 * ++ ----------.  \/\  .
 *  \\     \     \ /\  /
 *   \\     \     \   /
 *    \\     \     \ /
 *     -============'
 *
 * Copyright (c) 2018 Hevake and contributors, all rights reserved.
 *
 * This file is part of cpp-tbox (https://github.com/cpp-main/cpp-tbox)
 * Use of this source code is governed by MIT license that can be found
 * in the LICENSE file in the root of the source tree. All contributing
 * project authors may be found in the CONTRIBUTORS.md file in the root
 * of the source tree.
 */
#include "repeat_action.h"
#include <tbox/base/json.hpp>
#include <tbox/base/assert.h>

namespace tbox {
namespace flow {

using namespace std::placeholders;

RepeatAction::RepeatAction(event::Loop &loop, size_t times, Mode mode)
  : Action(loop, "Repeat")
  , repeat_times_(times)
  , mode_(mode)
{ }

RepeatAction::RepeatAction(event::Loop &loop, Action *child, size_t times, Mode mode)
  : Action(loop, "Repeat")
  , child_(child)
  , repeat_times_(times)
  , mode_(mode)
{
    TBOX_ASSERT(child_ != nullptr);
    child_->setFinishCallback(std::bind(&RepeatAction::onChildFinished, this, _1));
}

RepeatAction::~RepeatAction() {
    CHECK_DELETE_RESET_OBJ(child_);
}

void RepeatAction::toJson(Json &js) const {
    Action::toJson(js);
    child_->toJson(js["child"]);
    js["repeat_times"] = repeat_times_;
    js["remain_times"] = remain_times_;
}

bool RepeatAction::setChild(Action *child) {
    CHECK_DELETE_RESET_OBJ(child_);
    child_ = child;
    if (child_ != nullptr)
        child_->setFinishCallback(std::bind(&RepeatAction::onChildFinished, this, _1));
    return true;
}

bool RepeatAction::isReady() const {
    if (child_ == nullptr) {
        LogNotice("%d:%s[%s], no child", id(), type().c_str(), label().c_str());
        return false;
    }
    return child_->isReady();
}

void RepeatAction::onStart() {
    TBOX_ASSERT(child_ != nullptr);
    remain_times_ = repeat_times_ - 1;
    child_->start();
}

void RepeatAction::onStop() {
    TBOX_ASSERT(child_ != nullptr);
    child_->stop();
}

void RepeatAction::onPause() {
    TBOX_ASSERT(child_ != nullptr);
    child_->pause();
}

void RepeatAction::onResume() {
    TBOX_ASSERT(child_ != nullptr);
    child_->resume();
}

void RepeatAction::onReset() {
    TBOX_ASSERT(child_ != nullptr);
    child_->reset();
}

void RepeatAction::onChildFinished(bool is_succ) {
    if (state() == State::kRunning) {
        if ((mode_ == Mode::kBreakSucc && is_succ) ||
                (mode_ == Mode::kBreakFail && !is_succ)) {
            finish(is_succ);
        } else {
            if (remain_times_ > 0) {
                child_->reset();
                child_->start();
                --remain_times_;
            } else {
                finish(true);
            }
        }
    }
}

}
}
