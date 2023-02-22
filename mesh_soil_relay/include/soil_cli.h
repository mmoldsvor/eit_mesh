#ifndef SOIL_CLI_H__
#define SOIL_CLI_H__

#include "soil.h"
#include <zephyr/bluetooth/mesh/access.h>

struct bt_mesh_soil_cli;

#define BT_MESH_SOIL_CLI_INIT(_handlers)		\
{												\
	.handlers = _handlers,						\
}

/** Bluetooth Mesh Soil Server model handlers. */
struct bt_mesh_soil_cli_handlers {
	/** @brief Handler for a report message. 
	 *
	 * @param[in] cli Soil Server that received the report message.
	 */
	void (*const report) (struct bt_mesh_soil_cli *cli, struct bt_mesh_msg_ctx *ctx, struct bt_mesh_soil_report soil);
};

#define BT_MESH_MODEL_SOIL_CLI(_cli)			    					\
		BT_MESH_MODEL_VND_CB(CONFIG_BT_COMPANY_ID,  					\
			SOIL_CLI_MODEL_ID,                      					\
			_bt_mesh_soil_cli_op, NULL,       							\
			BT_MESH_MODEL_USER_DATA(struct bt_mesh_soil_cli, _cli),     \
			&_bt_mesh_soil_cli_cb)


struct bt_mesh_soil_cli {
    /** Application handler functions. */
	const struct bt_mesh_soil_cli_handlers * handlers;
	/** Model entry. */
	struct bt_mesh_model *model;
	/** Publish parameters. */
	struct bt_mesh_model_pub pub;
	/* Publication buffer */
	struct net_buf_simple pub_msg;
	/* Publication data */
	uint8_t buf[BT_MESH_MODEL_BUF_LEN(BT_MESH_SOIL_OP_SOIL_REPORT, BT_MESH_LEN_MIN(sizeof(struct bt_mesh_soil_report)))];
	/** Transaction ID tracker for the set messages. */
	struct bt_mesh_tid_ctx prev_transaction;
};

extern const struct bt_mesh_model_op _bt_mesh_soil_cli_op[];
extern const struct bt_mesh_model_cb _bt_mesh_soil_cli_cb;

#endif

