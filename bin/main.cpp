#include "iostream"
#include "vector"
#include "../lib/parser/ArgParser.h"
#include "../lib/ham_file//Archive.h"

struct Options {
    bool create;
    bool lst;
    bool extract;
    bool append;
    bool del;
    bool concatenate;
    std::string archive;
    std::vector<std::string> files;
};


int main (int argc, char** argv) {
    Options opt;
    bool temp;
    ArgumentParser::ArgParser parser("name");
    parser.AddFlag('c', "create").StoreValue(opt.create).Default(false);
    parser.AddFlag('a', "append").StoreValue(opt.append).Default(false);
    parser.AddFlag('d', "delete").StoreValue(opt.del).Default(false);
    parser.AddFlag('A', "concatenate").StoreValue(opt.concatenate).Default(false);
    parser.AddFlag('x', "extract").StoreValue(opt.extract).Default(false);
    parser.AddFlag('l', "list").StoreValue(opt.lst).Default(false);
    parser.AddStringArgument('f', "file").StoreValue(opt.archive);
    parser.AddStringArgument("files").StoreValues(opt.files).MultiValue().Positional().Default("");
    if (parser.Parse(argc, argv)) {
        if (!opt.archive.empty()) {
            Archive arc(opt.archive);
            if (opt.create) {
                arc.Create();
                if (opt.files[0] != "") {
                    for (auto i: opt.files) {;
                        arc.AppendFile(i);
                    }
                }
            }
            else if (opt.append) {
                if (opt.files[0] != "") {
                    for (auto i: opt.files) {
                        arc.AppendFile(i);
                    }
                }
            }
            else if (opt.del) {
                if (opt.files[0] != "") {
                    for (auto i: opt.files) {
                        arc.DeleteFile(i);
                    }
                }
            }
            else if (opt.lst) {
                arc.GetListOfFiles();
            }
            else if (opt.concatenate) {
                arc.Concatenate(opt.files[0], opt.files[1]);
            }
            else if (opt.extract) {
                if (opt.files[0] != "") {
                    for (auto i: opt.files) {
                        arc.ExtractFile(i);
                    }
                }
                else {
                    arc.ExtractAllFiles();
                }
            }
            else {
                std::cout << "NO OPTION PROVIDED";
            }
        }
        else {
            std::cout << "There is no archive provided";
            return EXIT_FAILURE;
        }
    }
    else {
        std::cout << "ERROR";
    }
//    std::cout << parser.GetFlag("create") << ' ' << opt.append << ' ' << opt.del << ' ' << opt.lst << ' ' << opt.extract <<  ' ' << opt.archive << '\n';

    return EXIT_SUCCESS;
}

