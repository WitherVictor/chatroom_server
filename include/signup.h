#pragma once

#include <nlohmann/json.hpp>

#include "session.h"

namespace signup {
    void process_request(session&, nlohmann::json);
};