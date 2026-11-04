#include <paths.hpp>

void Paths::move_pending(const Pending& entry) { pending.emplace_back(std::move(entry)); }