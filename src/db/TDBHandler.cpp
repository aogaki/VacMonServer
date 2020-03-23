#include <TAxis.h>
#include <TBufferJSON.h>

#include <iostream>
#include <utility>

#include "TDBHandler.hpp"

using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

TDBHandler::TDBHandler()
    // : fEliadePool(mongocxx::uri("mongodb://daq:nim2camac@172.18.4.56/ELIADE"))
    : fEliadePool(mongocxx::uri("mongodb://172.18.7.22/ELIADE"))
{
  fGraph.reset(new TGraph);
  fGraph->SetTitle("Pressure monitor;Time;Pressure [mbar]");
  // fGraph->SetMaximum(5.e+2);
  // fGraph->SetMinimum(5.e-8);
  // fGraph->SetMinimum(-2.);
  fGraph->SetMarkerStyle(8);
  fGraph->SetMarkerColor(kRed);
  fGraph->GetXaxis()->SetTimeDisplay(1);
  fGraph->GetXaxis()->SetTimeFormat("%H:%M");

  fCanvas.reset(new TCanvas("canvas", "Pressure monitor"));
  // fCanvas->SetLogy(kTRUE);
}

List<VacMonDto::ObjectWrapper>::ObjectWrapper TDBHandler::GetVacMonList()
{
  auto conn = fEliadePool.acquire();
  auto collection = (*conn)["ELIADE"]["VacMon"];

  auto currentTime = time(nullptr);
  auto oneDay = 24 * 60 * 60;
  auto th = currentTime - oneDay;

  auto cursor = collection.find(document{} << "time" << open_document << "$gte"
                                           << th << close_document << finalize);

  auto result = List<VacMonDto::ObjectWrapper>::createShared();
  for (auto doc : cursor) {
    auto dto = VacMonDto::createShared();
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->time = std::to_string(doc["time"].get_int64().value).c_str();
    dto->pressure = std::to_string(doc["pressure"].get_double().value).c_str();

    result->pushBack(dto);
  }

  return result;
}

List<VacMonDto::ObjectWrapper>::ObjectWrapper TDBHandler::GetVacMonList(
    long start, long stop)
{
  auto conn = fEliadePool.acquire();
  auto collection = (*conn)["ELIADE"]["VacMon"];

  if (stop != 0 && start > stop) std::swap(start, stop);

  // bsoncxx::builder::stream::document option;
  auto option = document{} << "time" << open_document << "$gte" << start
                           << "$lte" << stop << close_document << finalize;

  if (stop == 0) {
    option = document{} << "time" << open_document << "$gte" << start
                        << close_document << finalize;
  } else if (start == 0 && stop == 0) {
    option = document{} << finalize;
  }

  auto cursor = collection.find({option});

  auto result = List<VacMonDto::ObjectWrapper>::createShared();
  for (auto doc : cursor) {
    auto dto = VacMonDto::createShared();
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->time = std::to_string(doc["time"].get_int64().value).c_str();
    dto->pressure = std::to_string(doc["pressure"].get_double().value).c_str();

    result->pushBack(dto);
  }

  return result;
}

VacMonGraphDto::ObjectWrapper TDBHandler::GetVacMonGraph(long start, long stop,
                                                         std::string name)
{
  auto conn = fEliadePool.acquire();
  auto collection = (*conn)["ELIADE"]["VacMon"];

  auto currentTime = time(nullptr);
  auto oneDay = 24 * 60 * 60;
  auto th = currentTime - oneDay;

  if (stop != 0 && start > stop) std::swap(start, stop);

  auto option = document{} << "time" << open_document << "$gte" << start
                           << "$lte" << stop << close_document << "name" << name
                           << finalize;
  if (stop == 0) {
    option = document{} << "time" << open_document << "$gte" << start
                        << close_document << "name" << name << finalize;
  } else if (start == 0 && stop == 0) {
    option = document{} << "name" << name << finalize;
  }
  auto cursor = collection.find({option});

  // TGraph is not thread safe
  std::lock_guard<oatpp::concurrency::SpinLock> lock(fLock);
  fGraph->Set(0);
  auto counter = 0;
  for (auto doc : cursor) {
    fGraph->SetPoint(counter++, doc["time"].get_int64().value,
                     doc["pressure"].get_double().value);
  }

  fGraph->Draw("ALP");
  auto grJSON = TBufferJSON::ToJSON(fCanvas.get());

  auto result = VacMonGraphDto::createShared();

  result->canvas = grJSON;

  return result;
}
