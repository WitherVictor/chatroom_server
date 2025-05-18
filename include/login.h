#pragma once

#include "session.h"

#include <nlohmann/json.hpp>

namespace login {
    void process_request(session&, nlohmann::json);
}