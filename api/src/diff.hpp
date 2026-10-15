#pragma once
#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

using Lines = std::vector<std::string>;

inline constexpr std::string ours_label   = "<<< OURS  <<<";
inline constexpr std::string theirs_label = ">>> THEIRS >>>";

struct Edit {
    size_t base_begin, base_end;
    size_t side_begin, side_end;
};

struct Match {
    size_t a_begin;
    size_t b_begin;
    size_t length;
};

struct Snake {
    ptrdiff_t x0, y0;
    ptrdiff_t x1, y1;
};

struct Scratch {
    std::vector<ptrdiff_t> forward, reverse;

    void prepare(size_t n) {
        if (forward.size() < n) {
            forward.resize(n);
            reverse.resize(n);
        }
    }
};

struct Conflict {
    size_t begin, end;
    Lines  ours, theirs;
};

struct MergeResult {
    Lines                 lines;
    std::vector<Conflict> conflicts;
    std::filesystem::path path;
};

std::vector<Edit> diff(const Lines& base, const Lines& side);
void              print_diff(const Lines& base, const Lines& side, std::string path);
MergeResult       merge(const Lines& base, const Lines& ours, const Lines& theirs, const std::filesystem::path& path);