#ifndef FOLDER_HPP
#define FOLDER_HPP

#include <vector>
#include <string>

struct Folder
{
    std::string s_name;
    std::vector<std::string> files;
    std::vector<Folder> child_folders;
    std::string s_role;

public:
    Folder(std::string s_name, std::string s_role);

    void add_file(std::string s_file_name);

    void add_child_folder(Folder child_folder);

    void add_child_folder(std::string s_child_folder_name, std::string s_role);


};

#endif // FOLDER_HPP

