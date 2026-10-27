#pragma once
#include <cstddef>
#include <filesystem>
#include <paths.hpp>
#include <string>
#include <vector>

using Lines = std::vector<std::string>;

enum class MergeResult { no_change, merge, conflict };

inline constexpr std::string ours_label   = "<<<  OURS  <<<";
inline constexpr std::string theirs_label = ">>> THEIRS >>>";
inline constexpr std::string end_label    = "==============";

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

std::vector<Edit> diff(const Lines& base, const Lines& side);
void              print_diff(const Lines& base, const Lines& side, std::string path);
MergeResult       merge3(const std::filesystem::path path, const Lines& base, const Lines& ours, const Lines& theirs);