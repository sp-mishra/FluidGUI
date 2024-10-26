#pragma once

#ifndef FILEUTILS_HPP
#define FILEUTILS_HPP

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <stdexcept>
#include <random>
#include "Log.hpp"

namespace fs = std::filesystem;

namespace groklab {
    struct FileUtils {
        static std::string readFileAsString(const std::string &filePath) {
            // Check if file exists else throw an exception
            if (!fileExists(filePath)) {
                const std::string msg = std::format("File does not exist: {}", filePath);
                error(msg);
                throw std::runtime_error(msg);
            }

            // Read the file content else throw an exception
            std::ifstream file(filePath);
            if (!file.is_open()) {
                const std::string msg = std::format("Could not open file: {}", filePath);
                throw std::runtime_error("Could not open file: " + filePath);
            }

            // Read the file content and convert it to a string
            std::stringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        static bool fileExists(const std::string &filePath) {
            std::ifstream file(filePath);
            return file.good();
        }

        static std::string generateRandomString(size_t length) {
            const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            std::random_device rd;
            std::mt19937 generator(rd());
            std::uniform_int_distribution<> dist(0, characters.size() - 1);

            std::string randomStr;
            for (size_t i = 0; i < length; ++i) {
                randomStr += characters[dist(generator)];
            }
            return randomStr;
        }

        static fs::path createTemporaryFolder() {
            // Get the system's temporary directory path
            fs::path tempDir = fs::temp_directory_path();

            // Generate a unique directory name
            const std::string uniqueFolderName = "temp_folder_" + generateRandomString(8);

            // Construct the full path for the temporary folder
            const fs::path tempFolderPath = tempDir / uniqueFolderName;

            // Create the directory
            if (create_directory(tempFolderPath)) {
                info("Temporary folder created: {}", tempFolderPath.string());
            } else {
                critical("Failed to create temporary folder.");
            }

            // Return the path to the temporary folder
            return tempFolderPath;
        }

    };
}

#endif //FILEUTILS_HPP
