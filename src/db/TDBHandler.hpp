#ifndef TDBHandler_hpp
#define TDBHandler_hpp 1

#include <TCanvas.h>
#include <TGraph.h>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/json.hpp>
#include <memory>
#include <mongocxx/client.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <string>

#include "dto/DTOs.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"

using oatpp::data::mapping::type::List;

class TDBHandler
{
 public:
  TDBHandler();
  ~TDBHandler(){};

  List<VacMonDto::ObjectWrapper>::ObjectWrapper GetVacMonList();
  List<VacMonDto::ObjectWrapper>::ObjectWrapper GetVacMonList(long start,
                                                              long stop);
  VacMonGraphDto::ObjectWrapper GetVacMonGraph(long start = 0, long stop = 0,
                                               std::string name = "PA1");
  StatusDto::ObjectWrapper GetStatus();

 private:
  oatpp::concurrency::SpinLock fLock;

  mongocxx::pool fEliadePool;

  std::unique_ptr<TGraph> fGraph;
  std::unique_ptr<TCanvas> fCanvas;
};

#endif
