#include <diff.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <utility>

static constexpr ptrdiff_t UNREACHABLE = std::numeric_limits<ptrdiff_t>::lowest() / 4;

static void append_match(std::vector<Match>& matches, size_t a, size_t b, size_t length) {
    if (length == 0)
        return;
    if (!matches.empty()) {
        Match& previous = matches.back();
        if (previous.a_begin + previous.length == a && previous.b_begin + previous.length == b) {
            previous.length += length;
            return;
        }
    }
    matches.push_back({a, b, length});
}

static Snake middle_snake(
    const Lines& a, ptrdiff_t a_begin, ptrdiff_t a_end, const Lines& b, ptrdiff_t b_begin, ptrdiff_t b_end,
    Scratch& scratch) {
    const ptrdiff_t n         = a_end - a_begin;
    const ptrdiff_t m         = b_end - b_begin;
    const ptrdiff_t delta     = n - m;
    const bool      odd_delta = (delta & 1) != 0;
    const ptrdiff_t max_d     = (n + m + 1) / 2;
    const ptrdiff_t offset    = max_d + 2;
    const size_t    required  = 2 * max_d + 5;
    scratch.prepare(required);
    std::fill_n(scratch.forward.begin(), required, UNREACHABLE);
    std::fill_n(scratch.reverse.begin(), required, UNREACHABLE);
    std::vector<ptrdiff_t>& forward = scratch.forward;
    std::vector<ptrdiff_t>& reverse = scratch.reverse;
    forward[1 + offset]             = 0;
    reverse[1 + offset]             = 0;
    for (ptrdiff_t d = 0; d <= max_d; d++) {
        for (ptrdiff_t k = -d; k <= d; k += 2) {
            ptrdiff_t snake_x0;
            if (k == -d || (k != d && forward[k - 1 + offset] < forward[k + 1 + offset])) {
                snake_x0 = forward[k + 1 + offset];
            } else {
                snake_x0 = forward[k - 1 + offset] + 1;
            }
            ptrdiff_t snake_y0 = snake_x0 - k;
            ptrdiff_t x        = snake_x0;
            ptrdiff_t y        = snake_y0;
            while (x < n && y < m && a[a_begin + x] == b[b_begin + y]) {
                x++;
                y++;
            }
            forward[k + offset] = x;
            if (odd_delta) {
                const ptrdiff_t kr = delta - k;
                if (kr >= -(d - 1) && kr <= d - 1 && reverse[kr + offset] != UNREACHABLE) {
                    const ptrdiff_t reverse_x = n - reverse[kr + offset];
                    if (x >= reverse_x) {
                        return {snake_x0, snake_y0, x, y};
                    }
                }
            }
        }
        for (ptrdiff_t k = -d; k <= d; k += 2) {
            ptrdiff_t snake_p0;
            if (k == -d || (k != d && reverse[k - 1 + offset] < reverse[k + 1 + offset])) {
                snake_p0 = reverse[k + 1 + offset];
            } else {
                snake_p0 = reverse[k - 1 + offset] + 1;
            }
            ptrdiff_t snake_q0 = snake_p0 - k;
            ptrdiff_t p        = snake_p0;
            ptrdiff_t q        = snake_q0;
            while (p < n && q < m && a[a_end - 1 - p] == b[b_end - 1 - q]) {
                p++;
                q++;
            }
            reverse[k + offset] = p;
            if (!odd_delta) {
                const ptrdiff_t kf = delta - k;
                if (kf >= -d && kf <= +d && forward[kf + offset] != UNREACHABLE) {
                    const ptrdiff_t reverse_x = n - p;
                    if (forward[kf + offset] >= reverse_x) {
                        return {n - p, m - q, n - snake_p0, m - snake_q0};
                    }
                }
            }
        }
    }
    return {};
}

static void find_matches(
    const Lines& a, size_t a_begin, size_t a_end, const Lines& b, size_t b_begin, size_t b_end,
    std::vector<Match>& matches, Scratch& scratch) {
    size_t prefix = 0;
    while (a_begin + prefix < a_end && b_begin + prefix < b_end && a[a_begin + prefix] == b[b_begin + prefix]) {
        prefix++;
    }
    append_match(matches, a_begin, b_begin, prefix);
    a_begin += prefix;
    b_begin += prefix;
    size_t suffix = 0;
    while (a_begin < a_end - suffix && b_begin < b_end - suffix && a[a_end - 1 - suffix] == b[b_end - 1 - suffix]) {
        suffix++;
    }
    a_end -= suffix;
    b_end -= suffix;
    const size_t n = a_end - a_begin;
    const size_t m = b_end - b_begin;
    if (n != 0 && m != 0) {
        if (n == 1) {
            std::vector<std::string>::const_iterator first = b.begin() + b_begin;
            std::vector<std::string>::const_iterator last  = b.begin() + b_end;
            std::vector<std::string>::const_iterator found = std::find(first, last, a[a_begin]);
            if (found != last) {
                append_match(matches, a_begin, static_cast<size_t>(found - b.begin()), 1);
            }
        } else if (m == 1) {
            std::vector<std::string>::const_iterator first = a.begin() + a_begin;
            std::vector<std::string>::const_iterator last  = a.begin() + a_end;
            std::vector<std::string>::const_iterator found = std::find(first, last, b[b_begin]);
            if (found != last) {
                append_match(matches, static_cast<size_t>(found - a.begin()), b_begin, 1);
            }
        } else {
            const Snake  snake = middle_snake(a, a_begin, a_end, b, b_begin, b_end, scratch);
            const size_t sx0   = a_begin + snake.x0;
            const size_t sy0   = b_begin + snake.y0;
            const size_t sx1   = a_begin + snake.x1;
            const size_t sy1   = b_begin + snake.y1;
            if (sx0 == a_begin && sy0 == b_begin && sx1 == a_begin && sy1 == b_begin) {
                find_matches(a, a_begin + 1, a_end, b, b_begin, b_end, matches, scratch);
            } else if (sx0 == a_end && sy0 == b_end && sx1 == a_end && sy1 == b_end) {
                find_matches(a, a_begin, a_end, b, b_begin, b_end - 1, matches, scratch);
            } else {
                find_matches(a, a_begin, sx0, b, b_begin, sy0, matches, scratch);

                append_match(matches, sx0, sy0, sx1 - sx0);

                find_matches(a, sx1, a_end, b, sy1, b_end, matches, scratch);
            }
        }
    }
    append_match(matches, a_end, b_end, suffix);
}

std::vector<Edit> diff(const Lines& base, const Lines& side) {
    Scratch            scratch;
    std::vector<Match> matches;
    matches.reserve(std::min(base.size(), side.size()));
    find_matches(base, 0, base.size(), side, 0, side.size(), matches, scratch);
    std::vector<Edit> edits;
    size_t            base_pos = 0;
    size_t            side_pos = 0;
    for (const Match& match : matches) {
        if (base_pos != match.a_begin || side_pos != match.b_begin) {
            edits.push_back({base_pos, match.a_begin, side_pos, match.b_begin});
        }
        base_pos = match.a_begin + match.length;
        side_pos = match.b_begin + match.length;
    }
    if (base_pos != base.size() || side_pos != side.size()) {
        edits.push_back({base_pos, base.size(), side_pos, side.size()});
    }
    return edits;
}

void print_diff(const Lines& base, const Lines& side, std::string path) {
    const std::vector<Edit> edits = diff(base, side);

    if (edits.empty())
        return;

    std::cout << path << '\n';

    for (size_t i = 0; i < edits.size(); ++i) {
        const Edit&  edit       = edits[i];
        const size_t base_count = edit.base_end - edit.base_begin;
        const size_t side_count = edit.side_end - edit.side_begin;

        std::cout << ours_label << '\n';
        for (size_t line = edit.base_begin; line < edit.base_end; ++line)
            std::cout << "- " << (line + 1) << ": " << base[line] << '\n';
        std::cout << theirs_label << '\n';
        for (size_t line = edit.side_begin; line < edit.side_end; ++line)
            std::cout << "+ " << (line + 1) << ": " << side[line] << '\n';
    }
}

static Lines materialize_region(
    const Lines& base, const Lines& side, const std::vector<Edit>& edits, size_t edit_begin, size_t edit_end,
    size_t base_begin, size_t base_end) {
    Lines  result;
    size_t position = base_begin;
    for (size_t i = edit_begin; i < edit_end; i++) {
        const Edit& edit = edits[i];
        if (position < edit.base_begin) {
            result.insert(result.end(), base.begin() + position, base.begin() + edit.base_begin);
        }
        result.insert(
            result.end(), side.begin() + static_cast<std::ptrdiff_t>(edit.side_begin),
            side.begin() + static_cast<std::ptrdiff_t>(edit.side_end));

        position = edit.base_end;
    }
    if (position < base_end) {
        result.insert(result.end(), base.begin() + position, base.begin() + base_end);
    }
    return result;
}

static bool interacts(const Edit& a, const Edit& b) {
    const bool a_is_insertion = a.base_begin == a.base_end;
    const bool b_is_insertion = b.base_begin == b.base_end;
    if (a_is_insertion && b_is_insertion)
        return a.base_begin == b.base_begin;
    if (a_is_insertion)
        return b.base_begin < a.base_begin && a.base_begin < b.base_end;
    if (b_is_insertion)
        return a.base_begin < b.base_begin && b.base_begin < a.base_end;
    return std::max(a.base_begin, b.base_begin) < std::min(a.base_end, b.base_end);
}

static void emit_unchanged_base(const size_t& until, size_t& base_position, const Lines& base, std::ofstream& out) {
    if (base_position < until)
        for (size_t i = base_position; i < until; ++i)
            out << base[i] << '\n';
    base_position = until;
}

bool merge3(const std::filesystem::path path, const Lines& base, const Lines& ours, const Lines& theirs) {
    const std::vector<Edit> ours_edits   = diff(base, ours);
    const std::vector<Edit> theirs_edits = diff(base, theirs);
    bool                    conflict     = false;
    std::ofstream           out(path, std::ios::binary | std::ios::trunc | std::ios::out);
    size_t                  oi            = 0;
    size_t                  ti            = 0;
    size_t                  base_position = 0;
    while (oi < ours_edits.size() || ti < theirs_edits.size()) {
        const bool ours_starts_first =
            ti == theirs_edits.size() ||
            (oi < ours_edits.size() && ours_edits[oi].base_begin < theirs_edits[ti].base_begin);
        if (ours_starts_first && (ti == theirs_edits.size() || !interacts(ours_edits[oi], theirs_edits[ti]))) {
            const Edit& edit = ours_edits[oi];
            emit_unchanged_base(edit.base_begin, base_position, base, out);
            for (size_t i = edit.side_begin; i < edit.side_end; i++)
                out << ours[i] << '\n';
            base_position = edit.base_end;
            oi++;
            continue;
        }
        const bool theirs_starts_first =
            oi == ours_edits.size() ||
            (ti < theirs_edits.size() && theirs_edits[ti].base_begin < ours_edits[oi].base_begin);
        if (theirs_starts_first && (oi == ours_edits.size() || !interacts(ours_edits[oi], theirs_edits[ti]))) {
            const Edit& edit = theirs_edits[ti];
            emit_unchanged_base(edit.base_begin, base_position, base, out);
            for (size_t i = edit.side_begin; i < edit.side_end; i++)
                out << theirs[i] << '\n';
            base_position = edit.base_end;
            ti++;
            continue;
        }
        if (oi < ours_edits.size() && ti < theirs_edits.size() && !interacts(ours_edits[oi], theirs_edits[ti])) {
            if (ours_edits[oi].base_begin == ours_edits[oi].base_end) {
                const Edit& edit = ours_edits[oi++];
                emit_unchanged_base(edit.base_begin, base_position, base, out);
                for (size_t i = edit.side_begin; i < edit.side_end; i++)
                    out << ours[i] << '\n';
            } else {
                const Edit& edit = theirs_edits[ti++];
                emit_unchanged_base(edit.base_begin, base_position, base, out);
                for (size_t i = edit.side_begin; i < edit.side_end; i++)
                    out << theirs[i] << '\n';
            }
            continue;
        }
        const size_t ours_begin   = oi;
        const size_t theirs_begin = ti;
        size_t       region_begin = std::min(ours_edits[oi].base_begin, theirs_edits[ti].base_begin);
        size_t       region_end   = std::max(ours_edits[oi].base_end, theirs_edits[ti].base_end);
        oi++;
        ti++;
        bool expanded = true;
        while (expanded) {
            expanded = false;
            while (oi < ours_edits.size()) {
                const Edit region{region_begin, region_end, 0, 0};
                if (!interacts(region, ours_edits[oi]))
                    break;
                region_end = std::max(region_end, ours_edits[oi].base_end);
                oi++;
                expanded = true;
            }
            while (ti < theirs_edits.size()) {
                const Edit region{region_begin, region_end, 0, 0};
                if (!interacts(region, theirs_edits[ti]))
                    break;
                region_end = std::max(region_end, theirs_edits[ti].base_end);
                ti++;
                expanded = true;
            }
        }
        emit_unchanged_base(region_begin, base_position, base, out);
        Lines ours_version = materialize_region(base, ours, ours_edits, ours_begin, oi, region_begin, region_end);
        Lines theirs_version =
            materialize_region(base, theirs, theirs_edits, theirs_begin, ti, region_begin, region_end);
        if (ours_version == theirs_version) {
            for (std::string& line : ours_version)
                out << line << '\n';
        } else {
            conflict = true;
            out << ours_label << '\n';
            for (std::string& line : ours_version)
                out << line << '\n';
            out << theirs_label << '\n';
            for (std::string& line : theirs_version)
                out << line << '\n';
            out << end_label << '\n';
        }
        base_position = region_end;
    }
    emit_unchanged_base(base.size(), base_position, base, out);
    out.close();
    return conflict;
}