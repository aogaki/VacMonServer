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
class EnergyDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(EnergyDto, Object)

  DTO_FIELD(String, id);
  DTO_FIELD(String, mean);
  DTO_FIELD(String, fwhm);
  DTO_FIELD(String, time);
  DTO_FIELD(String, fit);
};

class EnergyListDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(EnergyListDto, Object)

  DTO_FIELD(String, id);
  DTO_FIELD(String, mean);
  DTO_FIELD(String, fwhm);
  DTO_FIELD(String, time);
};

class FluxDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(FluxDto, Object)

  DTO_FIELD(String, id);
  DTO_FIELD(String, count);
  DTO_FIELD(String, hz);
  DTO_FIELD(String, time);
};

class CalibrationDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(CalibrationDto, Object)

  DTO_FIELD(String, id);
  DTO_FIELD(String, time);
  DTO_FIELD(String, system);
  DTO_FIELD(String, detector);
  DTO_FIELD(String, HV);
  DTO_FIELD(String, p0);
  DTO_FIELD(String, p1);
  DTO_FIELD(String, RI);
  DTO_FIELD(String, peaks);
  DTO_FIELD(String, name);
};

class PositionDto : public oatpp::data::mapping::type::Object
{
  DTO_INIT(PositionDto, Object)

  DTO_FIELD(String, id);
  DTO_FIELD(String, img2D);
  DTO_FIELD(String, imgH);
  DTO_FIELD(String, imgV);
  DTO_FIELD(String, comment2D);
  DTO_FIELD(String, commentH);
  DTO_FIELD(String, commentV);
  DTO_FIELD(String, commentAll);
};

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
