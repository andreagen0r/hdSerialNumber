#include <libudev.h>
#include <sys/stat.h>

#include <filesystem>
#include <vector>
#include <algorithm>

#include <fmt/core.h>
#include <fmt/ranges.h>

enum class DiskAttribute {
    Id_model,
    Id_serial,
    Id_serial_short,
};

std::string get_disk_attribute(const DiskAttribute attr) {
    switch (attr) {
    case DiskAttribute::Id_model:
        return "ID_MODEL";
    case DiskAttribute::Id_serial:
        return "ID_SERIAL";
    case DiskAttribute::Id_serial_short:
        return "ID_SERIAL_SHORT";
    }
}

// Utils
std::vector<std::filesystem::path> get_files_path(const std::filesystem::path& path, const std::string& start_with) {
    if (!std::filesystem::exists(path)) {
        return {};
    }

    std::vector<std::filesystem::path> disks;

    for (const auto & d : std::filesystem::directory_iterator(path)) {
        if (!d.path().filename().string().compare(0,start_with.size(), start_with, 0, start_with.size())) {
            disks.emplace_back(d.path());
        }
    }

    return disks;
}

// Todo
// Fazer o log usando spdlog
std::vector<std::filesystem::path> get_disk_attribute(const std::vector<std::filesystem::path>& disks, const DiskAttribute attr) {
    if (disks.empty()) {
        return {};
    }

    std::vector<std::filesystem::path> out;

    struct udev *ud = udev_new();
    struct stat statbuf;
    struct udev_device     *device  = nullptr;
    struct udev_list_entry *entry   = nullptr;

    for (const auto& disk : disks) {

        if (nullptr == ud) {
            fmt::print(stderr, "Failed to create udev.\n");
        } else {
            if (0 != stat(disk.string().data(), &statbuf)) {
                fmt::print(stderr, "Failed to stat {}\n", disk.string());
            } else {
                device = udev_device_new_from_devnum(ud, 'b', statbuf.st_rdev);

                if (nullptr == device) {
                    fmt::print(stderr, "No device found {}\n", disk.string());
                } else {
                    entry = udev_device_get_properties_list_entry(device);

                    while (nullptr != entry)  {
                        auto attribute = get_disk_attribute(attr);
                        if (attribute.compare(udev_list_entry_get_name(entry)) == 0) {
                            break;
                        }

                        entry = udev_list_entry_get_next(entry);
                    }

                    out.emplace_back(std::string{udev_list_entry_get_value(entry)});
                }
            }
        }
    }

    udev_device_unref(device);
    udev_unref(ud);

    return out;
}

int main()
{
    auto disks = get_files_path("/dev", "sd");
    auto attrs = get_disk_attribute(disks, DiskAttribute::Id_serial_short);

    std::vector<std::string> sn(attrs.begin(), attrs.end());
    sn.erase(std::unique(sn.begin(), sn.end()), sn.end());

    for (const auto& i : sn) {
        fmt::print("{}\n", i);
    }

    return 0;
}


