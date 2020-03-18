//
//  MyDto.hpp
//  crud
//
//  Created by Leonid on 3/13/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class VacMonDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(VacMonDto, Object)

  DTO_FIELD(String, id);
  DTO_FIELD(String, time);
  DTO_FIELD(String, pressure);
};

class VacMonGraphDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(VacMonGraphDto, Object)

  DTO_FIELD(String, canvas);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */