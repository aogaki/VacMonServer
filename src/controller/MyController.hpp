//
//  MyController.hpp
//  web-starter-project
//
//  Created by Leonid on 2/12/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef MyController_hpp
#define MyController_hpp

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "db/TDBHandler.hpp"
#include "dto/DTOs.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;
/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController
{
 public:
  /**
   * Constructor with object mapper.
   * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
   */
  MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
      : oatpp::web::server::api::ApiController(objectMapper)
  {
  }

 private:
  OATPP_COMPONENT(std::shared_ptr<TDBHandler>, database);

  oatpp::concurrency::SpinLock fMutex;

 public:
/**
 *  Begin ENDPOINTs generation ('ApiController' codegen)
 */
#include OATPP_CODEGEN_BEGIN(ApiController)

  ENDPOINT("GET", "/", root)
  {
    const String html =
        "<html lang='en'>"
        "<head>"
        "<meta charset=utf-8/>"
        "</head>"
        "<body>"
        "<p>Web API for GBS diagnostics</p>"
        "<a href='swagger/ui'>Checkout Swagger-UI page</a>"
        "</body>"
        "</html>";
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

  // TODO Insert Your endpoints here !!!
  ENDPOINT_INFO(getEnergy)
  {
    info->summary = "Get the current/last Energy information";
    info->addResponse<EnergyDto::ObjectWrapper>(Status::CODE_200,
                                                "application/json");
  }
  ADD_CORS(getEnergy)
  ENDPOINT("GET", "/GBS/GetEnergy", getEnergy)
  {
    auto dto = database->GetEnergy();
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getEnergyList)
  {
    info->summary = "Get the list of Energy information";
    info->addResponse<List<EnergyDto::ObjectWrapper>::ObjectWrapper>(
        Status::CODE_200, "application/json");
  }
  ADD_CORS(getEnergyList)
  ENDPOINT("GET", "/GBS/GetEnergyList", getEnergyList)
  {
    auto dto = database->GetEnergyList();
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getFluxList)
  {
    info->summary = "Get the list of Flux information";
    info->addResponse<List<FluxDto::ObjectWrapper>::ObjectWrapper>(
        Status::CODE_200, "application/json");
  }
  ADD_CORS(getFluxList)
  ENDPOINT("GET", "/GBS/GetFluxList", getFluxList)
  {
    auto dto = database->GetFluxList();
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getPosition)
  {
    info->summary = "Get the current/last Position information";
    info->addResponse<PositionDto::ObjectWrapper>(Status::CODE_200,
                                                  "application/json");
  }
  ADD_CORS(getPosition)
  ENDPOINT("GET", "/GBS/GetPosition", getPosition)
  {
    auto dto = database->GetPosition();
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(postPosition)
  {
    info->summary = "Post the current/last Position information";
    info->addConsumes<PositionDto::ObjectWrapper>("application/json");
    info->addResponse<PositionDto::ObjectWrapper>(Status::CODE_200,
                                                  "application/json");
  }
  ADD_CORS(postPosition)
  ENDPOINT("POST", "/GBS/PostPosition", postPosition,
           BODY_DTO(PositionDto::ObjectWrapper, dto))
  {
    auto echo = database->PostPosition(dto);
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(postCalibration)
  {
    info->summary = "Post the current/last Calibration results";
    info->addConsumes<CalibrationDto::ObjectWrapper>("application/json");
    info->addResponse<CalibrationDto::ObjectWrapper>(Status::CODE_200,
                                                     "application/json");
  }
  ADD_CORS(postCalibration)
  ENDPOINT("POST", "/GBS/PostCalibration", postCalibration,
           BODY_DTO(CalibrationDto::ObjectWrapper, dto))
  {
    auto echo = database->PostCalibration(dto);
    auto response = createDtoResponse(Status::CODE_200, echo);
    return response;
  }

  ENDPOINT_INFO(getVacMonList)
  {
    info->summary = "Get the list of VacMon information";
    info->addResponse<List<VacMonDto::ObjectWrapper>::ObjectWrapper>(
        Status::CODE_200, "application/json");
  }
  ADD_CORS(getVacMonList)
  ENDPOINT("GET", "/ELIADE/GetVacMonList", getVacMonList)
  {
    auto dto = database->GetVacMonList();
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }

  ENDPOINT_INFO(getVacMonGraph)
  {
    info->summary = "Get the graph of VacMon information as JSON";
    info->addResponse<VacMonGraphDto::ObjectWrapper>(Status::CODE_200,
                                                     "application/json");

    info->queryParams.add<String>("start").description =
        "Start time of the graph in UNIX time, NOT JS Date::getTime()";
    info->queryParams["start"].required = false;
    info->queryParams.add<String>("stop").description =
        "End time of the graph in UNIX time, NOT JS Date::getTime()";
    info->queryParams["stop"].required = false;
  }
  ADD_CORS(getVacMonGraph)
  ENDPOINT("GET", "/ELIADE/GetVacMonGraph", getVacMonGraph,
           REQUEST(std::shared_ptr<IncomingRequest>, request))
  {
    std::lock_guard<oatpp::concurrency::SpinLock> lock(fMutex);
    auto start = request->getQueryParameter("start", "0");
    auto stop = request->getQueryParameter("stop", "0");
    // fMutex.unlock();

    auto dto = database->GetVacMonGraph(std::stol(start->std_str()),
                                        std::stol(stop->std_str()));
    auto response = createDtoResponse(Status::CODE_200, dto);
    return response;
  }
/**
 *  Finish ENDPOINTs generation ('ApiController' codegen)
 */
#include OATPP_CODEGEN_END(ApiController)
};

#endif /* MyController_hpp */
