#ifndef BT_MESH_SOIL_H__
#define BT_MESH_SOIL_H__

#include <zephyr/bluetooth/mesh.h>
#include <bluetooth/mesh/model_types.h>

#define SOIL_CLI_MODEL_ID 0x0011
#define SOIL_SRV_MODEL_ID 0x0012

/** Soil set message parameters.  */
struct bt_mesh_soil_report {
	uint8_t humidity;
    uint8_t temperature;
};

#define BT_MESH_SOIL_OP_SOIL_REPORT BT_MESH_MODEL_OP_3(0x11, CONFIG_BT_COMPANY_ID)

#endif

