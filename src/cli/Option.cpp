
#include "pbcopper/cli/Option.h"
#include <stdexcept>
using namespace PacBio;
using namespace PacBio::CLI;
using namespace PacBio::JSON;
using namespace std;

namespace PacBio {
namespace CLI {
namespace internal {

static const Option defaultHelpOption {
    "help",
    {"h", "help"},
    "Output this help."
};

static const Option defaultLogLevelOption {
    "log_level",
    {"log-level", "logLevel"},
    "Set log level.",
    Option::StringType("INFO")
};

static const Option defaultVerboseOption {
    "verbose",
    {"v", "verbose"},
    "Use verbose output."
};

static const Option defaultVersionOption {
    "version",
    "version",
    "Output version info."
};

} // namespace internal
} // namespace CLI
} // namespace PacBio

const Option& Option::DefaultHelpOption(void)
{ return internal::defaultHelpOption; }

const Option& Option::DefaultLogLevelOption(void)
{ return internal::defaultLogLevelOption; }

const Option& Option::DefaultVerboseOption(void)
{ return internal::defaultVerboseOption; }

const Option& Option::DefaultVersionOption(void)
{ return internal::defaultVersionOption; }

std::string Option::TypeId(void) const
{
    const bool hasChoices = HasChoices();
    const auto& type = DefaultValue().type();
    switch (type)
    {
        case Json::value_t::number_integer  : // fall through
        case Json::value_t::number_unsigned :
        {
            return (hasChoices ? "choice_integer" : "integer");
        }

        case Json::value_t::number_float :
        {
            return (hasChoices ? "choice_float" : "float");
        }
        case Json::value_t::string :
        {
            return (hasChoices ? "choice_string" : "string");
        }
        case Json::value_t::boolean :
        {
            return "boolean";
        }

        // unsupported/invalid JSON types
        case Json::value_t::array     : // fall through
        case Json::value_t::null      : // .
        case Json::value_t::object    : // .
        case Json::value_t::discarded : // .
        default:
            throw std::runtime_error{ "PacBio::CLI::ToolContract::JsonPrinter - unknown type for option: "+ Id() };
    }
}
