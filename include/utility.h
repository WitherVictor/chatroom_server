#pragma once

namespace utility {
    using cstring = const char*;
    using port = unsigned short;

    bool valid_port(cstring);
    port make_port(cstring);
}