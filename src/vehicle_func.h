/* $Id$ */

/** @file vehicle_func.h Functions related to vehicles. */

#ifndef VEHICLE_FUNC_H
#define VEHICLE_FUNC_H

#include "tile_type.h"
#include "strings_type.h"
#include "gfx_type.h"
#include "direction_type.h"
#include "cargo_type.h"
#include "command_type.h"
#include "vehicle_type.h"
#include "engine_type.h"
#include "transport_type.h"

#define is_custom_sprite(x) (x >= 0xFD)
#define IS_CUSTOM_FIRSTHEAD_SPRITE(x) (x == 0xFD)
#define IS_CUSTOM_SECONDHEAD_SPRITE(x) (x == 0xFE)

typedef void *VehicleFromPosProc(Vehicle *v, void *data);

void VehicleServiceInDepot(Vehicle *v);
void VehiclePositionChanged(Vehicle *v);
Vehicle *GetLastVehicleInChain(Vehicle *v);
const Vehicle *GetLastVehicleInChain(const Vehicle *v);
uint CountVehiclesInChain(const Vehicle *v);
bool IsEngineCountable(const Vehicle *v);
void DeleteVehicleChain(Vehicle *v);
void *VehicleFromPos(TileIndex tile, void *data, VehicleFromPosProc *proc);
void *VehicleFromPosXY(int x, int y, void *data, VehicleFromPosProc *proc);
void CallVehicleTicks();
Vehicle *FindVehicleOnTileZ(TileIndex tile, byte z);
uint8 CalcPercentVehicleFilled(const Vehicle *v, StringID *color);

void InitializeTrains();
byte VehicleRandomBits();
void ResetVehiclePosHash();
void ResetVehicleColorMap();

bool CanRefitTo(EngineID engine_type, CargoID cid_to);
CargoID FindFirstRefittableCargo(EngineID engine_type);
CommandCost GetRefitCost(EngineID engine_type);

void ViewportAddVehicles(DrawPixelInfo *dpi);

SpriteID GetRotorImage(const Vehicle *v);

uint32 VehicleEnterTile(Vehicle *v, TileIndex tile, int x, int y);

StringID VehicleInTheWayErrMsg(const Vehicle* v);
Vehicle *FindVehicleBetween(TileIndex from, TileIndex to, byte z, bool without_crashed = false);
Vehicle *GetVehicleTunnelBridge(TileIndex tile, TileIndex endtile);

void DecreaseVehicleValue(Vehicle *v);
void CheckVehicleBreakdown(Vehicle *v);
void AgeVehicle(Vehicle *v);
void VehicleEnteredDepotThisTick(Vehicle *v);

void BeginVehicleMove(const Vehicle *v);
void EndVehicleMove(const Vehicle *v);
void MarkSingleVehicleDirty(const Vehicle *v);

UnitID GetFreeUnitNumber(VehicleType type);

void TrainConsistChanged(Vehicle *v);
void TrainPowerChanged(Vehicle *v);
Money GetTrainRunningCost(const Vehicle *v);

/* Old style list kept for migration */
uint GenerateVehicleSortList(const Vehicle*** sort_list, uint16 *length_of_array, VehicleType type, PlayerID owner, uint32 index, uint16 window_type);
void GenerateVehicleSortList(VehicleList *list, VehicleType type, PlayerID owner, uint32 index, uint16 window_type);
void BuildDepotVehicleList(VehicleType type, TileIndex tile, VehicleList *engine_list, VehicleList *wagon_list);
CommandCost SendAllVehiclesToDepot(VehicleType type, uint32 flags, bool service, PlayerID owner, uint16 vlw_flag, uint32 id);
void VehicleEnterDepot(Vehicle *v);

CommandCost MaybeReplaceVehicle(Vehicle *v, uint32 flags, bool display_costs);
bool CanBuildVehicleInfrastructure(VehicleType type);

void CcCloneVehicle(bool success, TileIndex tile, uint32 p1, uint32 p2);


struct GetNewVehiclePosResult {
	int x, y;
	TileIndex old_tile;
	TileIndex new_tile;
};

/* returns true if staying in the same tile */
GetNewVehiclePosResult GetNewVehiclePos(const Vehicle *v);
Direction GetDirectionTowards(const Vehicle *v, int x, int y);

static inline bool IsPlayerBuildableVehicleType(VehicleType type)
{
	switch (type) {
		case VEH_TRAIN:
		case VEH_ROAD:
		case VEH_SHIP:
		case VEH_AIRCRAFT:
			return true;

		default: return false;
	}
}

static inline bool IsPlayerBuildableVehicleType(const BaseVehicle *v)
{
	return IsPlayerBuildableVehicleType(v->type);
}

const struct Livery *GetEngineLivery(EngineID engine_type, PlayerID player, EngineID parent_engine_type, const Vehicle *v);

/**
 * Get the colour map for an engine. This used for unbuilt engines in the user interface.
 * @param engine_type ID of engine
 * @param player ID of player
 * @return A ready-to-use palette modifier
 */
SpriteID GetEnginePalette(EngineID engine_type, PlayerID player);

/**
 * Get the colour map for a vehicle.
 * @param v Vehicle to get colour map for
 * @return A ready-to-use palette modifier
 */
SpriteID GetVehiclePalette(const Vehicle *v);

extern const uint32 _veh_build_proc_table[];
extern const uint32 _veh_sell_proc_table[];
extern const uint32 _veh_refit_proc_table[];
extern const uint32 _send_to_depot_proc_table[];

/* Functions to find the right command for certain vehicle type */
static inline uint32 GetCmdBuildVeh(VehicleType type)
{
	return _veh_build_proc_table[type];
}

static inline uint32 GetCmdBuildVeh(const BaseVehicle *v)
{
	return GetCmdBuildVeh(v->type);
}

static inline uint32 GetCmdSellVeh(VehicleType type)
{
	return _veh_sell_proc_table[type];
}

static inline uint32 GetCmdSellVeh(const BaseVehicle *v)
{
	return GetCmdSellVeh(v->type);
}

static inline uint32 GetCmdRefitVeh(VehicleType type)
{
	return _veh_refit_proc_table[type];
}

static inline uint32 GetCmdRefitVeh(const BaseVehicle *v)
{
	return GetCmdRefitVeh(v->type);
}

static inline uint32 GetCmdSendToDepot(VehicleType type)
{
	return _send_to_depot_proc_table[type];
}

static inline uint32 GetCmdSendToDepot(const BaseVehicle *v)
{
	return GetCmdSendToDepot(v->type);
}

bool EnsureNoVehicleOnGround(TileIndex tile);
void StopAllVehicles();

extern VehicleID _vehicle_id_ctr_day;
extern const Vehicle *_place_clicked_vehicle;
extern VehicleID _new_vehicle_id;
extern uint16 _returned_refit_capacity;

#endif /* VEHICLE_FUNC_H */
