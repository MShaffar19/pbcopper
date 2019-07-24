#include <pbcopper/cli2/Results.h>

#include <algorithm>
#include <stdexcept>
#include <thread>

#include <pbcopper/cli2/internal/BuiltinOptions.h>
#include <pbcopper/cli2/internal/OptionTranslator.h>
#include <pbcopper/cli2/internal/PositionalArgumentTranslator.h>

namespace PacBio {
namespace CLI_v2 {

Results& Results::AddPositionalArgument(size_t position, std::string arg)
{
    if (position >= posArgs_.size()) {
        std::ostringstream out;
        out << "[pbcopper] command line results ERROR: a maximum of " << posArgs_.size()
            << " positional arguments are supported, but too many have been provided.";
        throw std::invalid_argument{out.str()};
    }

    posArgs_[position].second = arg;
    return *this;
}

Results& Results::AddDefaultOption(const internal::OptionData& opt)
{
    // make default value
    auto result = std::make_shared<Result>(opt.defaultValue.get(), SetByMode::DEFAULT);

    // add for all option names
    for (const auto& name : opt.names)
        results_.insert({name, result});
    for (const auto& hiddenName : opt.hiddenNames)
        results_.insert({hiddenName, result});

    return *this;
}

Results& Results::AddObservedFlag(const std::string& name, const SetByMode setBy)
{
    AddObservedValue(name, OptionValue{true}, setBy);
    return *this;
}

Results& Results::AddObservedValue(const std::string& name, OptionValue value,
                                   const SetByMode setBy)
{
    const auto found = results_.find(name);
    if (found == results_.cend()) {
        auto result = std::make_shared<Result>(value, setBy);
        results_.insert({name, std::move(result)});
    } else
        *found->second = Result{value, setBy};

    return *this;
}

const std::string& Results::InputCommandLine() const { return inputCommandLine_; }

Results& Results::InputCommandLine(std::string cmdLine)
{
    inputCommandLine_ = std::move(cmdLine);
    return *this;
}

std::string Results::LogFile() const
{
    const auto& logFileOpt = (*this)[Builtin::LogFile];
    const std::string logFileStr = logFileOpt;
    return logFileStr;
}

PacBio::Logging::LogLevel Results::LogLevel() const
{
    const auto& logLevelOpt = (*this)[Builtin::LogLevel];
    const std::string logLevelStr = logLevelOpt;
    return PacBio::Logging::LogLevel(logLevelStr);
}

size_t Results::NumThreads() const
{
    const auto& numThreadOpt = (*this)[Builtin::NumThreads];
    const unsigned int requestedNumThreads = numThreadOpt;
    // NOTE: max may be 0 if unknown
    const unsigned int maxNumThreads = std::thread::hardware_concurrency();

    if (requestedNumThreads == 0) return std::max(1U, maxNumThreads);
    if (maxNumThreads == 0) return requestedNumThreads;
    return std::min(requestedNumThreads, maxNumThreads);
}

std::vector<std::string> Results::PositionalArguments() const
{
    std::vector<std::string> result;
    for (const auto& posArg : posArgs_)
        result.push_back(posArg.second);
    return result;
}

Results& Results::PositionalArguments(const std::vector<internal::PositionalArgumentData>& posArgs)
{
    for (const auto& posArg : posArgs)
        posArgs_.push_back(std::make_pair(posArg, ""));
    return *this;
}

const Result& Results::operator[](const Option& opt) const
{
    const auto optionNames = internal::OptionTranslator::OptionNames(opt);
    for (const auto& name : optionNames) {
        const auto found = results_.find(name);
        if (found != results_.cend()) return *(found->second.get());
    }

    // not found
    std::ostringstream out;
    out << "[pbcopper] command line results ERROR: unknown option, with name(s):\n";
    for (const auto& name : optionNames)
        out << "  " << name << '\n';
    throw std::invalid_argument{out.str()};
}

const std::string& Results::operator[](const PositionalArgument& posArg) const
{
    const auto requestedName = internal::PositionalArgumentTranslator::PositionalArgName(posArg);
    for (const auto& argResult : posArgs_) {
        if (argResult.first.name == requestedName) return argResult.second;
    }

    // not found
    std::ostringstream out;
    out << "[pbcopper] command line results ERROR: unknown positional argument, with name: "
        << requestedName << '\n';
    throw std::invalid_argument{out.str()};
}

}  // namespace CLI_v2
}  // namespace PacBio
