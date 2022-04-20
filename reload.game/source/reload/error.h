#pragma once
#include <vector>
#include <string>
namespace reload {
    class error
    {
        public:
                                    error     (const std::string p_message="");
            virtual                 ~error    ();

            std::string             message;
    };
}

