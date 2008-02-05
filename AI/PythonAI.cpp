#include "PythonAI.h"
#include "../util/AppInterface.h"
#include "../util/Directories.h"

#include "../Empire/Empire.h"

#include "../universe/Universe.h"
#include "../universe/UniverseObject.h"
#include "../universe/Fleet.h"
#include "../universe/Ship.h"
#include "../universe/Building.h"
#include "../universe/ResourceCenter.h"
#include "../universe/PopCenter.h"
#include "../universe/Planet.h"
#include "../universe/System.h"
#include "../universe/Special.h"

#include "../universe/Enums.h"

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

using boost::python::def;
using boost::python::return_value_policy;
using boost::python::copy_const_reference;
using boost::python::reference_existing_object;
using boost::python::return_by_value;
using boost::python::class_;
using boost::python::bases;
using boost::noncopyable;
using boost::python::no_init;
using boost::python::enum_;
using boost::python::vector_indexing_suite;
using boost::python::map_indexing_suite;

////////////////////////
// Python AIInterface //
////////////////////////

// disambiguation of overloaded functions
const std::string&      (*AIIntPlayerNameVoid)(void) =          &AIInterface::PlayerName;
const std::string&      (*AIIntPlayerNameInt)(int) =            &AIInterface::PlayerName;

const Empire*           (*AIIntGetEmpireVoid)(void) =           &AIInterface::GetEmpire;
const Empire*           (*AIIntGetEmpireInt)(int) =             &AIInterface::GetEmpire;

const UniverseObject*   (Universe::*UniverseGetObject)(int) =   &Universe::Object;
const Fleet*            (Universe::*UniverseGetFleet)(int) =    &Universe::Object;
const Ship*             (Universe::*UniverseGetShip)(int) =     &Universe::Object;
const Planet*           (Universe::*UniverseGetPlanet)(int) =   &Universe::Object;
const System*           (Universe::*UniverseGetSystem)(int) =   &Universe::Object;
const Building*         (Universe::*UniverseGetBuilding)(int) = &Universe::Object;

// convertion of STL container types to those which can be easily exposed to Python using built-in Boost-Python containers
template <typename T> std::vector<T> SetToVector(const std::set<T>& set);
template <typename T> std::vector<T> ListToVector(const std::set<T>& set);

std::vector<int>            (*IntSetToIntVector)(const std::set<int>&) =                &SetToVector;
std::vector<std::string>    (*StringSetToStringVector)(const std::set<std::string>&) =  &SetToVector;


// Expose AIInterface and all associated classes to Python
BOOST_PYTHON_MODULE(FreeOrionAIInterface)
{
    ///////////////////
    //  AIInterface  //
    ///////////////////
    def("PlayerName",               AIIntPlayerNameVoid,        return_value_policy<copy_const_reference>());
    def("PlayerName",               AIIntPlayerNameInt,         return_value_policy<copy_const_reference>());


    def("PlayerID",                 AIInterface::PlayerID);
    def("EmpirePlayerID",           AIInterface::EmpirePlayerID);
    def("AllPlayerIDs",             AIInterface::AllPlayerIDs,  return_value_policy<return_by_value>());

    def("PlayerIsAI",               AIInterface::PlayerIsAI);
    def("PlayerIsHost",             AIInterface::PlayerIsHost);

    def("EmpireID",                 AIInterface::EmpireID);
    def("PlayerEmpireID",           AIInterface::PlayerEmpireID);
    def("AllEmpireIDs",             AIInterface::AllEmpireIDs,  return_value_policy<return_by_value>());

    def("GetEmpire",                AIIntGetEmpireVoid,         return_value_policy<reference_existing_object>());
    def("GetEmpire",                AIIntGetEmpireInt,          return_value_policy<reference_existing_object>());

    def("GetUniverse",              AIInterface::GetUniverse,   return_value_policy<reference_existing_object>());

    def("CurrentTurn",              AIInterface::CurrentTurn);

    def("IssueFleetMoveOrder",      AIInterface::IssueFleetMoveOrder);
    def("IssueRenameOrder",         AIInterface::IssueRenameOrder);
    def("IssueNewFleetOrder",       AIInterface::IssueNewFleetOrder);
    def("IssueFleetColonizeOrder",  AIInterface::IssueFleetColonizeOrder);

    def("SendChatMessage",          AIInterface::SendPlayerChatMessage);

    def("DoneTurn",                 AIInterface::DoneTurn);

    def("LogOutput",                AIInterface::LogOutput);

    ///////////////////
    //     Empire    //
    ///////////////////
    class_<Empire, noncopyable>("Empire", no_init)
        .add_property("name",           make_function(&Empire::Name,        return_value_policy<copy_const_reference>()))
        .add_property("playerName",     make_function(&Empire::PlayerName,  return_value_policy<copy_const_reference>()))

        .add_property("empireID",       &Empire::EmpireID)
        .add_property("homeworldID",    &Empire::HomeworldID)
        .add_property("capitolID",      &Empire::CapitolID)

        .def("BuildingTypeAvailable",   &Empire::BuildingTypeAvailable)
        .def("AvailableBuildingTypes",  &Empire::AvailableBuildingTypes,    return_value_policy<copy_const_reference>()) 
        .def("TechResearched",          &Empire::TechResearched)
        .def("AvailableTechs",          &Empire::AvailableTechs,            return_value_policy<copy_const_reference>()) 
        .def("GetTechStatus",           &Empire::GetTechStatus)
        .def("ResearchStatus",          &Empire::ResearchStatus)

        .def("HasExploredSystem",       &Empire::HasExploredSystem)
    ;

    ////////////////////
    //    Universe    //
    ////////////////////
    class_<Universe, noncopyable>("Universe", no_init)
        .def("GetObject",           UniverseGetObject,                          return_value_policy<reference_existing_object>())
        .def("GetFleet",            UniverseGetFleet,                           return_value_policy<reference_existing_object>())
        .def("GetShip",             UniverseGetShip,                            return_value_policy<reference_existing_object>())
        .def("GetPlanet",           UniverseGetPlanet,                          return_value_policy<reference_existing_object>())
        .def("GetSystem",           UniverseGetSystem,                          return_value_policy<reference_existing_object>())
        .def("GetBuilding",         UniverseGetBuilding,                        return_value_policy<reference_existing_object>())
        .def("GetSpecial",          GetSpecial,                                 return_value_policy<reference_existing_object>())

        .add_property("allObjectIDs",  make_function(&Universe::FindObjectIDs<UniverseObject>, return_value_policy<return_by_value>()))

        .def("SystemHasStarlane",   &Universe::SystemReachable)
        .def("SystemsConnected",    &Universe::SystemsConnected)
    ;

    ////////////////////
    // UniverseObject //
    ////////////////////
    class_<UniverseObject, noncopyable>("UniverseObject", no_init)
        .add_property("id",                 &UniverseObject::ID)
        .add_property("name",               make_function(&UniverseObject::Name,        return_value_policy<copy_const_reference>()))
        .add_property("x",                  &UniverseObject::X)
        .add_property("y",                  &UniverseObject::Y)
        .add_property("systemID",           &UniverseObject::SystemID)
        .def("Unowned",                     &UniverseObject::Unowned)
        .def("OwnedBy",                     &UniverseObject::OwnedBy)
        .def("WhollyOwnedBy",               &UniverseObject::WhollyOwnedBy)
        .add_property("creationTurn",       &UniverseObject::CreationTurn)
        .add_property("ageInTurns",         &UniverseObject::AgeInTurns)
        .add_property("specials",           make_function(&UniverseObject::Specials,    return_value_policy<copy_const_reference>()))

        .def_readonly("INVALID_OBJECT_ID",  &UniverseObject::INVALID_OBJECT_ID)
        .def_readonly("INVALID_OBJECT_AGE", &UniverseObject::INVALID_OBJECT_AGE)
    ;

    ///////////////////
    //     Fleet     //
    ///////////////////
    class_<Fleet, bases<UniverseObject>, noncopyable>("Fleet", no_init)
        .add_property("finalDestinationID",         &Fleet::FinalDestinationID)
        .add_property("nextSystemID",               &Fleet::NextSystemID)
        .add_property("speed",                      &Fleet::Speed)
        .add_property("canChangeDirectionEnRoute",  &Fleet::CanChangeDirectionEnRoute)
        .add_property("hasArmedShips",              &Fleet::HasArmedShips)
        .add_property("numShips",                   &Fleet::NumShips)
        .def("ContainsShipID",                      &Fleet::ContainsShip)
        .add_property("shipIDs",                    make_function(&Fleet::ShipIDs,      return_value_policy<copy_const_reference>()))
    ;

    //////////////////
    //     Ship     //
    //////////////////
    class_<Ship, bases<UniverseObject>, noncopyable>("Ship", no_init)
        .add_property("fleetID",    &Ship::FleetID)
        .def("GetFleet",            &Ship::GetFleet,                return_value_policy<reference_existing_object>())
        .add_property("isArmed",    &Ship::IsArmed)
        .add_property("speed",      &Ship::Speed)
    ;

    //////////////////
    //   Building   //
    //////////////////
    class_<Building, bases<UniverseObject>, noncopyable>("Building", no_init)
        .def("GetBuildingType",         &Building::GetBuildingType, return_value_policy<reference_existing_object>())
        .add_property("operating",      &Building::Operating)
        .def("GetPlanet",               &Building::GetPlanet,       return_value_policy<reference_existing_object>())
    ;

    //////////////////
    // BuildingType //
    //////////////////
    class_<BuildingType, noncopyable>("BuildingType", no_init)
        .add_property("name",               make_function(&BuildingType::Name,          return_value_policy<copy_const_reference>()))
        .add_property("description",        make_function(&BuildingType::Description,   return_value_policy<copy_const_reference>()))
        .add_property("buildCost",          &BuildingType::BuildCost)
        .add_property("buildTime",          &BuildingType::BuildTime)
        .add_property("maintenanceCost",    &BuildingType::MaintenanceCost)
        .def("CaptureResult",               &BuildingType::GetCaptureResult)
    ;

    ////////////////////
    // ResourceCenter //
    ////////////////////
    class_<ResourceCenter, noncopyable>("ResourceCenter", no_init)
        .add_property("primaryFocus",       &ResourceCenter::PrimaryFocus)
        .add_property("secondaryFocus",     &ResourceCenter::SecondaryFocus)
    ;

    ///////////////////
    //   PopCenter   //
    ///////////////////
    class_<PopCenter, noncopyable>("PopCenter", no_init)
        .add_property("inhabitants",        &PopCenter::Inhabitants)
        .add_property("availableFood",      &PopCenter::AvailableFood)
    ;

    //////////////////
    //    Planet    //
    //////////////////
    class_<Planet, bases<UniverseObject, PopCenter, ResourceCenter>, noncopyable>("Planet", no_init)
        .add_property("size",               &Planet::Size)
        .add_property("type",               &Planet::Type)
        .add_property("buildings",          make_function(&Planet::Buildings,   return_value_policy<copy_const_reference>()))
    ;

    //////////////////
    //    System    //
    //////////////////
    class_<System, bases<UniverseObject>, noncopyable>("System", no_init)
        .add_property("starType",           &System::Star)
        .add_property("numOrbits",          &System::Orbits)
        .add_property("numStarlanes",       &System::Starlanes)
        .add_property("numWormholes",       &System::Wormholes)
        .def("HasStarlaneToSystemID",       &System::HasStarlaneTo)
        .def("HasWormholeToSystemID",       &System::HasWormholeTo)
    ;

    //////////////////
    //     Tech     //
    //////////////////
    class_<Tech, noncopyable>("Tech", no_init)
        .add_property("Name",               make_function(&Tech::Name,              return_value_policy<copy_const_reference>()))
        .add_property("Description",        make_function(&Tech::Description,       return_value_policy<copy_const_reference>()))
        .add_property("ShortDescription",   make_function(&Tech::ShortDescription,  return_value_policy<copy_const_reference>()))
        .add_property("Type",               &Tech::Type)
        .add_property("Category",           make_function(&Tech::Category,          return_value_policy<copy_const_reference>()))
        .add_property("ResearchCost",       &Tech::ResearchCost)
        .add_property("ResearchTurns",      &Tech::ResearchTurns)
        .add_property("Prerequisites",      make_function(&Tech::Prerequisites,     return_value_policy<copy_const_reference>()))
        .add_property("UnlockedTechs",      make_function(&Tech::UnlockedTechs,     return_value_policy<copy_const_reference>()))
    ;

    /////////////////
    //   Special   //
    /////////////////
    class_<Special, noncopyable>("Special", no_init)
        .add_property("Name",               make_function(&Special::Name,           return_value_policy<copy_const_reference>()))
        .add_property("Description",        make_function(&Special::Description,    return_value_policy<copy_const_reference>()))
    ;


    ////////////////////
    //     Enums      //
    ////////////////////
    enum_<StarType>("StarType")
        .value("Blue",      STAR_BLUE)
        .value("White",     STAR_WHITE)
        .value("Yellow",    STAR_YELLOW)
        .value("Orange",    STAR_ORANGE)
        .value("Red",       STAR_RED)
        .value("Neutron",   STAR_NEUTRON)
        .value("BlackHole", STAR_BLACK)
    ;
    enum_<PlanetSize>("PlanetSize")
        .value("Tiny",      SZ_TINY)
        .value("Small",     SZ_SMALL)
        .value("Medium",    SZ_MEDIUM)
        .value("Large",     SZ_LARGE)
        .value("Huge",      SZ_HUGE)
        .value("Asteroids", SZ_ASTEROIDS)
        .value("GasGiant",  SZ_GASGIANT)
    ;
    enum_<PlanetType>("PlanetType")
        .value("Swamp",     PT_SWAMP)
        .value("Radiated",  PT_RADIATED)
        .value("Toxic",     PT_TOXIC)
        .value("Inferno",   PT_INFERNO)
        .value("Barren",    PT_BARREN)
        .value("Tundra",    PT_TUNDRA)
        .value("Desert",    PT_DESERT)
        .value("Terran",    PT_TERRAN)
        .value("Ocean",     PT_OCEAN)
        .value("Asteroids", PT_ASTEROIDS)
        .value("GasGiant",  PT_GASGIANT)
    ;
    enum_<PlanetEnvironment>("PlanetEnvironment")
        .value("Uninhabitable", PE_UNINHABITABLE)
        .value("Hostile",       PE_HOSTILE)
        .value("Poor",          PE_POOR)
        .value("Adequate",      PE_ADEQUATE)
        .value("Good",          PE_GOOD)
    ;
    enum_<TechType>("TechType")
        .value("Theory",        TT_THEORY)
        .value("Application",   TT_APPLICATION)
        .value("Refinement",    TT_REFINEMENT)
    ;
    enum_<TechStatus>("TechStatus")
        .value("Unresearchable",    TS_UNRESEARCHABLE)
        .value("Researchable",      TS_RESEARCHABLE)
        .value("Complete",          TS_COMPLETE)
    ;
    enum_<MeterType>("MeterType")
        .value("Population",    METER_POPULATION)
        .value("Farming",       METER_FARMING)
        .value("Industry",      METER_INDUSTRY)
        .value("Research",      METER_RESEARCH)
        .value("Trade",         METER_TRADE)
        .value("Mining",        METER_MINING)
        .value("Construction",  METER_CONSTRUCTION)
        .value("Health",        METER_HEALTH)
    ;
    enum_<FocusType>("FocusType")
        .value("Balanced",  FOCUS_BALANCED)
        .value("Farming",   FOCUS_FARMING)
        .value("Industry",  FOCUS_INDUSTRY)
        .value("Mining",    FOCUS_MINING)
        .value("Research",  FOCUS_RESEARCH)
        .value("Trade",     FOCUS_TRADE)
    ;
    enum_<CaptureResult>("CaptureResult")
        .value("Capture",   CR_CAPTURE)
        .value("Destroy",   CR_DESTROY)
        .value("Retain",    CR_RETAIN)
        .value("Share",     CR_SHARE)
    ;


    ////////////////////
    // STL Containers //
    ////////////////////
    class_<std::vector<int> >("IntVec")
        .def(vector_indexing_suite<std::vector<int> >())
    ;
    class_<std::vector<std::string> >("StringVec")
        .def(vector_indexing_suite<std::vector<std::string> >())
    ;
    //class_<std::map<double, int> >("DoubleIntMap")
    //    .def(map_indexing_suite<std::map<double, int> >())
    //;

    /* std::set is not pre-wrapped by Boost Python, so these classes are trivially defined in order to
       be able to pass them to converters to the fully-exposed std::vector<> */
    class_<std::set<int> >("IntSet");
    def("IntSetToIntVector",        IntSetToIntVector);

    class_<std::set<std::string> >("StringSet");
    def("StringSetToStringVector",  StringSetToStringVector);
}
 
///////////////////////
//     PythonAI      //
///////////////////////
PythonAI::PythonAI()
{
    using boost::python::borrowed;

    Py_Initialize();    // initializes Python interpreter, allowing Python functions to be called from C++

    initFreeOrionAIInterface();   // allows the "FreeOrionAIInterface" C++ module to be imported within Python code


    // get access to Python main namespace, which is needed to call other functions below
    try {
        main_module = PyOBJECT((PyHANDLE(borrowed(PyImport_AddModule("__main__")))));
        dict = PyOBJECT(main_module.attr("__dict__"));
    } catch (PyERROR err) {
        Logger().errorStream() << "error with main module or namespace";
        return;
    }

    PyHANDLE handle;

    // import Python built-in sys module, giving access to sys.path below
    try {
        handle = PyHANDLE(PyRun_String("import sys", Py_file_input, dict.ptr(), dict.ptr()));
    } catch (PyERROR err) {
        Logger().errorStream() << "error importing sys";
        return;
    }

    // tell Python the path in which to locate AI script file
    std::string AI_path = (GetGlobalDir() / "default" / "AI").native_directory_string();
    std::string python_path_command = "sys.path.append('" + AI_path + "')";
    try {
        handle = PyHANDLE(PyRun_String(python_path_command.c_str(), Py_file_input, dict.ptr(), dict.ptr()));
    } catch (PyERROR err) {
        Logger().errorStream() << "error appending to sys.path";
        return;
    }

    // load Python script of AI functions
    try {
        handle = PyHANDLE(PyRun_String("import FreeOrionAI", Py_file_input, dict.ptr(), dict.ptr()));
    } catch (PyERROR err) {
        Logger().errorStream() << "error importing FreeOrionAI.py into Python";
        return;
    }

    // initialize AI within Python
    try {
        handle = PyHANDLE(PyRun_String("FreeOrionAI.InitFreeOrionAI()", Py_file_input, dict.ptr(), dict.ptr()));
    } catch(PyERROR err) {
        Logger().errorStream() << "error calling FreeOrionAI.InitFreeOrionAI()";
        return;
    }

    Logger().debugStream() << "Initialized PythonAI";
}

PythonAI::~PythonAI()
{
    Logger().debugStream() << "Cleaning up / destructing Python AI";
    Py_Finalize();      // stops Python interpreter and release its resources
}

void PythonAI::GenerateOrders()
{
    try {
        PyHANDLE handle = PyHANDLE(PyRun_String("FreeOrionAI.GenerateOrders()", Py_file_input, dict.ptr(), dict.ptr()));
    } catch(PyERROR err) {
        Logger().errorStream() << "error calling FreeOrionAI.GenerateOrders()";
        AIInterface::DoneTurn();
        return;
    }
}

void PythonAI::HandleChatMessage(int sender_id, const std::string& msg)
{}

//////////////////////////
// Container Conversion //
//////////////////////////
template <typename T>
std::vector<T> SetToVector(const std::set<T>& set)
{
    std::vector<T> retval;
    for (typename std::set<T>::const_iterator it = set.begin(); it != set.end(); ++it)
        retval.push_back(*it);
    return retval;
}
template <typename T>
std::vector<T> ListToVector(const std::set<T>& set)
{
    std::vector<T> retval;
    for (typename std::list<T>::const_iterator it = set.begin(); it != set.end(); ++it)
        retval.push_back(*it);
    return retval;
}
