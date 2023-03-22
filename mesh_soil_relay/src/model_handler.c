#include <zephyr/bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <dk_buttons_and_leds.h>
#include "model_handler.h"
#include "soil_cli.h"

#define GET_DATA_INTERVAL 3000

// static struct k_work_delayable get_data_work;

// static void get_data(struct k_work *work)
// {
// 	if (!bt_mesh_is_provisioned()) {
// 		k_work_schedule(&get_data_work, K_MSEC(GET_DATA_INTERVAL));
// 		return;
// 	}

// 	int err;

// 	err = bt_mesh_sensor_cli_get(&sensor_cli, NULL, &bt_mesh_sensor_present_amb_rel_humidity, NULL);
// 	if (err) {
// 		printk("Error getting soil humidity (%d)\n", err);
// 	}

// 	k_work_schedule(&get_data_work, K_MSEC(GET_DATA_INTERVAL));
// }

static void health_current_status(struct bt_mesh_health_cli *cli, uint16_t addr,
				  uint8_t test_id, uint16_t cid, uint8_t *faults,
				  size_t fault_count)
{
	size_t i;

	printk("Health Current Status from 0x%04x\n", addr);

	if (!fault_count) {
		printk("Health Test ID 0x%02x Company ID 0x%04x: no faults\n",
		       test_id, cid);
		return;
	}

	printk("Health Test ID 0x%02x Company ID 0x%04x Fault Count %zu:\n",
	       test_id, cid, fault_count);

	for (i = 0; i < fault_count; i++) {
		printk("\t0x%02x\n", faults[i]);
	}
}

static struct bt_mesh_health_cli health_cli = {
	.current_status = health_current_status,
};


static struct bt_mesh_cfg_cli cfg_cli = {
};

void handle_soil_report(struct bt_mesh_soil_cli *cli, struct bt_mesh_msg_ctx *ctx, struct bt_mesh_soil_report soil)
{
	printk("Data: %d, %d, %d\n", soil.humidity, soil.temperature, ctx->addr);
	// printk("Data: %d, %d\n", soil.humidity, soil.temperature);

}

static const struct bt_mesh_soil_cli_handlers soil_cb = {
	.report = handle_soil_report
};

struct bt_mesh_soil_cli soil_cli = BT_MESH_SOIL_CLI_INIT(&soil_cb);

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(0,
		BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV,
			BT_MESH_MODEL_CFG_CLI(&cfg_cli),
			BT_MESH_MODEL_HEALTH_CLI(&health_cli)
		),
		BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_SOIL_CLI(&soil_cli)
		)
	),
};

static const struct bt_mesh_comp comp = {
	.cid = CONFIG_BT_COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

const struct bt_mesh_comp *model_handler_init(void)
{
	// k_work_init_delayable(&get_data_work, get_data);
	
	// k_work_schedule(&get_data_work, K_MSEC(GET_DATA_INTERVAL));

	return &comp;
}
