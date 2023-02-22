#include <zephyr/bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/dk_prov.h>
#include <dk_buttons_and_leds.h>
#include "model_handler.h"

#define GROUP_ADDR 0xc000

static const uint16_t net_idx;
static const uint16_t app_idx;
static uint16_t self_addr = 1, node_addr;
static const uint8_t dev_uuid[16] = { 0xdd, 0xdd };
static uint8_t node_uuid[16];

K_SEM_DEFINE(sem_unprov_beacon, 0, 1);
K_SEM_DEFINE(sem_node_added, 0, 1);

static void setup_cdb(void)
{
	struct bt_mesh_cdb_app_key *key;

	key = bt_mesh_cdb_app_key_alloc(net_idx, app_idx);
	if (key == NULL) {
		printk("Failed to allocate app-key 0x%04x\n", app_idx);
		return;
	}

	bt_rand(key->keys[0].app_key, 16);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_app_key_store(key);
	}
}

static void configure_self(struct bt_mesh_cdb_node *self)
{
	struct bt_mesh_cdb_app_key *key;
	uint8_t status = 0;
	int err;

	printk("Configuring self...\n");

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		printk("No app-key 0x%04x\n", app_idx);
		return;
	}

	/* Add Application Key */
	err = bt_mesh_cfg_cli_app_key_add(self->net_idx, self->addr, self->net_idx, app_idx,
					  key->keys[0].app_key, &status);
	if (err || status) {
		printk("Failed to add app-key (err %d, status %d)\n", err,
		       status);
		return;
	}

	err = bt_mesh_cfg_cli_mod_app_bind(self->net_idx, self->addr, self->addr, app_idx, BT_MESH_MODEL_ID_HEALTH_CLI, &status);
	if (err || status) {
		printk("Failed to bind app-key (err %d, status %d)\n", err, status);
		return;
	}

	err = bt_mesh_cfg_cli_mod_app_bind(self->net_idx, self->addr, self->addr, app_idx, BT_MESH_MODEL_ID_SENSOR_CLI, &status);
	if (err || status) {
		printk("Failed to bind app-key (err %d, status %d)\n", err, status);
		return;
	}

	err = bt_mesh_cfg_cli_mod_sub_add(self->net_idx, self->addr, self->addr, GROUP_ADDR, BT_MESH_MODEL_ID_SENSOR_CLI, &status);
	if (err || status) {
		printk("Failed Subscribe (err: %d, status: %d)\n", err, status);
		return;
	}

	atomic_set_bit(self->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(self);
	}

	printk("Configuration complete\n");
}


static void configure_node(struct bt_mesh_cdb_node *node)
{
	NET_BUF_SIMPLE_DEFINE(buf, BT_MESH_RX_SDU_MAX);
	struct bt_mesh_comp_p0_elem elem;
	struct bt_mesh_cdb_app_key *key;
	struct bt_mesh_comp_p0 comp;
	uint8_t status;
	int err, elem_addr;

	printk("Configuring node 0x%04x...\n", node->addr);

	key = bt_mesh_cdb_app_key_get(app_idx);
	if (key == NULL) {
		printk("No app-key 0x%04x\n", app_idx);
		return;
	}

	/* Add Application Key */
	err = bt_mesh_cfg_cli_app_key_add(net_idx, node->addr, net_idx, app_idx, key->keys[0].app_key, &status);
	if (err || status) {
		printk("Failed to add app-key (err %d status %d)\n", err, status);
		return;
	}

	/* Get the node's composition data and bind all models to the appkey */
	err = bt_mesh_cfg_cli_comp_data_get(net_idx, node->addr, 0, &status, &buf);
	if (err || status) {
		printk("Failed to get Composition data (err %d, status: %d)\n", err, status);
		return;
	}

	err = bt_mesh_comp_p0_get(&comp, &buf);
	if (err) {
		printk("Unable to parse composition data (err: %d)\n", err);
		return;
	}

	elem_addr = node->addr;
	while (bt_mesh_comp_p0_elem_pull(&comp, &elem)) {
		printk("Element @ 0x%04x: %u + %u models\n", elem_addr, elem.nsig, elem.nvnd);
		for (int i = 0; i < elem.nsig; i++) {
			uint16_t id = bt_mesh_comp_p0_elem_mod(&elem, i);

			if (id == BT_MESH_MODEL_ID_CFG_CLI ||
			    id == BT_MESH_MODEL_ID_CFG_SRV) {
				continue;
			}
			printk("Binding AppKey to model 0x%03x:%04x\n", elem_addr, id);

			err = bt_mesh_cfg_cli_mod_app_bind(net_idx, node->addr, elem_addr, app_idx, id, &status);
			if (err || status) {
				printk("Failed (err: %d, status: %d)\n", err,
				       status);
			}

			if (id == BT_MESH_MODEL_ID_SENSOR_SRV) {
				printk("Setting Publication and subscription for Sensor Server\n");
				struct bt_mesh_cfg_cli_mod_pub pub = {
					.addr = GROUP_ADDR,
					.uuid = NULL,
					.app_idx = app_idx,
					.cred_flag = false,
					.ttl = 5,
					.period = BT_MESH_PUB_PERIOD_10SEC(1),
					.transmit = BT_MESH_TRANSMIT(0, 100)
				};
				err = bt_mesh_cfg_cli_mod_pub_set(net_idx, node->addr, elem_addr, id, &pub, &status);
				if (err || status) {
					printk("Failed Publication Set (err: %d, status: %d)\n", err,
						status);
				}

				err = bt_mesh_cfg_cli_mod_sub_add(net_idx, node->addr, elem_addr, GROUP_ADDR, id, &status);
				if (err || status) {
					printk("Failed Subscribe (err: %d, status: %d)\n", err,
						status);
				}
			}
		}

		elem_addr++;
	}

	atomic_set_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED);

	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		bt_mesh_cdb_node_store(node);
	}

	printk("Configuration complete\n");
}

static void unprovisioned_beacon(uint8_t uuid[16],
				 bt_mesh_prov_oob_info_t oob_info,
				 uint32_t *uri_hash)
{
	memcpy(node_uuid, uuid, 16);
	k_sem_give(&sem_unprov_beacon);
}

static void node_added(uint16_t net_idx, uint8_t uuid[16], uint16_t addr, uint8_t num_elem)
{
	node_addr = addr;
	k_sem_give(&sem_node_added);
}

static const struct bt_mesh_prov prov = {
	.uuid = dev_uuid,
	.unprovisioned_beacon = unprovisioned_beacon,
	.node_added = node_added,
};

static uint8_t check_unconfigured(struct bt_mesh_cdb_node *node, void *data)
{
	if (!atomic_test_bit(node->flags, BT_MESH_CDB_NODE_CONFIGURED)) {
		if (node->addr == self_addr) {
			configure_self(node);
		} else {
			// printk("Configure node\n");
			configure_node(node);
		}
	}

	return BT_MESH_CDB_ITER_CONTINUE;
}

static int bt_ready(void)
{
	uint8_t net_key[16], dev_key[16];
	int err;

	dk_leds_init();
	dk_buttons_init(NULL);

	err = bt_mesh_init(&prov, model_handler_init());
	if (err) {
		printk("Initializing mesh failed (err %d)\n", err);
		return err;
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	bt_rand(net_key, 16);

	err = bt_mesh_cdb_create(net_key);
	if (err == -EALREADY) {
		printk("Using stored CDB\n");
	} else if (err) {
		printk("Failed to create CDB (err %d)\n", err);
		return err;
	} else {
		printk("Created CDB\n");
		setup_cdb();
	}

	bt_rand(dev_key, 16);

	err = bt_mesh_provision(net_key, BT_MESH_NET_PRIMARY, 0, 0, self_addr, dev_key);
	if (err == -EALREADY) {
		printk("Using stored settings\n");
	} else if (err) {
		printk("Provisioning failed (err %d)\n", err);
		return err;
	} else {
		printk("Provisioning completed\n");
	}

	printk("Mesh initialized\n");

	return 0;
}

void main(void)
{
	char uuid_hex_str[32 + 1];

	int err;

	printk("Initializing...\n");

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}

	printk("Bluetooth initialized\n");
	bt_ready();

	while (1) {
		k_sem_reset(&sem_unprov_beacon);
		k_sem_reset(&sem_node_added);
		bt_mesh_cdb_node_foreach(check_unconfigured, NULL);

		printk("Waiting for unprovisioned beacon...\n");
		err = k_sem_take(&sem_unprov_beacon, K_SECONDS(10));
		if (err == -EAGAIN) {
			continue;
		}

		bin2hex(node_uuid, 16, uuid_hex_str, sizeof(uuid_hex_str));

		printk("Provisioning %s\n", uuid_hex_str);
		err = bt_mesh_provision_adv(node_uuid, net_idx, 0, 0);
		if (err < 0) {
			printk("Provisioning failed (err %d)\n", err);
			continue;
		}

		printk("Waiting for node to be added...\n");
		err = k_sem_take(&sem_node_added, K_SECONDS(10));
		if (err == -EAGAIN) {
			printk("Timeout waiting for node to be added\n");
			continue;
		}

		printk("Added node 0x%04x\n", node_addr);
	}
}
