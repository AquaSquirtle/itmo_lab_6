#pragma once
#include "iostream"
#include "map"
#include "vector"
#include "Argument.h"
#include "BoolArgument.h"

namespace ArgumentParser {
    enum class Option{
        long_option,
        short_option,
        no_option,
    };
    struct Result{
        std::string argument;
        std::string value;
        Option opt;
    };
    class ArgParser {
    private:
        enum class WhichMap {
            none_map,
            int_map,
            string_map,
            bool_map,
        };
        std::string name;
        std::map<std::string, Argument<std::string>*> string_arguments;
        std::map<std::string, Argument<int>*> int_arguments;
        std::map<std::string, BoolArgument*> bool_arguments;

        std::string long_help;
        std::string short_help;
        std::string help_description;
        bool is_help = false;

    public:
        explicit ArgParser(const std::string& name);
        bool Parse(int argc, char** argv);
        bool Parse(const std::vector<std::string>& argv);

        BoolArgument& AddFlag(const std::string& long_argument, const std::string& description = "");
        BoolArgument& AddFlag(const char& short_argument, const std::string& long_argument = "", const std::string& description = "");

        Argument<std::string>& AddStringArgument(const std::string& long_argument, const std::string& description = "");
        Argument<std::string>& AddStringArgument(const char& short_argument, const std::string& long_argument = "", const std::string& description = "");

        Argument<int>& AddIntArgument (const char& short_argument, const std::string& long_argument = "", const std::string& description = "");
        Argument<int>& AddIntArgument (const std::string& long_argument, const std::string& description = "");

        std::string GetStringValue (const std::string& argument, int which_value = -1);
        int GetIntValue (const std::string& argument, int which_value = -1);
        bool GetFlag(const std::string& argument);
        static ArgumentParser::Result SplitArgument (const std::string& str);

        void AddHelp(const std::string& long_argument, const std::string& description = "");
        void AddHelp(const char& short_argument, const std::string& long_argument, const std::string& description = "");
        std::string HelpDescription();
        bool Help() const;
    };
}