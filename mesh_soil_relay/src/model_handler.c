#include <zephyr/bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <dk_buttons_and_leds.h>
#include "model_handler.h"

#define GET_DATA_INTERVAL 3000

static void sensor_cli_data_cb(struct bt_mesh_sensor_cli *cli,
			       struct bt_mesh_msg_ctx *ctx,
			       const struct bt_mesh_sensor_type *sensor,
			       const struct sensor_value *value)
{
	printk("Sensor data callback: %d\n", sensor->id);
	printk("Data: %d, %d\n", value->val1, value->val2);
	printk("Context: %d\n", ctx->addr);
}

static void sensor_cli_desc_cb(struct bt_mesh_sensor_cli *cli, struct bt_mesh_msg_ctx *ctx,
			       const struct bt_mesh_sensor_info *sensor)
{
	
}

static const struct bt_mesh_sensor_cli_handlers bt_mesh_sensor_cli_handlers = {
	.data = sensor_cli_data_cb,
	.sensor = sensor_cli_desc_cb,
};

static struct bt_mesh_sensor_cli sensor_cli = BT_MESH_SENSOR_CLI_INIT(&bt_mesh_sensor_cli_handlers);

static struct k_work_delayable get_data_work;

static void get_data(struct k_work *work)
{
	if (!bt_mesh_is_provisioned()) {
		k_work_schedule(&get_data_work, K_MSEC(GET_DATA_INTERVAL));
		return;
	}

	int err;

	err = bt_mesh_sensor_cli_get(&sensor_cli, NULL, &bt_mesh_sensor_present_amb_rel_humidity, NULL);
	if (err) {
		printk("Error getting soil humidity (%d)\n", err);
	}

	k_work_schedule(&get_data_work, K_MSEC(GET_DATA_INTERVAL));
}

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

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(
		1, BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV,
			BT_MESH_MODEL_CFG_CLI(&cfg_cli),
			BT_MESH_MODEL_HEALTH_CLI(&health_cli),
			BT_MESH_MODEL_SENSOR_CLI(&sensor_cli)),
		BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
	.cid = CONFIG_BT_COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

const struct bt_mesh_comp *model_handler_init(void)
{
	k_work_init_delayable(&get_data_work, get_data);
	
	k_work_schedule(&get_data_work, K_MSEC(GET_DATA_INTERVAL));

	return &comp;
}
