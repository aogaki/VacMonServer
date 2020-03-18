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
    : fPool(mongocxx::uri("mongodb://daq:nim2camac@172.18.4.56/GBS")),
      fEliadePool(mongocxx::uri("mongodb://daq:nim2camac@172.18.4.56/ELIADE"))
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

EnergyDto::ObjectWrapper TDBHandler::GetEnergy()
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Energy"];

  // cursor has only one data.  opts.limit(1) means take one data from DB
  // order shows the sorting parameter (sort by time, -1 means reverse order)
  auto opts = mongocxx::options::find{};
  opts.limit(1);
  auto order = document{} << "time" << -1 << finalize;
  opts.sort(order.view());
  auto cursor = collection.find({}, opts);

  auto dto = EnergyDto::createShared();
  for (auto doc : cursor) {
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->mean = doc["mean"].get_utf8().value.to_string().c_str();
    dto->fwhm = doc["fwhm"].get_utf8().value.to_string().c_str();
    dto->time = doc["time"].get_utf8().value.to_string().c_str();
    dto->fit = doc["fit"].get_utf8().value.to_string().c_str();
  }

  return dto;
}

List<EnergyDto::ObjectWrapper>::ObjectWrapper TDBHandler::GetEnergyList()
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Energy"];
  auto cursor = collection.find({});

  auto result = List<EnergyDto::ObjectWrapper>::createShared();
  for (auto doc : cursor) {
    auto dto = EnergyDto::createShared();
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->mean = doc["mean"].get_utf8().value.to_string().c_str();
    dto->fwhm = doc["fwhm"].get_utf8().value.to_string().c_str();
    dto->time = doc["time"].get_utf8().value.to_string().c_str();
    // dto->fit = doc["fit"].get_utf8().value.to_string().c_str();
    dto->fit = "";
    result->pushBack(dto);
  }

  return result;
}

List<FluxDto::ObjectWrapper>::ObjectWrapper TDBHandler::GetFluxList()
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Flux"];
  auto cursor = collection.find({});

  auto result = List<FluxDto::ObjectWrapper>::createShared();
  for (auto doc : cursor) {
    auto dto = FluxDto::createShared();
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->count = doc["count"].get_utf8().value.to_string().c_str();
    dto->hz = doc["hz"].get_utf8().value.to_string().c_str();
    dto->time = doc["time"].get_utf8().value.to_string().c_str();

    result->pushBack(dto);
  }

  return result;
}

PositionDto::ObjectWrapper TDBHandler::GetPosition()
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Position"];

  // cursor has only one data.  opts.limit(1) means take one data from DB
  // order shows the sorting parameter (sort by time, -1 means reverse order)
  auto opts = mongocxx::options::find{};
  opts.limit(1);
  auto order = document{} << "time" << -1 << finalize;
  opts.sort(order.view());
  auto cursor = collection.find({}, opts);

  auto dto = PositionDto::createShared();
  for (auto doc : cursor) {
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->img2D = doc["img2D"].get_utf8().value.to_string().c_str();
    dto->comment2D = doc["comment2D"].get_utf8().value.to_string().c_str();
    dto->imgH = doc["imgH"].get_utf8().value.to_string().c_str();
    dto->commentH = doc["commentH"].get_utf8().value.to_string().c_str();
    dto->imgV = doc["imgV"].get_utf8().value.to_string().c_str();
    dto->commentV = doc["commentV"].get_utf8().value.to_string().c_str();
    dto->commentAll = doc["commentAll"].get_utf8().value.to_string().c_str();
  }

  return dto;
}

PositionDto::ObjectWrapper TDBHandler::PostPosition(
    PositionDto::ObjectWrapper dto)
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Position"];

  bsoncxx::builder::stream::document buf{};
  buf << "img2D" << dto->img2D->std_str() << "comment2D"
      << dto->comment2D->std_str() << "imgH" << dto->imgH->std_str()
      << "commentH" << dto->commentH->std_str() << "imgV"
      << dto->imgV->std_str() << "commentV" << dto->commentV->std_str()
      << "commentAll" << dto->commentAll->std_str() << "time"
      << std::to_string(time(0));
  collection.insert_one(buf.view());
  buf.clear();

  // echo response.  but image is empty to reduce the trafic
  dto->img2D = "";
  dto->imgH = "";
  dto->imgV = "";

  return dto;
}

CalibrationDto::ObjectWrapper TDBHandler::PostCalibration(
    CalibrationDto::ObjectWrapper dto)
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Calibration"];

  bsoncxx::builder::stream::document buf{};
  buf << "time" << dto->time->std_str() << "system" << dto->system->std_str()
      << "detector" << dto->detector->std_str() << "HV" << dto->HV->std_str()
      << "p0" << dto->p0->std_str() << "p1" << dto->p1->std_str() << "RI"
      << dto->RI->std_str() << "peaks" << dto->peaks->std_str() << "name"
      << dto->name->std_str();
  collection.insert_one(buf.view());
  buf.clear();

  return dto;
}

List<CalibrationDto::ObjectWrapper>::ObjectWrapper
TDBHandler::GetCalibrationList()
{
  auto conn = fPool.acquire();
  auto collection = (*conn)["GBS"]["Calibration"];
  auto cursor = collection.find({});

  auto result = List<CalibrationDto::ObjectWrapper>::createShared();
  for (auto doc : cursor) {
    auto dto = CalibrationDto::createShared();
    dto->id = doc["_id"].get_oid().value.to_string().c_str();
    dto->time = doc["time"].get_utf8().value.to_string().c_str();
    dto->system = doc["system"].get_utf8().value.to_string().c_str();
    dto->detector = doc["detector"].get_utf8().value.to_string().c_str();
    dto->HV = doc["HV"].get_utf8().value.to_string().c_str();
    dto->p0 = doc["p0"].get_utf8().value.to_string().c_str();
    dto->p1 = doc["p1"].get_utf8().value.to_string().c_str();
    dto->RI = doc["RI"].get_utf8().value.to_string().c_str();
    dto->peaks = doc["peaks"].get_utf8().value.to_string().c_str();
    dto->name = doc["name"].get_utf8().value.to_string().c_str();

    result->pushBack(dto);
  }

  return result;
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

VacMonGraphDto::ObjectWrapper TDBHandler::GetVacMonGraph(long start, long stop)
{
  auto conn = fEliadePool.acquire();
  auto collection = (*conn)["ELIADE"]["VacMon"];

  auto currentTime = time(nullptr);
  auto oneDay = 24 * 60 * 60;
  auto th = currentTime - oneDay;

  if (stop != 0 && start > stop) std::swap(start, stop);

  auto option = document{} << "time" << open_document << "$gte" << start
                           << "$lte" << stop << close_document << finalize;
  if (stop == 0) {
    option = document{} << "time" << open_document << "$gte" << start
                        << close_document << finalize;
  } else if (start == 0 && stop == 0) {
    option = document{} << finalize;
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
