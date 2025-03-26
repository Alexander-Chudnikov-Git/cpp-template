#ifndef OPTION_PARSER_HPP
#define OPTION_PARSER_HPP

#include <cxxopts.hpp>
#include <memory>
#include <string>
#include <type_traits>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace UTILS
{
class OptionParser
{
public:
    OptionParser(const std::string& app_name, const std::string& app_description);
    ~OptionParser();

    void parseOptions(const int argc, const char** argv);
    bool hasOption(const std::string& name) const;
    void addOption(const std::string& name, const std::string& description);
    size_t getOptionCount(const std::string& name) const;

    void logHelp() const;
    void debugLog(const int argc, const char** argv) const;

public:
    template <typename T>
    void addOption(const std::string& name, const std::string& description, const T& default_value)
    {
        static_assert(!std::is_same_v<T, void>, "Default value required for non-void options.");

        if (!m_options)
        {
            spdlog::error(" Unable to add option, Option Parser is not initialized");
            return;
        }

        this->m_options->add_options()(name, description, cxxopts::value<T>()->default_value(default_value));

        return;
    }

    template <typename T>
    void addOption(const std::string& name, const std::string& description)
    {
        if (!this->m_options)
        {
            spdlog::error(" Unable to add option, Option Parser is not initialized");
            return;
        }

        this->m_options->add_options()(name, description, cxxopts::value<T>());

        return;
    }

    template <typename T = std::string>
    T getOption(const std::string& name) const
    {
        if (!this->m_parsed_options)
        {
            spdlog::error(" Options have not been parsed yet. Call parseOptions() first.");
            return T{};
        }

        if (!hasOption(name))
        {
            spdlog::warn(" Option {} not found. Returning default value.", name);
            return T{};
        }

        try
        {
            return this->m_parsed_options->operator[](name).as<T>();
        }
        catch (const cxxopts::exceptions::exception& e)
        {
            spdlog::error(" Error getting option {}: {}", name, e.what());
        }
        catch (const std::exception& e)
        {
            spdlog::error(" Standard exception while getting option '{}': {}", name, e.what());
        }
        catch (...)
        {
            spdlog::error(" Unknown error occurred while getting option '{}'", name);
        }
        return T{};
    }

private:
    std::unique_ptr<cxxopts::Options>     m_options;
    std::unique_ptr<cxxopts::ParseResult> m_parsed_options;
};

} // namespace UTILS

#endif // OPTION_PARSER_HPP
