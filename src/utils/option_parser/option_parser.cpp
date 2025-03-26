#include "option_parser.hpp"

namespace UTILS
{
OptionParser::OptionParser(const std::string& app_name, const std::string& app_description) :
	m_options(std::make_unique<cxxopts::Options>(app_name, app_description))
{
	this->m_options->allow_unrecognised_options();
	return;
}

OptionParser::~OptionParser()
{
	this->m_options.reset();
	this->m_parsed_options.reset();

	return;
}

void OptionParser::parseOptions(const int argc, const char** argv)
{
	if (!this->m_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to parse options, Option Parser is not initialized.");
	}

	try
	{
		this->m_parsed_options = std::make_unique<cxxopts::ParseResult>(this->m_options->parse(argc, argv));

		for (const auto& unmatched_argument : this->m_parsed_options->unmatched())
		{
			SPD_WARN_CLASS(COMMON::d_settings_group_options,
						   fmt::format("Unsupported argument passed: {}", unmatched_argument));
		}

		return;
	}
	catch (const cxxopts::exceptions::exception& e)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, fmt::format("Error parsing command line options: {}", e.what()));
	}
	catch (...)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unknown error occurred while parsing arguments.");
	}

	exit(1);
}

bool OptionParser::hasOption(const std::string& name) const
{
	if (!this->m_parsed_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Options have not been parsed yet. Call parseOptions() first.");
		return false;
	}

	return this->m_parsed_options->count(name) > 0;
}

void OptionParser::addOption(const std::string& name, const std::string& description)
{
	if (!this->m_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to add option, Option Parser is not initialized.");
		return;
	}

	this->m_options->add_options()(name, description);

	return;
}

size_t OptionParser::getOptionCount(const std::string& name) const
{
	if (!this->m_parsed_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Options have not been parsed yet. Call parseOptions() first.");
		return 0;
	}

	return this->m_parsed_options->count(name);
}

void OptionParser::logHelp() const
{
	if (!this->m_options)
	{
		SPD_ERROR_CLASS(COMMON::d_settings_group_options, "Unable to get help message, Option Parser is not initialized.");
		return;
	}

	std::string		   help_message = this->m_options->help();
	std::istringstream iss(help_message);
	std::string		   line;

	while (std::getline(iss, line))
	{
		spdlog::info(" {}", line);
	}
}

void OptionParser::debugLog(const int argc, const char** argv) const
{
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "\tProject Information");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "-----------------------------------------------------------");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tProject Name: {:<24} {}", PROJECT_NAME, PROJECT_VERSION));
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tCompile Time: {}", COMPILE_TIME));
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tCompiler:     {:<24} {}", COMPILER_ID, COMPILER_VERSION));
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "-----------------------------------------------------------");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "\tCommand-Line Arguments");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "-----------------------------------------------------------");
	SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tArgument Count: {}", argc));
	for (int i = 0; i < argc; ++i)
	{
		SPD_INFO_CLASS(COMMON::d_settings_group_options, fmt::format("\tArgument [{}]: {}", i, argv[i]));
	}
	SPD_INFO_CLASS(COMMON::d_settings_group_options, "===========================================================");
}
} // namespace UTILS
