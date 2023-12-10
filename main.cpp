#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <limits>
#include <filesystem>
#include <vector>
using namespace std;
namespace fs = std::filesystem;

// Struct to represent a file
struct FileInfo {
    int number;
    string* filename;  // Using a pointer for string
    string path;
};

// Function prototypes
void displayMenu();
void newFile();
void readFile();
void editFile();
void deleteFile();
void listFiles();

int main() {
    displayMenu();
    return 0;
}

void displayMenu() {
    int choice;
    cout << endl;
    cout << "                    =========TEXT EDITOR=========                             " << endl;
    cout << "Selamat datang diprogram kami silahkan memilih opsi yang anda inginkan  " << endl;
    cout << endl;
    cout << "Menu: " << endl;
    cout << "1. New File" << endl;
    cout << "2. Read File" << endl;
    cout << "3. Edit File" << endl;
    cout << "4. Delete File" << endl;
    cout << "5. Exit" << endl;
    cout << "Enter your choice: ";

    while (!(cin >> choice)) {
        cout << "Invalid input. Please enter a number : ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    switch (choice) {
        case 1:
            newFile();
            displayMenu();
            break;
        case 2:
            listFiles();
            readFile();
            displayMenu();
            break;
        case 3:
            editFile();
            displayMenu();
            break;
        case 4:
            deleteFile();
            displayMenu();
            break;
        case 5:
            cout << "Exiting program." << endl;
            cout << "Terima kasih sudah mengunjungi kami." << endl;
            break;
        default:
            cout << "Invalid choice. Please enter a valid option." << endl;
            displayMenu();
    }
}

void newFile() {
    string* filename = new string;  // Dynamic memory allocation for string
    string text;
    ofstream file;

    cout << "Enter filename: ";
    cin >> *filename;  // Dereference the pointer to read the string value

    file.open(*filename);

    if (!file.is_open()) {
        cout << "Unable to open file." << endl;
        delete filename;  // Free memory if failed to open the file
        return;
    }

    cout << "Enter text (type 'exit' on a new line to exit and save): " << endl;
    cin.ignore();
    while (getline(cin, text)) {
        if (text == "exit") {
            break;
        }
        file << text << endl;
    }

    file.close();
    cout << "File saved." << endl;

    delete filename;  // Free memory after using the string
}

void readFile() {
    int fileNumber;
    cout << "Enter the number of the file to read: ";
    cin >> fileNumber;

    // Create a vector of FileInfo structs
    vector<FileInfo> files;

    int currentFileNumber = 1;  // To assign a number to each file

    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().filename().string().find(".exe") == std::string::npos) {
            FileInfo file;
            file.number = currentFileNumber++;
            file.filename = new string(entry.path().filename().string());  // Dynamic memory allocation for string
            file.path = entry.path().string();
            files.push_back(file);
        }
    }

    // Find the selected file by its number
    string* selectedFilename = nullptr;
    for (const auto& file : files) {
        if (file.number == fileNumber) {
            selectedFilename = file.filename;
            break;
        }
    }

    if (selectedFilename != nullptr) {
        // Read the selected file
        string text;
        ifstream readFile(*selectedFilename);

        if (!readFile.is_open()) {
            cout << "Unable to open file." << endl;
            delete selectedFilename;  // Free memory if failed to open the file
            return;
        }

        cout << "Contents of the file:" << endl;
        while (getline(readFile, text)) {
            cout << text << endl;
        }

        readFile.close();
        delete selectedFilename;  // Free memory after using the string
    } else {
        cout << "Invalid file number. Please enter a valid file number." << endl;
    }
}

void editFile() {
    listFiles();

    int fileNumber;
    cout << "Enter the number of the file to edit: ";
    cin >> fileNumber;

    // Create a vector of FileInfo structs
    //menyimpan informasi setiap file dalam bentuk objek FileInfo
    vector<FileInfo> files;

    int currentFileNumber = 1;  // To assign a number to each file

    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().filename().string().find(".exe") == std::string::npos) {
            FileInfo file;
            file.number = currentFileNumber++;
            file.filename = new string(entry.path().filename().string());  // Dynamic memory allocation for string
            file.path = entry.path().string();
            files.push_back(file);
        }
    }

    // Find the selected file by its number
    FileInfo* selectedFile = nullptr;
    for (auto& file : files) {
        if (file.number == fileNumber) {
            selectedFile = &file;
            break;
        }
    }

    if (selectedFile != nullptr) {
        // Display additional menu for editing options
        int editChoice;
        cout << "Choose what to edit for '" << *(selectedFile->filename) << "':" << endl;
        cout << "1. Edit Filename" << endl;
        cout << "2. Edit Content" << endl;
        cout << "Enter your choice: ";

        while (!(cin >> editChoice) || (editChoice != 1 && editChoice != 2)) {
            cout << "Invalid input. Please enter 1 or 2: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (editChoice == 1) {
            string newFilename;
            cout << "Enter new filename: ";
            cin >> newFilename;

            if (rename(selectedFile->filename->c_str(), newFilename.c_str()) != 0) {
                cout << "Unable to rename file." << endl;
            } else {
                cout << "File renamed." << endl;
                // Update the filename in the FileInfo struct
                delete selectedFile->filename;
                selectedFile->filename = new string(newFilename);
            }
        } else {
            // Edit the selected file content
            string text;
            //ios::trunc merupakan parameter untuk mode pembukaan file yang menunjukkan bahwa 
            //file harus dibuka dalam mode trunckate (memotong file ke ukuran nol) sebelum menulis.
            // Jadi, jika file sudah ada, isinya akan dihapus.
            ofstream file(selectedFile->path, ios::trunc);  // Truncate mode

            if (!file.is_open()) {
                cout << "Unable to open file." << endl;
                return;
            }

            cout << "Enter text to add (type 'exit' on a new line to exit and save):" << endl;
            cin.ignore();
            while (getline(cin, text)) {
                if (text == "exit") {
                    break;
                }
                file << text << endl;
            }

            file.close();
            cout << "File content edited and saved." << endl;
        }
    } else {
        cout << "Invalid file number. Please enter a valid file number." << endl;
    }

    // Free memory after using the string
    for (auto& file : files) {
        //Perintah delete digunakan untuk mengembalikan memori 
        //yang sebelumnya dialokasikan secara dinamis oleh operator new.
        delete file.filename;
    }
}


void deleteFile() {
    int fileNumber;
    listFiles();
    cout << "Enter the number of the file to delete: ";
    cin >> fileNumber;

    // Create a vector of FileInfo structs
    vector<FileInfo> files;

    int currentFileNumber = 1;  // To assign a number to each file

    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().filename().string().find(".exe") == std::string::npos) {
            FileInfo file;
            file.number = currentFileNumber++;
            file.filename = new string(entry.path().filename().string());  // Dynamic memory allocation for string
            file.path = entry.path().string();
            files.push_back(file);
        }
    }

    // Find the selected file by its number
    string* selectedFilename = nullptr;
    for (const auto& file : files) {
        if (file.number == fileNumber) {
            selectedFilename = file.filename;
            break;
        }
    }

    if (selectedFilename != nullptr) {
        // Delete the selected file
        if (remove(selectedFilename->c_str()) != 0) {
            cout << "Unable to delete file." << endl;
        } else {
            cout << "File deleted." << endl;
        }

        delete selectedFilename;  // Free memory after using the string
    } else {
        cout << "Invalid file number. Please enter a valid file number." << endl;
    }
}

void listFiles() {
    cout << "Available files in the current directory:\n";

    // Create a vector of FileInfo structs
    vector<FileInfo> files;

    int currentFileNumber = 1;  // To assign a number to each file

    for (const auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file() && entry.path().filename().string().find(".exe") == std::string::npos) {
            FileInfo file;
            file.number = currentFileNumber++;
            file.filename = new string(entry.path().filename().string());  // Dynamic memory allocation for string
            file.path = entry.path().string();
            files.push_back(file);
        }
    }

    // Display the list of files in a table with box borders
    const int columnWidths[] = {10, 25, 45};
    cout << setfill('-') << "+" << setw(columnWidths[0] + 2) << "+" << setw(columnWidths[1] + 2) << "+" << setw(columnWidths[2] + 2) << "+" << setfill(' ') << endl;
    cout << "| " << setw(columnWidths[0]) << left << "Number" << " | " << setw(columnWidths[1]) << left << "File Name" << " | " << setw(columnWidths[2]) << left << "Path" << " |" << endl;
    cout << setfill('-') << "+" << setw(columnWidths[0] + 2) << "+" << setw(columnWidths[1] + 2) << "+" << setw(columnWidths[2] + 2) << "+" << setfill(' ') << endl;

    for (const auto& file : files) {
        cout << "| " << setw(columnWidths[0]) << left << file.number << " | " << setw(columnWidths[1]) << left << *file.filename << " | " << setw(columnWidths[2]) << left << file.path << " |" << endl;
    }

    cout << setfill('-') << "+" << setw(columnWidths[0] + 2) << "+" << setw(columnWidths[1] + 2) << "+" << setw(columnWidths[2] + 2) << "+" << setfill(' ') << endl;
}
