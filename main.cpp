#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <curl/curl.h>

namespace fs = std::filesystem;

struct Version {
    std::string number;
    std::string date; 
    std::string url;
};

struct Package {
    std::string name;
    std::vector<Version> versions;
};

class Repo {
public:
    std::vector<Package> packages;

    std::string resolveVersion(const std::string& packageName, const std::string& date) {
        for (auto& pkg : packages) {
            if (pkg.name == packageName) {
                std::string bestVersion = "";
                for (auto& ver : pkg.versions) {
                    if (ver.date <= date) {
                        bestVersion = ver.number;
                    }
                }
                if (!bestVersion.empty())
                    return bestVersion;
                else
                    return "Keine Version verfügbar";
            }
        }
        return "Paket nicht gefunden";
    }

    std::string getVersionURL(const std::string& packageName, const std::string& versionNumber) {
        for (auto& pkg : packages) {
            if (pkg.name == packageName) {
                for (auto& ver : pkg.versions)
                    if (ver.number == versionNumber)
                        return ver.url;
            }
        }
        return "";
    }
};

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::ofstream* out = static_cast<std::ofstream*>(stream);
    size_t total = size * nmemb;
    out->write(static_cast<char*>(ptr), total);
    return total;
}

bool downloadFile(const std::string& url, const std::string& outPath) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::ofstream ofs(outPath, std::ios::binary);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    ofs.close();

    return (res == CURLE_OK);
}

void installPackage(const std::string& name, const std::string& version, const std::string& url) {
    std::string filename = name + "-" + version + ".pipe";
    std::cout << "Lade Paket herunter: " << url << std::endl;

    if (!downloadFile(url, filename)) {
        std::cout << "Download fehlgeschlagen!\n";
        return;
    }

    std::cout << "Installiere Paket..." << std::endl;
    fs::path installPath = fs::current_path() / name;
    fs::create_directories(installPath);
    fs::copy_file(filename, installPath / filename, fs::copy_options::overwrite_existing);

    std::cout << "Installation abgeschlossen: " << installPath << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage:\n";
        std::cout << "  pipey install <package> [YYYY-MM-DD]\n";
        return 0;
    }

    std::string cmd = argv[1];
    std::string packageName = argv[2];
    std::string date = ""; 
    if (argc >= 4) date = argv[3];

    Repo repo;
    Package p;
    p.name = "meinprog";
    p.versions = {
        {"1.0.0", "2024-01-01", "https://raw.githubusercontent.com/deinuser/pipey-demo/main/meinprog-1.0.0.pipe"},
        {"1.1.0", "2024-03-01", "https://raw.githubusercontent.com/deinuser/pipey-demo/main/meinprog-1.1.0.pipe"},
        {"1.2.0", "2024-06-01", "https://raw.githubusercontent.com/deinuser/pipey-demo/main/meinprog-1.2.0.pipe"}
    };
    repo.packages.push_back(p);

    if (cmd == "install") {
        std::string version;
        if (date.empty()) {
            version = p.versions.back().number;
        } else {
            version = repo.resolveVersion(packageName, date);
        }

        std::string url = repo.getVersionURL(packageName, version);
        if (url.empty()) {
            std::cout << "URL für Version nicht gefunden!\n";
            return 0;
        }

        std::cout << "Installiere Paket " << packageName << " Version " << version << std::endl;
        installPackage(packageName, version, url);
    }

    return 0;
}
