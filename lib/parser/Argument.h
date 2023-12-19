template <typename T>
class Argument {
public:
    Argument(const std::string& long_arg_,const std::string& short_arg_, const std::string& description_);
    Argument<T>& StoreValue(T& value_);
    Argument<T>& StoreValues(std::vector<T>& value_);
    Argument<T>& Default(T value_);
    Argument<T>& MultiValue(size_t amount = 0);
    Argument<T>& Positional();
    std::vector<T> GetValue();
    bool GetPositional();
    void CopyValues();
    void AddValue(T value_);
    bool IsCorrect();
    std::string GenerateArgDescription();
    std::string GenerateArgDescription(bool str);


private:

    std::vector<T>* stored_values = nullptr;
    T* stored_value = nullptr;

    bool is_multivalue = false;
    std::vector<T> values;
    bool is_positional = false;
    bool is_default = false;
    bool first_elem = true;
    size_t min_args = 0;
    std::string description;
    std::string long_arg;
    std::string short_arg;
    size_t cur_args = 0;
    bool is_generated = false;
};

template<typename T>
std::string Argument<T>::GenerateArgDescription(bool str) {
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
    if (is_multivalue || is_default || is_positional) {
        arg_description += " [";
        if (is_multivalue) {
            arg_description += " repeated,";
        }
        if (min_args > 0) {
            arg_description += " min args =" + std::to_string(min_args) + ",";
        }
        if (is_default) {
            arg_description += " default = " + values[0] + ",";
        }
        if (is_positional) {
            arg_description += " positional";
        }
        arg_description += ']';
    }
    arg_description += '\n';
    return arg_description;
}

template<typename T>
std::string Argument<T>::GenerateArgDescription() {
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
    if (is_multivalue || is_default || is_positional) {
        arg_description += " [";
        if (is_multivalue) {
            arg_description += "repeated, ";
        }
        if (min_args > 0) {
            arg_description += "min args = " + std::to_string(min_args) + ",";
        }
        if (is_default) {
            arg_description += "default = " + std::to_string(values[0]) + ", ";
        }
        if (is_positional) {
            arg_description += " positional";
        }
        arg_description += ']';
    }
    arg_description += '\n';
    return arg_description;
}

template<typename T>
Argument<T>::Argument(const std::string& long_arg_,const std::string& short_arg_, const std::string& description_) {
    description = description_;
    long_arg = long_arg_;
    short_arg = short_arg_;
}

template<typename T>
bool Argument<T>::GetPositional() {
    return is_positional;
}

template<typename T>
bool Argument<T>::IsCorrect() {
    if (cur_args >= min_args && cur_args != 0) {
        return true;
    }
    return false;
}


template<typename T>
void Argument<T>::AddValue(const T value_) {
    if (!is_default) {
        first_elem = false;
    }
    if (is_default && is_multivalue && first_elem) {
        values[0] = value_;
        first_elem = false;
    }
    else if (is_multivalue && !first_elem) {
        ++cur_args;
        values.push_back(value_);
    }
    else if (!is_multivalue && is_default) {
        values[0] = value_;
    }
    else if (!is_multivalue && !is_default) {
        ++cur_args;
        values.push_back(value_);
        is_default = true;
    }
    else {
        std::cout << "error";
    }
}

template<typename T>
Argument<T>& Argument<T>::Positional() {
    is_positional = true;
    return *this;
}

template<typename T>
Argument<T>& Argument<T>::StoreValues(std::vector<T> &value_) {
    stored_values = &value_;
    return *this;
}

template<typename T>
Argument<T>& Argument<T>::StoreValue(T &value_) {
    stored_value = &value_;
    return *this;
}


template<typename T>
Argument<T>& Argument<T>::MultiValue(size_t amount) {
    is_multivalue = true;
    min_args = amount;
    return *this;

}

template<typename T>
void Argument<T>::CopyValues() {
    if (stored_value != nullptr && !is_multivalue) {
        *stored_value = values[0];
    }
    if (stored_values != nullptr && is_multivalue) {
        *stored_values = values;
    }
}

template<typename T>
std::vector<T> Argument<T>::GetValue() {
    return values;
}

template<typename T>
Argument<T>& Argument<T>::Default(T value_) {
    ++cur_args;
    is_default = true;
    values.push_back(value_);
    return *this;
}