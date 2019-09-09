#include "curlinho/parameters.h"
#include "curlinho/util.h"

#include <initializer_list>
#include <string>


namespace curlinho {

Parameters::Parameters(const std::initializer_list<Parameter>& parameters) {
    for (const auto& parameter : parameters) {
        AddParameter(parameter);
    }
}

void Parameters::AddParameter(const Parameter& parameter) {
    if (!content.empty()) {
        content += "&";
    }

    auto escapedKey = curlinho::util::urlEncode(parameter.key);
    if (parameter.value.empty()) {
        content += escapedKey;
    } else {
        auto escapedValue = curlinho::util::urlEncode(parameter.value);
        content += escapedKey + "=" + escapedValue;
    }
}

} // namespace cpr
