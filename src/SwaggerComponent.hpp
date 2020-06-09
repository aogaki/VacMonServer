//
//  SwaggerComponent.hpp
//  crud
//
//  Created by Leonid on 7/31/18.
//  Copyright © 2018 oatpp. All rights reserved.
//

#ifndef SwaggerComponent_hpp
#define SwaggerComponent_hpp

#include "oatpp-swagger/Model.hpp"
#include "oatpp-swagger/Resources.hpp"
#include "oatpp/core/macro/component.hpp"

/**
 *  Swagger ui is served at
 *  http://host:port/swagger/ui
 */
class SwaggerComponent
{
 public:
  /**
   *  General API docs info
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::DocumentInfo>,
                         swaggerDocumentInfo)
  ([] {
    oatpp::swagger::DocumentInfo::Builder builder;

    builder.setTitle("Web API for beam diagnosis")
        .setDescription("For Ge detectors")
        .setVersion("1.0")
        .setContactName("Sohichiroh Aogaki")
        .setContactUrl("https://github.com/aogaki")

        .setLicenseName("Apache License, Version 2.0")
        .setLicenseUrl("http://www.apache.org/licenses/LICENSE-2.0")

        // .addServer("http://172.18.7.22", "server on 172.18.7.22");
        .addServer("http://172.18.4.56:8080", "server on 172.18.4.56:8080");

    return builder.build();
  }());

  /**
   *  Swagger-Ui Resources (<oatpp-examples>/lib/oatpp-swagger/res)
   */
  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::swagger::Resources>,
                         swaggerResources)
  ([] {
    // Make sure to specify correct full path to oatpp-swagger/res folder !!!
    return oatpp::swagger::Resources::loadResources(OATPP_SWAGGER_RES_PATH);
  }());
};

#endif /* SwaggerComponent_hpp */
