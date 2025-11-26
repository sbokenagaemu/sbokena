#include "utils.hh"

#include <filesystem>
#include <optional>
#include <span>

#include <nfd.h>
#include <nfd.hpp>

namespace fs = std::filesystem;

namespace sbokena::utils {

static constexpr nfdwindowhandle_t NULL_HANDLE = {
  .type   = NFD_WINDOW_HANDLE_TYPE_UNSET,
  .handle = nullptr,
};

std::optional<fs::path>
open_file_dialog(std::span<const nfdfilteritem_t> filter) {
  NFD::UniquePath path;

  const auto res = NFD::OpenDialog(
    path, filter.data(), filter.size(), nullptr, NULL_HANDLE
  );

  if (res != NFD_OKAY)
    return std::nullopt;

  fs::path path_ = path.get();
  return std::optional<fs::path>(path_);
}

std::optional<fs::path> open_folder_dialog() {
  NFD::UniquePath path;

  const auto res = NFD::PickFolder(path);

  if (res != NFD_OKAY)
    return std::nullopt;

  fs::path path_ = path.get();
  return std::optional<fs::path>(path_);
}

std::optional<fs::path>
save_file_dialog(std::span<const nfdfilteritem_t> filter) {
  NFD::UniquePath path;

  const auto res = NFD::SaveDialog(
    path,
    filter.data(),
    filter.size(),
    NULL,
    "untitiled.sb",
    NULL_HANDLE
  );
  if (res != NFD_OKAY)
    return std::nullopt;

  fs::path path_ = path.get();
  return std::optional<fs::path>(path_);
}

} // namespace sbokena::utils
