class BoolArgument {
private:
    bool* stored_value = nullptr;
    bool value = false;
    std::string description;
    std::string long_arg;
    std::string short_arg;
    bool is_generated = false;
public:
    BoolArgument(const std::string& long_arg_,const std::string& short_arg_, const std::string& description_){
        description = description_;
        long_arg = long_arg_;
        short_arg = short_arg_;
    };
    BoolArgument& StoreValue(bool& value_) {
        stored_value = &value_;
        return *this;
    };
    std::string GenerateArgDescription() {
        if (!is_generated) {
            is_generated = true;
        } else {
            return "";
        }
        std::string arg_description;
        if (!short_arg.empty()) {
            arg_description += '-' + short_arg + ", ";
        }
        arg_description += "--" + long_arg + ", ";
        arg_description += description + ' ';
        if (value) {
            arg_description += "[default = true]";
        }
        arg_description += '\n';
        return arg_description;
    }
    BoolArgument& Default(bool value_) {
        value = value_;
        return *this;
    };
    bool GetValue() const {
        return value;
    };
    void ChangeValue() {
        value = !value;
    };
    void CopyValues() {
        if (stored_value != nullptr) {
            *stored_value = value;
        }
    };
};