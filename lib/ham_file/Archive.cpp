#include "Archive.h"
//Структура архива
//8 байт под размер названия файла
//название файла, соответствующее размеру
//8 байт под размер файла
//файл

Archive::Archive(const std::string& path_)
    : path(path_)
{
}

void Archive::Create() {
    std::ofstream archive(path, std::ios::binary);
    archive.close();
}

void Archive::AppendFile(const std::string& name) {
    std::ofstream archive(path, std::ios::binary | std::ios::app | std::ios::ate);
    std::ifstream file (name, std::ios::binary);
    if (!archive.is_open()) {
        std::cerr << ("CANT OPEN ARCHIVE");
        return;
    }
    if (!file.is_open()) {
        std::cerr << ("CANT OPEN FILE");
        return;
    }
    size_t temp_size = name.length();
    archive.write(reinterpret_cast<const char*> (&temp_size), sizeof(size_t));
    archive.write(name.c_str(), name.length());
    char buffer[kSizeOfBuffer];
    file.seekg(0, std::ios::end);
    size_t bytes_left = file.tellg();
    size_t size_of_encoded_file = (kSizeOfBuffer + 1)*(bytes_left/kSizeOfBuffer) + bytes_left%kSizeOfBuffer + (bytes_left%kSizeOfBuffer != 0);
    file.seekg(0, std::ios::beg);
    archive.write(reinterpret_cast<const char*> (&size_of_encoded_file), sizeof(size_t));
    while (bytes_left > 0) {
        if (bytes_left/kSizeOfBuffer > 0) {
            file.read(buffer, kSizeOfBuffer);
            std::vector<char> temp_buffer;
            for (int i = 0; i < kSizeOfBuffer; ++i) {
                temp_buffer.push_back(buffer[i]);
            }
            temp_buffer = EncodeBuffer(temp_buffer);
            for (auto i: temp_buffer) {
                archive.write(reinterpret_cast<char*>(&i), 1);
            }
            temp_buffer.clear();
            bytes_left -= kSizeOfBuffer;
        }
        else {
            file.read(buffer, bytes_left);
            std::vector<char> temp_buffer;
            for (int i = 0; i < bytes_left; ++i) {
                temp_buffer.push_back(buffer[i]);
            }
            temp_buffer = EncodeBuffer(temp_buffer);
            for (auto i: temp_buffer) {
                archive.write(reinterpret_cast<char*>(&i), 1);
            }
            temp_buffer.clear();
            break;
        }
    }
    archive.close();
    file.close();
}

void Archive::ExtractFile(const std::string &name) {
    std::ifstream archive (path, std::ios::binary | std::ios::app);
    if (!archive.is_open()) {
        std::cerr << ("CANT OPEN FILE");
        return;
    }
    std::ofstream file_to_extract;
    size_t length_of_file_name;
    size_t length_of_file;
    std::string file_name;
    while (archive.read(reinterpret_cast<char*>(&length_of_file_name), sizeof(size_t))) {
        file_name.resize(length_of_file_name);
        archive.read(&file_name[0], length_of_file_name);
        archive.read(reinterpret_cast<char*>(&length_of_file), sizeof(size_t));


        if (file_name == name) {
            file_to_extract.open(name , std::ios::binary);
            size_t bytes_left = length_of_file;
            char buffer[kSizeOfBuffer + 1];
            while (bytes_left > 0) {
                if (bytes_left/(kSizeOfBuffer + 1) > 0) {
                    archive.read(buffer, kSizeOfBuffer + 1);
                    std::vector<char> temp_buffer;
                    for (int i = 0; i < kSizeOfBuffer + 1; ++i) {
                        temp_buffer.push_back(buffer[i]);
                    }
                    temp_buffer = DecodeBuffer(temp_buffer);
                    for (auto i: temp_buffer) {
                        file_to_extract.write(reinterpret_cast<char*>(&i), 1);
                    }
                    temp_buffer.clear();
                    bytes_left -= kSizeOfBuffer + 1;
                }
                else {
                    archive.read(buffer, bytes_left);
                    std::vector<char> temp_buffer;
                    for (int i = 0; i < bytes_left; ++i) {
                        temp_buffer.push_back(buffer[i]);
                    }
                    temp_buffer = DecodeBuffer(temp_buffer);
                    for (auto i: temp_buffer) {
                        file_to_extract.write(reinterpret_cast<char*>(&i), 1);
                    }
                    temp_buffer.clear();
                    break;
                }

            }
            break;
        }
        else {
            archive.seekg(length_of_file, std::ios::cur);
        }
    }
    archive.close();
    file_to_extract.close();
}

void Archive::GetListOfFiles() {
    std::ifstream archive(path, std::ios::binary);
    if (!archive.is_open()) {
        std::cerr << ("CANT OPEN FILE");
        return;
    }
    size_t length_of_file_name;
    std::string file_name;
    size_t length_of_file;
    while (archive.read(reinterpret_cast<char *>(&length_of_file_name), sizeof(size_t))) {
        file_name.resize(length_of_file_name);
        archive.read(&file_name[0], length_of_file_name);
        archive.read(reinterpret_cast<char*>(&length_of_file), sizeof(size_t));
        std::cout << "File name: " << file_name << "\nFile size: " << length_of_file;
        std::cout << '\n';
        archive.seekg(length_of_file, std::ios::cur);
    }
    archive.close();
}

void Archive::ExtractAllFiles() {
    std::ifstream archive (path, std::ios::binary | std::ios::app);
    if (!archive.is_open()) {
        std::cerr << ("CANT OPEN FILE");
        return;
    }
    std::ofstream file_to_extract;
    size_t length_of_file_name;
    size_t length_of_file;
    std::string file_name;
    char buffer[kSizeOfBuffer + 1];
    while (archive.read(reinterpret_cast<char*>(&length_of_file_name), sizeof(size_t))) {
        file_name.resize(length_of_file_name);
        archive.read(&file_name[0], length_of_file_name);
        archive.read(reinterpret_cast<char*>(&length_of_file), sizeof(size_t));
        file_to_extract.open(file_name , std::ios::binary);
        size_t bytes_left = length_of_file;
        while (bytes_left > 0) {
            if (bytes_left/(kSizeOfBuffer + 1) > 0) {
                archive.read(buffer, kSizeOfBuffer + 1);
                std::vector<char> temp_buffer;
                for (int i = 0; i < kSizeOfBuffer + 1; ++i) {
                    temp_buffer.push_back(buffer[i]);
                }
                temp_buffer = DecodeBuffer(temp_buffer);
                for (auto i: temp_buffer) {
                    file_to_extract.write(reinterpret_cast<char*>(&i), 1);
                }
                temp_buffer.clear();
                bytes_left -= kSizeOfBuffer + 1;
            }
            else {
                archive.read(buffer, bytes_left);
                std::vector<char> temp_buffer;
                for (int i = 0; i < bytes_left; ++i) {
                    temp_buffer.push_back(buffer[i]);
                }
                temp_buffer = DecodeBuffer(temp_buffer);
                for (auto i: temp_buffer) {
                    file_to_extract.write(reinterpret_cast<char*>(&i), 1);
                }
                temp_buffer.clear();
                break;
            }
        }
        file_to_extract.close();
    }
    archive.close();
}

void Archive::DeleteFile(const std::string &name) {
    std::ifstream archive(path, std::ios::binary | std::ios::app);
    std::ofstream temp_archive("temp.haf", std::ios ::binary);
    if (!archive.is_open()) {
        std::cerr << "CANT OPEN FILE";
        return;
    }
    size_t length_of_file_name;
    std::string file_name;
    size_t length_of_file;
    while (archive.read(reinterpret_cast<char*>(&length_of_file_name), sizeof(size_t))) {
        file_name.resize(length_of_file_name);
        archive.read(&file_name[0], length_of_file_name);
        archive.read(reinterpret_cast<char*>(&length_of_file), sizeof (size_t));
        if (file_name != name) {
            temp_archive.write(reinterpret_cast<char*>(&length_of_file_name), sizeof(size_t));
            temp_archive.write(reinterpret_cast<char*>(&file_name[0]), length_of_file_name);
            temp_archive.write(reinterpret_cast<char*>(&length_of_file), sizeof(size_t));
            char buffer[kSizeOfBuffer];
            size_t bytes_left = length_of_file;
            while (bytes_left > 0) {
                if (bytes_left/kSizeOfBuffer > 0) {
                    archive.read(buffer, kSizeOfBuffer);
                    temp_archive.write(buffer, kSizeOfBuffer);
                    bytes_left -= kSizeOfBuffer;
                }
                else {
                    archive.read(buffer, bytes_left);
                    temp_archive.write(buffer, bytes_left);
                    break;
                }
            }
        }
        else {
            archive.seekg(length_of_file, std::ios::cur);
        }
    }
    archive.close();
    temp_archive.close();
    std::remove(path.c_str());
    std::rename("temp.haf", path.c_str());
}

void Archive::Concatenate(const std::string &first_archive_name, const std::string &second_archive_name) {
    std::ifstream first_archive(first_archive_name, std::ios::binary | std::ios::app);
    std::ifstream second_archive(second_archive_name, std::ios::binary | std::ios::app);
    std::ofstream archive (path, std::ios::binary);
    if (!archive.is_open() || !first_archive.is_open() || !second_archive.is_open()) {
        std::cerr << ("CANT OPEN FILE");
        return;
    }
    char buffer[kSizeOfBuffer];
    first_archive.seekg(0, std::ios::end);
    size_t bytes_left = first_archive.tellg();
    first_archive.seekg(0, std::ios::beg);
    while (bytes_left > 0) {
        if (bytes_left/kSizeOfBuffer > 0) {
            first_archive.read(buffer, kSizeOfBuffer);
            archive.write(buffer, kSizeOfBuffer);
            bytes_left -= kSizeOfBuffer;
        }
        else {
            first_archive.read(buffer, bytes_left);
            archive.write(buffer, bytes_left);
            break;
        }
    }
    first_archive.close();
    second_archive.seekg(0, std::ios::end);
    bytes_left = second_archive.tellg();
    second_archive.seekg(0, std::ios::beg);
    while (bytes_left > 0) {
        if (bytes_left/kSizeOfBuffer > 0) {
            second_archive.read(buffer, kSizeOfBuffer);
            archive.write(buffer, kSizeOfBuffer);
            bytes_left -= kSizeOfBuffer;
        }
        else {
            second_archive.read(buffer, bytes_left);
            archive.write(buffer, bytes_left);
            break;
        }
    }
    archive.close();
    first_archive.close();
    second_archive.close();
}

std::vector<char> Archive::EncodeBuffer(std::vector<char>& message) {
        std::vector<std::bitset<8>> temp_message;
        std::vector<std::bitset<8>> temp;
        for (unsigned char i : message) {
            temp_message.push_back(i);
            temp.push_back(0);
        }
        int control_bis = 1;
        while ((1 << control_bis) < message.size() * kSizeOfByte + control_bis + 1) {
            ++control_bis;
        }
        size_t cur_bit = 0;
        for (size_t i = 0; i < (message.size() * kSizeOfByte) + control_bis; ++i) {
            if (i / kSizeOfByte >= temp.size()) {
                temp.push_back(0);
            }
            if (!((i + 1) & i)) {
                temp[i / kSizeOfByte][(kSizeOfByte - 1) - i % kSizeOfByte] = false;
            }
            else {
                temp[i / kSizeOfByte][(kSizeOfByte - 1) - i % kSizeOfByte] = temp_message[cur_bit / kSizeOfByte][(kSizeOfByte - 1) - cur_bit % kSizeOfByte];
                ++cur_bit;
            }
        }

        for (int i = 0;  i < control_bis; ++i) {
            int sum = 0;
            for (int j = 1 << i; j < temp.size() * 8; ++j) {
                if ((j + 1) & (1 << i)) {
                    sum ^= temp[j / kSizeOfByte][(kSizeOfByte - 1) - j % kSizeOfByte];
                }
            }
            temp[((1 << i) - 1) / kSizeOfByte][(kSizeOfByte - 1) - ((1 << i) - 1) % kSizeOfByte] = sum;
        }
        std::vector<char> result;
        for (auto i: temp) {
            result.push_back(i.to_ulong());
        }
        return result;
    }

std::vector<char> Archive::DecodeBuffer(std::vector<char> &message) {
    std::vector<std::bitset<8>> temp_message;
    size_t wrong_bit = 0;
    uint8_t control_bit = 0;
    for (unsigned char i : message) {
        temp_message.push_back(i);
    }
    for (int i = 0; i < message.size() * kSizeOfByte; ++i) {
        if (!((i + 1) & i)) {
            ++control_bit;
        }
    }

    for (int i = 0;  i < control_bit; ++i) {
        int sum = 0;
        for (int j = 1 << i; j < temp_message.size() * 8; ++j) {
            if ((j + 1) & (1 << i)) {
                sum ^= temp_message[j / kSizeOfByte][(kSizeOfByte - 1) - j % kSizeOfByte];
            }
        }
        if (sum != temp_message[((1 << i) - 1) / kSizeOfByte][(kSizeOfByte - 1) - ((1 << i) - 1) % kSizeOfByte]) {
            wrong_bit += 1 << i;
        }
    }
    if (wrong_bit) {
        --wrong_bit;
        temp_message[wrong_bit / 8][(kSizeOfByte - 1) - wrong_bit % kSizeOfByte].flip();
    }

    std::vector<std::bitset<8>> temp_res;
    for (int i = 0; i < ((message.size()*kSizeOfByte - control_bit - (kSizeOfByte - control_bit)))/kSizeOfByte; ++i) {
        temp_res.push_back(0);
    }
    size_t cur_bit = 0;
    for (size_t i = 0; i < (message.size() * kSizeOfByte) - (kSizeOfByte - control_bit); ++i) {
        if (((i + 1) & i)) {
            temp_res[cur_bit / kSizeOfByte][(kSizeOfByte - 1) - cur_bit % kSizeOfByte] = temp_message[i / kSizeOfByte][(kSizeOfByte - 1) - i % kSizeOfByte];
            ++cur_bit;
        }
    }
    std::vector<char> result;
    for (auto i: temp_res) {
        result.push_back(i.to_ulong());
    }
    return result;
}


