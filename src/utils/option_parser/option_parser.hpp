#ifndef OPTION_PARSER_HPP
#define OPTION_PARSER_HPP

#include "spdlog_wrapper.hpp"

#include <cxxopts.hpp>
#include <memory>
#include <ranges>
#include <string>
#include <type_traits>

namespace UTILS
{
class OptionParser
{
public:
	static std::shared_ptr<OptionParser> instance();

	~OptionParser();

private:
	OptionParser();

	OptionParser(const OptionParser&)			 = delete;
	OptionParser(OptionParser&&)				 = delete;
	OptionParser& operator=(const OptionParser&) = delete;
	OptionParser& operator=(OptionParser&&)		 = delete;

	void initialize();

public:
	void   setDescription(const std::string& app_name, const std::string& app_description);
	void   parseOptions(const int argc, const char** argv);
	bool   hasOption(const std::string& name) const;
	void   addOption(const std::string& name, const std::string& description);
	size_t getOptionCount(const std::string& name) const;

	void logHelp() const;
	void debugLog() const;

public:
	template<typename T>
	void addOption(const std::string& name, const std::string& description, const T& default_value)
	{
		static_assert(!std::is_same_v<T, void>, "Default value required for non-void options.");

		if (!m_options)
		{
			SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to add option, Option Parser is not initialized");
			return;
		}

		this->m_options->add_options()(name, description, cxxopts::value<T>()->default_value(default_value));

		return;
	}

	template<typename T>
	void addOption(const std::string& name, const std::string& description)
	{
		if (!this->m_options)
		{
			SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to add option, Option Parser is not initialized");
			return;
		}

		this->m_options->add_options()(name, description, cxxopts::value<T>());

		return;
	}

	template<typename T = std::string>
	T getOption(const std::string& name) const
	{
		if (!this->m_parsed_options)
		{
			SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Options have not been parsed yet. Call parseOptions() first.");
			return T {};
		}

		if (!hasOption(name))
		{
			SPD_WARN_CLASS(COMMON::d_settings_group_options, fmt::format("Option {} not found. Returning default value.", name));
			return T {};
		}

		try
		{
			return this->m_parsed_options->operator[](name).as<T>();
		}
		catch (const cxxopts::exceptions::exception& e)
		{
			SPD_ERROR_CLASS(COMMON::d_settings_group_options, fmt::format("Error getting option {}: {}", name, e.what()));
		}
		catch (const std::exception& e)
		{
			SPD_ERROR_CLASS(COMMON::d_settings_group_options, fmt::format("Standard exception while getting option '{}': {}", name, e.what()));
		}
		catch (...)
		{
			SPD_ERROR_CLASS(COMMON::d_settings_group_options, fmt::format("Unknown error occurred while getting option '{}'", name));
		}
		return T {};
	}

protected:
	static std::shared_ptr<OptionParser> m_instance;
	static std::mutex					 m_mutex;

private:
	std::unique_ptr<cxxopts::Options>	  m_options;
	std::unique_ptr<cxxopts::ParseResult> m_parsed_options;
};

} // namespace UTILS

#endif // OPTION_PARSER_HPP
