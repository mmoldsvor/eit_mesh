#include <zephyr/bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <bluetooth/mesh/dk_prov.h>
#include <dk_buttons_and_leds.h>
#include "model_handler.h"
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/adc.h>

#include "soil_srv.h"
#include "soil.h"


#if !DT_NODE_EXISTS(DT_PATH(zephyr_user)) || \
	!DT_NODE_HAS_PROP(DT_PATH(zephyr_user), io_channels)
#error "No suitable devicetree overlay specified"
#endif

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* Data of ADC io-channels specified in devicetree. */
static const struct adc_dt_spec adc_channels[] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels,
			     DT_SPEC_AND_COMMA)
};


static void configure_node() {
	printk("Configure node\n");
}


static void prov_complete(uint16_t net_idx, uint16_t addr)
{
	configure_node();
	printk("Provisioning complete\n");
}

static void prov_reset(void)
{
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

static uint8_t dev_uuid[16];

static const struct bt_mesh_prov prov = {
	.uuid = dev_uuid,
	.output_actions = BT_MESH_NO_OUTPUT,
	.complete = prov_complete,
	.reset = prov_reset,
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	dk_leds_init();
	dk_buttons_init(NULL);

	err = bt_mesh_init(&prov, model_handler_init());
	// err = bt_mesh_init(bt_mesh_dk_prov_init(), model_handler_init());
	if (err) {
		printk("Initializing mesh failed (err %d)\n", err);
		return;
	}

	if (IS_ENABLED(CONFIG_BT_MESH_LOW_POWER)) {
		bt_mesh_lpn_set(true);
	}

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	/* This will be a no-op if settings_load() loaded provisioning info */
	bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

	printk("Mesh initialized\n");
}


void my_work_handler(struct k_work *work)
{
	int16_t buf;
	int err;
	struct bt_mesh_soil_report soil;

	struct adc_sequence sequence = {
		.buffer = &buf,
		/* buffer size in bytes, not number of samples */
		.buffer_size = sizeof(buf),
	};

    printk("ADC reading:\n");
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
			// int32_t val_mv;

		printk("- %s, channel %d: ",
		       adc_channels[i].dev->name,
		       adc_channels[i].channel_id);

		(void)adc_sequence_init_dt(&adc_channels[i], &sequence);

		err = adc_read(adc_channels[i].dev, &sequence);
		if (err < 0) {
			printk("Could not read (%d)\n", err);
			continue;
		} else {
			printk("%d\n", buf);
			soil.humidity = buf;
			soil.temperature = 0;
		}

		soil_report(soil);

	}
}

K_WORK_DEFINE(adc_work, my_work_handler);

void my_timer_handler(struct k_timer *dummy)
{
    k_work_submit(&adc_work);
}

K_TIMER_DEFINE(my_timer, my_timer_handler, NULL);

/* start periodic timer that expires once every second */


void main(void)
{
	k_timer_start(&my_timer, K_SECONDS(1), K_SECONDS(1));

	int err;
	

	/* Configure channels individually prior to sampling. */
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!device_is_ready(adc_channels[i].dev)) {
			printk("ADC controller device not ready\n");
			return;
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) {
			printk("Could not setup channel #%d (%d)\n", i, err);
			return;
		}
	}

	printk("Initializing...\n");

	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
}
