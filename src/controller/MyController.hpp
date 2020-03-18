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
#include <fstream>
#include <iostream>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <string>

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

  const std::string fDirectory = "Monitor/";

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
        "<p>The monitor page</p>"
        "<a href='vacmon'>Monitor page</a>"
        "<div>"
        "<p>Web API for ELIADE</p>"
        "<a href='swagger/ui'>Checkout Swagger-UI page</a>"
        "</div>"
        "</body>"
        "</html>";
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

  // TODO Insert Your endpoints here !!!
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

  ENDPOINT("GET", "/vacmon", vacmonIndex)
  {
    auto fin = std::ifstream(fDirectory + "/index.html");
    std::string indexHTML{""};
    std::string buf{""};
    // while (fin >> buf) {
    while (std::getline(fin, buf)) {
      indexHTML += buf;
    }

    const String html(indexHTML.c_str());
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

  ENDPOINT("GET", "/vacmon/{path}", vacmon, PATH(String, path))
  {
    auto fileName = path->std_str();
    auto fin = std::ifstream(fDirectory + fileName);
    std::string indexHTML{""};
    std::string buf{""};
    // while (fin >> buf) {
    while (std::getline(fin, buf)) {
      indexHTML += buf;
    }
    const String html(indexHTML.c_str());

    auto response = createResponse(Status::CODE_200, html);
    if (fileName.find(".js") != std::string::npos) {
      response->putHeader(Header::CONTENT_TYPE, "application/javascript");
    } else if (fileName.find(".css") != std::string::npos) {
      response->putHeader(Header::CONTENT_TYPE, "text/css");
    }  // Favicon...

    return response;
  }

/**
 *  Finish ENDPOINTs generation ('ApiController' codegen)
 */
#include OATPP_CODEGEN_END(ApiController)
};

#endif /* MyController_hpp */