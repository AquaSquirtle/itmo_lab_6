#include "ArgParser.h"


Argument<std::string>&
ArgumentParser::ArgParser::AddStringArgument(const std::string &long_argument, const std::string& description) {
    auto* arg = new Argument<std::string> (long_argument, "", description);
    string_arguments.insert({long_argument, arg});
    return *arg;
}

Argument<std::string>&
ArgumentParser::ArgParser::AddStringArgument(const char &short_argument, const std::string &long_argument, const std::string& description) {
    auto* arg = new Argument<std::string> (long_argument, std::string(1, short_argument), description);
    string_arguments.insert({std::string(1, short_argument), arg});
    string_arguments.insert({long_argument, arg});
    return *arg;
}

Argument<int>&
ArgumentParser::ArgParser::AddIntArgument(const char &short_argument, const std::string &long_argument, const std::string& description) {
    auto* arg = new Argument<int> (long_argument, std::string(1, short_argument), description);
    int_arguments.insert({std::string(1, short_argument), arg});
    int_arguments.insert({long_argument, arg});
    return *arg;
}

Argument<int>&
ArgumentParser::ArgParser::AddIntArgument(const std::string &long_argument, const std::string& description) {
    auto* arg = new Argument<int> (long_argument, "", description);
    int_arguments.insert({long_argument, arg});
    return *arg;
}


BoolArgument&
ArgumentParser::ArgParser::AddFlag(const char &short_argument, const std::string &long_argument, const std::string& description) {
    auto* arg = new BoolArgument (long_argument, std::string(1, short_argument), description);
    bool_arguments.insert({std::string(1, short_argument), arg});
    bool_arguments.insert({long_argument, arg});
    return *arg;
}

BoolArgument&
ArgumentParser::ArgParser::AddFlag(const std::string &long_argument, const std::string& description) {
    auto* arg = new BoolArgument (long_argument, "", description);
    bool_arguments.insert({long_argument, arg});
    return *arg;
}

std::string ArgumentParser::ArgParser::GetStringValue(const std::string &argument, int which_value) {
    if (which_value == -1) {
        return string_arguments[argument]->GetValue()[0];
    }
    return string_arguments[argument]->GetValue()[which_value];
}

int ArgumentParser::ArgParser::GetIntValue(const std::string &argument, int which_value) {
    if (which_value == -1) {
        return int_arguments[argument]->GetValue()[0];
    }
    return int_arguments[argument]->GetValue()[which_value];
}

bool ArgumentParser::ArgParser::GetFlag(const std::string &argument) {
    return bool_arguments[argument]->GetValue();
}


ArgumentParser::ArgParser::ArgParser(const std::string &name_) {
    name = name_;
}

bool ArgumentParser::ArgParser::Parse(int argc, char **argv) {
    std::vector<std::string> str_argv;
    for (int i = 0; i < argc; ++i) {
        str_argv.push_back(argv[i]);
    }
    if (Parse(str_argv)) {
        return true;
    }
    return false;
}

bool ArgumentParser::ArgParser::Parse(const std::vector<std::string>& argv) {
    Result splitted_arg;
    std::string positional_arg;
    WhichMap positional_map;
    int positional_args_counter = 0;
    if (!int_arguments.empty()) {
        for (auto arg: int_arguments) {
            if (arg.second->GetPositional()) {
                positional_arg = arg.first;
                positional_map = WhichMap::int_map;
                ++positional_args_counter;
            }
        }
    }
    if (!string_arguments.empty()){
        for (auto arg: string_arguments) {
            if (arg.second->GetPositional()) {
                positional_arg = arg.first;
                positional_map = WhichMap::string_map;
                ++positional_args_counter;
            }
        }
    }
    if (positional_args_counter > 1) {
        return false;
    }
    std::string prev_key;
    for (int i = 1; i < argv.size(); ++i) {
        WhichMap map = WhichMap::none_map;
        splitted_arg = SplitArgument(argv[i]);
        if (int_arguments.find(splitted_arg.argument) != int_arguments.end()) {
            map = WhichMap::int_map;
        }
        else if (string_arguments.find(splitted_arg.argument) != string_arguments.end()) {
            map = WhichMap::string_map;
        }
        else if (bool_arguments.find(splitted_arg.argument) != bool_arguments.end() || splitted_arg.opt == Option::short_option) {
            map = WhichMap::bool_map;
        }
        else if (splitted_arg.argument == short_help || splitted_arg.argument == long_help) {
            is_help = true;
            return true;
        }
        switch (map) {
            case WhichMap::int_map: {
                int number;
                try {
                    number = std::stoi(splitted_arg.value);
                    int_arguments[splitted_arg.argument]->AddValue(number);
                }
                catch (...) {
                    return false;
                }
                break;

            }
            case WhichMap::string_map: {
                string_arguments[splitted_arg.argument]->AddValue(splitted_arg.value);
                break;

            }

            case WhichMap::bool_map: {
                if (splitted_arg.opt == Option::short_option) {
                    for (int j = 0; j < splitted_arg.argument.length(); ++j) {
                        try {
                            bool_arguments[std::string(1, splitted_arg.argument[j])]->ChangeValue();
                        }
                        catch (...) {
                            return false;
                        }
                    }
                }
                else {
                    bool_arguments[splitted_arg.argument]->ChangeValue();
                }
                break;
            }

            case WhichMap::none_map: {
                if (!positional_arg.empty()) {
                    if (positional_map == WhichMap::int_map) {
                        int number;
                        try {
                            number = std::stoi(splitted_arg.argument);
                            int_arguments[positional_arg]->AddValue(number);
                        }
                        catch (...) {
                            return false;
                        }
                    }
                    else if (positional_map == WhichMap::string_map) {
                        string_arguments[positional_arg]->AddValue(splitted_arg.argument);
                    }
                }
                else {
                    return false;
                }
                break;
            }
        }
    }
    if (!int_arguments.empty()) {
        for (auto arg: int_arguments) {
            if (!arg.second->IsCorrect()) {
                return false;
            }
            arg.second->CopyValues();
        }
    }
    if (!string_arguments.empty()){
        for (auto arg: string_arguments) {
            if (!arg.second->IsCorrect()) {
                return false;
            }
            arg.second->CopyValues();
        }
    }
    if (!bool_arguments.empty()) {
        for (auto arg: bool_arguments) {
            arg.second->CopyValues();
        }
    }
    return true;

}

ArgumentParser::Result ArgumentParser::ArgParser::SplitArgument(const std::string &str) {
    std::string argument;
    std::string value;
    int i = 0;
    bool arg_or_val = false;
    int option_counter = 0;
    while (str[i] != '\0') {
        if (str[i] == '-' && !arg_or_val) {
            ++option_counter;
            ++i;
            continue;
        }
        if (str[i] == '=' && !arg_or_val) {
            arg_or_val = true;
            ++i;
            continue;
        }
        if (!arg_or_val) {
            argument += str[i];
        }
        if (arg_or_val) {
            value += str[i];
        }
        ++i;
    }
    Result result;
    result.value = value;
    result.argument = argument;
    if (option_counter == 1) {
        result.opt = Option::short_option;
    }
    else if (option_counter == 2) {
        result.opt = Option::long_option;
    }
    else {
        result.opt = Option::no_option;
    }

    return result;
}

void ArgumentParser::ArgParser::AddHelp(const std::string &long_argument, const std::string &description) {
    long_help = long_argument;
    help_description = description;
}

void ArgumentParser::ArgParser::AddHelp(const char &short_argument, const std::string &long_argument, const std::string &description) {
    short_help = std::string(1, short_argument);
    long_help = long_argument;
    help_description = description;
}

bool ArgumentParser::ArgParser::Help() const {
    return is_help;
}

std::string ArgumentParser::ArgParser::HelpDescription() {
    std::string full_help_description;
    full_help_description += name + '\n';
    full_help_description += help_description + '\n';
    full_help_description += '\n';

    if (!int_arguments.empty()) {
        for (auto arg: int_arguments) {
            full_help_description += arg.second->GenerateArgDescription();
        }
    }
    if (!string_arguments.empty()){
        for (auto arg: string_arguments) {
            full_help_description += arg.second->GenerateArgDescription(true);
        }
    }
    if (!bool_arguments.empty()) {
        for (auto arg: bool_arguments) {
            full_help_description += arg.second->GenerateArgDescription();
        }
    }
    full_help_description += '\n';
    if (!short_help.empty()) {
        full_help_description += '-' + short_help + ", ";
    }
    full_help_description += "--" + long_help;
    full_help_description += " Display this help and exit\n";
    return full_help_description;
}