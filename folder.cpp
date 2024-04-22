#include "folder.hpp"

Folder::Folder(std::string s_name, std::string s_role) {
    this->s_name = s_name;
    this->s_role = s_role;
    this->files = std::vector<std::string>();
    this->child_folders = std::vector<Folder>();
}

void Folder::add_file(std::string s_file_name) {
    this->files.push_back(s_file_name);
}

void Folder::add_child_folder(Folder child_folder) {
    this->child_folders.push_back(child_folder);
}

void Folder::add_child_folder(std::string s_child_folder_name, std::string s_role) {
    Folder new_folder(s_child_folder_name, s_role);
    this->child_folders.push_back(new_folder);
}
