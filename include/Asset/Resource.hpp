//
// Created by Dániel Molnár on 2019-09-27.
//

#ifndef VULKANENGINE_RESOURCE_HPP
#define VULKANENGINE_RESOURCE_HPP

namespace Asset {
using ID = unsigned int;

class Resource {
   private:
    const ID _id;

   public:
    explicit Resource(ID id) : _id(id) {}
    virtual ~Resource() = 0;

    [[nodiscard]] ID id() const { return _id; }
};

}  // namespace Asset

#endif  // VULKANENGINE_RESOURCE_HPP
