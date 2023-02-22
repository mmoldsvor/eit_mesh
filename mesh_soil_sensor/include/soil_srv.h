#ifndef SOIL_SRV_H__
#define SOIL_SRV_H__

#include "soil.h"
#include <zephyr/bluetooth/mesh/access.h>

struct bt_mesh_soil_srv;

#define BT_MESH_MODEL_SOIL_SRV(_srv)			    \
		BT_MESH_MODEL_VND_CB(CONFIG_BT_COMPANY_ID,  \
			SOIL_SRV_MODEL_ID,                      \
			BT_MESH_MODEL_NO_OPS, &(_srv)->pub,     \
			BT_MESH_MODEL_USER_DATA(				\
				struct bt_mesh_soil_srv, _srv),     \
			&_bt_mesh_soil_srv_cb)
		

struct bt_mesh_soil_srv {
    /** Application handler functions. */
	const struct bt_mesh_soil_srv_handlers * handlers;
	/** Model entry. */
	struct bt_mesh_model *model;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/* Publication buffer */
	struct net_buf_simple pub_msg;
	/* Publication data */
	uint8_t buf[BT_MESH_MODEL_BUF_LEN(BT_MESH_SOIL_OP_SOIL_REPORT, BT_MESH_LEN_EXACT(1))];
	/** Transaction ID tracker for the set messages. */
	struct bt_mesh_tid_ctx prev_transaction;
};

int bt_mesh_soil_report(struct bt_mesh_soil_srv *srv, struct bt_mesh_soil_report soil);

// extern const struct bt_mesh_model_op _bt_mesh_soil_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_soil_srv_cb;

#endif

