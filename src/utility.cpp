#include "reflection/utility.h"

namespace lh::reflection::utility
{
    std::string signatureString(const std::vector<std::type_index> &arguments) noexcept
    {
        std::vector<std::string> names;
        names.reserve(arguments.size());
        std::ranges::transform(arguments, std::back_inserter(names),
                       [](const auto &argument) { return argument.name(); });

        return join(names, ", ");
    }

    std::size_t signatureHash(const std::vector<std::type_index> &arguments) noexcept
    {
        if (arguments.size() == 0)
            return 0;

        auto currentArgument = arguments.begin();

        size_t hash = (currentArgument++)->hash_code();
        for (; currentArgument < arguments.end(); ++currentArgument)
            hash ^= currentArgument->hash_code() + 0x9e3779b9 + (hash << 6) + (hash >> 2);

        return hash;
    }
}